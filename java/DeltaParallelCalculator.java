/**
 * Interfejs narzędzia wyznaczającego różnice pomiędzy 
 * zestawami danych.
 *
 */
public interface DeltaParallelCalculator {
	/**
	 * Metoda ustala liczbę wątków jaka ma być użyta do liczenia
	 * delty.
	 * 
	 * @param threads liczba wątków.
	 */
	public void setThreadsNumber(int threads);

	/**
	 * Przekazany jako parametr obiekt ma być używany
	 * do przekazywania za jego pomocą rezultatu.
	 * 
	 * @param receiver obiekt odbierający wyniki
	 */
	public void setDeltaReceiver(DeltaReceiver receiver);

	/**
	 * Przekazanie danych do przetworzenia.
	 * 
	 * @param data obiekt z danymi do przetworzenia
	 */
	public void addData(Data data);
}
