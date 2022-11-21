import java.util.ArrayList;
import java.util.List;

public class DeltaReceiverImpl implements DeltaReceiver {
    public final List<Delta> deltas = new ArrayList<>();
    @Override
    public void accept(List<Delta> deltas) {
        this.deltas.addAll(deltas);
    }
}
