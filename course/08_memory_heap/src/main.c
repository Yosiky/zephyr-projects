#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

K_HEAP_DEFINE(app_heap, 512);
LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
    /* Test 1: Allocate memory in the heap */
	void *block = k_heap_alloc(&app_heap, 128, K_NO_WAIT);
	if (block != NULL) {
		LOG_INF("Allocated 128 bytes at %p", block);
		k_heap_free(&app_heap, block);
		LOG_INF("Block released");
	} else {
		LOG_ERR("Allocation failed");
	}

    /* Test 2: Try to allocate memory region with size large then the heap size */
	void *too_large = k_heap_alloc(&app_heap, 1024, K_NO_WAIT);
	LOG_INF("Allocation of 1024 bytes: %s",
		too_large == NULL ? "failed as expected" : "unexpected success");

    /* Test 3: Allocate memory in the heap */
    block = k_heap_alloc(&app_heap, 128, K_NO_WAIT);
	if (block != NULL) {
		LOG_INF("Allocated 128 bytes at %p", block);
		k_heap_free(&app_heap, block);
		LOG_INF("Block released");
	} else {
		LOG_ERR("Allocation failed");
	}
}
