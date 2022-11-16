import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        int vectorAmount = 1000;
        int vectorSize = 100000;
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

        calc.setThreadsNumber(1);
        calc.setDeltaReceiver(receiver);

        long startTime = System.currentTimeMillis();
        for (int j=0;j<vectorAmount;j++) {
            calc.addData(datas[j]);
        }

        while(!calc.isFinished()) {
            try {
                Thread.sleep(3);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        long stopTime = System.currentTimeMillis();
        System.out.println((stopTime - startTime));
        System.out.println();

    }
}
