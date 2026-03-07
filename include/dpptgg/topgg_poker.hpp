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

#include <dpp/cluster.h>

#include "dpptgg/topgg_handler.hpp"
#include "dpptgg/timestamp.hpp"

namespace dpptgg {

	inline char constexpr text_plain[] = "text/plain";
	inline char constexpr application_json[] = "application/json";

	namespace v0 {
		inline std::string const BASE_API_URL = "https://top.gg/api/";

		struct bot_t;

		struct requested_bots_t {

			// The matching bots.
			std::vector <bot_t> results{};

			// The limit used in the request.
			uint8_t limit{};

			// The offset used in the request.
			uint64_t offset{};

			// The length of the results array.
			uint8_t count{};

			// The total number of bots that match your search.
			uint64_t total{};
		};

		requested_bots_t bots_from_json(nlohmann::json const& json);

		struct bot_t {

			// The ID of the bot on Discord.
			dpp::snowflake id{};

			// The username of the bot.
			std::string username{};

			// The discriminator of the bot.
			uint16_t discriminator{};

			// The avatar hash of the bot's avatar.
			std::string avatar{};

			// The CDN hash of the bot's avatar if the bot has none.
			std::string def_avatar{};

			// The library of the bot.
			std::string lib{};

			// The prefix of the bot.
			std::string prefix{};

			// The short description of the bot.
			std::string shortdesc{};

			// The long description of the bot.
			std::string longdesc{};

			// The tags of the bot.
			std::vector <std::string> tags{};

			// The website URL of the bot.
			std::string website{};

			// The support server invite code of the bot.
			std::string support{};

			// The link to the GitHub repo of the bot.
			std::string github{};

			// An array of the bot's owners. The element zero is the main owner.
			std::vector <dpp::snowflake> owners{};

			// An array of Discord servers featured on the bot's page.
			std::vector <dpp::snowflake> guilds{};

			// The custom bot invite url of the bot.
			std::string invite{};

			// The date when the bot was approved.
			datetime date{};

			// The amount of servers the bot has according to posted stats.
			uint64_t server_count{};

			// The amount of shards the bot has according to posted stats.
			int shard_count{};

			// The certified status of the bot.
			bool certified_bot{};

			// The vanity url of the bot.
			std::string vanity{};

			// The amount of votes the bot has.
			uint64_t points{};

			// The amount of votes the bot had this month.
			int monthly_points{};

			// The guild ID for the donatebot setup.
			dpp::snowflake donatebot_guild_id{};
		};

		bot_t bot_from_json(nlohmann::json const& json);

		struct vote_t {

			// The username of the user who voted.
			std::string username{};

			// The ID of the user who voted on Discord.
			dpp::snowflake id{};

			// The hash of the avatar of the user who voted.
			std::string avatar{};
		};

		vote_t vote_from_json(nlohmann::json const& json);

		using requested_votes_t = std::vector <vote_t>;

		requested_votes_t votes_from_json(nlohmann::json const& json);

		using server_count_t = uint64_t;

		server_count_t stats_from_json(nlohmann::json const& json);

		using voted_state_t = bool;

		voted_state_t voted_state_from_json(nlohmann::json const& json);

		struct user_t {
			dpp::snowflake id{};
			std::string username{};
			uint16_t discriminator{};
			std::string avatar{};
			std::string def_avatar{};
			std::string bio{};
			std::string banner{};
			std::string youtube{};
			std::string reddit{};
			std::string twitter{};
			std::string instagram{};
			std::string github{};
			std::string color{};
			bool supporter{};
			bool certified_dev{};
			bool mod{};
			bool web_mod{};
			bool admin{};
		};

		user_t user_from_json(nlohmann::json const& json);

		using callback_data_t = std::variant <std::monostate, requested_bots_t, bot_t, requested_votes_t, server_count_t, voted_state_t, user_t>;

		struct request_completion_t {

			// An std::variant with the underlying value. Holds std::monostate if top.gg returned an error.
			callback_data_t value{};

			// The raw JSON object returned from top.gg. Empty if the response is an error.
			nlohmann::json raw_json{};

