#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"
#include <unistd.h>

/* Define UNIT_TESTING so that the extension can skip declaring PG_MODULE_MAGIC */
#define UNIT_TESTING

/* include unit under test */
#include "../src/libchurl.c"

// TODO: delete this, it's just for debugging
#include "../src/pxfbridge.h"

/* include mock files */
#include "mock/pxfutils_mock.c"
#include "mock/curl_mock.c"

const char* uri_param = "pxf://localhost:51200/tmp/dummy1";

void
test_set_curl_option(void **state)
{
  // set up context with a curl_handle
  churl_context* context = palloc0(sizeof(churl_context));
  context->curl_handle = (CURL*)palloc0(sizeof(CURL));

  // set mock behavior for get_loopback_ip_addr
  will_return(get_loopback_ip_addr, pstrdup("127.0.0.1"));

  // set mock behavior for replace_string
  expect_string(replace_string, string, uri_param);
  expect_string(replace_string, replace, "localhost");
  expect_string(replace_string, replacement, "127.0.0.1");
  will_return(replace_string, pstrdup("pxf://127.0.0.1:51200/tmp/dummy1"));

  // set mock behavior for curl_easy_setopt
  expect_value(curl_easy_setopt, curl, context->curl_handle);
  expect_value(curl_easy_setopt, option, CURLOPT_URL);
  will_return(curl_easy_setopt, CURLE_OK);

  set_curl_option(context, CURLOPT_URL, uri_param);

  // cleanup
  pfree(context->curl_handle);
  pfree(context);
}


void
test_churl_init_upload(void **state)
{
  CHURL_HEADERS headers = palloc0(sizeof(CHURL_HEADERS));

  // set mock behavior for curl handle initialization
  will_return(curl_easy_init, palloc0(sizeof(CURL)));


  /* set mock behavior for all the set_curl_option calls */

  const CURLoption curl_options[] = {CURLOPT_URL,
                                     CURLOPT_VERBOSE,
                                     CURLOPT_ERRORBUFFER,
                                     CURLOPT_IPRESOLVE,
                                     CURLOPT_WRITEFUNCTION,
                                     CURLOPT_WRITEDATA,
                                     CURLOPT_HEADERDATA,
                                     CURLOPT_HTTPHEADER,
                                     CURLOPT_POST,
                                     CURLOPT_READFUNCTION,
                                     CURLOPT_READDATA}

  // extra mocks for the first set_curl_option call

  // set mock behavior for get_loopback_ip_addr
  will_return(get_loopback_ip_addr, pstrdup("127.0.0.1"));

  // set mock behavior for replace_string
  expect_string(replace_string, string, uri_param);
  expect_string(replace_string, replace, "localhost");
  expect_string(replace_string, replacement, "127.0.0.1");
  will_return(replace_string, pstrdup("pxf://127.0.0.1:51200/tmp/dummy1"));

  // set mock behavior for all the curl_easy_setopt
  for (int i = 0; i < sizeof(curl_options)/sizeof(curl_options[0]); i++) {
    expect_value(curl_easy_setopt, curl, context->curl_handle);
    expect_value(curl_easy_setopt, option, curl_options[i]);
    will_return(curl_easy_setopt, CURLE_OK);
  }


  /* set mock behavior for churl_headers_append */




  CHURL_HANDLE context = churl_init_upload(uri_param, headers);


}

void
test_churl_init_download(void **state)
{

}

void
test_churl_read(void **state)
{

}



int
main(int argc, char* argv[])
{
    cmockery_parse_arguments(argc, argv);

    const UnitTest tests[] = {
            unit_test(test_set_curl_option),
            unit_test(test_churl_init_upload),
            unit_test(test_churl_init_download),
            unit_test(test_churl_read)
    };

    MemoryContextInit();

    return run_tests(tests);
}
