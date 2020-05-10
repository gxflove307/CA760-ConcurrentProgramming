package sleeping_barber;

public class BarberThread extends Thread {
    private int barberId;
    private Shop shop;

    public BarberThread(Shop shop, int id) {
        this.shop = shop;
        this.barberId = id;
    }

    public void run() {
        while (true) {
            try {
                shop.serveCustomer(barberId);
                shop.finishHaircut(barberId);
            } catch (InterruptedException e1) {
                e1.printStackTrace();
            }
        }
    }
}
