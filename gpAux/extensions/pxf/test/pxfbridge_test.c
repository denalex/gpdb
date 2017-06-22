#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

/* Define UNIT_TESTING so that the extension can skip declaring PG_MODULE_MAGIC */
#define UNIT_TESTING

/* include unit under test */
#include "../src/pxfbridge.c"

void
test_pxfbridge_cleanup(void **state)
{
    //TODO make sure context elements are cleaned up
}

void
test_pxfbridge_import_start(void **state)
{
    //TODO make sure context elements are initialized
}

void
test_pxfbridge_read(void **state)
{
    GpIdentity.segindex = 31;
    const char *EXPECTED_TUPLE = "31,hello world 31";

    /* setup call arguments */
    gphadoop_context *context = palloc0(sizeof(gphadoop_context));
    ListCell *fragment = palloc0(sizeof(ListCell));
    fragment->next = NULL;
    context->current_fragment = fragment;
    int datalen = 100;
    char *databuf = palloc0(datalen);

    /* set mock expectations */
    //TODO mock churl_read when libchurl is integrated

    int result = gpbridge_read(context, databuf, datalen);

    assert_int_equal(result, strlen(databuf));             // return number of bytes actually read
    assert_string_equal(databuf, EXPECTED_TUPLE);          // databuf has expected data
    assert_int_equal(context->current_fragment, fragment); // fragment is not exhausted

    /* clean up */
    pfree(fragment);
    pfree(databuf);
}

int
main(int argc, char* argv[])
{
    cmockery_parse_arguments(argc, argv);

    const UnitTest tests[] = {
            unit_test(test_pxfbridge_cleanup),
            unit_test(test_pxfbridge_import_start),
            unit_test(test_pxfbridge_read)
    };

    MemoryContextInit();

    return run_tests(tests);
}