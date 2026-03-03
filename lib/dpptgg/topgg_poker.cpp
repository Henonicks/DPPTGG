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


#include "dpptgg/topgg_poker.hpp"

dpptgg::poker::poker(std::string_view const token_arg, dpp::cluster const* const poker_cluster_arg) : token(std::string("Bearer ") + token_arg.data()) {
	if (poker_cluster_arg == nullptr) {
		this->poker_cluster = new dpp::cluster();
		this->default_cluster = true;
	}
	else {
		this->default_cluster = false;
	}
}

dpptgg::topgg_request_error_t dpptgg::error_from_json(nlohmann::json const& json) {
	topgg_request_error_t res{};
	if (json.count("type")) {
		res.type = json["type"];
	}
	if (json.count("title")) {
		res.title = json["title"];
	}
	if (json.count("status")) {
		res.status = json["status"];
	}
	if (json.count("detail")) {
		res.detail = json["detail"];
	}
	if (json.count("errors")) {
		res.errors = json["errors"];
	}
	if (json.count("traceId")) {
		res.trace_id = json["traceId"];
	}
	return res;
}

bool dpptgg::topgg_request_completion_t::is_error() const {
	return this->error.status;
}

dpptgg::requested_project_t dpptgg::project_from_json(nlohmann::json const& json) {
	return {
		.id = json["id"].get_ref <std::string const&>(),
		.name = json["name"],
		.platform = project_platform_from_str(json["platform"].get_ref <std::string const&>()),
		.type = project_type_from_str(json["type"].get_ref <std::string const&>()),
		.headline = json["headline"],
		.tags = json["tags"],
		.votes = json["votes"],
		.votes_total = json["votes_total"],
		.review_score = json["review_score"],
		.review_count = json["review_count"],
	};
}

dpptgg::vote_t dpptgg::vote_from_json(nlohmann::json const& json) {
	return {
		.user_id = json["user_id"].get_ref <std::string const&>(),
		.user_platform_id = json["platform_id"].get_ref <std::string const&>(),
		.weight = static_cast <uint8_t>(json["weight"].get <int>()),
		.created_at = datetime::parse(json["created_at"].get_ref <std::string const&>()),
		.expires_at = datetime::parse(json["expires_at"].get_ref <std::string const&>()),
	};
}

dpptgg::requested_votes_t dpptgg::votes_from_json(nlohmann::json const& json) {
	requested_votes_t res = {
		.cursor = json["cursor"],
	};
	if (json.contains("data")) {
		for (nlohmann::json const& x : json["data"]) {
			res.data.push_back(vote_from_json(x));
		}
	}
	return res;
}

dpptgg::vote_status_t dpptgg::vote_status_from_json(nlohmann::json const& json) {
	return {
		.created_at = datetime::parse(json["created_at"].get_ref <std::string const&>()),
		.expires_at = datetime::parse(json["expires_at"].get_ref <std::string const&>()),
		.weight = json["weight"],
	};
}

void dpptgg::poker::get_current_project(topgg_completion_event const& topgg_callback) const {
	this->poker_cluster->request(BASE_API_URL + "projects/@me", dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		topgg_request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = error_from_json(callback.raw_json);
			}
			else {
				callback.value = project_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->token}
	});
}

void dpptgg::poker::update_discord_bot_commands(slashcommand_array const& commands, topgg_completion_event const& topgg_callback) {
	this->poker_cluster->request(BASE_API_URL + "projects/@me/commands", dpp::m_post, [topgg_callback](dpp::http_request_completion_t const& request) {
		topgg_request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			std::cout << request.body << std::endl;
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = error_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, nlohmann::json(commands).dump(), application_json, {
		{"Authorization", this->token}
	});

}

void dpptgg::poker::get_votes(std::string_view const cursor, topgg_completion_event const& topgg_callback) const {
	this->poker_cluster->request(BASE_API_URL + "projects/@me/votes?cursor=" + cursor.data(), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		topgg_request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = error_from_json(callback.raw_json);
			}
			else {
				callback.value = votes_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->token}
	});
}

void dpptgg::poker::get_votes(datetime const& start_date, topgg_completion_event const& topgg_callback) const {
	this->poker_cluster->request(BASE_API_URL + "projects/@me/votes?startDate=" + start_date.get_timestamp(), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		topgg_request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = error_from_json(callback.raw_json);
			}
			else {
				callback.value = votes_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->token}
	});
}

void dpptgg::poker::get_vote_status_by_user(dpp::snowflake const user_id, topgg_completion_event const& topgg_callback, user_sources const user_source) const {
	this->poker_cluster->request(BASE_API_URL + "projects/@me/votes/" + user_id.str() + "?source=" + str_from_user_source(user_source), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		topgg_request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = error_from_json(callback.raw_json);
			}
			else {
				callback.value = vote_status_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->token},
	});
}

dpp::cluster* dpptgg::poker::get_cluster() const {
	return this->poker_cluster;
}

void dpptgg::poker::start(dpp::start_type const return_after) const {
	this->poker_cluster->start(return_after);
}

void dpptgg::poker::shutdown() const {
	this->poker_cluster->shutdown();
}

dpptgg::poker::~poker() {
	if (this->default_cluster) {
		delete this->poker_cluster;
	}
}

