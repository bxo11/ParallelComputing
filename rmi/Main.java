import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.sql.Timestamp;
import java.util.Set;

public class Main {
    public static void main(String[] args) throws RemoteException {

        ReservationSystem system = new ReservationSystem();

        system.configuration(5, 5000);

        system.reservation("me", Set.of(0));
        System.out.println(system.notReservedSeats());
        system.confirmation("me");
        system.reservation("me2", Set.of(0));
//        system.reservation("me2", Set.of(0));
        System.out.println("Hello world!");




    }
}