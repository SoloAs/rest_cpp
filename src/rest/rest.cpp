/* Copyright (c) 2013-2018 the Civetweb developers
 * Copyright (c) 2013 No Face Press, LLC
 * License http://opensource.org/licenses/mit-license.php MIT License
 */

// Simple example program on how to use Embedded C++ interface.

#include "CivetServer.h"
#include <cstring>
#include <nlohmann/json.hpp>
#include <iostream>
#include "ll_api_class.h"
#include "ll_typedefs.h"
#include "rest_typedefs.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define DEBUG

#define DOCUMENT_ROOT "."
#define PORT "8081"
#define EXAMPLE_URI "/example"
#define EXIT_URI "/exit"

#define VERSION "/v1.0"
#define BASE_URI VERSION "/"
#define STATUS_URI VERSION "/status" 
#define INIT_URI VERSION "/init"
#define RESET_URI VERSION "/reset"
#define CONFIG_URI VERSION "/config"
#define STREAMS_URI VERSION "/streams"

using json = nlohmann::json;



config *current_config = new config();
config default_config = {"RX", "Macnica", "192.168.59.11", "8.8.8.8", false, false};


/* Exit flag for main loop */
volatile bool exitNow = false;

class BaseHandler : public CivetHandler
{
	public:

		bool handleGet(CivetServer *server, struct mg_connection *conn)
		{
			auto base = R"(
			[
			    "/status",
			    "/config",
			    "/streams"
			]
			)"_json;

			#ifdef DEBUG
			std::cout << base.dump(4) << std::endl;
			#endif

			return success(server, conn, 200, "OK", base);

		}
};

class StatusHandler : public CivetHandler
{
	public:
		bool handleGet(CivetServer *server, struct mg_connection *conn)
		{
			
			// const mg_request_info *params = mg_get_request_info(conn);
			// const char *data = params->local_uri;
			// std::cout << data << std::endl;



			ll_api_class *ll_api = ll_api_class::get_instance();
			unsigned int check_result;

			LLError result = ll_api->check_modules_version(&check_result); // 0 - compatible, 1 - not compatible
			if (result != LLError::LL_NO_ERROR || !check_result != 0) 
			{
				unsigned int firmware_version;
				unsigned int software_version;
		
				result = ll_api->get_version(&firmware_version, &software_version);
				if (result == LLError::LL_NO_ERROR) 
				{
					json versions = {
						{"firmware_version", 1},
						{"software_version", 1}
					};
					
					return success(server, conn, 200, "OK", versions);

				} else
				{
					return error(server, conn, 500, "Internal Server Error", "Hardware Failure");
				}
				
			} else
			{
				if (result != LLError::LL_NO_ERROR)
				{
					return error(server, conn, 500, "Internal Server Error", "Hardware Failure");

				} else
				{
					return error(server, conn, 409, "Conflict", "Software and firmware modules aren't compatible");
				}
				
			}




		}
};

class InitHandler : public CivetHandler
{
	public:

		bool handlePost(CivetServer *server, struct mg_connection *conn)
		{
			ll_api_class *ll_api = ll_api_class::get_instance();
			LLError result = ll_api->init();

			if (result == LLError::LL_NO_ERROR)
			{
				return success(server, conn, 200, "OK");

			} else 
			{
				return error(server, conn, 500, "Internal Server Error", "Hardware Failure");
			}
		}
};

class ResetHandler : public CivetHandler
{
	public:

		bool handlePost(CivetServer *server, struct mg_connection *conn)
		{
			ll_api_class *ll_api = ll_api_class::get_instance();
			LLError result = ll_api->init();

			if (result == LLError::LL_NO_ERROR)
			{
				return success(server, conn, 200, "OK");

			} else 
			{
				return error(server, conn, 500, "Internal Server Error", "Hardware Failure");
			}
		}

};

class ConfigHandler : public CivetHandler
{
	public:
	
		bool handleGet(CivetServer *server, struct mg_connection *conn)
		{
			json json_config = {
				{"mode", current_config->mode},
				{"company", current_config->company},
				{"ip", current_config->ip},
				{"dns", current_config->dns},
				{"dhcp", current_config->dhcp},
				{"igmp", current_config->igmp}
			};

			return success(server, conn, 200, "OK", json_config);
		};

		bool handlePost(CivetServer *server, struct mg_connection *conn)
		{
			const mg_request_info *ri = mg_get_request_info(conn);
			char *post_data_c = new char[ri->content_length + 1]; // one byte for null symbol
			int result = get_request_body(conn, post_data_c, ri->content_length);
			std::string post_data(post_data_c); 

		
			if (result > 0) 
			{
				json j = json::parse(post_data);
				
				current_config->mode = j["mode"];
				current_config->company = j["company"];
				current_config->ip = j["ip"];
				current_config->dns = j["dns"];
				current_config->dhcp = j["dhcp"];
				current_config->igmp = j["igmp"];

				
				// TODO: handle params change


			} else 
			{
				return error(server, conn, 400, "Bad Request", "Incorrect or missing mandatory attributes");
			}

			delete[] post_data_c;
			
			return success(server, conn, 201, "Created");


		};

