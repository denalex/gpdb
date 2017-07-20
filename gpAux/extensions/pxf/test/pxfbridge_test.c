#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

/* Define UNIT_TESTING so that the extension can skip declaring PG_MODULE_MAGIC */
#define UNIT_TESTING

/* include unit under test */
#include "../src/pxfbridge.c"

/* include mock files */
#include "mock/libchurl_mock.c"
#include "mock/pxfuriparser_mock.c"
#include "mock/pxfheaders_mock.c"

/*
void gpbridge_cleanup(gphadoop_context *context);
void gpbridge_import_start(gphadoop_context *context);
int gpbridge_read(gphadoop_context *context, char *databuf, int datalen);
*/

void
test_gpbridge_cleanup(void **state)
{
    /* init data in context that will be cleaned up */
    gphadoop_context *context = palloc0(sizeof(gphadoop_context));
    context->churl_handle = (CHURL_HANDLE) palloc0(sizeof(CHURL_HANDLE));
    context->churl_headers = (CHURL_HEADERS) palloc0(sizeof(CHURL_HEADERS));
    context->gphd_uri = (GPHDUri *) palloc0(sizeof(GPHDUri));

    /* set mock behavior for churl cleanup */
    expect_value(churl_cleanup, handle, context->churl_handle);
    expect_value(churl_cleanup, after_error, false);
    will_be_called(churl_cleanup);

    expect_value(churl_headers_cleanup, headers, context->churl_headers);
    will_be_called(churl_headers_cleanup);

    expect_value(freeGPHDUri, uri, context->gphd_uri);
    will_be_called(freeGPHDUri);

    /* call function under test */
    gpbridge_cleanup(context);

    /* assert call results */
    assert_true(context->churl_handle == NULL);
    assert_true(context->churl_headers == NULL);
    assert_true(context->gphd_uri == NULL);

    /* cleanup */
    pfree(context);
}

void
test_gpbridge_import_start(void **state)
{
    //TODO make sure context elements are initialized
}

void
test_gpbridge_read(void **state)
{
    /*
    GpIdentity.segindex = 31;
    const char *EXPECTED_TUPLE = "31,hello world 31";

    / setup call arguments /
    gphadoop_context *context = palloc0(sizeof(gphadoop_context));
    ListCell *fragment = palloc0(sizeof(ListCell));
    fragment->next = NULL;
    context->current_fragment = fragment;
    int datalen = 100;
    char *databuf = palloc0(datalen);

    / set mock expectations /
    //TODO mock churl_read when libchurl is integrated

    int result = gpbridge_read(context, databuf, datalen);

    assert_int_equal(result, strlen(databuf));             // return number of bytes actually read
    assert_string_equal(databuf, EXPECTED_TUPLE);          // databuf has expected data
    assert_int_equal(context->current_fragment, fragment); // fragment is not exhausted

    / clean up /
    pfree(fragment);
    pfree(databuf);
     */
}

int
main(int argc, char* argv[])
{
    cmockery_parse_arguments(argc, argv);

    const UnitTest tests[] = {
            unit_test(test_gpbridge_cleanup),
            unit_test(test_gpbridge_import_start),
            unit_test(test_gpbridge_read)
    };

    MemoryContextInit();

    return run_tests(tests);
}