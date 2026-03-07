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

#include <ctime>
#include <iostream>

std::string dpptgg::datetime::get_timestamp() const {
	uint16_t
		fixed_year = this->year,
		fixed_month = this->month,
		fixed_day = this->day,
		fixed_hour = this->hour,
		fixed_minute = this->minute,
		fixed_second = this->second,
		fixed_nanosecond = this->nanosecond;
	fixed_hour += this->timezone_hours;
	fixed_minute += this->timezone_minutes;
	if (fixed_minute >= 60) {
		fixed_minute = 60 - fixed_minute;
		++fixed_hour;
	}
	else if (fixed_minute < 0) {
		fixed_minute += 60;
		--fixed_hour;
	}
	if (fixed_hour >= 24) {
		fixed_hour = 24 - fixed_minute;
		++fixed_day;
	}
	else if (fixed_minute < 0) {
		fixed_hour += 24;
		--fixed_day;
	}
	switch (fixed_month) {
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		if (fixed_day > 31) {
			fixed_day -= 31;
			++fixed_month;
		}
		else if (fixed_day < 1) {
			fixed_day += 31;
			--fixed_month;
		}
		break;
	case 2:
		if (fixed_day > 28 + (fixed_year % 4 == 0)) {
			fixed_day -= 28 + (fixed_year % 4 == 0);
			++fixed_month;
		}
		else if (fixed_day < 1) {
			fixed_day += 28 + (fixed_year % 4 == 0);
			--fixed_month;
		}
		break;
	case 4: case 6: case 9: case 11:
		if (fixed_day > 30) {
			fixed_day -= 30;
			++fixed_month;
		}
		else if (fixed_day < 1) {
			fixed_day += 30;
			--fixed_month;
		}
		break;
	default: break;
	}
	if (fixed_month > 12) {
		fixed_month -= 12;
		++fixed_year;
	}
	else if (fixed_month < 1) {
		fixed_month += 12;
		--fixed_year;
	}
	std::string res = std::to_string(fixed_year) + '-';
	if (fixed_month < 10) {
		res += '0';
	}
	res += std::to_string(fixed_month) + '-';
	if (fixed_day < 10) {
		res += '0';
	}
	res += std::to_string(fixed_day) + 'T';
	if (fixed_hour < 10) {
		res += '0';
	}
	res += std::to_string(fixed_hour) + ':';
	if (fixed_minute < 10) {
		res += '0';
	}
	res += std::to_string(fixed_minute) + ':';
	if (fixed_second < 10) {
		res += '0';
	}
	res += std::to_string(fixed_second) + '.';
	res += std::to_string(fixed_nanosecond);
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
			case 1: case 3: case 5: case 7: case 8: case 10: case 12:
				if (res.day > 31) {
					throw timestamp_exception(timestamp, i);
				}
				break;
			case 2:
				if (res.day > 28 + (res.year % 4 == 0)) {
					throw timestamp_exception(timestamp, i);
				}
				break;
			case 4: case 6: case 9: case 11:
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
		else if ((timestamp[i] != '+' && timestamp[i] != '-' && timestamp[i] != 'Z') || res.nanosecond == UINT16_MAX) {
			throw timestamp_exception(timestamp, i);
		}
		else {
			++i;
			break;
		}
	}
	--i;
	bool is_positive;
	if (timestamp[i] == '+') {
		is_positive = true;
	}
	else if (timestamp[i] == '-') {
		is_positive = false;
	}
	else if (timestamp[i] != 'Z' || timestamp.size() - 1 > i) {
		throw timestamp_exception(timestamp, i);
	}
	else {
		return res;
	}
	size_t const leftover_size = timestamp.size() - (i + 1);
	if (leftover_size < 4 || leftover_size > 5) {
		throw timestamp_exception(timestamp, timestamp.size() - 1);
	}
	std::string_view const timezone_str = timestamp.substr(i + 1, leftover_size);
	if (!isdigit(timezone_str[0])) {
		throw timestamp_exception(timestamp, i + 1);
	}
	res.timezone_hours = timezone_str[0] + '0';
	if (!isdigit(timezone_str[1])) {
		throw timestamp_exception(timestamp, i + 2);
	}
	res.timezone_hours = res.timezone_hours * 10 + timezone_str[0] + '0';
	if (!isdigit(timezone_str[2]) && timezone_str[2] != ':') {
		throw timestamp_exception(timestamp, i + 3);
	}
	uint8_t digit_pos = isdigit(timezone_str[2]) ? 2 : 3;
	if (!isdigit(timezone_str[digit_pos])) {
		throw timestamp_exception(timestamp, i + digit_pos + 1);
	}
	res.timezone_minutes = timezone_str[digit_pos] + '0';
	if (!isdigit(timezone_str[digit_pos + 1])) {
		throw timestamp_exception(timestamp, i + digit_pos + 2);
	}
	res.timezone_minutes = res.timezone_minutes * 10 + timezone_str[digit_pos + 1] + '0';
	res.timezone_hours *= is_positive ? 1 : -1;
	res.timezone_minutes *= is_positive ? 1 : -1;
	return res;
}
