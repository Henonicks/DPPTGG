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
	dpptgg::secrets_map secrets;
	for (auto const& x : config["TOPGG_WEBHOOK_SECRETS"].get <std::vector <std::map <std::string, std::string>>>()) {
		for (const auto& [endpoint, secret] : x) {
			secrets[endpoint].push_back(secret);
		}
	}

	dpptgg::listener listener(IP, PORT, secrets, [](dpptgg::topgg_request const& request) {
		std::cout << request.request->get_path() << '\n';
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

	listener.start();

	return 0;
}
