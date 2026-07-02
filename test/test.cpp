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

	/// ---------- V0

	poker.get_cluster()->on_log(dpp::utility::cout_logger());

	poker.start(dpp::st_return);

	poker.get_bots([](dpptgg::v0::request_completion_t const& callback) {
		std::cout << "get_bots: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << '\n';
	}, 5, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	poker.get_user_vote([](dpptgg::v0::request_completion_t const& callback) {
		std::cout << "get_user_vote: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << '\n';
	}, USER_ID);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	// poker.post_server_count([&poker](dpptgg::v0::request_completion_t const& callback) {
	// 	std::cout << "post_server_count: " << callback.request.status << '\n';
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	// 	poker.get_server_count([](dpptgg::v0::request_completion_t const& callback) {
	// 		std::cout << "get_server_count: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << '\n';
	// 	});
	// }, 2);

	/// ---------- V1

	auto const now = std::chrono::system_clock::now();
	auto const time_t = std::chrono::system_clock::to_time_t(now);

	dpptgg::datetime const start_date = {
		.year = static_cast <uint16_t>(1900 + std::localtime(&time_t)->tm_year),
		.month = static_cast <uint8_t>(std::localtime(&time_t)->tm_mon + 1),
		.day = static_cast <uint8_t>(std::localtime(&time_t)->tm_mday),
	};

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	poker.get_current_project([](dpptgg::v1::request_completion_t const& callback) {
		std::cout << "get_current_project: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << std::endl;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	dpptgg::v1::headline_map_t headline_map = {
		{"en", "DPPTGG headline"}
	};
	poker.update_current_project(headline_map, {}, [](dpptgg::v1::request_completion_t const& callback) {
		std::cout << "update_current_project: " << callback.request.status << ' ' << callback.request.body.substr(0, 300) << std::endl;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	poker.create_announcement("DPPTGG announcement", "DPPTGG announcement content", dpptgg::ac_announcement, [](dpptgg::v1::request_completion_t const& callback) {
		std::cout << "create_announcement: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << std::endl;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	dpptgg::v1::bot_metrics_t bot_metrics = {
		.server_count = 420691337,
		.shard_count = 52,
	};
	poker.update_project_metrics(bot_metrics, [](dpptgg::v1::request_completion_t const& callback) {
		std::cout << "update_project_metrics: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << std::endl;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	dpptgg::v1::bot_metrics_batch_t metrics_batch = {
		{
			.timestamp = start_date,
			.metrics = bot_metrics
		}
	};
	poker.update_project_metrics_batch(metrics_batch, [](dpptgg::v1::request_completion_t const& callback) {
		std::cout << "update_project_metrics_batch: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << std::endl;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	poker.get_votes(start_date, [&poker](dpptgg::v1::request_completion_t const& callback) {
		if (!callback.is_error()) {
			std::string const& cursor = callback.get <dpptgg::v1::requested_votes_t>().cursor;
			poker.get_votes(cursor, [](dpptgg::v1::request_completion_t const& inner_callback) {
				std::cout << "get_votes: " << inner_callback.request.status << ' ' << inner_callback.get <dpptgg::v1::requested_votes_t>().cursor << std::endl;
			});
		}
		else {
			std::cout << "get_votes: " << callback.error.status << std::endl;
		}
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	poker.get_vote_status_by_user(USER_ID, [](dpptgg::v1::request_completion_t const& callback) {
		std::cout << "get_vote_status_by_user: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << std::endl;
	}, dpptgg::us_discord);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	dpp::slashcommand test("test", "DPPTGG test command", poker.get_cluster()->me.id);

	dpptgg::v1::slashcommand_array commands = {test};

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	// poker.update_discord_bot_commands(commands, [](dpptgg::v1::request_completion_t const& callback) {
	// 	std::cout << "update_discord_bot_commands: " << callback.request.status << ' ' << callback.request.body.substr(0, 100) << '\n';
	// });

	listener.start();

	return 0;
}
