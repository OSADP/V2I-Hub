/*
 * Clock.h
 *
 *  Created on: Aug 30, 2016
 *      Author: ivp
 */

#ifndef SRC_CLOCK_H_
#define SRC_CLOCK_H_

#include <chrono>
#include <string>

namespace tmx {
namespace utils {

class Clock {
public:
	/**
	 * Print information about the standard C++ clocks defined in std::chrono.
	 */
	static void PrintInfoForChronoClocks();

	/**
	 * Convert a time_point into a local calendar string.
	 * Example return format: Mon Sep 12 11:28:53 2016.
	 */
	static std::string ToLocalTimeString(const std::chrono::system_clock::time_point& tp);

	/**
	 * Convert a time_point into a UTC calendar string.
	 * Example return format: Mon Sep 12 11:28:53 2016.
	 */
	static std::string ToUtcTimeString(const std::chrono::system_clock::time_point& tp);

	/**
	 * Convert a time_point into a local calendar string using abbreviated format and fractional seconds.
	 * Example return format: 2016-09-12 11:28:53.773.
	 */
	static std::string ToLocalPreciseTimeString(const std::chrono::system_clock::time_point& tp);

	/**
	 * Convert a time_point into a UTC calendar string using abbreviated format and fractional seconds.
	 * Example return format: 2016-09-12 11:28:53.773.
	 */
	static std::string ToUtcPreciseTimeString(const std::chrono::system_clock::time_point& tp);

	/**
	 * Convert a chrono::milliseconds duration into a string formatted as hh:mm:ss.fff.
	 */
	static std::string ToTimeString(const std::chrono::milliseconds ms);
};

} /* namespace utils */
} /* namespace tmx */

#endif /* SRC_CLOCK_H_ */
