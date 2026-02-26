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
	};

	/**
	 * @brief The types of a vote.
	 */
	enum vote_types : bool {
		vt_vote_create,
		vt_webhook_test,
	};

	/**
	 * @brief The types of a project.
	 */
	enum project_types : bool {
		pt_bot, // Bot.
		pt_server, // Guild.
	};

	/**
	 * @brief top.gg includes the platform in its JSONs, even though it's always "discord".
	 */
	enum project_platforms : bool {
		pp_discord,
	};

	/**
	 * @brief Generates an HMAC SHA256 digest, used to verify the identity of the request sender as top.gg.
	 * @param key Key.
	 * @param message Message.
	 * @return A hexadecimal HMAC SHA256 digest.
	 */
	std::string get_hmac_sha256_hex(std::string_view key, std::string_view message);
}
