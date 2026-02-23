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

#include <dpp/cluster.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

dpptgg::sender_identification_statuses dpptgg::identify_sender(dpp::http_server_request const* const request, std::string_view const topgg_bot_webhook_secret, std::string_view const topgg_server_webhook_secret, nlohmann::json& payload_json) {
	dpp::cluster const* const server_cluster = request->owner;
	const std::string signature = request->get_header("x-topgg-signature");
	std::string timestamp, v1;
	if (signature.find("t=") != 0) {
		server_cluster->log(dpp::ll_error, "The signature doesn't start with a timestamp.");
		return sis_signature_no_timestamp;
	}
	if (request->get_header("content-type").find("application/json") != 0) {
		server_cluster->log(dpp::ll_error, "The content type isn't json.");
		return sis_content_type_not_json;
	}
	size_t i = 2;
	for (; i < signature.size(); i++) {
		if (signature[i] >= '0' && signature[i] <= '9') {
			timestamp += signature[i];
		}
		else {
			if (signature[i] != ',') {
				server_cluster->log(dpp::ll_error, "The top.gg signature timestamp contains non-digit characters.");
				return sis_signature_no_timestamp;
			}
			break;
		}
	}
	if (signature.size() - (timestamp.size() + 2) <= 4) {
		server_cluster->log(dpp::ll_error, "There's not enough space for a top.gg signature.");
		return sis_bad_signature;
	}
	if (signature.substr(i + 1, 3) != "v1=") {
		server_cluster->log(dpp::ll_error, "The v1 part of the signature is missing.");
		return sis_bad_signature;
	}
	for (i += 4; i < signature.size(); i++) {
		v1 += signature[i];
	}
	try {
		payload_json = nlohmann::json::parse(request->get_request_body());
	}
	catch (nlohmann::detail::parse_error const&) { // Couldn't parse the request body json
		server_cluster->log(dpp::ll_error, "Couldn't parse the request body json.");
		return sis_bad_json_format;
	}
	project_types project_type;
	if (payload_json.find("data") != payload_json.end() &&
	payload_json["data"].find("project") != payload_json["data"].end()
	&& payload_json["data"]["project"].find("type") != payload_json["data"]["project"].end()) {
		project_type = (payload_json["data"]["project"]["type"] == "bot" ? pt_bot : pt_server);
	}
	else {
		return sis_bad_json_format;
	}
	const std::string hmac_sha256 = get_hmac_sha256_hex(
		project_type == pt_bot ? topgg_bot_webhook_secret : topgg_server_webhook_secret,
		timestamp + '.' + request->get_request_body());
	if (hmac_sha256 != v1) {
		server_cluster->log(dpp::ll_error, "The HMAC SHA256 doesn't match the V1 signature.");
		return sis_bad_authorisation;
	}
	return sis_topgg;
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

