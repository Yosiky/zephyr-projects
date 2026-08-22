#include "zephyr/sys/printk.h"
#include "zephyr/toolchain.h"
#include <zephyr/kernel.h>

K_SEM_DEFINE(tick_sem, 0, 1);

static void timer_expiry(struct k_timer *timer)
{
	ARG_UNUSED(timer);
	printk("Timer epxiry\n");
	k_sem_give(&tick_sem);
}

static void timer_stoped(struct k_timer *timer)
{
	printk("Timer Stoped\n");
}
K_TIMER_DEFINE(periodic_timer, timer_expiry, timer_stoped);

int main()
{
	k_timer_start(&periodic_timer, K_MSEC(500), K_SECONDS(1));

	return 0;
}

/* Thread A */
#define THREAD_A_STACK_SIZE 0x1000
#define THREAD_A_PRIORITY   10
#define MAX_TICKS_COUNT     5

static void thread_a(void *, void *, void *)
{
	static int ticks = 0;
	int64_t prev = 0;
	int64_t now;

	while (ticks < MAX_TICKS_COUNT) {
		k_sem_take(&tick_sem, K_FOREVER);
		now = k_uptime_get();
		ticks++;
		printk("tick: %d, delta: %lld ms\n", ticks, now - prev);
		prev = now;
		k_msleep(1500);
	}

	k_timer_stop(&periodic_timer);
}

K_THREAD_DEFINE(threadA, THREAD_A_STACK_SIZE, thread_a, NULL, NULL, NULL, THREAD_A_PRIORITY, 0, 0);
