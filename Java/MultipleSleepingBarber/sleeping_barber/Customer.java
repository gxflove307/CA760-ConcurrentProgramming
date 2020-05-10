package sleeping_barber;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Customer {
    private int id;
    private int barberId;
    private Lock lock;
    private Condition condition;

    public Customer(int id) {
        this.id = id;
        lock = new ReentrantLock(true);
        condition = lock.newCondition();
    }

    public void setBarberId(int barberId) {
        this.barberId = barberId;
    }

    public int getBarberId() {
        return barberId;
    }

    public int getId() {
        return id;
    }

    public Lock getLock() {
        return lock;
    }

    public Condition getCondition() {
        return condition;
    }
}