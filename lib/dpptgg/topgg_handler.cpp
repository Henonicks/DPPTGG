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


#include "dpptgg/topgg_handler.hpp"

#include "dpptgg/topgg_listener.hpp"

#include <dpp/cluster.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

dpptgg::sender_identification_statuses dpptgg::listener::identify_sender(dpp::http_server_request const* const request, nlohmann::json& payload_json) {
	std::string const path = request->get_path();
	if (this->secrets.find(path) == this->secrets.end()) {
		this->log(dpp::ll_error, "There are no secrets set for this endpoint.");
		return sis_no_secrets;
	}
	std::vector <std::string> const& curr_secrets = this->secrets.at(path);
	const std::string signature = request->get_header("x-topgg-signature");
	std::string timestamp, v1;
	if (signature.find("t=") != 0) {
		this->log(dpp::ll_error, "The signature doesn't start with a timestamp.");
		return sis_signature_no_timestamp;
	}
	if (request->get_header("content-type").find("application/json") != 0) {
		this->log(dpp::ll_error, "The content type isn't json.");
		return sis_content_type_not_json;
	}
	size_t i = 2;
	for (; i < signature.size(); i++) {
		if (signature[i] >= '0' && signature[i] <= '9') {
			timestamp += signature[i];
		}
		else {
			if (signature[i] != ',') {
				this->log(dpp::ll_error, "The top.gg signature timestamp contains non-digit characters.");
				return sis_signature_no_timestamp;
			}
			break;
		}
	}
	if (signature.size() - (timestamp.size() + 2) <= 4) {
		this->log(dpp::ll_error, "There's not enough space for a top.gg signature.");
		return sis_bad_signature;
	}
	if (signature.substr(i + 1, 3) != "v1=") {
		this->log(dpp::ll_error, "The v1 part of the signature is missing.");
		return sis_bad_signature;
	}
	for (i += 4; i < signature.size(); i++) {
		v1 += signature[i];
	}
	try {
		payload_json = nlohmann::json::parse(request->get_request_body());
	}
	catch (nlohmann::detail::parse_error const&) { // Couldn't parse the request body json
		this->log(dpp::ll_error, "Couldn't parse the request body json.");
		return sis_bad_json_format;
	}
	for (std::string const& x : curr_secrets) {
		std::string const hmac_sha256 = get_hmac_sha256_hex(x, timestamp + '.' + request->get_request_body());
		if (hmac_sha256 == v1) {
			return sis_topgg;
		}
	}
	this->log(dpp::ll_error, "None of the generated HMAC SHA256 digests match the V1 signature.");
	return sis_bad_authorisation;
}

dpptgg::vote_types dpptgg::vote_type_from_str(std::string_view const s) {
	return s == "vote.create" ? vt_vote_create : s == "webhook.test" ? vt_webhook_test : vt_na;
}

dpptgg::project_types dpptgg::project_type_from_str(std::string_view const s) {
	return s == "bot" ? pt_bot : s == "server" ? pt_server : pt_na;
}

dpptgg::project_platforms dpptgg::project_platform_from_str(std::string_view const s) {
	return s == "discord" ? pp_discord : pp_na;
}

dpptgg::user_sources dpptgg::user_source_from_str(std::string_view const s) {
	return s == "topgg" ? us_topgg : s == "discord" ? us_discord : us_na;
}

std::string dpptgg::str_from_user_source(user_sources user_source) {
	switch (user_source) {
	case us_topgg:
		return "topgg";
	case us_discord:
		return "discord";
	default:
		return "";
	}
}


std::string dpptgg::get_hmac_sha256_hex(std::string_view const key, std::string_view const message) {
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

