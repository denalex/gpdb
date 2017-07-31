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
// #include "mock/stringinfo_mock.c"

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
curl_easy_setopt_test_helper(CURL* curl_handle, CURLoption option)
{
  expect_value(curl_easy_setopt, curl, curl_handle);
  expect_value(curl_easy_setopt, option, option);
  will_return(curl_easy_setopt, CURLE_OK);
}

void
curl_slist_append_test_helper(char* header_string, struct curl_slist* slist)
{
  expect_any(curl_slist_append, list);
  expect_string(curl_slist_append, string, header_string);
  will_return(curl_slist_append, slist);
}

void
test_churl_init_upload(void **state)
{
  CHURL_HEADERS headers = palloc0(sizeof(CHURL_HEADERS));

  // set mock behavior for curl handle initialization
  CURL* mock_curl_handle = palloc0(sizeof(CURL));

  will_return(curl_easy_init, mock_curl_handle);


  /* set mock behavior for all the set_curl_option calls */

  // extra mocks for the first set_curl_option call

  // set mock behavior for get_loopback_ip_addr
  will_return(get_loopback_ip_addr, pstrdup("127.0.0.1"));

  // set mock behavior for replace_string
  expect_string(replace_string, string, uri_param);
  expect_string(replace_string, replace, "localhost");
  expect_string(replace_string, replacement, "127.0.0.1");
  will_return(replace_string, pstrdup("pxf://127.0.0.1:51200/tmp/dummy1"));

  // set mock behavior for all the curl_easy_setopt calls
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_URL);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_VERBOSE);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_ERRORBUFFER);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_IPRESOLVE);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_WRITEFUNCTION);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_WRITEDATA);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_HEADERFUNCTION);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_HEADERDATA);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_HTTPHEADER);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_POST);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_READFUNCTION);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_READDATA);

  struct curl_slist* mock_curl_slist = palloc0(sizeof(struct curl_slist));

  curl_slist_append_test_helper("Content-Type: application/octet-stream", mock_curl_slist);
  curl_slist_append_test_helper("Transfer-Encoding: chunked", mock_curl_slist);
  curl_slist_append_test_helper("Expect: 100-continue", mock_curl_slist);


  /* setup_multi_handle mock setup */

  CURLM* mock_multi_handle = palloc0(sizeof(CURLM));

  will_return(curl_multi_init, mock_multi_handle);

  expect_value(curl_multi_add_handle, multi_handle, mock_multi_handle);
  expect_value(curl_multi_add_handle, curl_handle, mock_curl_handle);
  will_return(curl_multi_add_handle, CURLM_OK);

  expect_value(curl_multi_perform, multi_handle, mock_multi_handle);
  expect_any(curl_multi_perform, running_handles);
  will_return(curl_multi_perform, CURLM_OK);


  CHURL_HANDLE handle = churl_init_upload(uri_param, headers);

  churl_context* context = (churl_context*)handle;
  assert_true(context->upload == true);
  assert_true(context->curl_error_buffer[0] == 0);
  assert_true(context->download_buffer != NULL);
  assert_true(context->upload_buffer != NULL);
  assert_true(context->curl_handle != NULL);
  assert_true(context->multi_handle != NULL);
  assert_true(context->last_http_reponse == NULL);
  assert_true(context->curl_still_running == 0);


  // TODO: ask about multiple runs
  // TODO: ask about expect_any

  /* tear down */
  pfree(headers);
  pfree(handle);
}

void
test_churl_init_download(void **state)
{
  CHURL_HEADERS headers = palloc0(sizeof(CHURL_HEADERS));

  // set mock behavior for curl handle initialization
  CURL* mock_curl_handle = palloc0(sizeof(CURL));
  will_return(curl_easy_init, mock_curl_handle);


  /* set mock behavior for all the set_curl_option calls */
  
  // extra mocks for the first set_curl_option call

  // set mock behavior for get_loopback_ip_addr
  will_return(get_loopback_ip_addr, pstrdup("127.0.0.1"));

  // set mock behavior for replace_string
  expect_string(replace_string, string, uri_param);
  expect_string(replace_string, replace, "localhost");
  expect_string(replace_string, replacement, "127.0.0.1");
  will_return(replace_string, pstrdup("pxf://127.0.0.1:51200/tmp/dummy1"));

  // set mock behavior for all the curl_easy_setopt calls
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_URL);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_VERBOSE);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_ERRORBUFFER);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_IPRESOLVE);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_WRITEFUNCTION);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_WRITEDATA);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_HEADERFUNCTION);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_HEADERDATA);
  curl_easy_setopt_test_helper(mock_curl_handle, CURLOPT_HTTPHEADER);

  /* setup_multi_handle mock setup */

  CURLM* mock_multi_handle = palloc0(sizeof(CURLM));

  will_return(curl_multi_init, mock_multi_handle);

  expect_value(curl_multi_add_handle, multi_handle, mock_multi_handle);
  expect_value(curl_multi_add_handle, curl_handle, mock_curl_handle);
  will_return(curl_multi_add_handle, CURLM_OK);

  expect_value(curl_multi_perform, multi_handle, mock_multi_handle);
  expect_any(curl_multi_perform, running_handles);
  will_return(curl_multi_perform, CURLM_OK);

  /* function call */

  CHURL_HANDLE handle = churl_init_download(uri_param, headers);

  churl_context* context = (churl_context*)handle;

  /* test assertions */
  assert_true(context->upload == false);
  assert_true(context->curl_error_buffer[0] == 0);
  assert_true(context->download_buffer != NULL);
  assert_true(context->upload_buffer != NULL);
  assert_true(context->curl_handle != NULL);
  assert_true(context->multi_handle != NULL);
  assert_true(context->last_http_reponse == NULL);

  /* tear down */
  pfree(headers);
  pfree(handle);
}

void
test_churl_read(void **state)
{
  // CHURL_HANDLE handle = palloc0(sizeof(CHURL_HANDLE));
  // churl_context* context = (churl_context*)handle;
  //
  // context->curl_still_running = 1;
  // context->download_buffer->top = 0;
  // context->download_buffer->bot = 0;
  //
  // // Do we need to test FD_ZERO or memcpy?
  // for (int i = 0; i < 3; i++) {
  //   expect_any(FD_ZERO, fdset);
  //   will_be_called(FD_ZERO);
  // }
  //
  // will_be_called(CHECK_FOR_INTERRUPTS);
  //
  // expect_value(curl_multi_fdset, multi_handle, context->multi_handle);
  // expect_any(curl_multi_fdset, read_fd_set);
  // expect_any(curl_multi_fdset, write_fd_set);
  // expect_any(curl_multi_fdset, exc_fd_set);
  // expect_any(curl_multi_fdset, max_fd);
  // will_return(curl_multi_fdset, CURLM_OK);
}

void
test_churl_write(void **state)
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
            unit_test(test_churl_read),
            unit_test(test_churl_write)
    };

    MemoryContextInit();

    return run_tests(tests);
}
