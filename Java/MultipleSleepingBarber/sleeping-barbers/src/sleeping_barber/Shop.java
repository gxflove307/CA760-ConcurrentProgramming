package sleeping_barber;

import java.text.SimpleDateFormat;
import java.util.LinkedList;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Date;

public class Shop {
    private int nBarbers;
    private int nChairs;
    private ConcurrentHashMap<Integer, Barber> barbers;
    private LinkedList<Customer> waitingCustomers;//等待队列

    private Lock lock = new ReentrantLock(true);
    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss:SSS");

    /**
     * initialize the Shop
     *
     * @param numBarbers
     * @param numChairs
     */
    public Shop(int numBarbers, int numChairs) {
        this.nBarbers = numBarbers;
        this.nChairs = numChairs;
        waitingCustomers = new LinkedList<>();
        barbers = new ConcurrentHashMap<>();
        for (int i = 0; i < nBarbers; i++) {
            barbers.put(i, new Barber(i));
        }
    }

    /**
     * A new customer entries into the shop
     *
     * @param customer
     * @return
     * @throws InterruptedException
     */
    public int entryShop(Customer customer) throws InterruptedException {
        lock.lock();
        int barberId;
        Barber barber;
        if (waitingCustomers.size() >= nChairs) {
            System.out.println("Customer " + customer.getId() + " left the shop since there is no waiting chair available, chair size = " + waitingCustomers.size());
            lock.unlock();
            return -1;
        }
        //if all barbers are busy, the customer thread will wait
        barberId = findSleepingBarber();
        if (barberId == -1) {
            waitingCustomers.add(customer);
            System.out.println("Customer " + customer.getId() + " has found a seat at waiting chair No." + waitingCustomers.size());
            lock.unlock();

            // customer thread waits until a barber to notify this customer thread
            customer.getLock().lock();
            customer.getCondition().await();
            customer.getLock().unlock();

            // when this customer thread is wakened up by a barber, the customer would know which barber is
            lock.lock();
            barberId = customer.getBarberId();
            barber = barbers.get(barberId);
        } else {
            // bind the customer with the barber and setting the barber's status to "not idle"
            customer.setBarberId(barberId);
            barber = barbers.get(barberId);
            barber.setCustomer(customer);
            barber.setIdle(false);
            barbers.put(barberId, barber);
//            System.out.println("Customer " + customer.getId() + " wakened up barber " + barberId);
        }

        lock.unlock();

        // after acquired an idle barber, this customer thread wakens up all barber threads.
        barber.getLock().lock();
        barber.getCondition().signalAll();
        barber.getLock().unlock();
        return barberId;
    }

    /**
     * @param customerId
     * @param barberId
     * @throws InterruptedException
     */
    public void exitShop(int customerId, int barberId) throws InterruptedException {
        lock.lock();
        Barber barber = barbers.get(barberId);
        Customer customer = barber.getCustomer();
        System.out.println("Customer " + customerId + " is sleeping while " + "barber " + barberId + " is cutting hair");
        lock.unlock();

        customer.getLock().lock();
        customer.getCondition().await();
        customer.getLock().unlock();

        lock.lock();
        System.out.println(getCurrentTime() + "    |    Customer-" + customerId + " terminated");

        // the barber is ready to serve next customer
        barber.getLock().lock();
        barber.getCondition().signalAll();
        barber.getLock().unlock();
        lock.unlock();
    }

    /**
     * @param barberId
     * @throws InterruptedException
     */
    public void serveCustomer(int barberId) throws InterruptedException {
        lock.lock();
        Barber barber = barbers.get(barberId);
        Customer customer;
        barber.getLock().lock();
        if (waitingCustomers.size() == 0) {
            System.out.println("Barber " + barberId + " goes to sleep since there is no customer");
            barber.setIdle(true);
            lock.unlock();

            barber.getLock().lock();
            barber.getCondition().await();
            barber.getLock().unlock();

            // After the customer waken up this barber thread, the barber would be able to get his customer
            lock.lock();
            customer = barber.getCustomer();
        } else {
            customer = waitingCustomers.poll();

            // bind the customer with a barber
            customer.setBarberId(barberId);

            // bind this barber with the customer
            barber.setCustomer(customer);
            barbers.put(barberId, barber);
            lock.unlock();

            // the barber notifies his customer's thread
            customer.getLock().lock();
            customer.getCondition().signalAll();
            customer.getLock().unlock();

            // the barber waits for the thread of his customer
            barber.getLock().lock();
            barber.getCondition().await();
            barber.getLock().unlock();

            lock.lock();
        }
        System.out.println("Barber " + barberId + " is serving his customer " + customer.getId());
        lock.unlock();

        // Cutting hair
        try {
            Thread.sleep(new Random().nextInt(1000));
        } catch (InterruptedException e) {
            System.out.print("Bar" + barberId + "异常");
        }
    }

    /**
     * @param barberId
     * @throws InterruptedException
     */
    public void finishHaircut(int barberId) throws InterruptedException {
        lock.lock();
        Barber barber = barbers.get(barberId);
        Customer customer = barber.getCustomer();
//        System.out.println("Barber " + barberId + " wakens customer " + customer.getId() + ".");
        lock.unlock();

        // the barber wakens up his customer's thread
        customer.getLock().lock();
        customer.getCondition().signalAll();
        customer.getLock().unlock();

        barber.getLock().lock();
        barber.getCondition().await();
        barber.getLock().unlock();

        lock.lock();
        System.out.println("Barber " + barberId + " finished his work, and is ready to serve next customer.");
        lock.unlock();
    }

    /**
     * finding a barber whose status is idle in the barber list
     *
     * @return barberId
     */
    public int findSleepingBarber() {
        lock.lock();
        for (Map.Entry<Integer, Barber> barber : barbers.entrySet()) {
            if (barber.getValue() != null && barber.getValue().getIdle()) {
                lock.unlock();
                return barber.getKey();
            }
        }
        lock.unlock();
        return -1;
    }


    /**
     *
     * @return
     */
    public String getCurrentTime(){
        Date date = new Date();
        return sdf.format(date);
    }
}