import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class ParallelCalculator implements DeltaParallelCalculator {
    private int threadNumber;
    private int nextDeltaIndexToReturn = 0;
    private DeltaReceiver deltaReceiver;
    private final Map<Integer, Data> waitingVectors = new HashMap<>();
    private final Set<Integer> vectorHistory = new HashSet<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2
    private final Map<Integer, Integer> numberOfVectorsUsage = new HashMap<>();
    private ExecutorService findDiffsExecutor;
    private final Map<Integer, DeltaSender> deltasForVector = new TreeMap<>();


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

    @Override
    public void setThreadsNumber(int threads) {
        this.threadNumber = threads;
        this.findDiffsExecutor = new ThreadPoolExecutor(
                this.threadNumber,
                this.threadNumber,
                1000,
                TimeUnit.MILLISECONDS,
                new PriorityBlockingQueue<>(threadNumber, (o1, o2) -> Comparator.comparingInt(ProcessVector::getId).compare((ProcessVector) o1, (ProcessVector) o2))
        );
    }

    @Override
    public void setDeltaReceiver(DeltaReceiver receiver) {
        this.deltaReceiver = receiver;
    }

    @Override
    synchronized public void addData(Data data) {
        this.waitingVectors.put(data.getDataId(), data);
        this.vectorHistory.add(data.getDataId());
        this.numberOfVectorsUsage.put(data.getDataId(), 0);

        if (vectorHistory.contains(data.getDataId() - 1)) {
            compute(data.getDataId() - 1);
        }

        if (vectorHistory.contains(data.getDataId() + 1)) {
            compute(data.getDataId());
        }
    }

    private void compute(int id) {
        deltasForVector.put(id, new DeltaSender());
//        increaseUsage(id);
//        increaseUsage(id + 1);

        Data d1 = waitingVectors.get(id);
        Data d2 = waitingVectors.get(id + 1);

        int chunkSize = (d1.getSize() + threadNumber - 1) / threadNumber; // divide by threads rounded up.
        for (int t = 0; t < threadNumber; t++) {
            int start = t * chunkSize;
            int end = Math.min(start + chunkSize, d1.getSize());
            findDiffsExecutor.execute(new ProcessVector(start, end, id, d1, d2));
        }
//        removeCheckedData();
    }

    private class DeltaSender {
        private final List<Delta> deltas = new ArrayList<>();
        private int counter = 0;

        public void sendDeltas(List<Delta> inputDeltas) {
            synchronized (deltasForVector) {
                deltas.addAll(inputDeltas);
                counter++;

                if (counter != threadNumber) {
                    return;
                }

                for (int id : deltasForVector.keySet()) {
                    if (nextDeltaIndexToReturn == id) {
                        if (deltasForVector.get(id).counter != threadNumber) {
                            return;
                        }

                        deltaReceiver.accept(deltasForVector.get(id).deltas);
                        nextDeltaIndexToReturn++;
                    }
                }
            }
        }
    }

    class ProcessVector implements Runnable {
        private final int start;
        private final int end;
        private final int id;
        private final Data d1;
        private final Data d2;

        public int getId() {
            return id;
        }

        public ProcessVector(int start, int end, int id, Data d1, Data d2) {
            this.start = start;
            this.end = end;
            this.id = id;
            this.d1 = d1;
            this.d2 = d2;
        }

        @Override
        public void run() {
            List<Delta> diffs = new ArrayList<>();
            for (int i = start; i < end; i++) {
                int diff;
                diff = d2.getValue(i) - d1.getValue(i);
                if (diff != 0) {
                    diffs.add(new Delta(id, i, diff));
                }
            }
            synchronized (deltasForVector) {
                deltasForVector.get(id).sendDeltas(diffs);
            }
        }
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
// // // // // // // //