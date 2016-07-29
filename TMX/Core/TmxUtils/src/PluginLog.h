/*
 * PluginLog.h : A header-only logging implementation for use in the plugins
 *
 * Adapted from:
 * http://www.drdobbs.com/cpp/logging-in-c/201804215
 *
 * Logging In C++
 * By Petru Marginean, September 05, 2007
 *
 *  Created on: Mar 3, 2016
 *      Author: BAUMGARDNER
 */

#ifndef SRC_PLUGINLOG_H_
#define SRC_PLUGINLOG_H_

#include <cstdio>
#include <sstream>
#include <tmx/apimessages/TmxEventLog.hpp>
#include <tmx/messages/routeable_message.hpp>
#include <tmx/tmx.h>
#include <tmx/IvpPlugin.h>
#include "UdpClient.h"

#define DEFAULT_EVENTLOG_UDP_PORT 24625

namespace tmx {
namespace utils {

inline std::string NowTime();


enum PluginLogLevel
{
	logERROR,
	logWARNING,
	logINFO,
	logDEBUG,
	logDEBUG1,
	logDEBUG2,
	logDEBUG3,
	logDEBUG4
};

#ifndef NO_EVENTLOG_UDP
static UdpClient client("localhost", DEFAULT_EVENTLOG_UDP_PORT);
#endif

template <typename T>
class PluginLog
{
public:
	PluginLog();
	virtual ~PluginLog();
	std::ostream &Get(PluginLogLevel level = logINFO, std::string name = "", std::string plugin = "");

    static PluginLogLevel& ReportingLevel();
    static FILE* &Stream();
    static std::string ToString(PluginLogLevel level);
    static PluginLogLevel FromString(const std::string& level);
    static IvpLogLevel ToEventLogLevel(PluginLogLevel level);
    static PluginLogLevel FromEventLogLevel(IvpLogLevel level);
protected:
	std::ostringstream os;
private:
	PluginLog(const PluginLog&);
	PluginLog &operator=(const PluginLog &);
	int msgIndex = 0;
	tmx::routeable_message *tmxMessage;
};

template <typename T>
PluginLog<T>::PluginLog(): tmxMessage(NULL)
{
	os.setf(std::ios::boolalpha);
}

template <typename T>
PluginLog<T>::~PluginLog()
{
    os << std::endl;
    T::Output(os.str());

#ifndef NO_EVENTLOG_UDP
    if (tmxMessage)
    {
    	// Add the description to the event log message
    	tmx::messages::TmxEventLogMessage elm =
    			tmxMessage->get_payload<tmx::messages::TmxEventLogMessage>();
    	elm.set_description(&(os.str().c_str()[msgIndex]));
    	tmxMessage->set_payload(elm);
    	std::string msg = tmxMessage->to_string();

    	// Clean up the pointer
    	delete tmxMessage;
    	tmxMessage = NULL;

    	client.Send(msg);
    }
#endif
}

template <typename T>
std::ostream& PluginLog<T>::Get(PluginLogLevel level, std::string prefix, std::string plugin)
{
	tmx::messages::TmxEventLogMessage elm;
	elm.set_level(ToEventLogLevel(level));

	tmxMessage = new tmx::routeable_message();
	if (plugin.empty())
		tmxMessage->initialize(elm, prefix);
	else
		tmxMessage->initialize(elm, plugin);

    os << "[" << NowTime() << "] ";
    os << prefix << " " << ToString(level) << ": ";
    os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
    msgIndex = os.str().length();
    return os;
}

template <typename T>
PluginLogLevel& PluginLog<T>::ReportingLevel()
{
    static PluginLogLevel reportingLevel = logINFO;
    return reportingLevel;
}

template <typename T>
FILE* &PluginLog<T>::Stream()
{
	return T::Stream();
}

template <typename T>
std::string PluginLog<T>::ToString(PluginLogLevel level)
{
	static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
    return buffer[level];
}

template <typename T>
PluginLogLevel PluginLog<T>::FromString(const std::string& level)
{
    if (level == "DEBUG4")
        return logDEBUG4;
    if (level == "DEBUG3")
        return logDEBUG3;
    if (level == "DEBUG2")
        return logDEBUG2;
    if (level == "DEBUG1")
        return logDEBUG1;
    if (level == "DEBUG")
        return logDEBUG;
    if (level == "INFO")
        return logINFO;
    if (level == "WARNING")
        return logWARNING;
    if (level == "ERROR")
        return logERROR;
    PluginLog<T>().Get(logWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return logINFO;
}

template <typename T>
IvpLogLevel PluginLog<T>::ToEventLogLevel(PluginLogLevel level)
{
	switch(level)
	{
	case logDEBUG:
	case logDEBUG1:
	case logDEBUG2:
	case logDEBUG3:
	case logDEBUG4:
		return IvpLogLevel_debug;
	case logINFO:
		return IvpLogLevel_info;
	case logWARNING:
		return IvpLogLevel_warn;
	case logERROR:
		return IvpLogLevel_error;
	}
	return IvpLogLevel_info;
}

template <typename T>
PluginLogLevel PluginLog<T>::FromEventLogLevel(IvpLogLevel level)
{
	switch (level)
	{
	case IvpLogLevel_debug:
		return logDEBUG;
	case IvpLogLevel_info:
		return logINFO;
	case IvpLogLevel_warn:
		return logWARNING;
	case IvpLogLevel_error:
	case IvpLogLevel_fatal:
		return logERROR;
	}
	return logINFO;
}

class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
    static FILE* pStream = stdout;
    return pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

class Output2EventLog
{
public:
	static FILE*& Stream();
	static void Output(const std::string &msg);
};

inline FILE* &Output2EventLog::Stream()
{
	return Output2FILE::Stream();
}

typedef PluginLog<Output2FILE> FILELog;

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logDEBUG4
#endif

#define FILE_LOG(prefix, level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > tmx::utils::FILELog::ReportingLevel() || !tmx::utils::Output2FILE::Stream()) ; \
    else tmx::utils::FILELog().Get(level, prefix)
#define PLUGIN_LOG(prefix, level, plugin) \
	    if (level > FILELOG_MAX_LEVEL) ;\
	    else if (level > tmx::utils::FILELog::ReportingLevel() || !tmx::utils::Output2FILE::Stream()) ; \
	    else tmx::utils::FILELog().Get(level, prefix, plugin)

#include <sys/time.h>

inline std::string NowTime()
{
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
    return result;
}

}} // namespace tmx::utils

#endif /* SRC_PLUGINLOG_H_ */
