<<<<<<< HEAD
bool are_ips_equal(char *ip1, char *ip2) {
    check_expected(ip1);
    check_expected(ip2);
    return (bool) mock();
}

void port_to_str(char** port, int new_port) {
    check_expected(port);
    check_expected(new_port);
    mock();
}

void call_rest(GPHDUri *hadoop_uri, ClientContext *client_context, char* rest_msg) {
    check_expected(hadoop_uri);
    check_expected(client_context);
    check_expected(rest_msg);
    mock();
}

char* get_loopback_ip_addr(void) {
    return (char*) mock();
}

char* replace_string(const char* string, const char* replace, const char* replacement) {
    check_expected(string);
    check_expected(replace);
    check_expected(replacement);
    return (char*) mock();
}

char* normalize_key_name(const char* key) {
    check_expected(key);
    return (char*) mock();
}

char* TypeOidGetTypename(Oid typid) {
    check_expected(typid);
    return (char*) mock();
}
=======

bool
are_ips_equal(char * ip1, char * ip2)
{
	check_expected(ip1);
	check_expected(ip2);
	optional_assignment(ip1);
	optional_assignment(ip2);
	return (bool) mock();
}


void
port_to_str(char ** port, int new_port)
{
	check_expected(port);
	check_expected(new_port);
	optional_assignment(port);
	mock();
}


void
call_rest(GPHDUri * hadoop_uri, ClientContext * client_context, char * rest_msg)
{
	check_expected(hadoop_uri);
	check_expected(client_context);
	check_expected(rest_msg);
	optional_assignment(hadoop_uri);
	optional_assignment(client_context);
	optional_assignment(rest_msg);
	mock();
}


static void
process_request(ClientContext* client_context, char * uri)
{
	mock();
}


char*
get_loopback_ip_addr(void)
{
	return (char*) mock();
}


char*
replace_string(const char* string, const char* replace, const char* replacement)
{
	check_expected(string);
	check_expected(replace);
	check_expected(replacement);
	optional_assignment(string);
	optional_assignment(replace);
	optional_assignment(replacement);
	return (char*) mock();
}


char*
TypeOidGetTypename(Oid typid)
{
	check_expected(typid);
	return (char*) mock();
}
>>>>>>> first unit test done
