#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

/* Define UNIT_TESTING so that the extension can skip declaring PG_MODULE_MAGIC */
#define UNIT_TESTING

/* include unit under test */
#include "../src/pxfheaders.c"

/* include mock files */
#include "mock/libchurl_mock.c"
#include "mock/pxfutils_mock.c"

void
test_get_format_name(void **state)
{
    char *formatName = get_format_name('t');
    assert_string_equal(formatName, TextFormatName);

    formatName = get_format_name('c');
    assert_string_equal(formatName, TextFormatName);

    formatName = get_format_name('b');
    assert_string_equal(formatName, GpdbWritableFormatName);

    MemoryContext old_context = CurrentMemoryContext;
    PG_TRY();
    {
        formatName = get_format_name('x');
        assert_false("Expected Exception");
    }
    PG_CATCH();
    {
        ErrorData  *edata;
        MemoryContextSwitchTo(old_context);
        edata = CopyErrorData();
        FlushErrorState();
        assert_string_equal(edata->message, "Unable to get format name for format code: x");
    }
    PG_END_TRY();

}

/* test setup and teardown methods */
void
before_test(void)
{
    // set global variables

}

void
after_test(void)
{
    // no-op, but the teardown seems to be required when the test fails, otherwise CMockery issues a mismatch error
}

int
main(int argc, char* argv[])
{
    cmockery_parse_arguments(argc, argv);

    const UnitTest tests[] = {
            /*
            unit_test_setup_teardown(, before_test, after_test),
            unit_test_setup_teardown(, before_test, after_test),
            unit_test_setup_teardown(, before_test, after_test),
            unit_test_setup_teardown(, before_test, after_test),
            */
            unit_test_setup_teardown(test_get_format_name, before_test, after_test)
    };

    MemoryContextInit();

    return run_tests(tests);
}
