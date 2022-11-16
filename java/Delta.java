import java.util.Objects;

/**
 * Obiektowa reprezentacja pojedynczej różnicy pomiędzy zestawami danych.
 */
public class Delta {
	private final int dataID;
	private final int idx;
	private final int delta;

	public Delta(int dataID, int idx, int delta) {
		this.dataID = dataID;
		this.idx = idx;
		this.delta = delta;
	}

	/**
	 * Numer pierwszej z pary danych. Jeśli w poszukiwanu różnicy przetwarzane były
	 * dane o numerach 10 i 11 to dataID wynosi 10.
	 * 
	 * @return numer pierwszej z pary danych
	 */
	public int getDataID() {
		return dataID;
	}

	/**
	 * Numer indeksu, pod którym wykryto różnicę pomiędzy dwoma zestawami danych.
	 * 
	 * @return indeks
	 */
	public int getIdx() {
		return idx;
	}

	/**
	 * Wartość różnicy. Różnica wartości zapisanych pod indeksem idx w danych o
	 * numerach identyfikacyjnych dataID i dataID+1 wyliczana jest jako:
	 * 
	 * <pre>
	 * delta = data(dataID + 1)[idx] - data(dataID)[idx]
	 * </pre>
	 * 
	 * @return
	 */
	public int getDelta() {
		return delta;
	}

	@Override
	public int hashCode() {
		return Objects.hash(dataID, delta, idx);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Delta other = (Delta) obj;
		return dataID == other.dataID && delta == other.delta && idx == other.idx;
	}
}
