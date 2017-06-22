#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

/* Define UNIT_TESTING so that the extension can skip declaring PG_MODULE_MAGIC */
#define UNIT_TESTING

/* include unit under test */
#include "../src/pxfprotocol.c"

void
test_pxfprotocol_validate_urls(void **state)
{
    Datum d = pxfprotocol_validate_urls(NULL);
    assert_int_equal(DatumGetInt32(d), 0);
}

void
test_pxfprotocol_export(void **state)
{
    Datum d = pxfprotocol_export(NULL);
    assert_int_equal(DatumGetInt32(d), 0);
}

void
test_pxfprotocol_import_first_call(void **state)
{
    /* setup call info with no call context */
    PG_FUNCTION_ARGS = palloc(sizeof(FunctionCallInfoData));
    fcinfo->context = palloc(sizeof(ExtProtocolData));
    fcinfo->context->type = T_ExtProtocolData;
    EXTPROTOCOL_GET_DATALEN(fcinfo) = 100;
    EXTPROTOCOL_GET_DATABUF(fcinfo) = palloc0(EXTPROTOCOL_GET_DATALEN(fcinfo));
    ((ExtProtocolData*) fcinfo->context)->prot_last_call = false;

    /* set mock behavior for bridge */
    const int EXPECTED_SIZE = 31; // expected number of bytes read from the bridge
    expect_any(gpbridge_read, context);
    expect_value(gpbridge_read, databuf, EXTPROTOCOL_GET_DATABUF(fcinfo));
    expect_value(gpbridge_read, datalen, EXTPROTOCOL_GET_DATALEN(fcinfo));
    will_return(gpbridge_read, EXPECTED_SIZE);

    Datum d = pxfprotocol_import(fcinfo);

    assert_int_equal(DatumGetInt32(d), EXPECTED_SIZE);     // return number of bytes read from the bridge
    assert_true(EXTPROTOCOL_GET_USER_CTX(fcinfo) != NULL); // context has been created

    /* cleanup */
    pfree(EXTPROTOCOL_GET_USER_CTX(fcinfo));
    pfree(EXTPROTOCOL_GET_DATABUF(fcinfo));
    pfree(fcinfo->context);
    pfree(fcinfo);
}

void
test_pxfprotocol_import_second_call(void **state)
{
    /* setup call info with call context */
    PG_FUNCTION_ARGS = palloc(sizeof(FunctionCallInfoData));
    fcinfo->context = palloc(sizeof(ExtProtocolData));
    fcinfo->context->type = T_ExtProtocolData;
    EXTPROTOCOL_GET_DATALEN(fcinfo) = 100;
    EXTPROTOCOL_GET_DATABUF(fcinfo) = palloc0(EXTPROTOCOL_GET_DATALEN(fcinfo));
    ((ExtProtocolData*) fcinfo->context)->prot_last_call = false;
    gphadoop_context *call_context = palloc0(sizeof(gphadoop_context));
    EXTPROTOCOL_SET_USER_CTX(fcinfo, call_context);

    /* set mock behavior for bridge */
    const int EXPECTED_SIZE = 0; // expected number of bytes read from the bridge
    expect_value(gpbridge_read, context, call_context);
    expect_value(gpbridge_read, databuf, EXTPROTOCOL_GET_DATABUF(fcinfo));
    expect_value(gpbridge_read, datalen, EXTPROTOCOL_GET_DATALEN(fcinfo));
    will_return(gpbridge_read, EXPECTED_SIZE);

    Datum d = pxfprotocol_import(fcinfo);

    assert_int_equal(DatumGetInt32(d), EXPECTED_SIZE);             // return number of bytes read from the bridge
    assert_true(EXTPROTOCOL_GET_USER_CTX(fcinfo) == call_context); // context is still the same

    /* cleanup */
    pfree(call_context);
    pfree(EXTPROTOCOL_GET_DATABUF(fcinfo));
    pfree(fcinfo->context);
    pfree(fcinfo);
}

void
test_pxfprotocol_import_last_call(void **state)
{
    /* setup call info with a call context and last call indicator */
    PG_FUNCTION_ARGS = palloc(sizeof(FunctionCallInfoData));
    fcinfo->context = palloc(sizeof(ExtProtocolData));
    fcinfo->context->type = T_ExtProtocolData;
    gphadoop_context *call_context = palloc(sizeof(gphadoop_context));
    EXTPROTOCOL_SET_USER_CTX(fcinfo, call_context);
    EXTPROTOCOL_SET_LAST_CALL(fcinfo);

    Datum d = pxfprotocol_import(fcinfo);

    assert_int_equal(DatumGetInt32(d), 0);                 // 0 is returned from function
    assert_true(EXTPROTOCOL_GET_USER_CTX(fcinfo) == NULL); // call context is cleaned up

    /* cleanup */
    pfree(fcinfo->context);
    pfree(fcinfo);
}

/* mock functions for pxfbridge.h */
void gpbridge_cleanup(gphadoop_context *context) {
}

void gpbridge_import_start(gphadoop_context *context) {
}

int gpbridge_read(gphadoop_context *context, char *databuf, int datalen) {
    check_expected(context);
    check_expected(databuf);
    check_expected(datalen);
    return (int) mock();
}

int
main(int argc, char* argv[])
{
    cmockery_parse_arguments(argc, argv);

    const UnitTest tests[] = {
            unit_test(test_pxfprotocol_validate_urls),
            unit_test(test_pxfprotocol_export),
            unit_test(test_pxfprotocol_import_first_call),
            unit_test(test_pxfprotocol_import_second_call),
            unit_test(test_pxfprotocol_import_last_call)
    };

    MemoryContextInit();

    return run_tests(tests);
}