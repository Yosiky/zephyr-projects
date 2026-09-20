#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

int main(void)
{
    const struct device *uart = DEVICE_DT_GET(UART_NODE);

    if (!device_is_ready(uart)) {
        printk("UART is not ready\n");
        return 0;
    }

    uart_poll_out(uart, '>');
    uart_poll_out(uart, ' ');

    while (true) {
        unsigned char c;

        if (uart_poll_in(uart, &c) == 0) {
            uart_poll_out(uart, c);
        } else {
            k_msleep(10);
        }
    }
}
