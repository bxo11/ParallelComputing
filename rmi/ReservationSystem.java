import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.*;

public class ReservationSystem extends UnicastRemoteObject implements Cinema {
    private Map<Integer, Seat> seatsRegistry = new HashMap<>();

    private Map<Integer, String> seatToUser = new HashMap<>();

    public ReservationSystem() throws RemoteException {
        super();
        try {
            System.setProperty("java.rmi.server.hostname","192.168.1.2");
            Cinema stub = (Cinema) UnicastRemoteObject.exportObject(this, 0);
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind(SERVICE_NAME, stub);
//            registry.bind(SERVICE_NAME, stub);
            System.out.println(SERVICE_NAME + " bound");
        } catch (Exception e) {
            System.err.println(SERVICE_NAME + " exception:");
            e.printStackTrace();
        }
    }

    @Override
    public void configuration(int seats, long timeForConfirmation) throws RemoteException {
        for (int i = 0; i < seats; i++) {
            seatsRegistry.put(i, new Seat(i, timeForConfirmation));
        }
    }

    @Override
    public Set<Integer> notReservedSeats() throws RemoteException {
        Set<Integer> freeSeats = new TreeSet<>();
        for (Map.Entry<Integer, Seat> entry : seatsRegistry.entrySet()) {
            if (entry.getValue().canBeReserved()) {
                freeSeats.add(entry.getKey());
            }
        }

        return freeSeats;
    }

    @Override
    public boolean reservation(String user, Set<Integer> seats) throws RemoteException {
        for (int number : seats) {
            if (!seatsRegistry.get(number).canBeReserved()) {
                return false;
            }
        }

        for (int number : seats) {
            seatsRegistry.get(number).setReservation();
            seatToUser.put(number, user);
        }

        return true;
    }

    @Override
    public boolean confirmation(String user) throws RemoteException {
        for (Map.Entry<Integer, String> entry : seatToUser.entrySet()) {
            if (entry.getValue().equals(user)) {
                if (seatsRegistry.get(entry.getKey()).timeHasPass()) {
                    return false;
                }
            }
        }

        List<Integer> toRemove = new ArrayList<>();
        for (Map.Entry<Integer, String> entry : seatToUser.entrySet()) {
            if (entry.getValue().equals(user)) {
                seatsRegistry.get(entry.getKey()).confirmReservation(user);
                toRemove.add(entry.getKey());
            }
        }

        for (int seat : toRemove) {
            seatToUser.remove(seat);
        }

        return true;
    }

    @Override
    public String whoHasReservation(int seat) throws RemoteException {
        return seatsRegistry.get(seat).user;
    }
}
