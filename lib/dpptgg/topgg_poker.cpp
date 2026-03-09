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

dpptgg::poker::poker(std::string_view const token_arg, dpp::cluster const* const poker_cluster_arg) {
	this->v1_token = std::string("Bearer ") + token_arg.data();
	this->v0_token = std::string_view(this->v1_token).substr(7, token_arg.size());
	if (poker_cluster_arg == nullptr) {
		this->poker_cluster = new dpp::cluster();
		this->default_cluster = true;
	}
	else {
		this->default_cluster = false;
	}
}

dpptgg::v0::requested_bots_t dpptgg::v0::bots_from_json(nlohmann::json const& json) {
	requested_bots_t res = {
		.limit = json["limit"],
		.offset = json["offset"],
		.count = json["count"],
		.total = json["total"]
	};
	for (nlohmann::json const& x : json["results"]) {
		res.results.push_back(bot_from_json(x));
	}
	return res;
}

std::vector <dpp::snowflake> str_vector_to_snowflake(std::vector <std::string> const& vector) {
	std::vector <dpp::snowflake> res;
	for (std::string const& x : vector) {
		res.push_back(x);
	}
	return res;
}

dpptgg::v0::bot_t dpptgg::v0::bot_from_json(nlohmann::json const& json) {
	uint16_t discriminator{};
	if (json.contains("discriminator")) {
		discriminator = static_cast <uint16_t>(std::stoi(json["discriminator"].get_ref <std::string const&>()));
	}
	std::string avatar{};
	if (json.contains("avatar")) {
		avatar = json["avatar"];
	}
	std::string def_avatar{};
	if (json.contains("defAvatar")) {
		def_avatar = json["defAvatar"];
	}
	std::string longdesc{};
	if (json.contains("longdesc")) {
		longdesc = json["longdesc"];
	}
	std::string website{};
	if (json.contains("website")) {
		website = json["website"];
	}
	std::string support{};
	if (json.contains("support")) {
		support = json["support"];
	}
	std::string github{};
	if (json.contains("github")) {
		github = json["github"];
	}
	std::string invite{};
	if (json.contains("invite")) {
		invite = json["invite"];
	}
	uint64_t server_count{};
	if (json.contains("server_count")) {
		server_count = json["server_count"];
	}
	int shard_count{};
	if (json.contains("shard_count")) {
		shard_count = json["shard_count"];
	}
	std::string vanity{};
	if (json.contains("vanity")) {
		vanity = json["vanity"];
	}
	std::string donatebot_gulid_id{};
	if (json.contains("donatebot_gulid_id")) {
		donatebot_gulid_id = json["donatebotgulidid"];
	}
	return {
		.id = json["id"].get_ref <std::string const&>(),
		.username = json["username"],
		.discriminator = discriminator,
		.avatar = avatar,
		.def_avatar = def_avatar,
		.lib = json["lib"],
		.prefix = json["prefix"],
		.shortdesc = json["shortdesc"],
		.longdesc = longdesc,
		.tags = json["tags"],
		.website = website,
		.support = support,
		.github = github,
		.owners = str_vector_to_snowflake(json["owners"]),
		.guilds = str_vector_to_snowflake(json["guilds"]),
		.invite = invite,
		.date = datetime::parse(json["date"].get_ref <std::string const&>()),
		.server_count = server_count,
		.shard_count = shard_count,
		.certified_bot = json["certifiedBot"],
		.vanity = vanity,
		.points = json["points"],
		.monthly_points = json["monthlyPoints"],
		.donatebot_guild_id = donatebot_gulid_id,
	};
}

dpptgg::v0::vote_t dpptgg::v0::vote_from_json(nlohmann::json const& json) {
	return {
		.username = json["username"],
		.id = json["id"].get_ref <std::string const&>(),
		.avatar = json["avatar"],
	};
}

dpptgg::v0::requested_votes_t dpptgg::v0::votes_from_json(nlohmann::json const& json) {
	requested_votes_t res;
	for (nlohmann::json const& x : json) {
		res.push_back(vote_from_json(x));
	}
	return res;
}

dpptgg::v0::server_count_t dpptgg::v0::server_count_from_json(nlohmann::json const& json) {
	return json["server_count"];
}

