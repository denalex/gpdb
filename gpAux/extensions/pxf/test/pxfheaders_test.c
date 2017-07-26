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

/*
void build_http_headers(PxfInputData *input)
{

    churl_headers_append(headers, "X-GP-SEGMENT-ID", ev.GP_SEGMENT_ID);
    churl_headers_append(headers, "X-GP-SEGMENT-COUNT", ev.GP_SEGMENT_COUNT);
    churl_headers_append(headers, "X-GP-XID", ev.GP_XID);

    add_alignment_size_httpheader(headers);

    churl_headers_append(headers, "X-GP-URL-HOST", gphduri->host);
    churl_headers_append(headers, "X-GP-URL-PORT", gphduri->port);
    churl_headers_append(headers, "X-GP-DATA-DIR", gphduri->data);

    add_location_options_httpheader(headers, gphduri);

    churl_headers_append(headers, "X-GP-URI", gphduri->uri);

    churl_headers_append(headers, "X-GP-HAS-FILTER", "0");

}
*/

void
test_build_http_headers(void **state) {
    //extvar_t ev;
    PxfInputData *input = (PxfInputData *) palloc0(sizeof(PxfInputData));
    input->headers = (CHURL_HEADERS) palloc0(sizeof(CHURL_HEADERS));
    input->gphduri = (GPHDUri *) palloc0(sizeof(GPHDUri));
    input->gphduri->uri = "uri";
    input->rel = (Relation) palloc0(sizeof(RelationData));
    ExtTableEntry ext_tbl;
    ext_tbl.fmtcode = 'c';
    input->rel->rd_id = 56;
    struct tupleDesc tuple;
    tuple.natts = 0;
    input->rel->rd_att = &tuple;

    expect_value(GetExtTableEntry, relid, input->rel->rd_id);
    will_return(GetExtTableEntry, &ext_tbl);
    expect_headers_append(input->headers, "X-GP-FORMAT", TextFormatName);
    expect_headers_append(input->headers, "X-GP-ATTRS", "0");

    expect_any(external_set_env_vars, extvar);
    expect_string(external_set_env_vars, uri, input->gphduri->uri);
    expect_value(external_set_env_vars, csv, false);
    expect_value(external_set_env_vars, escape, NULL);
    expect_value(external_set_env_vars, quote, NULL);
    expect_value(external_set_env_vars, header, false);
    expect_value(external_set_env_vars, scancounter, 0);

    struct extvar_t mock_extvar;
    mock_extvar.GP_SEGMENT_ID = "segID";
    mock_extvar.GP_SEGMENT_COUNT = "10";
    mock_extvar.GP_XID = "20";

    will_assign_memory(external_set_env_vars, extvar, &mock_extvar, sizeof(extvar_t));
    will_be_called(external_set_env_vars);


    pfree(input->gphduri);
    pfree(input->headers);
    pfree(input);
}

static void
expect_headers_append(CHURL_HEADERS headers_handle, const char* header_key, const char* header_value)
{
    expect_value(churl_headers_append, headers, headers_handle);
    expect_string(churl_headers_append, key, header_key);
    expect_string(churl_headers_append, value, header_value);
    will_be_called(churl_headers_append);
}

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

static void
setup_external_vars()
{
    extvar_t *mock_extvar = palloc0(sizeof(extvar_t));

    snprintf(mock_extvar->GP_SEGMENT_ID, sizeof(mock_extvar->GP_SEGMENT_ID), "badID");
    snprintf(mock_extvar->GP_SEGMENT_COUNT, sizeof(mock_extvar->GP_SEGMENT_COUNT), "lots");
    snprintf(mock_extvar->GP_XID, sizeof(mock_extvar->GP_XID), "badXID");
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
