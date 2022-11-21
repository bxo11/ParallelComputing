import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        int vectorAmount = 10;
        int vectorSize = 10000000;
        DataImpl[] datas = new DataImpl[vectorAmount];

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

        for (Integer j : shuffledIndexes) {

            calc.addData(datas[j]);
        }


//            try {
//                Thread.sleep(3000);
//            } catch (InterruptedException e) {
//                throw new RuntimeException(e);
//            }


        long stopTime = System.currentTimeMillis();
        System.out.println((stopTime - startTime));
        System.out.println("end");

    }
}
