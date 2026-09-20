#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

struct message {
	void *fifo_reserved;
	uint32_t sequence;
};

K_FIFO_DEFINE(message_fifo);
K_MEM_SLAB_DEFINE(message_slab, sizeof(struct message), 4, 4);

/* Producer thread */
#define PRODUCER_THREAD_STACK_SIZE 0x1000
#define PRODUCER_THREAD_PRIORITY   10
#define PRODUCER_THREAD_OPTIONS    0
#define PRODUCER_THREAD_DELAY      100

#define PRODUCER_THREAD_CNT  10
#define PRODUCER_THREAD_MSEC 1500
static void producer_task(void *, void *, void *)
{
	uint32_t sequence = 0;

	while (sequence < PRODUCER_THREAD_CNT) {
        struct message *msg;
        int rc = k_mem_slab_alloc(&message_slab, (void **)&msg, K_NO_WAIT);

        if (rc != 0) {
            LOG_WRN("No free message blocks");
            continue;
        }

        msg->sequence = sequence++;
        k_fifo_put(&message_fifo, msg);
        k_msleep(500);
	}
}

K_THREAD_DEFINE(producer, PRODUCER_THREAD_STACK_SIZE, producer_task, NULL, NULL, NULL,
		PRODUCER_THREAD_PRIORITY, PRODUCER_THREAD_OPTIONS, PRODUCER_THREAD_DELAY);

/* Consumer thread */
#define CONSUMER_THREAD_STACK_SIZE 0x1000
#define CONSUMER_THREAD_PRIORITY   10
#define CONSUMER_THREAD_OPTIONS    0
#define CONSUMER_THREAD_DELAY      0

static void consumer_task(void *, void *, void *)
{
	size_t i = 0;
	while (i < PRODUCER_THREAD_CNT) {
        struct message *msg = k_fifo_get(&message_fifo, K_FOREVER);

        LOG_INF("Received: %u", msg->sequence);
        k_mem_slab_free(&message_slab, (void *)msg);
        k_msleep(1000);
        ++i;
	}
}

K_THREAD_DEFINE(consumer, CONSUMER_THREAD_STACK_SIZE, consumer_task, NULL, NULL, NULL,
		CONSUMER_THREAD_PRIORITY, CONSUMER_THREAD_OPTIONS, CONSUMER_THREAD_DELAY);
