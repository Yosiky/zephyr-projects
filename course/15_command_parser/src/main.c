#include "zephyr/sys/atomic_types.h"
#include "zephyr/sys/util.h"
#include "zephyr/toolchain.h"
#include <stdint.h>
#include <zephyr/device.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "command_parser.h"

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

static void uart_write(const struct device *uart, const char *text)
{
	while (*text != '\0') {
		uart_poll_out(uart, *text++);
	}
}

#define COMMAND_MAX_SIZE 32

struct command_info_t {
	const char *name;
	int (*helper)(void);
};

static int exec_command_help()
{
	const struct device *uart = DEVICE_DT_GET(UART_NODE);

	uart_write(uart, "help\n"
			 "\thelp   -- show existen instructions\n"
			 "\tstatus -- show dropped count of dropped symbols\n");
	return 0;
}

static int exec_command_status()
{
	const struct device *uart = DEVICE_DT_GET(UART_NODE);
	const size_t num_size = 32;
	char num[num_size];
	const atomic_val_t value = atomic_get(&dropped);
	snprintf(num, num_size, "%ld", value);

	uart_write(uart, "Count of dropped symbols is ");
	uart_write(uart, num);
	uart_write(uart, "\n");
	return 0;
}

static int exec_command_reset()
{
	const struct device *uart = DEVICE_DT_GET(UART_NODE);

	atomic_set(&dropped, 0);
	uart_write(uart, "Dropped counter reset\r\n");
	return 0;
}

static const struct command_info_t commands[] = {
	[COMMAND_HELP] = {.name = "help", .helper = exec_command_help},
	[COMMAND_STATUS] = {.name = "status", .helper = exec_command_status},
	[COMMAND_RESET] = {.name = "reset", .helper = exec_command_reset},
};

static int exec_command(const char *command)
{
	const struct device *uart = DEVICE_DT_GET(UART_NODE);

	enum command_id command_idx = command_parse(command);
	if (command_idx != COMMAND_UNKNOWN) {
		if (commands[command_idx].helper) {
			return commands[command_idx].helper();
		}
		uart_write(uart, "Command function is not implemented yet.\n");
	}

	return -1;
}

int main(void)
{
	char command[COMMAND_MAX_SIZE];
	size_t length = 0;
	const struct device *uart = DEVICE_DT_GET(UART_NODE);

	if (!device_is_ready(uart)) {
		printk("UART device is not ready.\n");
		return -1;
	}

	uart_irq_callback_user_data_set(uart, uart_isr, NULL);
	uart_irq_rx_enable(uart);

	/* ERROR: Why does it show before Zephyr hello? */
	// uart_write(uart, "\n> ");
	while (true) {
		uint8_t byte;

		k_msgq_get(&rx_bytes, &byte, K_FOREVER);
		/* Symbol is '\n' */
		switch (byte) {
		case '\r':
			command[length] = '\0';
			uart_write(uart, "\r\n");

			if (exec_command(command)) {
				uart_write(uart, command);
				uart_write(uart, ": Unknown command\n");
			}
			length = 0;
			uart_write(uart, "> ");
			break;
			// case '\n':
			// 	uart_write(uart, "\n");
			//
			//           if (command_exec(command, length)) {
			//               uart_write(uart, "Unknown command\n");
			//           }
			// 	break;
		case '\n':
			break;
		case 127:
			if (length > 0) {
				uart_write(uart, "\b \b");
				--length;
			}
			break;
		default:
			if (length < COMMAND_MAX_SIZE - 1) {
				command[length++] = byte;
			}
			uart_poll_out(uart, byte);
		}

		// printk("Symbol = \"%d\"\n", (int)byte);
	}
}