		bool handlePatch(CivetServer *server, struct mg_connection *conn)
		{
			const mg_request_info *ri = mg_get_request_info(conn);
			char *patch_data_c = new char[ri->content_length + 1]; // one byte for null symbol
			int result = get_request_body(conn, patch_data_c, ri->content_length);
			std::string patch_data(patch_data_c); 

			if (result > 0) 
			{
				json j = json::parse(patch_data);
				
				// TODO: handle params change
				for (json::iterator it = j.begin(); it != j.end(); ++it) {
					if (it.key() == "company") 
					{
						current_config->company = j["company"];
					}
					if (it.key() == "dhcp") 
					{
						current_config->dhcp = j["dhcp"];
					}
					if (it.key() == "dns") 
					{
						current_config->dns = j["dns"];
					}
					if (it.key() == "igmp") 
					{
						current_config->igmp = j["igmp"];
					}
					if (it.key() == "ip") 
					{
						current_config->ip = j["ip"];
					}
					if (it.key() == "mode") 
					{
						current_config->mode = j["mode"];
					}
				}
			} else
			{
				return error(server, conn, 400, "Bad Request", "Incorrect or missing mandatory attributes");
			}

			json json_config = {
				{"mode", current_config->mode},
				{"company", current_config->company},
				{"ip", current_config->ip},
				{"dns", current_config->dns},
				{"dhcp", current_config->dhcp},
				{"igmp", current_config->igmp}
			};

			return success(server, conn, 200, "OK", json_config);
			
		};

		bool handleDelete(CivetServer *server, struct mg_connection *conn)
		{
			bool config_is_default = false;

			if (current_config->mode == default_config.mode &&
				current_config->company == default_config.company &&
				current_config->ip == default_config.ip &&
				current_config->dns == default_config.dns &&
				current_config->dhcp == default_config.dhcp &&
				current_config->igmp == default_config.igmp) 
			{
				config_is_default = true;
			}

			if (!config_is_default) 
			{
				*current_config = default_config;
				
				json json_config = {
					{"mode", current_config->mode},
					{"company", current_config->company},
					{"ip", current_config->ip},
					{"dns", current_config->dns},
					{"dhcp", current_config->dhcp},
					{"igmp", current_config->igmp}
				};

				return success(server, conn, 200, "OK", json_config);
			} else
			{
				return error(server, conn, 404, "Not Found", "No active configuration found");
			}			
		}


};

class StreamsHandler : public CivetHandler
{
	bool handleGet(CivetServer *server, struct mg_connection *conn)
	{

	}

};


class ExitHandler : public CivetHandler
{
  public:
	bool
	handleGet(CivetServer *server, struct mg_connection *conn)
	{
		mg_printf(conn,
		          "HTTP/1.1 200 OK\r\nContent-Type: "
		          "text/plain\r\nConnection: close\r\n\r\n");
		mg_printf(conn, "Bye!\n");
		exitNow = true;
		return true;
	}
};


int main(int argc, char *argv[])
{
	const char *options[] = {
	    "document_root", DOCUMENT_ROOT, "listening_ports", PORT, 0};
    
    std::vector<std::string> cpp_options;
    for (int i=0; i<(sizeof(options)/sizeof(options[0])-1); i++) {
        cpp_options.push_back(options[i]);
    }

	

	// CivetServer server(options); // <-- C style start
	CivetServer server(cpp_options); // <-- C++ style start
	*current_config = default_config;

	BaseHandler h_base;
	server.addHandler(BASE_URI, h_base);

	InitHandler h_init;
	server.addHandler(INIT_URI, h_init);

	ResetHandler h_reset;
	server.addHandler(RESET_URI, h_reset);

	StatusHandler h_status;
	server.addHandler(STATUS_URI, h_status);

	ExitHandler h_exit;
	server.addHandler(EXIT_URI, h_exit);
	
	ConfigHandler h_config;
	server.addHandler(CONFIG_URI, h_config);

	StreamsHandler h_streams;
	server.addHandler(STREAMS_URI, h_streams);


	printf("Run server at http://localhost:%s%s\n", PORT, EXAMPLE_URI);
	printf("Exit at http://localhost:%s%s\n", PORT, EXIT_URI);
	printf("exit uri %s\n", EXIT_URI);

	while (!exitNow) {
		sleep(1);
	}

	printf("Bye!\n");
	delete current_config;

	return 0;
}