			/**
			 * @brief See if top.gg returned an error.
			 * @return Whether top.gg returned an error.
			 */
			[[nodiscard]] bool is_error() const;

			/**
			 * @brief Get the underlying value as the specified type.
			 * @tparam T The type to get the value as.
			 * @return The value as the specified value.
			 */
			template <typename T>
			[[nodiscard]] T const& get() const {
				return std::get <T>(this->value);
			}

			// The underlying http request object.
			dpp::http_request_completion_t request;
		};

		using completion_event = std::function <void(request_completion_t const&)>;

	}

	namespace v1 {
		inline std::string const BASE_API_URL = "https://top.gg/api/v1/";

		struct request_error_t {

			// Type.
			std::string type{};

			// Title.
			std::string title{};

			// Status.
			uint16_t status{};

			// Detail.
			std::string detail{};

			// Errors.
			nlohmann::json errors{};

			// Trace ID.
			std::string trace_id{};
		};

		/**
		 * @brief Convert a JSON to a topgg_request_error_t object.
		 * @param json The JSON to get the error from.
		 * @return A topgg_request_error_t object with the JSON's fields.
		 */
		request_error_t error_from_json(nlohmann::json const& json);

		struct requested_project_t {

			// The ID of the project on top.gg (NOT on Discord!).
			dpp::snowflake id{};

			// The name of the project on Discord.
			std::string name{};

			// The platform this project originates from.
			project_platforms platform{};

			// The type of the project.
			project_types type{};

			// The short description of the project on top.gg.
			std::string headline{};

			// Every tag of the project.
			std::vector <std::string> tags{};

			// The current amount of votes which counts towards ranking.
			uint64_t votes{};

			// The total number of votes this project had.
			uint64_t votes_total{};

			// The average score of all reviews summed.
			float review_score{};

			// The total number of reviews left on the project.
			uint64_t review_count{};
		};

		/**
		 * @brief Convert a JSON to a requested_project_t object.
		 * @param json The JSON to get the project from.
		 * @return A requested_project_t object with the JSON's fields.
		 */
		requested_project_t project_from_json(nlohmann::json const& json);

		struct vote_t {

			// The ID of the user who voted on top.gg (NOT Discord!).
			dpp::snowflake user_id{};

			// The ID of the user who voted on Discord.
			dpp::snowflake user_platform_id{};

			// The amount of points this vote is worth.
			uint8_t weight{};

			// When this vote was made.
			datetime created_at{};

			// When the user is able to vote again.
			datetime expires_at{};
		};

		/**
		 * @brief Convert a JSON to a vote_t object.
		 * @param json The JSON to get the vote from.
		 * @return A vote_t object with the JSON's fields.
		 */
		vote_t vote_from_json(nlohmann::json const& json);

		struct requested_votes_t {

			// A pagination cursor from a previous response. You can make a request with it again to get a page of votes.
			std::string cursor{};

			// A list of votes for the project.
			std::vector <vote_t> data{};
		};

		/**
		 * @brief Convert a JSON to a vote_t object.
		 * @param json The JSON to get the votes from.
		 * @return A requested_votes_t object with the JSON's fields.
		 */
		requested_votes_t votes_from_json(nlohmann::json const& json);

		struct vote_status_t {

			// When the vote was made.
			datetime created_at{};

			// When the user is able to vote again.
			datetime expires_at{};

			// How many points this vote is worth.
			uint8_t weight{};
		};

		/**
		 * @brief Convert a JSON to a vote_status_t object.
		 * @param json The JSON to get the vote status from.
		 * @return A vote_status_t object with the JSON's fields.
		 */
		vote_status_t vote_status_from_json(nlohmann::json const& json);

		using callback_data_t = std::variant <std::monostate, requested_project_t, requested_votes_t, vote_status_t>;

		struct request_completion_t {

			// An std::variant with the underlying value. Holds std::monostate if top.gg returned an error.
			callback_data_t value{};

			// An error object containing fields provided. The value of those that were not provided are set to zero.
			request_error_t error{};

			// The raw JSON object returned from top.gg.
			nlohmann::json raw_json;

