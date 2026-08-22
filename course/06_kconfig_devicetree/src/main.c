#include "zephyr/logging/log_core.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#define COURSE_CONSOLE_NODE DT_ALIAS(course_console)
#define COURSE_CFG          DT_NODELABEL(course_cfg)

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main()
{
	LOG_INF("Application started");

	const struct device *console = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	if (device_is_ready(console)) {
		LOG_INF("Console device is ready: %s", console->name);
	} else {
		LOG_ERR("Console device is not ready");
	}

	const struct device *course_console = DEVICE_DT_GET(COURSE_CONSOLE_NODE);
	LOG_INF("Course alias: %s", DT_NODE_PATH(COURSE_CONSOLE_NODE));
	LOG_INF("Course console is ready: %d", device_is_ready(course_console));

	LOG_INF("Greeting: %s", DT_PROP(COURSE_CFG, greeting));
	LOG_INF("Configured period: %u ms", DT_PROP(COURSE_CFG, tick_period_ms));
	LOG_INF("Kconfig period: %d ms", CONFIG_COURSE_TICK_PERIOD_MS);

	return 0;
}
