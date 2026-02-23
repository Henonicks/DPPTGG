/**************************************************************************
 * Copyright 2025 Ramskyi Roman
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
***************************************************************************/


#pragma once

#include <dpp/http_server.h>

#include "dpptgg/topgg_handler.hpp"
#include "dpptgg/exception.hpp"
#include "dpptgg/timestamp.hpp"

namespace dpptgg {

	/**
	 * @brief An HTTP request with the top.gg payload data easily accessible.
	 */
	struct topgg_request {

		// The type of the vote, either "vote_create" or "webhook_test".
		vote_types vote_type;

		// The ID of the vote itself.
		dpp::snowflake vote_id;

		// The weight of the vote, e.g. how many points it's worth. Set to 0 if not provided in the payload (vote_type == webhook_test).
		uint8_t vote_weight{};

		// Date-time of when the vote was made. Every value is set to 0 if not provided in the payload (vote_type == webhook_test).
		datetime created_at{};

		// Date-time of when the same user becomes able to vote again. Every value is set to 0 if not provided in the payload (vote_type == webhook_test).
		datetime expires_at{};

		// The ID of the project on top.gg (NOT Discord!).
		dpp::snowflake project_id;

		// The type of the project, either "bot" or "server".
		project_types project_type;

		// The platform that the project belongs to, always be "discord".
		project_platforms project_platform;

		// The ID of the project on its platform (e.g. on Discord).
		dpp::snowflake project_platform_id;

		// The top.gg ID of the user who voted (NOT their Discord user ID!).
		dpp::snowflake user_id;

		// The ID of the user who voted on the project's platform (e.g. on Discord).
		dpp::snowflake user_platform_id;

		// The username of the user who voted.
		std::string username;

		// The avatar URL of the user who voted.
		std::string user_avatar_url;

		// The underlying request.
		dpp::http_server_request* request;
	};

	/**
	 * @brief An HTTP request with the reason as to why it failed to verify as a request from top.gg.
	 */
	struct non_topgg_request {

		// The verification status, e.g. the reason it failed.
		sender_identification_statuses status;

		// The underlying request.
		dpp::http_server_request* request;
	};
	using topgg_request_event = std::function <void(topgg_request const&)>;
	using non_topgg_request_event = std::function <void(non_topgg_request const&)>;
	class listener {

		// The IP that the server will listen to.
		std::string ip;

		// The port that the server will listen to.
		uint16_t port;

		// The webhook secret of the bot on top.gg, starting with "whs_".
		std::string topgg_bot_webhook_secret;

		// The webhook secret of the server on top.gg, starting with "whs_".
		std::string topgg_server_webhook_secret;

		// The function that will get called whenever a request comes in from top.gg.
		topgg_request_event topgg_handler;

		// The function that will get called whenever a non-top.gg request comes in.
		non_topgg_request_event non_topgg_handler;

		// Whether we initialised a default cluster (true) or attached to an existing one (false).
		bool default_cluster;

		// The cluster to attach a server to.
		dpp::cluster* server_cluster;

		/**
		 * @brief Log a message using a logger from an on_log, unless it's empty, then server_cluster->log will be used instead.
		 * @param severity One of the values from dpp::loglevel, indicating the severity of the message.
		 * @param msg The message to log.
		 */
		void log(dpp::loglevel severity, std::string const& msg) const;
	public:
		listener() = delete;

		/**
		 * @brief Construct a new top.gg listener.
		 * @param ip The IP to bind the listener to. "0.0.0.0" binds to all local addresses.
		 * @param port The port to bind the listener to. It's advised to use a port > 1024.
		 * @param topgg_bot_webhook_secret_arg Your bot's webhook secret on top.gg, starting with "whs_".
		 * @param topgg_server_webhook_secret_arg Your server's webhook secret on top.gg, starting with "whs_".
		 * @param topgg_handler_arg The callback to call if the incoming request is from top.gg.
		 * @param non_topgg_handler_arg The callback to call if the incoming request is not from top.gg.
		 * @param new_cluster A pointer to the cluster to attach the listener to. The default value is nullptr which tells the constructor to create a new cluster.
		 */
		listener(std::string_view ip, uint16_t port, std::string_view topgg_bot_webhook_secret_arg, std::string_view topgg_server_webhook_secret_arg,
			topgg_request_event topgg_handler_arg, non_topgg_request_event non_topgg_handler_arg = non_topgg_request_event(), dpp::cluster* new_cluster = nullptr);

		listener(listener const&) = delete;
		listener(listener&&) = delete;

		// The underlying server that will listen to requests on ip:port.
		dpp::http_server* server;

		// Callbacks to call when a log message is to be written to the log with listener::log. The function works the same way as dpp::cluster::on_log, in fact, if listener::on_log is empty, it will default to dpp::cluster::log.
		dpp::event_router_t <dpp::log_t> on_log;

		/**
		 * @brief Start the underlying cluster and therefore the http_server.
		 * @param return_after dpp::st_wait to keep running in a loop or dpp::st_return to run once and continue execution.
		 */
		void start(dpp::start_type return_after) const;

		/**
		 * @brief Shut down the underlying cluster and therefore the http_server.
		 */
		void shutdown() const;

		~listener();
	};
}
