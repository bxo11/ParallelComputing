import java.io.Serializable;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.sql.Timestamp;
import java.util.*;

public class ReservationSystem extends UnicastRemoteObject implements Cinema {
    private Map<Integer, Seat> seatsRegistry = new HashMap<>();

    private Map<Integer, String> seatToUser = new HashMap<>();

    public static void main(String[] args) throws RemoteException {
        ReservationSystem system = new ReservationSystem();
        system.configuration(10, 30000);
        System.out.println("Server is running");
    }

    public ReservationSystem() throws RemoteException {
        super();
        try {
            Naming.rebind(SERVICE_NAME, this);
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }
    }

    @Override
    synchronized public void configuration(int seats, long timeForConfirmation) throws RemoteException {
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
    synchronized public boolean reservation(String user, Set<Integer> seats) throws RemoteException {
        for (int number : seats) {
            if (!seatsRegistry.get(number).canBeReserved()) {
                return false;
            }
        }

        long reservationTime = System.currentTimeMillis();
        for (int number : seats) {
            seatsRegistry.get(number).setReservation(reservationTime);
            seatToUser.put(number, user);
        }

        return true;
    }

    @Override
    synchronized public boolean confirmation(String user) throws RemoteException {
        int numberOfReservations=0;

        for (Map.Entry<Integer, String> entry : seatToUser.entrySet()) {
            if (entry.getValue().equals(user)) {
                numberOfReservations++;
                if (seatsRegistry.get(entry.getKey()).timeHasPass()) {
                    return false;
                }
            }
        }
        if (numberOfReservations==0) return false;

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

class Seat implements Serializable {
    final int number;
    String user = null;
    boolean isFree = true;
    Timestamp latestReservationTime;
    final float timeForConfirmation;

    public Seat(int number, float timeForConfirmation) {
        this.number = number;
        this.timeForConfirmation = timeForConfirmation;
    }

    void setReservation(long reservationTime) {
        this.latestReservationTime = new Timestamp(reservationTime + (long) timeForConfirmation);
    }

    boolean canBeReserved() {
        return this.isFree && timeHasPass();
    }

    boolean timeHasPass() {
        if (latestReservationTime == null) return true;
        return this.latestReservationTime.before(new Timestamp(System.currentTimeMillis()));
    }

    void confirmReservation(String user) {
        this.isFree = false;
        this.user = user;
    }
}