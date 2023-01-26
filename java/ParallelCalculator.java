import java.util.*;
import java.util.concurrent.*;

public class ParallelCalculator implements DeltaParallelCalculator {
    private int threadNumber;
    private DeltaReceiver deltaReceiver;
    private final Map<Integer, DataImpl> waitingVectors = new HashMap<>();
    private final Set<Integer> vectorHistory = new HashSet<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2
    private final Map<Integer, Integer> numberOfVectorsUsage = new HashMap<>();
    private final Map<Integer, List<Delta>> deltaQueue = new TreeMap<>();
    private int nextDeltaIndexToReturn = 0;
    private final Map<Integer, Integer> taskOrder = new TreeMap<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2
    private ExecutorService tasksExecutor;
    List<Future<Boolean>> taskFutures = new ArrayList<>();

    //first and last vector have max usage equals 1
    synchronized private void increaseUsage(int waitingDataId) {
        int actualValue = numberOfVectorsUsage.get(waitingDataId);
        numberOfVectorsUsage.put(waitingDataId, actualValue + 1);
    }

    synchronized private void removeCheckedData() {
        List<Integer> indexesToRemove = new ArrayList<>();
        for (Map.Entry<Integer, Integer> entry : numberOfVectorsUsage.entrySet()) {
            if (entry.getValue() == 2) {
                indexesToRemove.add(entry.getKey());
            }
        }

        for (int index : indexesToRemove) {
            waitingVectors.remove(index);
            numberOfVectorsUsage.remove(index);
        }
    }

    synchronized private void returnDeltas() {
        Iterator<Map.Entry<Integer, List<Delta>>> iterator = deltaQueue.entrySet().iterator();
        Map.Entry<Integer, List<Delta>> current;
        if (iterator.hasNext()) {
            current = iterator.next();
        } else {
            return;
        }
        List<Integer> indexesToRemove = new ArrayList<>();

        List<Delta> listToSend = new ArrayList<>();
        while (nextDeltaIndexToReturn == current.getKey()) {
            listToSend.addAll(current.getValue());
//            this.deltaReceiver.accept(current.getValue());
            indexesToRemove.add(current.getKey());
            if (iterator.hasNext()) current = iterator.next();
            nextDeltaIndexToReturn++;
        }

        for (int i : indexesToRemove) {
            deltaQueue.remove(i);
        }
        this.deltaReceiver.accept(listToSend);
        if(nextDeltaIndexToReturn == vectorHistory.size() -1){
            tasksExecutor.shutdown();
        }
    }

    synchronized private int popFirstTask() {
        int returnId = taskOrder.entrySet().iterator().next().getKey();
        taskOrder.remove(returnId);
        return returnId;
    }

    @Override
    public void setThreadsNumber(int threads) {
        this.threadNumber = threads;
        this.tasksExecutor = Executors.newSingleThreadExecutor();
    }

    @Override
    public void setDeltaReceiver(DeltaReceiver receiver) {
        this.deltaReceiver = receiver;
    }

    @Override
    public void addData(Data data) {
        synchronized (waitingVectors){
//        if (this.vectorHistory.contains(data.getDataId())) return;

        this.waitingVectors.put(data.getDataId(), (DataImpl) data);
        this.vectorHistory.add(data.getDataId());
        this.numberOfVectorsUsage.put(data.getDataId(), 0);
        int repeats = 0;

        if (vectorHistory.contains(data.getDataId() - 1)) {
            taskOrder.put(data.getDataId() - 1, data.getDataId() - 1);
            repeats++;
        }

        if (vectorHistory.contains(data.getDataId() + 1)) {
            taskOrder.put(data.getDataId(), data.getDataId());
            repeats++;
        }

        for (int i = 0; i < repeats; i++) {
            tasksExecutor.execute(new DiffsFinder());
        }
    }}

    class DiffsFinder implements Runnable {

        @Override
        public void run() {
            int id = popFirstTask();
//            increaseUsage(id);
//            increaseUsage(id + 1);
            List<Integer> d1;
            List<Integer> d2;

            d1 = waitingVectors.get(id).getVector();
            d2 = waitingVectors.get(id + 1).getVector();

            List<FutureTask<List<Delta>>> futureTasks = new ArrayList<>();
            Thread[] threadArray = new Thread[threadNumber];

            int chunkSize = (d1.size() + threadNumber - 1) / threadNumber; // divide by threads rounded up.
            for (int t = 0; t < threadNumber; t++) {
                int start = t * chunkSize;
                int end = Math.min(start + chunkSize, d1.size());
//                final Future<List<Delta>> future = findDiffsExecutor.submit(new ProcessVector(start, end, id, d1, d2));
                final FutureTask<List<Delta>> futureTask = new FutureTask<>(new ProcessVector(id, d1.subList(start,end), d2.subList(start,end)));
//                futures.add(future);
                futureTasks.add(futureTask);
                threadArray[t] = new Thread(futureTask);
                threadArray[t].start();
            }

            List<Delta> diffs = new ArrayList<>();
            for (FutureTask<List<Delta>> f : futureTasks) {
                try {
                    diffs.addAll(f.get());
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                } catch (ExecutionException e) {
                    throw new RuntimeException(e);
                }
            }

            deltaQueue.put(id, diffs);
            returnDeltas();

//            removeCheckedData();
        }
    }

}

class ProcessVector implements Callable<List<Delta>> {
    private final int id;

    private final List<Integer> d1;
    private final List<Integer> d2;

    public ProcessVector(int id, List<Integer> d11, List<Integer> d22) {
        this.id =id;
        this.d1 = d11;
        this.d2 = d22;
    }

    @Override
    public List<Delta> call() {
        List<Delta> diffs = new ArrayList<>();
        int diff;
        for (int i = 0; i < d1.size(); i++) {
            diff = d2.get(i) - d1.get(i);
            if (diff != 0) {
                diffs.add(new Delta(id, i, diff));
            }
        }
        return diffs;
    }
}

class DeltaReceiverImpl implements DeltaReceiver {
    public final List<Delta> deltas = new ArrayList<>();
    @Override
    synchronized public void accept(List<Delta> deltas) {
        this.deltas.addAll(deltas);
    }
}

class DataImpl implements Data {
    private final int dataId;
    private final List<Integer> vector;

    public DataImpl(int dataId, List<Integer> vector) {
        this.dataId = dataId;
        this.vector = vector;
    }

    public List<Integer> getVector() {
        return vector;
    }

    @Override
    public int getDataId() {
        return dataId;
    }

    @Override
    public int getSize() {
        return vector.size();
    }

    @Override
    public int getValue(int idx) {
        return vector.get(idx);
    }
}
