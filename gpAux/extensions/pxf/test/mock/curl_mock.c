CURL_EXTERN CURLcode
curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  check_expected(curl);
  check_expected(option);
  optional_assignment(curl);
  return (CURLcode) mock();
}

CURL_EXTERN CURL*
curl_easy_init(void)
{
  return (CURL*) mock()
}
