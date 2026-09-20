#include "zephyr/toolchain.h"
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define UART_NODE DT_CHOSEN(zephyr_console)
K_MSGQ_DEFINE(rx_bytes, sizeof(uint8_t), 16, __alignof__(uint8_t));

static atomic_t dropped = 0;

static void uart_isr(const struct device *uart, void *user_data)
{
	ARG_UNUSED(user_data);

	uart_irq_update(uart);
	while (uart_irq_rx_ready(uart)) {
		uint8_t byte;

		if (uart_fifo_read(uart, &byte, 1) != 1) {
			break;
		}

		const int msgq_res = k_msgq_put(&rx_bytes, &byte, K_NO_WAIT);
		if (msgq_res) {
			atomic_inc(&dropped);
		}
	}
}

int main(void)
{
	const struct device *uart = DEVICE_DT_GET(UART_NODE);

	if (!device_is_ready(uart)) {
		printk("UART device is not ready.\n");
		return -1;
	}

	uart_irq_callback_user_data_set(uart, uart_isr, NULL);
	uart_irq_rx_enable(uart);

	while (true) {
		uint8_t byte;

		k_msgq_get(&rx_bytes, &byte, K_FOREVER);
		/* Symbol is '\n' */
		switch (byte) {
		case 13:
			printk("\n");
			break;
		case 127:
			printk("\b");

			break;
		default:
			uart_poll_out(uart, byte);
		}

		// printk("Symbol = \"%d\"\n", (int)byte);
	}
}

static void dropped_reporter(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		k_sleep(K_SECONDS(10));

		atomic_val_t count = atomic_clear(&dropped);

		if (count > 0) {
			printk("\nDropped symbols: %ld\n", (long)count);
		}
	}
}
K_THREAD_DEFINE(reporter, 1024, dropped_reporter, NULL, NULL, NULL, 10, 0, 0);
