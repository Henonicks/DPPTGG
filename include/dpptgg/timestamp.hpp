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

#include "dpptgg/exception.hpp"

#include <cstdint>

namespace dpptgg {
	struct datetime {
		uint16_t year{};
		uint16_t month{};
		uint16_t day{};
		uint16_t hour{};
		uint16_t minute{};
		uint16_t second{};
		uint32_t nanosecond{};
		int16_t  timezone_hours{};
		uint16_t timezone_minutes{};
		[[nodiscard]] std::string get_timestamp() const;
		[[nodiscard]] static datetime parse(std::string_view timestamp);
	};
}