			/**
			 * @brief See if top.gg returned an error.
			 * @return Whether top.gg returned an error.
			 */
			[[nodiscard]] bool is_error() const;

			/**
			 * @brief Get the underlying value as the specified type.
			 * @tparam T The type to get the value as.
			 * @return The value as the specified value.
			 */
			template <typename T>
			[[nodiscard]] T const& get() const {
				return std::get <T>(this->value);
			}

			// The underlying http request object.
			dpp::http_request_completion_t request;
		};

		using completion_event = std::function <void(request_completion_t const&)>;
		using slashcommand_array = std::vector <dpp::slashcommand>;
	}

	class poker {

		// Whether we initialised a default cluster (true) or attached to an existing one (false).
		bool default_cluster;

		// The cluster to poke with.
		dpp::cluster* poker_cluster;

		// The authorisation token to use when making requests to top.gg's v0 endpoints.
		std::string_view v0_token;

		// The authorisation token to use when making requests to top.gg's v1 endpoints, including "Bearer ".
		std::string v1_token;
	public:
		/**
		 * @brief Constructs a new poker. It will poke top.gg's API.
		 * @param token_arg The token to use. "Bearer " is attached by the constructor, so simply paste what top.gg shows you.
		 * @param poker_cluster_arg The cluster to attach the poker to. If left untouched or set to nullptr, a new one will be created.
		 */
		explicit poker(std::string_view token_arg, dpp::cluster const* poker_cluster_arg = nullptr);

		poker() = delete;
		poker(poker const&) = delete;
		poker(poker&&) = delete;

		void get_bots(v0::completion_event const& topgg_callback,
			uint16_t limit = 500, uint64_t offset = 0, bot_fields sort_field = sf_na, std::vector <bot_fields> const& fields = {});

		void get_stats(v0::completion_event const& topgg_callback, dpp::snowflake bot_id);

		void get_user_vote(v0::completion_event const& topgg_callback, dpp::snowflake bot_id, dpp::snowflake user_id);

		void post_stats(v0::completion_event const& topgg_callback, dpp::snowflake bot_id, uint64_t server_count);

		/**
		 * @brief Get stats of the current project from top.gg.
		 * @param topgg_callback The callback to call when a response is received.
		 */
		void get_current_project(v1::completion_event const& topgg_callback) const;

		/**
		 * @brief Update the commands for the current project, assuming it's a bot.
		 * @param commands An std::vector with the bot's slash commands (dpp::slashcommand objects).
		 * @param topgg_callback The callback to call when a response is received.
		 */
		void update_discord_bot_commands(v1::slashcommand_array const& commands, v1::completion_event const& topgg_callback);

		/**
		 * @brief Get votes of the current project via a pagination cursor.
		 * @param cursor The pagination cursor.
		 * @param topgg_callback The function to call when a response is received.
		 */
		void get_votes(std::string_view cursor, v1::completion_event const& topgg_callback) const;

		/**
		 * @brief Get votes of the current project since the specified date.
		 * @param start_date The date to search since. Up to a year ago is allowed.
		 * @param topgg_callback The function to call when a response is received.
		 */
		void get_votes(datetime const& start_date, v1::completion_event const& topgg_callback) const;

		/**
		 * @brief Get the vote status by specified user.
		 * @param user_id The user to check the status of.
		 * @param topgg_callback The callback to call when a response is received.
		 * @param user_source The source this user ID came from.
		 */
		void get_vote_status_by_user(dpp::snowflake user_id, v1::completion_event const& topgg_callback, user_sources user_source = us_topgg) const;

		/**
		 * @brief Get a pointer to the underlying cluster.
		 * @return A pointer to the underlying cluster.
		 */
		[[nodiscard]] dpp::cluster* get_cluster() const;

		/**
		 * @brief Start the underlying cluster and therefore enable HTTP requests.
		 * @param return_after dpp::st_wait (the default) to keep running in a loop or dpp::st_return to run once and continue execution.
		 */
		void start(dpp::start_type return_after = dpp::st_wait) const;

		/**
		 * @brief Shut down the underlying cluster and therefore the poker.
		 */
		void shutdown() const;

		~poker();
	};
}
