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

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <ios>
#include <sstream>
#include <tmx/apimessages/TmxEventLog.hpp>
#include <tmx/messages/routeable_message.hpp>
#include <tmx/tmx.h>
#include <tmx/IvpPlugin.h>
#include "TmxLog.h"
#include "UdpClient.h"

#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL "INFO"
#endif

#define DEFAULT_EVENTLOG_UDP_PORT 24625

#define FILE_NAME_MAX_WIDTH 32

namespace tmx {
namespace utils {

inline std::string NowTime();

#ifndef NO_EVENTLOG_UDP
static UdpClient client("localhost", DEFAULT_EVENTLOG_UDP_PORT);
#endif

template <typename T>
class PluginLog
{
public:
	PluginLog();
	virtual ~PluginLog();
	std::ostream &Get(LogLevel level = logINFO, std::string file = "Unknown", unsigned int line = -1, std::string plugin = "");

    static LogLevel& ReportingLevel();
    static FILE* &Stream();
    static std::string ToString(LogLevel level);
    static LogLevel FromString(const std::string& level);
    static IvpLogLevel ToEventLogLevel(LogLevel level);
    static LogLevel FromEventLogLevel(IvpLogLevel level);

    int MessageIndex();
protected:
	std::ostringstream os;
private:
	PluginLog(const PluginLog&);
	PluginLog &operator=(const PluginLog &);
	int msgIndex = 0;
	tmx::routeable_message *tmxMessage;

	T writer;
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
    writer.Output(os.str());

    if (tmxMessage)
    {
#ifndef NO_EVENTLOG_UDP
    	// Add the description to the event log message
    	tmx::messages::TmxEventLogMessage elm =
    			tmxMessage->get_payload<tmx::messages::TmxEventLogMessage>();
    	elm.set_description(&(os.str().c_str()[msgIndex]));
    	tmxMessage->set_payload(elm);
    	std::string msg = tmxMessage->to_string();
#endif

    	// Clean up the pointer
    	delete tmxMessage;
    	tmxMessage = NULL;

#ifndef NO_EVENTLOG_UDP
    	client.Send(msg);
#endif
    }
}

template <typename T>
std::ostream& PluginLog<T>::Get(LogLevel level, std::string file, unsigned int line, std::string plugin)
{
	static constexpr int fileMaxLen = FILE_NAME_MAX_WIDTH;

	writer.Visit(this, level, file, line, plugin);

	tmx::messages::TmxEventLogMessage elm;
	elm.set_level(ToEventLogLevel(level));

	tmxMessage = new tmx::routeable_message();

	if (plugin.empty())
	{
		plugin = file;
		size_t i = plugin.find_last_of('/');
		if (i < plugin.size())
			plugin = plugin.substr(i + 1);

		if (plugin.compare(plugin.size() - 4, 4, ".cpp") == 0)
			plugin = plugin.substr(0, plugin.size() - 4);
		else if (plugin.compare(plugin.size() - 2, 2, ".h") == 0 ||
				 plugin.compare(plugin.size() - 2, 2, ".c") == 0)
			plugin = plugin.substr(0, plugin.size() - 2);
	}

	tmxMessage->initialize(elm, plugin);

	time_t now = (time_t)(tmxMessage->get_timestamp() / 1000);
	short ms = tmxMessage->get_timestamp() % 1000;
	struct tm *myTm = localtime(&now);
	char tmBuffer[20];
	strftime(tmBuffer, 20, "%F %T", myTm);

	if (line > 0)
	{
		file.append(" (");
		file.append(std::to_string(line));
		file.append(")");
	}

	os << "[" << tmBuffer << "." << std::setfill('0') << std::setw(3) << ms << "] " << std::setfill(' ');
	if (file.length() > fileMaxLen)
		os << std::right << std::setw(fileMaxLen) << file.substr(file.size() - fileMaxLen);
	else
		os << std::right << std::setw(fileMaxLen) << file;

	os << " - " << std::left << std::setw(7) << ToString(level) << ": ";
	//os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
	msgIndex = os.str().length();
    return os;
}

template <typename T>
int PluginLog<T>::MessageIndex()
{
	return msgIndex;
}

template <typename T>
LogLevel& PluginLog<T>::ReportingLevel()
{
    static LogLevel reportingLevel = FromString(DEFAULT_LOG_LEVEL);
    return reportingLevel;
}

template <typename T>
FILE* &PluginLog<T>::Stream()
{
	return T::Stream();
}

template <typename T>
std::string PluginLog<T>::ToString(LogLevel level)
{
	static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
    return buffer[level];
}

template <typename T>
LogLevel PluginLog<T>::FromString(const std::string& level)
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
IvpLogLevel PluginLog<T>::ToEventLogLevel(LogLevel level)
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
LogLevel PluginLog<T>::FromEventLogLevel(IvpLogLevel level)
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
    void Output(const std::string& msg);
    void Visit(PluginLog<Output2FILE> *clazz, LogLevel level, std::string file, unsigned int line, std::string plugin);
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

inline void Output2FILE::Visit(PluginLog<Output2FILE> *clazz, LogLevel level, std::string file, unsigned int line, std::string plugin)
{
}

class Output2Syslog
{
public:
	static int ToSyslogLevel(LogLevel lvl);
	void Output(const std::string &msg);
	void Visit(PluginLog<Output2Syslog> *clazz, LogLevel level, std::string file, unsigned int line, std::string plugin);
private:
	int level;
	PluginLog<Output2Syslog> *logger;
};

typedef PluginLog<Output2FILE> FILELog;
typedef PluginLog<Output2Syslog> SYSLog;

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL tmx::utils::logDEBUG4
#endif

#define FILE_LOG(level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > tmx::utils::FILELog::ReportingLevel() || !tmx::utils::Output2FILE::Stream()) ; \
    else tmx::utils::FILELog().Get(level, __FILE__, __LINE__)
#define SYS_LOG(level, plugin) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > tmx::utils::SYSLog::ReportingLevel()) ; \
    else tmx::utils::SYSLog().Get(level, __FILE__, __LINE__, plugin)
#define PLUGIN_LOG(level, plugin) \
	    if (level > FILELOG_MAX_LEVEL) ;\
	    else if (level > tmx::utils::FILELog::ReportingLevel() || !tmx::utils::Output2FILE::Stream()) ; \
	    else tmx::utils::FILELog().Get(level, __FILE__, __LINE__, plugin)

#define PLOG(X) FILE_LOG(X)

}} // namespace tmx::utils

#endif /* SRC_PLUGINLOG_H_ */
