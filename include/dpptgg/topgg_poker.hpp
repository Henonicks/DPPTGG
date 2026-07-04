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
	constexpr auto no_conversion = nullptr;

	struct v0 {
		inline static std::string const BASE_API_URL = "https://top.gg/api/";

		struct request_error_t {

			// The HTTP status code.
			uint16_t status{};
		};

		/**
		 * @brief Convert a JSON to a topgg_request_error_t object.
		 * @param json The JSON to get the error from.
		 * @return A topgg_request_error_t object with the JSON's fields.
		 */
		static request_error_t error_from_json(nlohmann::json const& json);

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

		/**
		 * @brief Convert a JSON to a bot_t object.
		 * @param json The JSON to get the bot from.
		 * @return A bot_t object with the JSON's fields.
		 */
		static bot_t bot_from_json(nlohmann::json const& json);

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

		/**
		 * @brief Convert a JSON to a requested_bots_t object.
		 * @param json The JSON to get the bots from.
		 * @return A requested_bots_t object with the JSON's fields.
		 */
		static requested_bots_t bots_from_json(nlohmann::json const& json);

		struct vote_t {

			// The username of the user who voted.
			std::string username{};

			// The ID of the user who voted on Discord.
			dpp::snowflake id{};

			// The hash of the avatar of the user who voted.
			std::string avatar{};
		};

		using server_count_t = uint64_t;

		static server_count_t server_count_from_json(nlohmann::json const& json);

		using voted_state_t = bool;

		static voted_state_t voted_state_from_json(nlohmann::json const& json);

		using callback_data_t = std::variant <std::monostate, requested_bots_t, server_count_t, voted_state_t>;

		struct request_completion_t {

			// An std::variant with the underlying value. Holds std::monostate if top.gg returned an error.
			callback_data_t value{};

			// An error object which contains the HTTP status.
			request_error_t error{};

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
	};

	struct v1 {
		inline static std::string const BASE_API_URL = "https://top.gg/api/v1/";

		struct request_error_t {

			// A URI identifying the error type.
			std::string type{};

			// A short human-readable error message.
			std::string title{};

			// The HTTP status code.
			uint16_t status{};

			// A human-readable detailed error message.
			std::string detail{};
		};

		/**
		 * @brief Convert a JSON to a topgg_request_error_t object.
		 * @param json The JSON to get the error from.
		 * @return A topgg_request_error_t object with the JSON's fields.
		 */
		static request_error_t error_from_json(nlohmann::json const& json);

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
		static requested_project_t project_from_json(nlohmann::json const& json);

		// An unordered map keyed by locale strings.
		using headline_map_t = std::unordered_map <std::string, std::string>;

		// An unordered map keyed by locale strings.
		using content_map_t = std::unordered_map <std::string, std::string>;

		struct announcement_t {
			// The title of the announcement.
			std::string title{};

			// The announcement content.
			std::string content{};

			// The timestamp of when the announcement was created.
			datetime created_at{};
		};

		/**
		 * @brief Convert a JSON to a requested_project_t object.
		 * @param json The JSON to get the announcement from.
		 * @return An announcement_t object with the JSON's fields.
		 */
		static announcement_t announcement_from_json(nlohmann::json const& json);

		using server_count_t = uint64_t;
		using shard_count_t = int;

		using member_count_t = uint64_t;
		using online_count_t = uint64_t;

		// Discord bot metrics struct
		struct bot_metrics_t {
			server_count_t server_count{};
			shard_count_t shard_count{};
		};

		// Discord server metrics struct
		struct server_metrics_t {
			member_count_t member_count{};
			online_count_t online_count{};
		};

		// Roblox isn't (at least currently) supported.

		struct timestamped_bot_metrics_t {
			datetime timestamp{};
			bot_metrics_t metrics{};
		};

		struct timestamped_server_metrics_t {
			datetime timestamp{};
			server_metrics_t metrics{};
		};

		using bot_metrics_batch_t = std::vector <timestamped_bot_metrics_t>;
		using server_metrics_batch_t = std::vector <timestamped_server_metrics_t>;

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
		static vote_t vote_from_json(nlohmann::json const& json);

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
		static requested_votes_t votes_from_json(nlohmann::json const& json);

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
		static vote_status_t vote_status_from_json(nlohmann::json const& json);

		using callback_data_t = std::variant <std::monostate, requested_project_t, announcement_t, requested_votes_t, vote_status_t>;

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
	};

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
		explicit poker(std::string_view token_arg, dpp::cluster* poker_cluster_arg = nullptr);

		poker() = delete;
		poker(poker const&) = delete;
		poker(poker&&) = delete;

		template <typename api_version>
		[[nodiscard]] std::string_view get_token() const {
			if constexpr (std::is_same_v <api_version, v0>) {
				return this->v0_token;
			}
			else if constexpr (std::is_same_v <api_version, v1>) {
				return this->v1_token;
			}
			else {
				static_assert(false, "This API version doesn't exist or the type used isn't defined in the dpptgg namespace.");
				return "";
			}
		}

		template <auto conversion_rule, typename api_version>
		void poke(std::string_view const path, dpp::http_method const method, typename api_version::completion_event const& topgg_callback, std::string_view const post_data = "") const {
			this->poker_cluster->request(api_version::BASE_API_URL + path.data(), method, [topgg_callback](dpp::http_request_completion_t const& request){
				typename api_version::request_completion_t callback = {
					.request = request
				};
				try {
					callback.raw_json = nlohmann::json::parse(request.body);
					if (request.status / 100 != 2) {
						callback.error = api_version::error_from_json(callback.raw_json);
					}
					else {
						if constexpr (conversion_rule != no_conversion) {
							callback.value = conversion_rule(callback.raw_json);
						}
					}
				}
				catch (...) {
					if (request.status / 100 != 2) {
						callback.error.status = request.status;
					}
				}
				topgg_callback(callback);
			}, post_data.data(), application_json, {
				{"Authorization", this->get_token <api_version>().data()},
			});
		}

		/// ---------- V0

		/**
		 * https://docs.top.gg/api/v0/bots#get-/bots
		 * @brief Get a list of bots like the one you get by visiting the index top.gg page.
		 * @param topgg_callback The callback to call when a response is received. value has dpptgg::v0::server_count_t as the underlying type upon success, holds std::monostate otherwise..
		 * @param limit The amount of bots to return. 50 by default, 500 max.
		 * @param offset The amount of bots to skip.
		 * @param sort_field (Doesn't seem to be functional) The field to sort by. Multiply your enum value by -1 to reverse the order.
		 * @param fields (Doesn't seem to be functional) The fields to show and fill in.
		 */
		void get_bots(v0::completion_event const& topgg_callback,
			uint16_t limit = 50, uint64_t offset = 0, bot_fields sort_field = sf_na, std::vector <bot_fields> const& fields = {}) const;

		/**
		 * https://docs.top.gg/api/v0/bots#get-/bots/bot_id/stats
		 * @brief Get the amount of servers the bot is in.
		 * @param topgg_callback The callback to call when a response is received. value has dpptgg::v0::server_count_t as the underlying type upon success, holds std::monostate otherwise.
		 */
		void get_server_count(v0::completion_event const& topgg_callback) const;

		/**
		 * https://docs.top.gg/api/v0/bots#get-/bots/bot_id/check
		 * @brief Get the vote status of a user (whether they have voted within the last 12 hours or not).
		 * @param topgg_callback The callback to call when a response is received. value has dpptgg::v0::vote_status_t as the underlying type upon success, holds std::monostate otherwise.
		 * @param user_id The Discord user ID of the user to get the vote status of.
		 * @deprecated v0 function. This only tells you whether the user is unable to vote or not. Use get_vote_status_by_user instead.
		 */
		[[deprecated]] void get_user_vote(v0::completion_event const& topgg_callback, dpp::snowflake user_id) const;

		/**
		 * https://docs.top.gg/api/v0/bots#post-/bots/bot_id/stats
		 * @brief Post a number to have top.gg display as your bot's server count.
		 * @param topgg_callback The callback to call when a response is received. value has dpptgg::v0::server_count_t as the underlying type upon success, holds std::monostate otherwise.
		 * @param server_count The amount of servers to tell top.gg your bot is in.
		 */
		void post_server_count(v0::completion_event const& topgg_callback, uint64_t server_count) const;

		/// ---------- V1

		/**
		 * https://docs.top.gg/api/v1/projects#get-/projects/@me
		 * @brief Get stats of the current project from top.gg.
		 * @param topgg_callback The callback to call when a response is received. value has dpptgg::v1::requested_project_t as the underlying type upon success, holds std::monostate otherwise.
		 */
		void get_current_project(v1::completion_event const& topgg_callback = {}) const;

		/**
		 * https://docs.top.gg/api/v1/projects#patch-/projects/@me
		 * @brief Update the current project's headline and/or content, supporting translations. Each argument map can be empty to leave the respective value untouched.
		 * @param headline The headline to set, supports translations, hence the keying by locales. Example headline translation: {"en", "English headline"}. 3-140 characters allowed.
		 * @param page_content The page content to set, supports translation, hence the keying by locales. Example content translation: {"en", "English content"}. 300-50'000 characters allowed.
		 * @param topgg_callback The function to call when a response is received. value always holds std::monostate as top.gg returns 204 (No Content) as a successful response on this endpoint.
		 */
		void update_current_project(v1::headline_map_t const& headline, v1::content_map_t const& page_content, v1::completion_event const& topgg_callback = {}) const;

#ifdef topgg_v1_minigap // This way the comment documentation doesn't clash with the next defined function so clangd doesn't give me a warning anymore

		/**
		 * @brief Create an announcement for your project.
		 * @param title The title of the announcement, 3-100 characters allowed.
		 * @param content The announcement content, 10-2'000 characters allowed.
		 * @param category The announcement category, ac_announcement if omitted.
		 * @param topgg_callback The function to call when a response is received. value has dpptgg::v1::announcement_t as the underlying type upon success, holds std::monostate otherwise.
		 */
		void create_announcement(std::string_view title, std::string_view content, announcement_categories category = ac_announcement, v1::completion_event const& topgg_callback = {}) const;

		/**
		 * @brief Update your project's metrics. Use this overload if your project is a Discord bot.
		 * @param metrics The bot's metrics - its server and shard count. Unedited values default to 0.
		 * @param topgg_callback The function to call when a response is received. value always holds std::monostate as top.gg returns 204 (No Content) as a successful response on this endpoint.
		 * @deprecated Doesn't appear to be functional. Looking into it.
		 */
		[[deprecated]] void update_project_metrics(v1::bot_metrics_t const& metrics, v1::completion_event const& topgg_callback = {}) const;

		/**
		 * @brief Update your project's metrics. Use this overload if your project is a Discord server.
		 * @param metrics The server's metrics - its total and currently online member count. Unedited values default to 0.
		 * @param topgg_callback The function to call when a response is received. value always holds std::monostate as top.gg returns 204 (No Content) as a successful response on this endpoint.
		 * @deprecated Doesn't appear to be functional. Looking into it.
		 */
		[[deprecated]] void update_project_metrics(v1::server_metrics_t const& metrics, v1::completion_event const& topgg_callback = {}) const;

		/**
		 * @brief Update your project's metrics by submitting a batch of them. Use this overload if your project is a Discord bot.
		 * @param data The batch of metrics entries. 1-100 entries allowed.
		 * @param topgg_callback The function to call when a response is received. value always holds std::monostate as top.gg returns 204 (No Content) as a successful response on this endpoint.
		 * @deprecated Doesn't appear to be functional. Looking into it.
		 */
		[[deprecated]] void update_project_metrics_batch(v1::bot_metrics_batch_t const& data, v1::completion_event const& topgg_callback = {}) const;

		/**
		 * @brief Update your project's metrics by submitting a batch of them. Use this overload if your project is a Discord server.
		 * @param data The batch of metrics entries. 1-100 entries allowed.
		 * @param topgg_callback The function to call when a response is received. value always holds std::monostate as top.gg returns 204 (No Content) as a successful response on this endpoint.
		 * @deprecated Doesn't appear to be functional. Looking into it.
		 */
		[[deprecated]] void update_project_metrics_batch(v1::server_metrics_batch_t const& data, v1::completion_event const& topgg_callback = {}) const;

		/// DYSFUNCTIONAL ENDPOINTS AREN'T INCLUDED

#endif

		/**
		 * https://docs.top.gg/api/v1/projects#put-/projects/@me/commands
		 * @brief Update the commands for the current project, assuming it's a bot.
		 * @param commands An std::vector with the bot's slash commands (dpp::slashcommand objects).
		 * @param topgg_callback The function to call when a response is received. value always holds std::monostate as top.gg returns 204 (No Content) as a successful response on this endpoint.
		 */
		void update_discord_bot_commands(v1::slashcommand_array const& commands, v1::completion_event const& topgg_callback = {}) const;

		/**
		 * https://docs.top.gg/api/v1/votes#get-/projects/@me/votes
		 * @brief Get votes of the current project via a pagination cursor.
		 * @param cursor The pagination cursor.
		 * @param topgg_callback The function to call when a response is received. value has dpptgg::v1::requested_votes_t as the underlying type upon success, holds std::monostate otherwise.
		 */
		void get_votes(std::string_view cursor, v1::completion_event const& topgg_callback = {}) const;

		/**
		 * https://docs.top.gg/api/v1/votes#get-/projects/@me/votes
		 * @brief Get votes of the current project since the specified date.
		 * @param start_date The date to search since. Up to a year ago is allowed.
		 * @param topgg_callback The function to call when a response is received. value has dpptgg::v1::requested_votes_t as the underlying type upon success, holds std::monostate otherwise.
		 */
		void get_votes(datetime const& start_date, v1::completion_event const& topgg_callback) const;

		/**
		 * https://docs.top.gg/api/v1/votes#get-/projects/@me/votes/user_id
		 * @brief Get the vote status by specified user.
		 * @param user_id The user to check the status of.
		 * @param topgg_callback The callback to call when a response is received. value has dpptgg::v1::vote_status_t as the underlying type upon success, holds std::monostate otherwise.
		 * @param user_source The source this user ID came from. top.gg by default (therefore the top.gg ID is to be used, not the Discord one! Unless you explicitly set it to us_discord).
		 */
		void get_vote_status_by_user(dpp::snowflake user_id, v1::completion_event const& topgg_callback = {}, user_sources user_source = us_topgg) const;

		/// ---------- misc or something

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
