#include <zephyr/ztest.h>

#include "command_parser.h"

ZTEST(command_parser, test_recognizes_help)
{
	zassert_equal(command_parse("help"), COMMAND_HELP);
}

ZTEST(command_parser, test_recognizes_status)
{
	zassert_equal(command_parse("status"), COMMAND_STATUS);
}

ZTEST(command_parser, test_recognizes_reset)
{
    zassert_equal(command_parse("reset"), COMMAND_RESET);
}

ZTEST(command_parser, test_rejects_unknown_and_empty)
{
	zassert_equal(command_parse("unknown"), COMMAND_UNKNOWN);
	zassert_equal(command_parse(""), COMMAND_UNKNOWN);
}

ZTEST_SUITE(command_parser, NULL, NULL, NULL, NULL, NULL);
