import java.io.Serializable;
import java.sql.Timestamp;

public class Seat implements Serializable {
    final int number;
    String user = null;
    boolean isFree = true;
    Timestamp latestReservationTime;
    final float timeForConfirmation;

    public Seat(int number, float timeForConfirmation) {
        this.number = number;
        this.timeForConfirmation = timeForConfirmation;
    }

    void setReservation() {
        this.latestReservationTime = new Timestamp(System.currentTimeMillis() + (long) timeForConfirmation);
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
