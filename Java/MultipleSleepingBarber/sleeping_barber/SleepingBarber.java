package sleeping_barber;

import java.util.Calendar;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class SleepingBarber {
    private static Shop shop;
    private static int numBarbers;
    private static int numChairs;
    private static int numCustomers;

    public static void main(String[] args) throws InterruptedException {

        numBarbers = 2;
        numChairs = 5;
        numCustomers = 500;
        shop = new Shop(numBarbers, numChairs);
        ExecutorService service = Executors.newFixedThreadPool(numBarbers);

        for (int i = 0; i < numBarbers; i++) {
            service.submit(new BarberThread(shop, i));
        }

        Calendar calendar = Calendar.getInstance();
        calendar.getTime();
        for (int i = 0; i < numCustomers; i++) {
            CustomerThread customerThread = new CustomerThread(shop, i);
            System.out.println(shop.getCurrentTime() + "    |    Customer-"+ i + " created");
            Thread.sleep(new Random().nextInt(1000));
            customerThread.start();
        }
    }
}
