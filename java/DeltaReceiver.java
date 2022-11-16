import java.util.List;

/**
 * Interfejs odbiorcy danych
 *
 */
public interface DeltaReceiver {
	/**
	 * Do tej metody należy dostarczyć listę wykrytych różnic pomiędzy danymi
	 * z różnych zestawów.
	 * 
	 * @param deltas lista różnic
	 */
	public void accept( List<Delta> deltas );
}
