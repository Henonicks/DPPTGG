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

#include "dpptgg/dpptgg.hpp"

#include <fstream>
#include <chrono>

int main() {
	nlohmann::json config;
	std::ifstream file;
	file.open("../config.json");
	file >> config;
	file.close();
	std::string const& IP = config["TOPGG_WEBHOOK_LISTEN_IP"];
	int16_t const PORT = config["TOPGG_WEBHOOK_LISTEN_PORT"];
	std::string const& TOKEN = config["TOPGG_BOT_TOKEN"];
	dpp::snowflake const USER_ID = config["USER_ID"];
	dpptgg::secrets_map secrets;
	for (auto const& x : config["TOPGG_WEBHOOK_SECRETS"].get <std::vector <std::map <std::string, std::string>>>()) {
		for (const auto& [endpoint, secret] : x) {
			secrets[endpoint].push_back(secret);
		}
	}

	dpptgg::listener listener(IP, PORT, secrets, [](dpptgg::topgg_request const& request) {
		std::cout << request.vote_type
		          << request.vote_id << '\n'
		          << request.vote_weight << '\n'
		          << request.created_at.get_timestamp() << '\n'
		          << request.expires_at.get_timestamp() << '\n'
		          << request.project_id << '\n'
		          << request.project_type << '\n'
		          << request.project_platform << '\n'
		          << request.project_platform_id << '\n'
		          << request.user_id << '\n'
		          << request.user_platform_id << '\n'
		          << request.username << '\n'
		          << request.user_avatar_url << '\n';
	});

	listener.on_log(dpp::utility::cout_logger());

	dpptgg::poker poker(TOKEN);

	poker.get_cluster()->on_log(dpp::utility::cout_logger());

	auto const now = std::chrono::system_clock::now();
	auto const time_t = std::chrono::system_clock::to_time_t(now);

	dpptgg::datetime start_date = {
		.year = static_cast <uint16_t>(1900 + std::localtime(&time_t)->tm_year),
		.month = static_cast <uint8_t>(std::localtime(&time_t)->tm_mon + 1),
		.day = static_cast <uint8_t>(std::localtime(&time_t)->tm_mday),
	};

	poker.get_current_project([](dpptgg::topgg_request_completion_t const& callback) {
		std::cout << "get_current_project: " << callback.request.body << std::endl;
	});

	poker.get_votes(start_date, [&poker](dpptgg::topgg_request_completion_t const& callback) {
		poker.get_votes(callback.get <dpptgg::requested_votes_t>().cursor, [](dpptgg::topgg_request_completion_t const& inner_callback) {
			std::cout << "get_votes: " << inner_callback.get <dpptgg::requested_votes_t>().cursor << std::endl;
		});
	});

	poker.get_vote_status_by_user(USER_ID, [](dpptgg::topgg_request_completion_t const& callback) {
		std::cout << "get_vote_status_by_user: " << callback.request.body << std::endl;
	}, dpptgg::us_discord);

	dpp::slashcommand test("test", "test command", poker.get_cluster()->me.id);

	dpptgg::slashcommand_array commands = {test};

	poker.update_discord_bot_commands(commands, [](dpptgg::topgg_request_completion_t const& callback) {
		std::cout << "update_discord_bot_commands: " << callback.request.status << '\n';
	});

	poker.start(dpp::st_return);
	listener.start();

	return 0;
}
