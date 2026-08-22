#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>

/* Queue */
struct msg_t {
	int value;
	int dropped;
};
K_MSGQ_DEFINE(msgq, sizeof(struct msg_t), 1, 4);

void thread_a(void *, void *, void *)
{
	struct msg_t msg = {0, 0};
	int err_code;

	while (1) {
		if (msg.value > 1000) {
			msg.value = 0;
		}
		++msg.value;

		err_code = k_msgq_put(&msgq, &msg, K_NO_WAIT);
		if (err_code) {
			if (msg.dropped > 1000) {
				msg.dropped = 0;
			}
			++msg.dropped;
			k_msgq_purge(&msgq);
			k_msgq_put(&msgq, &msg, K_NO_WAIT);
		}

		k_msleep(100);
	}
}

void thread_b(void *, void *, void *)
{
	struct msg_t msg;

	while (1) {
		k_msgq_get(&msgq, &msg, K_FOREVER);
		printk("Thread B: value=%d, dropped=%d\n", msg.value, msg.dropped);
		k_msleep(1000);
	}
}

/* Thread declaration */
#define THREAD_STACK_SIZE 0x1000
#define THREAD_A_PRIORITY 5
#define THREAD_B_PRIORITY 10

K_THREAD_DEFINE(thread_A, THREAD_STACK_SIZE, thread_a, NULL, NULL, NULL, THREAD_A_PRIORITY, 0, 0);

K_THREAD_DEFINE(thread_B, THREAD_STACK_SIZE, thread_b, NULL, NULL, NULL, THREAD_B_PRIORITY, 0, 0);
