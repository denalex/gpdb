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
int gpbridge_read(gphadoop_context *context, char *databuf, int datalen);
*/

static void expect_set_headers_call(CHURL_HEADERS headers_handle, const char* header_key, const char* header_value);

static const char* AUTHORITY = "127.0.0.1";

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
    /* init data in context that will be cleaned up */
    gphadoop_context *context = (gphadoop_context *) palloc0(sizeof(gphadoop_context));
    initStringInfo(&context->uri);

    /* setup list of fragments */
    FragmentData *fragment = (FragmentData *) palloc0(sizeof(FragmentData));
    fragment->authority = AUTHORITY;
    fragment->fragment_md = "md";
    fragment->index = "1";
    fragment->profile = NULL;
    fragment->source_name = "source";
    fragment->user_data = "user_data";

    context->gphd_uri = (GPHDUri *) palloc0(sizeof(GPHDUri));
    context->gphd_uri->fragments = lappend(NIL, fragment);
    context->gphd_uri->profile = "profile";

    CHURL_HEADERS headers = (CHURL_HEADERS) palloc0(sizeof(CHURL_HEADERS));
    will_return(churl_headers_init, headers);

    expect_any(build_http_headers, input); // might verify params later
    will_be_called(build_http_headers);

    expect_set_headers_call(headers, "X-GP-DATA-DIR", fragment->source_name);
    expect_set_headers_call(headers, "X-GP-DATA-FRAGMENT", fragment->index);
    expect_set_headers_call(headers, "X-GP-FRAGMENT-METADATA", fragment->fragment_md);
    expect_set_headers_call(headers, "X-GP-FRAGMENT-INDEX", fragment->index);
    expect_set_headers_call(headers, "X-GP-FRAGMENT-USER-DATA", fragment->user_data);
    expect_set_headers_call(headers, "X-GP-PROFILE", context->gphd_uri->profile);

    CHURL_HANDLE handle = (CHURL_HANDLE) palloc0(sizeof(CHURL_HANDLE));
    expect_value(churl_init_download, url, context->uri.data);
    expect_value(churl_init_download, headers, headers);
    will_return(churl_init_download, handle);

    expect_value(churl_read_check_connectivity, handle, handle);
    will_be_called(churl_read_check_connectivity);

    /* call function under test */
    gpbridge_import_start(context);

    /* assert call results */
    assert_int_equal(context->current_fragment, list_head(context->gphd_uri->fragments));

    StringInfoData expected_uri;
    initStringInfo(&expected_uri);
    appendStringInfo(&expected_uri,
                     "http://%s/%s/%s/Bridge/",
                     AUTHORITY, PXF_SERVICE_PREFIX, PXF_VERSION);
    assert_string_equal(context->uri.data, expected_uri.data);
    assert_int_equal(context->churl_headers, headers);
    assert_int_equal(context->churl_handle, handle);

    /* cleanup */
    pfree(handle);
    pfree(headers);
    pfree(fragment);
    pfree(context->gphd_uri);
    pfree(context);
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

static void expect_set_headers_call(CHURL_HEADERS headers_handle, const char* header_key, const char* header_value) {
    expect_string(churl_headers_override, headers, headers_handle);
    expect_string(churl_headers_override, key, header_key);
    expect_string(churl_headers_override, value, header_value);
    will_be_called(churl_headers_override);
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