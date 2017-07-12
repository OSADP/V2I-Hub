/*
 * PluginLog.cpp
 *
 *  Created on: Oct 20, 2016
 *      Author: ivp
 */

#include "PluginLog.h"

#include <syslog.h>

namespace tmx {
namespace utils {

int Output2Syslog::ToSyslogLevel(LogLevel lvl)
{
	switch (lvl)
	{
	case logDEBUG:
	case logDEBUG1:
	case logDEBUG2:
	case logDEBUG3:
	case logDEBUG4:
		return LOG_DEBUG;
	case logINFO:
		return LOG_INFO;
	case logWARNING:
		return LOG_WARNING;
	case logERROR:
		return LOG_ERR;
	default:
		return LOG_NOTICE;
	}
}

void Output2Syslog::Output(const std::string &msg)
{
	syslog(level, "%s", &msg.c_str()[logger ? logger->MessageIndex() : 0]);
}

void Output2Syslog::Visit(PluginLog<Output2Syslog> *clazz, LogLevel lvl, std::string file, unsigned int line, std::string plugin)
{
	openlog(plugin.c_str(), LOG_NDELAY, LOG_USER);
	level = ToSyslogLevel(lvl);
	logger = clazz;
}

std::ostream &_logtime(std::ostream &os, uint64_t timestamp)
{
	struct timeval tv;

	if (timestamp == 0)
	{
		gettimeofday(&tv, NULL);
	}
	else
	{
		tv.tv_sec = timestamp / 1000;
		tv.tv_usec = timestamp % 1000 * 1000;
	}

	time_t time = tv.tv_sec;
	short ms = tv.tv_usec / 1000;

	struct tm *myTm = localtime(&time);
	char tmBuffer[20];
	strftime(tmBuffer, 20, "%F %T", myTm);

	os << "[" << tmBuffer << "." << std::setfill('0') << std::setw(3) << ms << "] " << std::setfill(' ');

	return os;
}

std::ostream &_logsource(std::ostream &os, std::string file, unsigned int line)
{
	static constexpr int fileMaxLen = FILE_NAME_MAX_WIDTH;

	if (line > 0)
	{
		file.append(" (");
		file.append(std::to_string(line));
		file.append(")");
	}

	if (file.length() > fileMaxLen)
		os << std::right << std::setw(fileMaxLen) << file.substr(file.size() - fileMaxLen);
	else
		os << std::right << std::setw(fileMaxLen) << file;

	return os;
}

std::ostream &_loglevel(std::ostream &os, LogLevel level)
{
	os << " - " << std::left << std::setw(7) << FILELog::ToString(level) << ": ";
	return os;
}

}} // namespace tmx::utils
