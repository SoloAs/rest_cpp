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

init_params *hw_config = new init_params();

// tx_stream_info tx_streams[8];
// rx_stream_info rx_streams[8];

std::vector<tx_stream_info> tx_streams;
std::vector<rx_stream_info> rx_streams;


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

			// const mg_request_info *ri = mg_get_request_info(conn);
			// char *post_data_c = new char[ri->content_length + 1]; // one byte for null symbol
			// int request_result = get_request_body(conn, post_data_c, ri->content_length);
			// std::string post_data(post_data_c); 

		
			// if (request_result > 0) 
			// {
			// 	json j = json::parse(post_data);
				
			// 	hw_config->init_hw_clk_rate = j["clockrate"];
			// 	hw_config->tx_port = j["tx_port"];
			// 	hw_config->rx_port = j["rx_port"];
			// 	hw_config->dev_number = j["dev_number"];
			// 	current_config->dhcp = j["dhcp"];
			// 	current_config->igmp = j["igmp"];

			// } TODO: extend when real init comes down



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
				
				
				// current_config->mode = ((std::string)j["mode"]).c_str();
				strcpy(current_config->mode, ((std::string)j["mode"]).c_str());
				// current_config->company = j["company"];
				strcpy(current_config->company, ((std::string)j["company"]).c_str());
				// current_config->ip = j["ip"];
				strcpy(current_config->ip, ((std::string)j["ip"]).c_str());
				//current_config->dns = j["dns"];
				strcpy(current_config->dns, ((std::string)j["dns"]).c_str());
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
						// current_config->company = j["company"];
						strcpy(current_config->company, ((std::string)j["company"]).c_str());
					}
					if (it.key() == "dhcp") 
					{
						current_config->dhcp = j["dhcp"];
					}
					if (it.key() == "dns") 
					{
						// current_config->dns = j["dns"];
						strcpy(current_config->dns, ((std::string)j["dns"]).c_str());
					}
					if (it.key() == "igmp") 
					{
						current_config->igmp = j["igmp"];
					}
					if (it.key() == "ip") 
					{
						// current_config->ip = j["ip"];
						strcpy(current_config->ip, ((std::string)j["ip"]).c_str());
					}
					if (it.key() == "mode") 
					{
						// current_config->mode = j["mode"];
						strcpy(current_config->mode, ((std::string)j["mode"]).c_str());
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
		const mg_request_info *ri = mg_get_request_info(conn);
		std::string stream_id = retrieve_id(ri);

		if (stream_id == "preview")
		{
			return success(server, conn, 200, "OK"); // TODO: finish preview acquiring
		}
		if (stream_id != "streams") // call for single stream (/streams/:id)
		{
			if (strcmp(current_config->mode, "TX") == 0) 
			{
				for (uint i = 0; i < tx_streams.size(); i++)
				{
					if (strcmp(tx_streams[i].id, stream_id.c_str()) == 0)
					{
						json stream_json = {
							{"source_port",  std::to_string(tx_streams[i].udp_sp)},
							{"destination_port",  std::to_string(tx_streams[i].udp_dp)},
							{"ttl",  std::to_string(tx_streams[i].ttl)},
							{"smpte_type",  std::to_string(tx_streams[i].smpte_type)},
							{"video_format",  std::to_string(tx_streams[i].video_pt)},
							{"media_channel",  std::to_string(tx_streams[i].media_chan)},
							{"vlan",  std::to_string(tx_streams[i].vlan)},
							{"audio_format",  std::to_string(tx_streams[i].audio_pt)},
							{"destination_address", tx_streams[i].ip_da},
							{"id", tx_streams[i].id}
						};
						return success(server, conn, 200, "OK", stream_json);
					}
				}
				return error(server, conn, 404, "Not Found", "Not Found");
			} else 
			{
				for (uint i = 0; i < rx_streams.size(); i++)
				{
					if (strcmp(rx_streams[i].id, stream_id.c_str()) == 0)
					{
						json stream_json = {
							{"playout_delay", std::to_string(rx_streams[i].playout_delay)},
							{"destination_port", std::to_string(rx_streams[i].udp_dp)},
							{"frame_rate", std::to_string(rx_streams[i].frame_rate)},
							{"smpte_type", std::to_string(rx_streams[i].smpte_type)},
							{"media_channel",std::to_string(rx_streams[i].media_chan)},
							{"audio_channel", std::to_string(rx_streams[i].aud_chan)},
							{"video_format", rx_streams[i].vid_format},
							{"destination_address", rx_streams[i].ip_da},
							{"id", rx_streams[i].id}
						};
						return success(server, conn, 200, "OK", stream_json);
					}
				}
				return error(server, conn, 404, "Not Found", "Not Found");
			}
		} else // call for multiple streams (/streams)
		{
			auto streams_json = json::array();
			// TODO: implement return multiple streams
			if (strcmp(current_config->mode, "TX") == 0)
			{
				for (int i = 0; i < tx_streams.size(); i++)
				{
					json stream_json = {
						{"source_port",  std::to_string(tx_streams[i].udp_sp)},
						{"destination_port",  std::to_string(tx_streams[i].udp_dp)},
						{"ttl",  std::to_string(tx_streams[i].ttl)},
						{"smpte_type",  std::to_string(tx_streams[i].smpte_type)},
						{"video_format",  std::to_string(tx_streams[i].video_pt)},
						{"media_channel",  std::to_string(tx_streams[i].media_chan)},
						{"vlan",  std::to_string(tx_streams[i].vlan)},
						{"audio_format",  std::to_string(tx_streams[i].audio_pt)},
						{"destination_address", tx_streams[i].ip_da},
						{"id", tx_streams[i].id}
					};
					streams_json.push_back(stream_json);
				}
			} else
			{
				for (uint i = 0; i < rx_streams.size(); i++)
				{
					json stream_json = {
							{"playout_delay", std::to_string(rx_streams[i].playout_delay)},
							{"destination_port", std::to_string(rx_streams[i].udp_dp)},
							{"frame_rate", std::to_string(rx_streams[i].frame_rate)},
							{"smpte_type", std::to_string(rx_streams[i].smpte_type)},
							{"media_channel",std::to_string(rx_streams[i].media_chan)},
							{"audio_channel", std::to_string(rx_streams[i].aud_chan)},
							{"video_format", rx_streams[i].vid_format},
							{"destination_address", rx_streams[i].ip_da},
							{"id", rx_streams[i].id}
						};
					streams_json.push_back(stream_json);
				}
			}
			return success(server, conn, 200, "OK", streams_json);
		}
	}

		
		
	

	bool handlePut(CivetServer *server, struct mg_connection *conn)
	{
		ll_api_class *ll_api = ll_api_class::get_instance();
		const mg_request_info *ri = mg_get_request_info(conn);
		
		std::string stream_id = retrieve_id(ri);


		char *put_data_c = new char[ri->content_length + 1]; // one byte for null symbol
		int result = get_request_body(conn, put_data_c, ri->content_length);
		std::string put_data(put_data_c);

		json j = json::parse(put_data);

		if (strcmp(current_config->mode, "TX") == 0) 
		{
			if (j["frame_rate"].is_null()) // TODO: check if incoming flow is TX
			{
				if (result > 0) 
				{	
					tx_stream_info tx_stream;
					tx_stream.udp_sp = std::stoi((std::string)j["source_port"]);
					tx_stream.udp_dp = std::stoi((std::string)j["destination_port"]);
					tx_stream.ttl = std::stoi((std::string)j["ttl"]);
					tx_stream.smpte_type = std::stoi((std::string)j["smpte_type"]);
					tx_stream.video_pt = std::stoi((std::string)j["video_format"]);
					tx_stream.media_chan = std::stoi((std::string)j["media_channel"]);
					tx_stream.vlan = std::stoi((std::string)j["vlan"]);
					tx_stream.audio_pt = std::stoi((std::string)j["audio_format"]);
					
					strcpy(tx_stream.ip_da, ((std::string)j["destination_address"]).c_str());
					strcpy(tx_stream.id, stream_id.c_str());

					LLError result = ll_api->set_tx_stream(tx_stream);
					if (result == LLError::LL_NO_ERROR) 
					{
						tx_streams.push_back(tx_stream);
						return success(server, conn, 201, "Created");
					} else 
					{
						return error(server, conn, 500, "Internal Server Error", "Hardware failure");
					}
				} else 
				{
					return error(server, conn, 400, "Bad Request", "Incorrect or missing mandatory attributes");
				}
			} else
			{
				return error(server, conn, 400, "Bad Request", "Incorrect or missing mandatory attributes");
			}
		} else
		{
			if (!j["frame_rate"].is_null()) // TODO: check if incoming flow is RX
			{
				if (result > 0) 
				{	
					rx_stream_info rx_stream;
					rx_stream.udp_dp = std::stoi((std::string)j["destination_port"]);
					rx_stream.playout_delay = std::stoi((std::string)j["playout_delay"]);
					rx_stream.smpte_type = std::stoi((std::string)j["smpte_type"]);
					rx_stream.media_chan = std::stoi((std::string)j["media_channel"]);
					rx_stream.aud_chan = std::stoi((std::string)j["audio_channel"]);
					rx_stream.frame_rate = std::stoi((std::string)j["frame_rate"]);

					strcpy(rx_stream.vid_format, ((std::string)j["video_format"]).c_str());
					strcpy(rx_stream.ip_da, ((std::string)j["destination_address"]).c_str());
					strcpy(rx_stream.id, stream_id.c_str());
					LLError result = ll_api->set_rx_stream(rx_stream);
					if (result == LLError::LL_NO_ERROR) 
					{
						rx_streams.push_back(rx_stream);
						return success(server, conn, 201, "Created");
					} else 
					{
						return error(server, conn, 500, "Internal Server Error", "Hardware failure");
					}
				} else 
				{
					return error(server, conn, 400, "Bad Request", "Incorrect or missing mandatory attributes");
				}

			} else
			{
				return error(server, conn, 400, "Bad Request", "Incorrect or missing mandatory attributes");
			}
			
		}
	}

	bool handleDelete(CivetServer *server, struct mg_connection *conn)
	{
		ll_api_class *ll_api = ll_api_class::get_instance();
		const mg_request_info *ri = mg_get_request_info(conn);
		
		std::string stream_id = retrieve_id(ri);

		// TODO: implement delete stream


		LLError result = ll_api->stop_stream(const_cast<char*>(stream_id.c_str()));
		if (result == LLError::LL_NO_ERROR)
		{
			if (strcmp(current_config->mode, "TX") == 0) 
			{
				for (uint i = 0; i < tx_streams.size(); i++)
				{
					if (stream_id == tx_streams[i].id)
					{
						tx_streams.erase(tx_streams.begin() + i);
						return success(server, conn, 204, "Deleted");
					}
				}
			} else 
			{
				for (uint i = 0; i < rx_streams.size(); i++)
				{
					if (stream_id == rx_streams[i].id)
					{
						rx_streams.erase(rx_streams.begin() + i);
						return success(server, conn, 204, "Deleted");
					}
				}
			}		
		} else 
		{
			if (result == LLError::LL_WRONG_PARAM) // is it what LL API returns if stream is non-existent ??
			{
				return error(server, conn, 404, "Not Found", "Not Found"); 
			} else
			{
				return error(server, conn, 500, "Internal Server Error", "Hardware failure");
			}
		}
		
		
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
    for (uint i = 0; i < (sizeof(options)/sizeof(options[0])-1); i++) {
        cpp_options.push_back(options[i]);
    }

	// TODO: get call to LL API to receive all the flows information

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

	while (!exitNow) {
		sleep(1);
	}

	printf("Bye!\n");
	delete current_config;

	return 0;
}
