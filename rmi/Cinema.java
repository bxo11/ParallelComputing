import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.Set;

/**
 * Interfejs serwisu rezerwacji miesc
 */
public interface Cinema extends Remote {

	public static String SERVICE_NAME = "CINEMA";

	/**
	 * Metoda konfigurująca parametry systemu.
	 * 
	 * @param seats               liczba miejsc
	 * @param timeForConfirmation czas w milisekundach na potwierdzenie rezerwacji
	 * @throws RemoteException wyjątek wymagany przez RMI
	 */
	public void configuration(int seats, long timeForConfirmation) throws RemoteException;

	/**
	 * Miejsca, które można zarezerwować. Zbiór nie zawiera miejsc, których
	 * rezerwacja została już potwierdzona i tych, których czas oczekiwania na
	 * potwierdzenie rezerwacji jeszcze nie minął.
	 * 
	 * @return zbiór numerów niezarezerwowanych miejsc
	 * @throws RemoteException wyjątek wymagany przez RMI
	 */
	public Set<Integer> notReservedSeats() throws RemoteException;

	/**
	 * Zgłoszenie rezerwacji przez użytkownika o podanej nazwie.
	 * 
	 * @param user  nazwa użytkownika serwisu
	 * @param seats zbiór miejsc, które użytkownik serwisu chce zarejestrować.
	 * @return true - miejsca o podanych numerach mogą zostać zarezerwowane. false -
	 *         rezerwacja nie może być zrealizowana, bo nie wszystkie wymienione
	 *         miesca są dostępne
	 * @throws RemoteException wyjątek wymagany przez RMI
	 */
	public boolean reservation(String user, Set<Integer> seats) throws RemoteException;

	/**
	 * Po uzyskaniu potwierdzenia, że miejsca mogą być zarezerwowane użytkownik musi
	 * rezerwację jeszcze potwierdzić. Rezerwacja wykonana w czasie
	 * timeForConfirmation od uzyskania informacji o dostępności musi zostać przez
	 * system zaakceptowana i zarezerwowane miejsca nie mogą być przez ten czas
	 * nikomu oferowane. Jeśli potwierdzenie pojawi się później, nie ma gwaracji, że
	 * miejsca są jeszcze dostępne.
	 * 
	 * @param user nazwa użytkownika serwisu
	 * @return true - rezerwacja miesc potwierdzona, false - miejsca nie są już
	 *         dostępne (tylko w przypadku spoźnionego potwierdzenia i rezerwacji
	 *         (nawet niepotwierdzonej) miejsca przez kogoś innego)
	 * @throws RemoteException wyjątek wymagany przez RMI
	 */
	public boolean confirmation(String user) throws RemoteException;

	/**
	 * Informacja o użytkowniku, który dokonał potwierdzonej rezerwacji miejsca. Do
	 * chwili zaakceptowania potwierdzenia metoda zwraca null.
	 * 
	 * @param seat numer miejsca
	 * @return nazwa użytkownika, który z sukcesem przeprowadził proces rejestracji
	 * @throws RemoteException wyjątek wymagany przez RMI
	 */
	public String whoHasReservation(int seat) throws RemoteException;
}
