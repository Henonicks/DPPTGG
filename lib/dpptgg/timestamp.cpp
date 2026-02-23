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


#include "dpptgg/timestamp.hpp"

std::string dpptgg::datetime::get_timestamp() const {
	std::string res = std::to_string(year) + '-';
	if (month < 10) {
		res += '0';
	}
	res += std::to_string(month) + '-';
	if (day < 10) {
		res += '0';
	}
	res += std::to_string(day) + 'T';
	if (hour < 10) {
		res += '0';
	}
	res += std::to_string(hour) + ':';
	if (minute < 10) {
		res += '0';
	}
	res += std::to_string(minute) + ':';
	if (second < 10) {
		res += '0';
	}
	res += std::to_string(second) + '.';
	res += std::to_string(nanosecond) + (timezone_hours >= 0 ? '+' : '-');
	if (std::abs(timezone_hours) < 10) {
		res += '0';
	}
	res += std::to_string(timezone_hours) + ':';
	if (timezone_minutes < 10) {
		res += '0';
	}
	res += std::to_string(timezone_minutes);
	return res;
}

dpptgg::datetime dpptgg::datetime::parse(std::string_view const timestamp) {
	datetime res;
	size_t i = 0;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			res.year *= 10;
			res.year += timestamp[i] - '0';
		}
		else if (timestamp[i] != '-') {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			res.month *= 10;
			res.month += timestamp[i] - '0';
			if (res.month > 12) {
				throw timestamp_exception(timestamp, i);
			}
		}
		else if (timestamp[i] != '-' || res.month == 0) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			res.day *= 10;
			res.day += timestamp[i] - '0';
			switch (res.month) {
			case 1: case 3: case 5: case 8: case 10: case 12:
				if (res.day > 31) {
					throw timestamp_exception(timestamp, i);
				}
				break;
			case 2:
				if (res.day > 28 + (res.year % 4 == 0)) {
					throw timestamp_exception(timestamp, i);
				}
				break;
			case 4: case 6: case 7: case 9: case 11:
				if (res.day > 30) {
					throw timestamp_exception(timestamp, i);
				}
				break;
			default:
				throw timestamp_exception(timestamp, i);
			}
		}
		else if (timestamp[i] != 'T' || res.day == 0) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	res.hour = UINT16_MAX;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			if (res.hour == UINT16_MAX) {
				res.hour = 0;
			}
			else {
				res.hour *= 10;
			}
			res.hour += timestamp[i] - '0';
			if (res.hour > 24) {
				throw timestamp_exception(timestamp, i);
			}
		}
		else if (timestamp[i] != ':' || res.hour == UINT16_MAX) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	res.minute = UINT16_MAX;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			if (res.minute == UINT16_MAX) {
				res.minute = 0;
			}
			else {
				res.minute *= 10;
			}
			res.minute += timestamp[i] - '0';
			if (res.minute > 60) {
				throw timestamp_exception(timestamp, i);
			}
		}
		else if (timestamp[i] != ':' || res.minute == UINT16_MAX) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	res.second = UINT16_MAX;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			if (res.second == UINT16_MAX) {
				res.second = 0;
			}
			else {
				res.second *= 10;
			}
			res.second += timestamp[i] - '0';
			if (res.second > 60) {
				throw timestamp_exception(timestamp, i);
			}
		}
		else if (timestamp[i] != '.' || res.second == UINT16_MAX) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	res.nanosecond = UINT32_MAX;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			if (res.nanosecond == UINT32_MAX) {
				res.nanosecond = 0;
			}
			else {
				res.nanosecond *= 10;
			}
			res.nanosecond += timestamp[i] - '0';
		}
		else if (timestamp[i] != '+' || res.nanosecond == UINT16_MAX) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	bool const is_positive = timestamp[i] == '+';
	res.timezone_hours = INT16_MAX;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			if (res.timezone_hours == INT16_MAX) {
				res.timezone_hours = 0;
			}
			else {
				res.timezone_hours *= 10;
			}
			res.timezone_hours += timestamp[i] - '0';
			if (res.timezone_hours > 24) {
				throw timestamp_exception(timestamp, i);
			}
		}
		else if (timestamp[i] != ':' || res.timezone_hours == INT16_MAX) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	res.timezone_hours *= is_positive ? 1 : -1;
	if (i >= timestamp.size()) {
		throw timestamp_exception(timestamp);
	}
	res.timezone_minutes = UINT16_MAX;
	for (; i < timestamp.size(); i++) {
		if (timestamp[i] >= '0' && timestamp[i] <= '9') {
			if (res.timezone_minutes == UINT16_MAX) {
				res.timezone_minutes = 0;
			}
			else {
				res.timezone_minutes *= 10;
			}
			res.timezone_minutes += timestamp[i] - '0';
			if (res.timezone_minutes > 60) {
				throw timestamp_exception(timestamp, i);
			}
		}
		else {
			throw timestamp_exception(timestamp, i);
		}
	}
	return res;
}
