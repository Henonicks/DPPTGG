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

#include <dpp/http_server_request.h>
#include <dpp/nlohmann/json.hpp>

namespace dpptgg {

	/**
	 * @brief Values that indicate if a request came from top.gg, otherwise which part of the verification failed.
	 */
	enum sender_identification_statuses : uint8_t {
		sis_topgg, // Verification successful.
		sis_no_secrets, // No secrets for the endpoint set.
		sis_signature_no_timestamp, // The signature doesn't start with a timestamp as described in top.gg's documentation.
		sis_content_type_not_json, // The content does not start with application/json.
		sis_bad_signature, // The signature is badly formatted/missing.
		sis_bad_json_format, // Couldn't parse the request body json.
		sis_bad_authorisation, // The HMAC SHA256 digest doesn't match the V1 signature.
		sis_unsupported_type, // The request did come from top.gg but the project type is unsupported.
	};

	/**
	 * @brief The types of a vote.
	 */
	enum vote_types : uint8_t {
		vt_na,
		vt_vote_create,
		vt_webhook_test,
	};

	vote_types vote_type_from_str(std::string_view s);

	/**
	 * @brief The types of a project.
	 */
	enum project_types : uint8_t {
		pt_na,
		pt_bot, // Bot.
		pt_server, // Guild.
	};

	project_types project_type_from_str(std::string_view s);

	/**
	 * @brief top.gg includes the platform in its JSONs, even though it's always "discord".
	 */
	enum project_platforms : bool {
		pp_na,
		pp_discord,
	};

	project_platforms project_platform_from_str(std::string_view s);

	enum user_sources : uint8_t {
		us_na,
		us_topgg,
		us_discord,
	};

	user_sources user_source_from_str(std::string_view s);
	std::string str_from_user_source(user_sources user_source);

	enum bot_fields : uint8_t {
		sf_na,
		sf_id,
		sf_username,
		sf_discriminator,
		sf_avatar,
		sf_def_avatar,
		sf_lib,
		sf_prefix,
		sf_shortdesc,
		sf_longdesc,
		sf_tags,
		sf_website,
		sf_support,
		sf_github,
		sf_owners,
		sf_guilds,
		sf_invite,
		sf_date,
		sf_server_count,
		sf_shard_count,
		sf_certified_bot,
		sf_vanity,
		sf_points,
		sf_monthly_points,
		sf_donatebot_guild_id,
	};

	std::string str_from_sort_field(bot_fields sort_field);

	/**
	 * @brief Generates an HMAC SHA256 digest, used to verify the identity of the request sender as top.gg.
	 * @param key Key.
	 * @param message Message.
	 * @return A hexadecimal HMAC SHA256 digest.
	 */
	std::string get_hmac_sha256_hex(std::string_view key, std::string_view message);
}
