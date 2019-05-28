#include "CivetServer.h"
#include <cstring>
#include <nlohmann/json.hpp>
#include <iostream>
#include "ll_api_class.h"
#include "ll_typedefs.h"
#include "rest_typedefs.h"

using json = nlohmann::json;


bool error(CivetServer *server, struct mg_connection *conn, unsigned int code, char const *description, char const *message)
{	
	json error = {
					{"error", {
						{"status", code},
						{"description", description},
						{"message", message}
					}}
				 };
	std::string error_string = error.dump();

	mg_printf(conn,
		"HTTP/1.1 %u %s\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: "
		"application/json\r\nConnection: close\r\n\r\n", code, description);
	mg_printf(conn, error_string.c_str());

	return true;
}

bool success(CivetServer *server, struct mg_connection *conn, unsigned int code, char const *description, json body = NULL)
{
	std::string body_string = body.dump();
	mg_printf(conn,
		"HTTP/1.1 %u %s\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: "
		"application/json\r\nConnection: close\r\n\r\n", code, description);
	if (body != NULL) 
		mg_printf(conn, body_string.c_str());
	else 
		mg_printf(conn, "{}");

	return true;
}

int get_request_body(struct mg_connection *conn, char *post_data, long long length)
{
	post_data[length] = 0;						// null at the end of the string;
	return mg_read(conn, post_data, length);
}

std::string retrieve_id(const struct mg_request_info *params)
{
    std::string data = params->local_uri;
    if (data[data.size()-1] == '/')
        data.pop_back();
    std::string stream_id = data.substr(data.rfind("/") + 1, data.size());
    return stream_id;
}