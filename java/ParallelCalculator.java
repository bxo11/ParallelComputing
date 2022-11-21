import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class ParallelCalculator implements DeltaParallelCalculator {
    private int threadNumber;
    private DeltaReceiverImpl deltaReceiver;
    private final Map<Integer, DataImpl> waitingData = new ConcurrentHashMap<>();
    private final Map<Integer, Integer> dataUsageTimes = new ConcurrentHashMap<>();

    private final Map<Integer, List<Delta>> deltaQueue = Collections.synchronizedMap(new TreeMap<>());
    private final AtomicInteger nextDeltaIndexToApply = new AtomicInteger(0);
    private final List<Integer> tasks = Collections.synchronizedList(new ArrayList<>()); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2

    private ExecutorService tasksExecutor;
    private ExecutorService findDiffsExecutor;

    public boolean isFinished() {
        return deltaQueue.size() == 0;
    }

    synchronized private void increaseUsage(int waitingDataId) {
        int actualValue = dataUsageTimes.get(waitingDataId);
        dataUsageTimes.put(waitingDataId, actualValue + 1);
    }

    synchronized private void removeCheckedData(int waitingDataId) {
        if (dataUsageTimes.get(waitingDataId) == 2) {
            waitingData.remove(waitingDataId);
            dataUsageTimes.remove(waitingDataId);
        }
    }

    /**
     * @param id smaller id of a pair of data
     * @return differences
     */
    public void findDiffs(int id) {
        increaseUsage(id);
        increaseUsage(id + 1);

        List<Integer> d1 = waitingData.get(id).getVector();
        List<Integer> d2 = waitingData.get(id + 1).getVector();
        List<Future<List<Delta>>> futures = new ArrayList<>();

        int chunkSize = (d1.size() + this.threadNumber - 1) / this.threadNumber; // divide by threads rounded up.
        for (int t = 0; t < this.threadNumber; t++) {
            int start = t * chunkSize;
            int end = Math.min(start + chunkSize, d1.size());
            final Future<List<Delta>> future = findDiffsExecutor.submit(new MyInfoCallable(start, end, id, d1, d2));
            futures.add(future);
//            findDiffsExecutor.submit(() -> {

//        exe[t] = new Thread(() -> {
//                List<Delta> diffs = new ArrayList<>();
//                for (int i = start; i < end; i++) {
//                    int diff;
//                        diff = d2.get(i) - d1.get(i);
//                        if (diff != 0) {
//                            diffs.add(new Delta(id, i, diff));
//                    }
//                }
//                synchronized (deltaQueue) {
//                    if (!deltaQueue.containsKey(id)) {
//                        this.deltaQueue.put(id, diffs);
//                    } else {
//                        this.deltaQueue.get(id).addAll(diffs);
//                    }
//                }
//            });
        }

//        for (int t = 0; t < this.threadNumber; t++) {
//            try {
//                exe[t].join();
//            } catch (InterruptedException e) {
//                throw new RuntimeException(e);
//            }
//        }

        List<Delta> diffs = new ArrayList<>();
        for (Future<List<Delta>> f : futures) {
            try {
                diffs.addAll(f.get());
            } catch (InterruptedException | ExecutionException ignored) {
            }
        }

        synchronized (deltaQueue) {
            this.deltaQueue.put(id, diffs);
            returnDeltas();
        }
    }

    private void returnDeltas() {
        Iterator<Map.Entry<Integer, List<Delta>>> iterator = deltaQueue.entrySet().iterator();
        Map.Entry<Integer, List<Delta>> current;
        if (iterator.hasNext()) {
            current = iterator.next();
        } else {
            return;
        }
        List<Integer> indexesToRemove = new ArrayList<>();

        while (nextDeltaIndexToApply.get() == current.getKey()) {
            this.deltaReceiver.accept(current.getValue());
//            System.out.println(deltaReceiver.deltas.size());
            indexesToRemove.add(current.getKey());
            if (iterator.hasNext()) current = iterator.next();
            nextDeltaIndexToApply.incrementAndGet();
        }

        for (int i : indexesToRemove) {
            deltaQueue.remove(i);
        }
    }


    private int popFirstTask() {
        int returnId = tasks.get(0);
        tasks.remove(0);
        return returnId;
    }

    @Override
    public void setThreadsNumber(int threads) {
        this.threadNumber = threads;
        this.tasksExecutor = Executors.newSingleThreadExecutor();
        this.findDiffsExecutor = Executors.newFixedThreadPool(threads);
    }

    @Override
    public void setDeltaReceiver(DeltaReceiver receiver) {
        this.deltaReceiver = (DeltaReceiverImpl) receiver;
    }

    @Override
    synchronized public void addData(Data data) {
        this.waitingData.put(data.getDataId(), (DataImpl) data);
        this.dataUsageTimes.put(data.getDataId(), 0);

        if (waitingData.containsKey(data.getDataId() - 1)) {
            tasks.add(data.getDataId() - 1);
        }

        if (waitingData.containsKey(data.getDataId() + 1)) {
            tasks.add(data.getDataId());
        }

        Collections.sort(tasks);

        while (tasks.size() > 0) {
            int popId = popFirstTask();
            findDiffs(popId);
//            tasksExecutor.execute(() -> findDiffs(popId));
        }
    }
}

class MyInfoCallable implements Callable<List<Delta>> {
    private int start;
    private int end;

    private int id;
    private List<Integer> d1;
    private List<Integer> d2;

    public MyInfoCallable(int start, int end, int id, List<Integer> d1, List<Integer> d2) {
        this.start = start;
        this.end = end;
        this.id = id;
        this.d1 = d1;
        this.d2 = d2;
    }

    @Override
    public List<Delta> call() {
        List<Delta> diffs = new ArrayList<>();
        for (int i = start; i < end; i++) {
            int diff;
            diff = d2.get(i) - d1.get(i);
            if (diff != 0) {
                diffs.add(new Delta(id, i, diff));
            }
        }
        return diffs;
    }

}