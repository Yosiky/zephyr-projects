#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

K_MUTEX_DEFINE(mutex);

void thread_a(void *, void *, void *)
{
	while (1) {
		if (k_mutex_lock(&mutex, K_MSEC(100)) == 0) {
			printk("A: start\n");
			k_msleep(1000);
			printk("A: end\n");

			k_mutex_unlock(&mutex);
		}
	}
}

void thread_b(void *, void *, void *)
{
	while (1) {
		if (k_mutex_lock(&mutex, K_FOREVER) == 0) {
			printk("B: start\n");
			k_msleep(1000);
			printk("B: end\n");

			k_mutex_unlock(&mutex);
		}
	}
}

#define THREAD_STACK_SIZE 0x1000
#define THREAD_A_PRIO     5
#define THREAD_B_PRIO     5
// #difine

K_THREAD_DEFINE(threadA, THREAD_STACK_SIZE, thread_a, NULL, NULL, NULL, THREAD_A_PRIO, 0, 0);

K_THREAD_DEFINE(threadB, THREAD_STACK_SIZE, thread_b, NULL, NULL, NULL, THREAD_B_PRIO, 0, 0);
