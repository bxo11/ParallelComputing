import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        int vectorAmount = 10;
        int vectorSize = 2000000;
        Data[] datas = new DataImpl[vectorAmount];

        for (int j=0;j<vectorAmount;j++) {
            List<Integer> l1 = new ArrayList<>(vectorSize);
            for (int i=0;i<vectorSize;i++) {
                l1.add(i);
            }
            l1.set(j, 100);
            datas[j] = new DataImpl(j,l1);
        }

        ParallelCalculator calc = new ParallelCalculator();
        DeltaReceiver receiver = new DeltaReceiverImpl();

        calc.setThreadsNumber(4);
        calc.setDeltaReceiver(receiver);

        long startTime = System.currentTimeMillis();

        List<Integer> shuffledIndexes = new ArrayList<>();
        for (int j=0;j<vectorAmount;j++) {
            shuffledIndexes.add(j);
        }
        Collections.shuffle(shuffledIndexes);

//        int x=0;
//        while(x<10){
//        }
        for (Integer j : shuffledIndexes) {
            calc.addData(datas[j]);
        }

        if (!calc.isFinished()){
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        long stopTime = System.currentTimeMillis();
        System.out.println((stopTime - startTime));
        System.out.println("end");

    }
}
