#include "zephyr/sys/printk.h"
#include "zephyr/toolchain.h"
#include <zephyr/kernel.h>

#define MAX_TIMER_CNT 5
static void work_timer_cnt(struct k_work *work);
K_WORK_DEFINE(timer_work, work_timer_cnt);

static void timer_expiry(struct k_timer *timer)
{
	ARG_UNUSED(timer);
    k_work_submit(&timer_work);
}

static void timer_stoped(struct k_timer *timer)
{
	printk("Timer Stoped\n");
}
K_TIMER_DEFINE(periodic_timer, timer_expiry, timer_stoped);

#define MAX_TIMER_CNT 5
static void work_timer_cnt(struct k_work *work)
{
    ARG_UNUSED(work);
    static int cnt = 0;

    cnt++;
    printk("%s: tick=%d\n", __func__, cnt);

    if (cnt == MAX_TIMER_CNT) {
        k_timer_stop(&periodic_timer);
    }
    k_msleep(1500);
}

int main()
{
	k_timer_start(&periodic_timer, K_MSEC(500), K_SECONDS(1));

	return 0;
}
