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
    private Map<Integer, DeltaProxy> deltas = new TreeMap<>();


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
                100L,
                TimeUnit.MILLISECONDS,
                new PriorityBlockingQueue<>(threadNumber, (firstTask, secondTask) -> Comparator.comparingInt(ProcessVector::getId).compare( (ProcessVector) firstTask, (ProcessVector) secondTask ))
        );
    }

    @Override
    public void setDeltaReceiver(DeltaReceiver receiver) {
        this.deltaReceiver = receiver;
    }

    @Override
    public void addData(Data data) {
        this.waitingVectors.put(data.getDataId(), data);
        this.vectorHistory.add(data.getDataId());
        this.numberOfVectorsUsage.put(data.getDataId(), 0);

        if (vectorHistory.contains(data.getDataId() - 1)) {
            extracted(data.getDataId() - 1);
        }

        if (vectorHistory.contains(data.getDataId() + 1)) {
            extracted(data.getDataId());
        }
    }

    private void extracted(int id) {
        deltas.put(id, new DeltaProxy());
        increaseUsage(id);
        increaseUsage(id + 1);

        Data d1 = waitingVectors.get(id);
        Data d2 = waitingVectors.get(id + 1);

        int chunkSize = (d1.getSize() + threadNumber - 1) / threadNumber; // divide by threads rounded up.
        for (int t = 0; t < threadNumber; t++) {
            int start = t * chunkSize;
            int end = Math.min(start + chunkSize, d1.getSize());
            findDiffsExecutor.execute(new ProcessVector(start, end, id, d1, d2));
        }
        removeCheckedData();
    }


    private class DeltaProxy {
        private final List<Delta> deltas;
        private final AtomicInteger counter;
        DeltaProxy() {
            counter = new AtomicInteger(0);
            deltas = new ArrayList<>();
        }

       synchronized public void addAll(List<Delta> incomingDeltas) {

            synchronized (deltas) {

                deltas.addAll(incomingDeltas);
                counter.set(counter.get() + 1);

                if (counter.get() != threadNumber) {
                    return;
                }
            }

            for (int id : ParallelCalculator.this.deltas.keySet()) {

                synchronized (deltas) {

                    if (nextDeltaIndexToReturn == id) {

                        if (ParallelCalculator.this.deltas.get(id).counter.get() != threadNumber) {
                            return;
                        }

                        deltaReceiver.accept(ParallelCalculator.this.deltas.get(id).deltas);
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
            deltas.get(id).addAll(diffs);
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