dpptgg::v0::voted_state_t dpptgg::v0::voted_state_from_json(nlohmann::json const& json) {
	return json["voted"] == 1;
}

dpptgg::v0::user_t dpptgg::v0::user_from_json(nlohmann::json const& json) {
	std::string avatar{};
	if (json.contains("avatar")) {
		avatar = json["avatar"];
	}
	std::string bio{};
	if (json.contains("bio")) {
		bio = json["bio"];
	}
	std::string banner{};
	if (json.contains("banner")) {
		banner = json["banner"];
	}
	std::string youtube{};
	if (json["social"].contains("youtube")) {
		youtube = json["youtube"];
	}
	std::string reddit{};
	if (json["social"].contains("reddit")) {
		reddit = json["reddit"];
	}
	std::string twitter{};
	if (json["social"].contains("twitter")) {
		twitter = json["twitter"];
	}
	std::string instagram{};
	if (json["social"].contains("instagram")) {
		instagram = json["instagram"];
	}
	std::string github{};
	if (json["social"].contains("github")) {
		github = json["github"];
	}
	std::string color{};
	if (json.contains("color")) {
		color = json["color"];
	}
	return {
		.id = json["id"].get_ref <std::string const&>(),
		.username = json["username"],
		.discriminator = static_cast <uint16_t>(std::stoi(json["discriminator"].get_ref <std::string const&>())),
		.avatar = avatar,
		.def_avatar = json["defAvatar"],
		.bio = bio,
		.banner = banner,
		.youtube = youtube,
		.reddit = reddit,
		.twitter = twitter,
		.instagram = instagram,
		.github = github,
		.color = color,
		.supporter = json["supporter"],
		.certified_dev = json["certifiedDev"],
		.mod = json["mod"],
		.web_mod = json["webMod"],
		.admin = json["admin"],
	};
}

void dpptgg::poker::get_bots(v0::completion_event const& topgg_callback, uint16_t const limit, uint64_t const offset, bot_fields const sort_field, std::vector <bot_fields> const& fields) {
	std::string request_arguments = "limit=" + std::to_string(limit) + "&offset=" + std::to_string(offset);
	if (sort_field != sf_na) {
		request_arguments += "&sort_field=" + std::to_string(sort_field);
	}
	if (!fields.empty()) {
		request_arguments += "&fields=";
		for (int i = 0; i < fields.size() - 1; i++) {
			request_arguments += str_from_sort_field(fields[i]) + ',';
		}
		request_arguments += str_from_sort_field(fields.back());
	}
	this->poker_cluster->request(v0::BASE_API_URL + "bots?" + request_arguments, dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v0::request_completion_t callback = {
			.request = std::move(request)
		};
		if (request.status / 100 == 2) {
			callback.raw_json = nlohmann::json::parse(request.body);
			callback.value = v0::bots_from_json(callback.raw_json);
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v0_token.data()}
	});
}

void dpptgg::poker::get_server_count(v0::completion_event const& topgg_callback, dpp::snowflake const bot_id) {
	this->poker_cluster->request(v0::BASE_API_URL + "bots/" + bot_id.str() + "/stats", dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v0::request_completion_t callback = {
			.request = std::move(request)
		};
		if (request.status / 100 == 2) {
			callback.raw_json = nlohmann::json::parse(request.body);
			callback.value = v0::server_count_from_json(callback.raw_json);
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v0_token.data()}
	});
}

void dpptgg::poker::get_user_vote(v0::completion_event const& topgg_callback, dpp::snowflake const bot_id, dpp::snowflake const user_id) {
	this->poker_cluster->request(v0::BASE_API_URL + "bots/" + bot_id.str() + "/check?userId=" + user_id.str(), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v0::request_completion_t callback = {
			.request = std::move(request)
		};
		if (request.status / 100 == 2) {
			callback.raw_json = nlohmann::json::parse(request.body);
			callback.value = v0::voted_state_from_json(callback.raw_json);
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v0_token.data()}
	});
}

