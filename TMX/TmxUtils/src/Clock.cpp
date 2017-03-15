/*
 * Clock.cpp
 *
 *  Created on: Aug 30, 2016
 *      Author: ivp
 */

#include "Clock.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace std::chrono;

namespace tmx {
namespace utils {

template <typename C>
static string GetChronoClockInfo()
{
	ostringstream ss;

	ss << "\tPrecision: ";

	// If time unit is less than or equal to one millisecond.

	// Type of the time unit.
	typedef typename C::period P;
	if (ratio_less_equal<P,milli>::value)
	{
		// Convert to milliseconds.
		typedef typename ratio_multiply<P,kilo>::type TT;
		ss << fixed << double(TT::num)/TT::den << " milliseconds" << endl;
	}
	else
	{
		// Print as seconds.
		ss << fixed << double(P::num)/P::den << " seconds" << endl;
	}

	ss << "\tIs Steady: " << boolalpha << C::is_steady << endl;

	return ss.str();
}

void Clock::PrintInfoForChronoClocks()
{
	chrono::system_clock::time_point tp;
	cout << "system_clock" << endl;
	cout << GetChronoClockInfo<chrono::system_clock>();
	cout << "\tEpoch Local: " << ToLocalTimeString(tp) << endl;
	cout << "\tEpoch UTC:   " << ToUtcTimeString(tp) << endl;
	tp = chrono::system_clock::time_point::max();
	cout << "\tMax   UTC:   " << ToUtcTimeString(tp) << endl;
	tp = chrono::system_clock::now();
	cout << "\tNow Local: " << ToLocalTimeString(tp) << endl;
	cout << "\tNow UTC:   " << ToUtcTimeString(tp) << endl;

	cout << "high_resolution_clock" << endl;
	cout << GetChronoClockInfo<std::chrono::high_resolution_clock>();

	cout << "steady_clock" << endl;
	cout << GetChronoClockInfo<std::chrono::steady_clock>();
}

std::string Clock::ToLocalTimeString(const std::chrono::system_clock::time_point& tp)
{
    // Convert to system time.
    std::time_t t = chrono::system_clock::to_time_t(tp);
    // Convert to calendar time string.
    // Note: could have also called std:ctime(&t) - it's an alias.
    std::string calStr = std::asctime(localtime(&t));
    // Remove trailing newline.
    calStr.resize(calStr.size()-1);
    return calStr;
}

std::string Clock::ToUtcTimeString(const std::chrono::system_clock::time_point& tp)
{
    // Convert to system time.
    std::time_t t = chrono::system_clock::to_time_t(tp);
    // Convert to calendar time string.
    std::string calStr = std::asctime(gmtime(&t));
    // Remove trailing newline.
    calStr.resize(calStr.size()-1);
    return calStr;
}

std::string Clock::ToLocalPreciseTimeString(const std::chrono::system_clock::time_point& tp)
{
    std::time_t t = chrono::system_clock::to_time_t(tp);
	short ms = tp.time_since_epoch() / std::chrono::milliseconds(1) % 1000;
	struct tm *myTm = localtime(&t);
	char tmBuffer[20];
	strftime(tmBuffer, 20, "%F %T", myTm);

	ostringstream ss;
    ss << tmBuffer << "." << std::setfill('0') << std::setw(3) << ms;
    return ss.str();
}

std::string Clock::ToUtcPreciseTimeString(const std::chrono::system_clock::time_point& tp)
{
    std::time_t t = chrono::system_clock::to_time_t(tp);
	short ms = tp.time_since_epoch() / std::chrono::milliseconds(1) % 1000;
	struct tm *myTm = gmtime(&t);
	char tmBuffer[20];
	strftime(tmBuffer, 20, "%F %T", myTm);

	ostringstream ss;
    ss << tmBuffer << "." << std::setfill('0') << std::setw(3) << ms;
    return ss.str();
}

std::string Clock::ToTimeString(const std::chrono::milliseconds ms)
{
	// Split into hours, minutes, seconds, and milliseconds.
	hours   hh = duration_cast<hours>(ms);
	minutes mm = duration_cast<minutes>(ms % chrono::hours(1));
	seconds ss = duration_cast<seconds>(ms % chrono::minutes(1));
	milliseconds msec = duration_cast<milliseconds>(ms % chrono::seconds(1));

	ostringstream oss;

	oss << setfill('0')
		<< setw(2) << hh.count() << ":"
		<< setw(2) << mm.count() << ":"
		<< setw(2) << ss.count() << "."
		<< setw(3) << msec.count();

	return oss.str();
}

} /* namespace utils */
} /* namespace tmx */
