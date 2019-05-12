#include "CivetServer.h"
#include <nlohmann/json.hpp>

bool error(CivetServer *server, struct mg_connection *conn, unsigned int code, char const *description, char const *message);

bool success(CivetServer *server, struct mg_connection *conn, unsigned int code, char const *description, nlohmann::json body = NULL);

int get_request_body(struct mg_connection *conn, char *post_data, long long length);
