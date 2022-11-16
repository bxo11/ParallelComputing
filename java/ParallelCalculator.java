import java.util.*;
import java.util.concurrent.Callable;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ParallelCalculator implements DeltaParallelCalculator {
    private int threadNumber;
    private DeltaReceiver deltaReceiver;
    private final Map<Integer, DataImpl> waitingData = new HashMap<>();
    private final List<Integer> tasks = new ArrayList<>(); //single value represent smaller index for a pair of Data, e.g. 1 represents a pair of 1 and 2

    private ExecutorService executor;

    public boolean isFinished(){
        if(tasks.size()==0) return true;
        return false;
    }
    /**
     * @param id smaller id of a pair of data
     * @return differences
     */
    public void findDiffs(int id){
        synchronized (this) {
            tasks.remove(tasks.get(0));
        }

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
        try {
            Thread.sleep(3);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
//        tasks.remove(Integer.valueOf(id));
        this.deltaReceiver.accept(diffs);
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
