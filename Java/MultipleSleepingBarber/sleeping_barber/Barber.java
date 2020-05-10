package sleeping_barber;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Barber {
    private int id;
    private Customer customer;
    private Lock lock;
    private Condition condition;
    private boolean idle;

    public Barber(int id) {
        this.id = id;
        lock = new ReentrantLock(true);
        condition = lock.newCondition();
        idle = false;
    }

    public int getId() {
        return id;
    }

    public void setIdle(Boolean b) {
        idle = b;
    }

    public Boolean getIdle() {
        return idle;
    }

    public Lock getLock() {
        return lock;
    }

    public Condition getCondition() {
        return condition;
    }

    public void setCustomer(Customer customer) {
        this.customer = customer;
    }

    public Customer getCustomer() {
        return customer;
    }
}