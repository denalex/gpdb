#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

/* Define UNIT_TESTING so that the extension can skip declaring PG_MODULE_MAGIC */
#define UNIT_TESTING

/* include unit under test */
#include "../src/pxfutils.c"

/* include mock files */
#include "mock/libchurl_mock.c"

void
test_are_ips_equal(void **state)
{
    assert_false(are_ips_equal(NULL, "ip"));
    assert_false(are_ips_equal("ip", NULL));
    assert_false(are_ips_equal("ip", "pi"));
    assert_true(are_ips_equal("ip", "ip"));
}

void
test_port_to_str(void **state)
{

    char* port = pstrdup("123");
    port_to_str(&port, 65535);
    assert_string_equal(port, "65535");

    /* test null original port */
    MemoryContext old_context = CurrentMemoryContext;
    PG_TRY();
    {
        port = pstrdup("123");
        port_to_str(NULL, 65535);
        assert_false("Expected Exception");
    }
    PG_CATCH();
    {
        MemoryContextSwitchTo(old_context);
        ErrorData *edata = CopyErrorData();
        assert_true(edata->elevel == ERROR);
        char* expected_message = pstrdup("unexpected internal error in pxfutils.c");
        assert_string_equal(edata->message, expected_message);
        pfree(expected_message);
        pfree(port);
    }
    PG_END_TRY();
}

void
test_replace_string(void **state)
{
    /* substring is not in the original string */
    char* replaced = replace_string("bar", "foo", "xyz");
    assert_string_equal(replaced, "bar");
    pfree(replaced);

    /* substring is not in the original string, but there's partial match */
    replaced = replace_string("fobar", "foo", "xyz");
    assert_string_equal(replaced, "fobar");
    pfree(replaced);

    /* substring is first in the original string, repeats 2 times, only 1st occurence is replaced */
    replaced = replace_string("foofoobar", "foo", "xyz");
    assert_string_equal(replaced, "xyzfoobar");
    pfree(replaced);

    /* substring is last in the original string */
    replaced = replace_string("barfoo", "foo", "xyz");
    assert_string_equal(replaced, "barxyz");
    pfree(replaced);

}

int
main(int argc, char* argv[])
{
    cmockery_parse_arguments(argc, argv);

    const UnitTest tests[] = {
            unit_test(test_are_ips_equal),
            unit_test(test_port_to_str),
            unit_test(test_replace_string)
    };

    MemoryContextInit();

    return run_tests(tests);
}
