
/**
 * Interfejs pojedynczego zestawu danych
 *
 */
public interface Data {
	/**
	 * Numer zestawu danych. Każdy zestaw danych ma unikalny numer. Zestawy
	 * numerowane są od 0 wzwyż.
	 * 
	 * @return liczba całkowita oznaczająca numer zestawu danych
	 */
	public int getDataId();

	/**
	 * Rozmiar zestawu danych. Poprawne indeksy dla danych mieszczą się od 0 do
	 * getSize-1.
	 * 
	 * @return liczba danych.
	 */
	public int getSize();

	/**
	 * Odczyt danej z podanego indeksu. Poprawne indeksy dla danych mieszczą się od
	 * 0 do getSize-1.
	 * 
	 * @param idx numer indeksu
	 * @return odczytana wartość
	 */
	public int getValue(int idx);
}
