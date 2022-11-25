import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.Scanner;
import java.util.Set;

public class Client {
    public static void main(String[] args) throws RemoteException, MalformedURLException, NotBoundException {

        Cinema system = (Cinema) Naming.lookup("rmi://localhost/CINEMA");

        Scanner reader = new Scanner(System.in);  // Reading from System.in
        String user = args[0];
        while (true) {
            System.out.println();
            System.out.println("1-reservation\n2-confirmation\n3-notReservedSeats\n4-whoHasReservation");
            int n = reader.nextInt();
            switch (n) {
                case 1 -> {
                    int m = reader.nextInt();
                    System.out.println(system.reservation(user, Set.of(m)));
                }
                case 2 -> System.out.println(system.confirmation(user));
                case 3 -> System.out.println(system.notReservedSeats());
                case 4 -> {
                    int x = reader.nextInt();
                    System.out.println(system.whoHasReservation(x));
                }
            }
        }
    }
}