void dpptgg::poker::post_server_count(v0::completion_event const& topgg_callback, dpp::snowflake const bot_id, uint64_t const server_count) {
	this->poker_cluster->request(v0::BASE_API_URL + "bots/" + bot_id.str() + "/stats", dpp::m_post, [topgg_callback](dpp::http_request_completion_t const& request) {
		topgg_callback({
			.request = std::move(request)
		});
	}, "{\"server_count\":" + std::to_string(server_count) + '}', text_plain, {
		{"Authorization", this->v0_token.data()}
	});
}

dpptgg::v1::request_error_t dpptgg::v1::error_from_json(nlohmann::json const& json) {
	request_error_t res{};
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

bool dpptgg::v1::request_completion_t::is_error() const {
	return this->error.status;
}

dpptgg::v1::requested_project_t dpptgg::v1::project_from_json(nlohmann::json const& json) {
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

dpptgg::v1::vote_t dpptgg::v1::vote_from_json(nlohmann::json const& json) {
	return {
		.user_id = json["user_id"].get_ref <std::string const&>(),
		.user_platform_id = json["platform_id"].get_ref <std::string const&>(),
		.weight = static_cast <uint8_t>(json["weight"].get <int>()),
		.created_at = datetime::parse(json["created_at"].get_ref <std::string const&>()),
		.expires_at = datetime::parse(json["expires_at"].get_ref <std::string const&>()),
	};
}

dpptgg::v1::requested_votes_t dpptgg::v1::votes_from_json(nlohmann::json const& json) {
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

dpptgg::v1::vote_status_t dpptgg::v1::vote_status_from_json(nlohmann::json const& json) {
	return {
		.created_at = datetime::parse(json["created_at"].get_ref <std::string const&>()),
		.expires_at = datetime::parse(json["expires_at"].get_ref <std::string const&>()),
		.weight = json["weight"],
	};
}

void dpptgg::poker::get_current_project(v1::completion_event const& topgg_callback) const {
	this->poker_cluster->request(v1::BASE_API_URL + "projects/@me", dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v1::request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = v1::error_from_json(callback.raw_json);
			}
			else {
				callback.value = v1::project_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v1_token}
	});
}

void dpptgg::poker::update_discord_bot_commands(v1::slashcommand_array const& commands, v1::completion_event const& topgg_callback) {
	this->poker_cluster->request(v1::BASE_API_URL + "projects/@me/commands", dpp::m_post, [topgg_callback](dpp::http_request_completion_t const& request) {
		v1::request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			if (request.status / 100 != 2) {
				callback.raw_json = nlohmann::json::parse(request.body);
				callback.error = v1::error_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, nlohmann::json(commands).dump(), application_json, {
		{"Authorization", this->v1_token}
	});

}

void dpptgg::poker::get_votes(std::string_view const cursor, v1::completion_event const& topgg_callback) const {
	this->poker_cluster->request(v1::BASE_API_URL + "projects/@me/votes?cursor=" + cursor.data(), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v1::request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = v1::error_from_json(callback.raw_json);
			}
			else {
				callback.value = v1::votes_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v1_token}
	});
}

void dpptgg::poker::get_votes(datetime const& start_date, v1::completion_event const& topgg_callback) const {
	this->poker_cluster->request(v1::BASE_API_URL + "projects/@me/votes?startDate=" + start_date.get_timestamp(), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v1::request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = v1::error_from_json(callback.raw_json);
			}
			else {
				callback.value = v1::votes_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v1_token}
	});
}

void dpptgg::poker::get_vote_status_by_user(dpp::snowflake const user_id, v1::completion_event const& topgg_callback, user_sources const user_source) const {
	this->poker_cluster->request(v1::BASE_API_URL + "projects/@me/votes/" + user_id.str() + "?source=" + str_from_user_source(user_source), dpp::m_get, [topgg_callback](dpp::http_request_completion_t const& request) {
		v1::request_completion_t callback = {
			.request = std::move(request)
		};
		try {
			callback.raw_json = nlohmann::json::parse(request.body);
			if (request.status / 100 != 2) {
				callback.error = v1::error_from_json(callback.raw_json);
			}
			else {
				callback.value = v1::vote_status_from_json(callback.raw_json);
			}
		}
		catch (...) {
			callback.error.status = request.status;
		}
		topgg_callback(callback);
	}, "", text_plain, {
		{"Authorization", this->v1_token},
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
