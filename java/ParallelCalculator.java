import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class ParallelCalculator implements DeltaParallelCalculator {
    private int threadNumber;
    private DeltaReceiver deltaReceiver;
    private final Map<Integer, DataImpl> waitingData = new HashMap<>();
    private final Map<Integer, Integer> dataUsageTimes = new HashMap<>();

    private final Map<Integer, List<Delta>> deltaQueue = new TreeMap<>();
    private AtomicInteger nextDeltaIndexToApply = new AtomicInteger(0);
    private final List<Integer> tasks = new ArrayList<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2

    private ExecutorService executor;

    public boolean isFinished(){
        if(tasks.size()==0) return true;
        return false;
    }

    synchronized private void increaseUsage(int waitingDataId){
        int actualValue = dataUsageTimes.get(waitingDataId);
        dataUsageTimes.put(waitingDataId,actualValue+1);
    }

    synchronized private void removeCheckedData(int waitingDataId){
        if(dataUsageTimes.get(waitingDataId)==2) {
            waitingData.remove(waitingDataId);
            dataUsageTimes.remove(waitingDataId);
        }
    }

    /**
     * @param id smaller id of a pair of data
     * @return differences
     */
    public void findDiffs(int id){
        synchronized (this) {
            tasks.remove(tasks.get(0));
        }

        increaseUsage(id);
        increaseUsage(id+1);

        List<Integer> d1 = waitingData.get(id).getVector();
        List<Integer> d2 = waitingData.get(id+1).getVector();
        List<Delta> diffs = new ArrayList<>();

        int diff;
        for(int i=0;i<d1.size();i++){
            diff = d2.get(i)-d1.get(i);
            if (diff != 0){
                diffs.add(new Delta(id,i, diff));
            }
        }
//        try {
//            Thread.sleep(3);
//        } catch (InterruptedException e) {
//            throw new RuntimeException(e);
//        }
//        tasks.remove(Integer.valueOf(id));
        removeCheckedData(id);
        this.deltaQueue.put(id, diffs);
        returnDeltas();

//        this.deltaReceiver.accept(diffs);
    }

    synchronized private void returnDeltas(){
        Iterator<Map.Entry<Integer, List<Delta>>> iterator = deltaQueue.entrySet().iterator();

        Map.Entry<Integer, List<Delta>> current = iterator.next();

        while(nextDeltaIndexToApply.get() == current.getKey() ){
            this.deltaReceiver.accept(current.getValue());
            deltaQueue.remove(current.getKey());
            nextDeltaIndexToApply.incrementAndGet();
        }
//        Map.Entry<Integer, List<Delta>> current = iterator.next();
//        Map.Entry<Integer, List<Delta>> next = iterator.next();
//
//        while(current.getKey() == next.getKey() -1 ){
//            this.deltaReceiver.accept(current.getValue());
//            deltaQueue.remove(current.getKey());
//            current = iterator.next();
//            next = iterator.next();
//        }
    }

    @Override
    public void setThreadsNumber(int threads) {
        this.threadNumber = threads;
        this.executor = Executors.newFixedThreadPool(threads);
    }

    @Override
    public void setDeltaReceiver(DeltaReceiver receiver) {
        this.deltaReceiver = receiver;
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

        if (tasks.size()>0) {
            executor.execute(() ->  findDiffs(tasks.get(0)));
        }
    }
}
