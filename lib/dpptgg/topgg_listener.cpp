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

dpptgg::listener::listener(std::string_view const ip, uint16_t const port, secrets_map secrets_arg, topgg_request_event topgg_handler_arg, non_topgg_request_event non_topgg_handler_arg, dpp::cluster* const new_cluster) :
ip(ip), port(port), secrets(std::move(secrets_arg)), topgg_handler(std::move(topgg_handler_arg)), non_topgg_handler(std::move(non_topgg_handler_arg)) {
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
		sender_identification_statuses const status = this->identify_sender(request, payload_json);
		if (status == sis_topgg) {
			request->set_status(204); // OK No Content
			vote_types const vote_type = vote_type_from_str(payload_json["type"].get_ref <std::string&>());
			project_types const project_type = project_type_from_str(payload_json["data"]["project"]["type"].get_ref <std::string&>());
			project_platforms const project_platform = project_platform_from_str(payload_json["data"]["project"]["platform"].get_ref <std::string&>());
			if (vote_type == vt_na || project_type == pt_na || project_platform == pp_na) {
				request->set_status(500);
				if (vote_type == vt_na) {
					log(dpp::ll_trace, "The request type is not supported.");
				}
				if (project_type == pt_na) {
					log(dpp::ll_trace, "The project type is not supported.");
				}
				if (project_platform == pp_na) {
					log(dpp::ll_trace, "The project platform is not supported.");
				}
				this->non_topgg_handler({status, request});
			}
			dpp::snowflake vote_id{};
			uint8_t vote_weight{};
			datetime created_at{};
			datetime expires_at{};
			if (vote_type == vt_vote_create) {
				vote_id = payload_json["data"]["id"].get_ref <std::string&>();
				vote_weight = static_cast <uint8_t>(payload_json["data"]["weight"].get <int>());
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

dpp::cluster* dpptgg::listener::get_cluster() const {
	return this->server_cluster;
}

void dpptgg::listener::start(dpp::start_type const return_after) const {
	this->server_cluster->start(return_after);
}

void dpptgg::listener::shutdown() const {
	this->server_cluster->shutdown();
}

dpptgg::listener::~listener() {
	delete this->server;
	if (this->default_cluster) {
		delete this->server_cluster;
	}
}
