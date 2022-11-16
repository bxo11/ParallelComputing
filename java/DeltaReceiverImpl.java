import java.util.ArrayList;
import java.util.List;

public class DeltaReceiverImpl implements DeltaReceiver {
    private final List<Delta> deltas = new ArrayList<>();
    @Override
    synchronized public void accept(List<Delta> deltas) {
        this.deltas.addAll(deltas);
    }
}
