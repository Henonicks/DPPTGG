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


#include <dpp/dpp.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

#include "henifig/henifig.hpp"

#include <fmt/format.h>

dpp::cluster* server;
std::string TOPGG_WEBHOOK_LISTEN_IP;
uint16_t TOPGG_WEBHOOK_LISTEN_PORT;
std::string TOPGG_WEBHOOK_SECRET;
dpp::webhook TOPGG_WEBHOOK;

std::ofstream my_logs;

void log(std::string_view const message) {
	my_logs << fmt::format("[{0}]: {1}", dpp::utility::current_date_time(), message) << std::endl;
}

std::string cleanup_request_path(std::string_view const path) {
	std::string res = "/";
	for (char const x : path) {
		if (res.back() != '/' || (res.back() == '/' && x != '/')) {
			res += x;
		}
	}
	size_t const new_size = res.size() - 1 - (res.back() == '/');
	res = res.substr(1, new_size);
	return res;
}

std::vector <std::string> get_path_points(std::string_view const path) {
	std::vector <std::string> res;
	std::string curr_point;
	for (char const x : path) {
		if (x == '/') {
			res.push_back(curr_point);
			curr_point.clear();
		}
		else {
			curr_point += x;
		}
	}
	if (!curr_point.empty()) {
		res.push_back(curr_point);
	}
	return res;
}

int16_t send_vote_info(const nlohmann::json& info_json) { // The bot will pick the message up
	// The json format can be found here: https://docs.top.gg/docs/API/v1/webhooks/
	std::string const& type = info_json["type"];
	log(fmt::format("top.gg vote with type {}", type));
	if (type != "vote.create" && type != "webhook.test") {
		log("That one's unacceptable.");
		return dpp::err_unprocessable_content;
	}
	int16_t const weight = type == "vote.create" ? info_json["weight"].get <int16_t>() : 1;
	log(fmt::format("The vote weight is {}", weight));
	dpp::snowflake const user_id = info_json["data"]["user"]["platform_id"].get <std::string>();
	log(fmt::format("The user ID is {}", user_id));
	server->execute_webhook(TOPGG_WEBHOOK, dpp::message(TOPGG_WEBHOOK.channel_id,
		fmt::format("{0} {1}", dpp::utility::user_mention(user_id), weight))
	);
	return 204; // No Content (success)
}

std::string get_hmac_sha256_hex(std::string_view const key, std::string_view const message) {
	uint8_t hash[EVP_MAX_MD_SIZE];
	uint32_t hash_size;
	HMAC(
		EVP_sha256(),
		key.data(),
		static_cast <int>(key.size()),
		reinterpret_cast <const uint8_t*>(message.data()),
		message.size(),
		hash,
		&hash_size
	);
	std::stringstream res;
	for (uint32_t i = 0; i < hash_size; i++) {
		res << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast <int>(hash[i]);
	}
	return res.str();
}

int main() {
	henifig::config_t const config("../config.hfg");
	TOPGG_WEBHOOK_LISTEN_IP = config["TOPGG_WEBHOOK_LISTEN_IP"].get <std::string>();
	TOPGG_WEBHOOK_LISTEN_PORT = config["TOPGG_WEBHOOK_LISTEN_PORT"];
	TOPGG_WEBHOOK_SECRET = config["TOPGG_WEBHOOK_SECRET"].get <std::string>();
	TOPGG_WEBHOOK = dpp::webhook(config["TOPGG_WEBHOOK_LINK"].get <std::string>());

	server = new dpp::cluster();

	std::ofstream dpp_logs;
	if (!std::filesystem::exists("../logging")) {
		std::filesystem::create_directory("../logging");
	}
	my_logs.open("../logging/logs.log");
	dpp_logs.open("../logging/dpp_logs.log");
	server->on_log([&dpp_logs](dpp::log_t const& log) {
		dpp_logs << '[' << dpp::utility::current_date_time() << "]: " << dpp::utility::loglevel(log.severity) << ": " << log.message << std::endl;
	});

	dpp::http_server topgg_server(server, TOPGG_WEBHOOK_LISTEN_IP, TOPGG_WEBHOOK_LISTEN_PORT, [](dpp::http_server_request* request) {
		log(fmt::format("In came a request to {}", request->get_path()));
		const std::vector <std::string> path_points = get_path_points(cleanup_request_path(request->get_path()));
		if (path_points.size() != 3 || path_points[0] != "guidinglight" || path_points[1] != "api" || path_points[2] != "topgg-votes") {
			log("That's not a valid endpoint. Maybe someone ended up here by mistake?");
			request->set_response_body("This page doesn't exist.");
			request->set_status(dpp::err_not_found);
			return;
		}
		const std::string signature = request->get_header("x-topgg-signature");
		std::string timestamp, v1;
		bool error{};
		if (!signature.starts_with("t=") || !request->get_header("content-type").starts_with("application/json")) {
			log("The signature doesn't involve a timestamp or the content type isn't json.");
			error = true;
		}
		else {
			size_t i = 2;
			for (; i < signature.size(); i++) {
				if (signature[i] >= '0' && signature[i] <= '9') {
					timestamp += signature[i];
				}
				else {
					if (signature[i] != ',') {
						log("The top.gg signature contains non-digit characters.");
						error = true;
					}
					break;
				}
			}
			if (signature.size() - (timestamp.size() + 2) <= 4) {
				log("There's not enough space for a top.gg signature.");
				error = true;
			}
			else for (i += 4; i < signature.size(); i++) {
				v1 += signature[i];
			}
		}
		if (error) { // The format of the headers is wrong, if they even exist to begin with. That's not top.gg then.
			log(fmt::format("Sending {}.", static_cast <int>(dpp::err_forbidden)));
			request->set_status(dpp::err_forbidden);
			return;
		}
		nlohmann::json request_json;
		try {
			std::stringstream(request->get_request_body()) >> request_json;
		}
		catch (const nlohmann::detail::parse_error&) { // Couldn't parse the request body json
			log(fmt::format("Couldn't parse the request body json. Sending {}.", static_cast <int>(dpp::err_forbidden)));
			request->set_status(dpp::err_forbidden);
			return;
		}
		const std::string hmac_sha256 = get_hmac_sha256_hex(TOPGG_WEBHOOK_SECRET, timestamp + '.' + request->get_request_body());
		if (hmac_sha256 != v1) {
			log(fmt::format("The HMAC SHA26 doesn't match the V1 signature. Sending {}.", static_cast <int>(dpp::err_forbidden)));
			request->set_status(dpp::err_forbidden);
		}
		request->set_status(send_vote_info(request_json));
	});

	server->start();

	return 0;
}
