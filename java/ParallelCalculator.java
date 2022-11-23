import java.util.*;
import java.util.concurrent.*;

public class ParallelCalculator implements DeltaParallelCalculator {
    private int threadNumber;
    private DeltaReceiverImpl deltaReceiver;
    private final Map<Integer, DataImpl> waitingVectors = new HashMap<>();
    private final Set<Integer> vectorHistory = new HashSet<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2
    private final Map<Integer, Integer> numberOfVectorsUsage = new HashMap<>();
    private final Map<Integer, List<Delta>> deltaQueue = new TreeMap<>();
    private int nextDeltaIndexToReturn = 0;
    private final Map<Integer, Integer> taskOrder = new TreeMap<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2
    private ExecutorService tasksExecutor;
    private ExecutorService findDiffsExecutor;
    List<Future<Boolean>> taskFutures = new ArrayList<>();

    public boolean isFinished() {
        for (Future<Boolean> f : taskFutures) {
            try {
                f.get();
            } catch (InterruptedException | ExecutionException ignored) {
            }
        }
        return true;
    }

    //first and last vector have max usage equals 1
    private void increaseUsage(int waitingDataId) {
        int actualValue = numberOfVectorsUsage.get(waitingDataId);
        numberOfVectorsUsage.put(waitingDataId, actualValue + 1);
    }

    private void removeCheckedData() {
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

    private void returnDeltas() {
        Iterator<Map.Entry<Integer, List<Delta>>> iterator = deltaQueue.entrySet().iterator();
        Map.Entry<Integer, List<Delta>> current;
        if (iterator.hasNext()) {
            current = iterator.next();
        } else {
            return;
        }
        List<Integer> indexesToRemove = new ArrayList<>();

        while (nextDeltaIndexToReturn == current.getKey()) {
            this.deltaReceiver.accept(current.getValue());
            indexesToRemove.add(current.getKey());
            if (iterator.hasNext()) current = iterator.next();
            nextDeltaIndexToReturn++;
        }

        for (int i : indexesToRemove) {
            deltaQueue.remove(i);
        }
    }

    private int popFirstTask() {
        int returnId = taskOrder.entrySet().iterator().next().getKey();
        taskOrder.remove(returnId);
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
        if (this.vectorHistory.contains(data.getDataId())) return;

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
            final Future<Boolean> future = tasksExecutor.submit(new DiffsFinder());
            taskFutures.add(future);
        }
    }

    class DiffsFinder implements Callable<Boolean> {

        @Override
        public Boolean call() {
            int id = popFirstTask();
            increaseUsage(id);
            increaseUsage(id + 1);

            List<Integer> d1 = waitingVectors.get(id).getVector();
            List<Integer> d2 = waitingVectors.get(id + 1).getVector();
            List<Future<List<Delta>>> futures = new ArrayList<>();

            int chunkSize = (d1.size() + threadNumber - 1) / threadNumber; // divide by threads rounded up.
            for (int t = 0; t < threadNumber; t++) {
                int start = t * chunkSize;
                int end = Math.min(start + chunkSize, d1.size());
                final Future<List<Delta>> future = findDiffsExecutor.submit(new ProcessVector(start, end, id, d1, d2));
                futures.add(future);
            }

            List<Delta> diffs = new ArrayList<>();
            for (Future<List<Delta>> f : futures) {
                try {
                    diffs.addAll(f.get());
                } catch (InterruptedException | ExecutionException ignored) {
                }
            }

            //TODO: remove synchronized block
            synchronized (deltaQueue) {
                deltaQueue.put(id, diffs);
                returnDeltas();
            }
            removeCheckedData();

            return true;
        }
    }

}

class ProcessVector implements Callable<List<Delta>> {
    private final int start;
    private final int end;
    private final int id;
    private final List<Integer> d1;
    private final List<Integer> d2;

    public ProcessVector(int start, int end, int id, List<Integer> d1, List<Integer> d2) {
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

class DeltaReceiverImpl implements DeltaReceiver {
    public final List<Delta> deltas = new ArrayList<>();
    @Override
    public void accept(List<Delta> deltas) {
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
