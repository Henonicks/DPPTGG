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


#include "dpptgg/exception.hpp"

dpptgg::timestamp_exception::timestamp_exception(std::string_view const timestamp, size_t const pos) {
	error_message = "Bad timestamp format";
	if (!timestamp.empty()) {
		error_message += std::string(" in `") + timestamp.data() + '`';
	}
	if (pos != std::string::npos) {
		error_message += " at position " + std::to_string(pos);
	}
}

char const* dpptgg::timestamp_exception::what() const noexcept {
	return error_message.c_str();
}
