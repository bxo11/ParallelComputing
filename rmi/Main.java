import java.rmi.RemoteException;
import java.util.Set;

public class Main {
    public static void main(String[] args) throws RemoteException {
        Cinema system = new ReservationSystem();
        system.configuration(10, 30000);
        system.reservation("a", Set.of(0));
        system.confirmation("a");

    }
}
