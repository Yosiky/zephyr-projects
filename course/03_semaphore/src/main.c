#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

K_SEM_DEFINE(data_ready, 0, 1);

void thread_a(void *, void *, void *)
{
	while (1) {
		printk("Thread A: produced\n");
		k_sem_give(&data_ready);
		k_sem_give(&data_ready);
		k_sleep(K_SECONDS(10));
	}
}

void thread_b(void *, void *, void *)
{
	while (1) {
		int sem_status = k_sem_take(&data_ready, K_SECONDS(1));

		if (sem_status == 0) {
			printk("Thread B: consumed\n");
		} else {
			printk("Thread B: timeout\n");
		}
		k_sleep(K_SECONDS(5));
	}
}

/* Thread A parameters */
#define THREAD_A_STACK_SIZE 0x1000
#define THREAD_A_PRIORITY   10

K_THREAD_DEFINE(threadA, THREAD_A_STACK_SIZE, thread_a, NULL, NULL, NULL, THREAD_A_PRIORITY, 0, 0);

/* Thread B parameters */
#define THREAD_B_STACK_SIZE 0x1000
#define THREAD_B_PRIORITY   10

K_THREAD_DEFINE(threadB, THREAD_B_STACK_SIZE, thread_b, NULL, NULL, NULL, THREAD_B_PRIORITY, 0, 0);
