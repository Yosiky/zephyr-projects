#include <stdint.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

struct measurement {
	uint32_t sequence;
	int32_t value;
};

K_MSGQ_DEFINE(measurements, sizeof(struct measurement), 4, __alignof__(struct measurement));

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
    int32_t value = 0;
    uint32_t dropped = 0;

	while (sequence < PRODUCER_THREAD_CNT) {
        struct measurement measurement = {
            .sequence = sequence++,
            .value = value++,
        };

        int rc = k_msgq_put(&measurements, &measurement, K_FOREVER);

        if (rc != 0) {
            dropped++;
            LOG_WRN("Queue full; dropped=%u", dropped);
        }

        k_msleep(100);
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
	while (true) {
        struct measurement measurement;

        k_msgq_get(&measurements, &measurement, K_FOREVER);
        LOG_INF("Received: sequence=%u value=%d",
                measurement.sequence, measurement.value);
        k_msleep(1000);
	}
}

K_THREAD_DEFINE(consumer, CONSUMER_THREAD_STACK_SIZE, consumer_task, NULL, NULL, NULL,
		CONSUMER_THREAD_PRIORITY, CONSUMER_THREAD_OPTIONS, CONSUMER_THREAD_DELAY);
