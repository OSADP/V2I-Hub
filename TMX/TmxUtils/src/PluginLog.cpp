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

}} // namespace tmx::utils
