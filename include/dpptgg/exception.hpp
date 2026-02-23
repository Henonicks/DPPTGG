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

#include <string>
#include <exception>

namespace dpptgg {
	/**
	 * @brief Thrown when dpptgg::datetime tries to parse a timestamp that's not formatted like the top.gg ones.
	 */
	class timestamp_exception final : public std::exception {
		std::string error_message;
	public:
		timestamp_exception() = delete;

		/**
		 * @brief Construct a new timestamp_exception.
		 * @param timestamp The problematic timestamp.
		 * @param pos The position at which the issue arises.
		 */
		explicit timestamp_exception(std::string_view timestamp = "", size_t pos = std::string::npos);
		char const* what() const noexcept;
	};
}
