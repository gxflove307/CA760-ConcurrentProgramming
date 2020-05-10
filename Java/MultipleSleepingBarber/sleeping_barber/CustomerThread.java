package sleeping_barber;

public class CustomerThread extends Thread {
    private int customerId;
    private int barber = -1;
    private Shop shop;

    public CustomerThread(Shop shop, int customerId) {
        this.shop = shop;
        this.customerId = customerId;
    }

    @Override
    public void run() {
        try {
            barber = shop.entryShop(new Customer(customerId));
            if (barber != -1) {
                shop.exitShop(customerId, barber);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
