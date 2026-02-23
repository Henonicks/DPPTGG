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


#include "dpptgg/topgg_listener.hpp"

void dpptgg::listener::log(dpp::loglevel const severity, std::string const& msg) const {
	if (this->on_log.empty()) {
		this->server_cluster->log(severity, msg);
	}
	else {
		dpp::log_t log;
		log.severity = severity;
		log.message = msg;
		this->on_log.call(log);
	}
}

dpptgg::listener::listener(std::string_view const ip, uint16_t const port, std::string_view const topgg_bot_webhook_secret_arg, std::string_view const topgg_server_webhook_secret_arg, topgg_request_event topgg_handler_arg, non_topgg_request_event non_topgg_handler_arg, dpp::cluster* const new_cluster) :
ip(ip), port(port), topgg_bot_webhook_secret(topgg_bot_webhook_secret_arg), topgg_server_webhook_secret(topgg_server_webhook_secret_arg), topgg_handler(std::move(topgg_handler_arg)), non_topgg_handler(std::move(non_topgg_handler_arg)) {
	if (new_cluster == nullptr) {
		default_cluster = true;
		server_cluster = new dpp::cluster();
	}
	else {
		default_cluster = false;
		server_cluster = new_cluster;
	}
	this->server = new dpp::http_server(this->server_cluster, ip, port, [this](dpp::http_server_request* request) {
		this->log(dpp::ll_trace, "In came a request to " + request->get_path());
		nlohmann::json payload_json;
		sender_identification_statuses const status = identify_sender(request, this->topgg_bot_webhook_secret, this->topgg_server_webhook_secret, payload_json);
		if (status == sis_topgg) {
			request->set_status(204); // OK No Content
			vote_types const vote_type = payload_json["type"] == "vote.create" ? vt_vote_create : vt_webhook_test;
			project_types const project_type = payload_json["data"]["project"]["type"] == "bot" ? pt_bot : pt_server;
			project_platforms constexpr project_platform = pp_discord;

			dpp::snowflake vote_id{};
			int8_t vote_weight{};
			datetime created_at{};
			datetime expires_at{};
			if (vote_type == vt_vote_create) {
				vote_id = payload_json["data"]["id"].get_ref <std::string&>();
				vote_weight = static_cast <int8_t>(payload_json["data"]["weight"].get <int>());
				created_at = datetime::parse(payload_json["data"]["created_at"].get_ref <std::string&>());
				expires_at = datetime::parse(payload_json["data"]["expires_at"].get_ref <std::string&>());
			}

			this->topgg_handler({
				.vote_type = vote_type,
				.vote_id = vote_id,
				.vote_weight = vote_weight,
				.created_at = created_at,
				.expires_at = expires_at,
				.project_id = payload_json["data"]["project"]["id"].get_ref <std::string&>(),
				.project_type = project_type,
				.project_platform = project_platform,
				.project_platform_id = payload_json["data"]["project"]["platform_id"].get_ref <std::string&>(),
				.user_id = payload_json["data"]["user"]["id"].get_ref <std::string&>(),
				.user_platform_id = payload_json["data"]["user"]["platform_id"].get_ref <std::string&>(),
				.username = payload_json["data"]["user"]["name"],
				.user_avatar_url = payload_json["data"]["user"]["avatar_url"],
				.request = request
			});
		}
		else {
			request->set_status(400);
			this->non_topgg_handler({status, request});
		}
	});
}

void dpptgg::listener::start() const {
	this->server_cluster->start();
}

void dpptgg::listener::shutdown() const {
	this->server_cluster->shutdown();
}

dpptgg::listener::~listener() {
	if (this->default_cluster) {
		delete this->server_cluster;
	}
	delete this->server;
}
