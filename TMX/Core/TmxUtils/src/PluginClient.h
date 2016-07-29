/*
 * @file PluginClient.h
 *
 *  Created on: Feb 25, 2016
 *      Author: ivp
 */

#ifndef SRC_PLUGINCLIENT_H_
#define SRC_PLUGINCLIENT_H_

#include <atomic>
#include <map>
#include <mutex>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <tmx/apimessages/TmxEventLog.hpp>
#include <tmx/messages/routeable_message.hpp>
#include <tmx/messages/TmxJ2735.hpp>
#include <tmx/IvpPlugin.h>

#include "PluginLog.h"

#define PLOG(level) PLUGIN_LOG(_logPrefix, level, _name)
//#define PLOG(level) FILE_LOG(_logPrefix, level)

#define USAGE "Usage"
#define LOG_LEVEL_CFG "TMXLogLevel"
#ifdef TMX_DEBUG
#define DEFAULT_LOG_LEVEL "DEBUG"
#else
#define DEFAULT_LOG_LEVEL "INFO"
#endif

namespace tmx {
namespace utils {

// C++ wrapper for an ivpapi plugin.
// If the ivpapi is rewritten in C++, this class will be moved to the API, and it will
// no longer be a wrapper, but a first class citizen.
class PluginClient {
public:
	PluginClient(std::string name);
	virtual ~PluginClient();

	/// Static map used to track which PluginClient instance goes with which IvpPlugin* created.
	/// This allows the static callback functions below to call the instance virtual callback functions.
	static std::map<IvpPlugin*, PluginClient*> _instanceMap;

	static void StaticOnConfigChanged(IvpPlugin *plugin, const char *key, const char *value);
	static void StaticOnError(IvpPlugin *plugin, IvpError err);
	static void StaticOnMessageReceived(IvpPlugin *plugin, IvpMessage *msg);
	static void StaticOnStateChange(IvpPlugin *plugin, IvpPluginState state);

	void AddMessageFilter(const char *type, const char *subtype, IvpMsgFlags flags = IvpMsgFlags_None);
	void SubscribeToMessages();

	/// @return The name given to the plugin
	inline std::string GetName() { return _name; }

	template <typename MsgType, class HandlerType>
	void AddMessageFilter(HandlerType *plugin, void (HandlerType::*handler)(MsgType &, tmx::routeable_message &) = 0)
	{
		typedef MsgType msg_type;

		AddMessageFilter(MsgType::MessageType, MsgType::MessageSubType);

		std::string typeName(MsgType::MessageType);
		std::string subTypeName(MsgType::MessageSubType);

		static msg_type msgType;

		PLOG(logINFO) << "Registering message type " <<
				battelle::attributes::type_id_name<msg_type>() <<
				" for receiving message Type=" << typeName <<
				", SubType=" << subTypeName;

		if (handler)
			this->register_handler(plugin, typeName, subTypeName, handler);
		else
			this->register_handler(plugin, typeName, subTypeName, &HandlerType::handleMessage);
	}

	/// Broadcast a message to TMX core and optionally route it out the DSRC radio.
	/// @param message The message to send.  The message must be of type tmx::message
	/// and must have static char* members called MessageType and MessageSubType.
	/// @param source The source of the message, defaults to empty string
	/// @param sourceId The source ID of the message, defaults to 0
	/// @param flags Bit-or'ed flags.  Specify IvpMsgFlags_RouteDSRC to send the message out the DSRC radio.
	template <typename MsgType>
	void BroadcastMessage(MsgType& message, std::string source = "", unsigned int sourceId = 0, IvpMsgFlags flags = IvpMsgFlags_None)
	{
		tmx::routeable_message routeableMsg;
		routeableMsg.initialize(message, source, sourceId, flags);
		BroadcastMessage(routeableMsg);
	}

	/// Broadcast a given message to TMX core.  The message is routed verbatim.
	/// @param routeableMsg The TMX routeable message to send
	void BroadcastMessage(tmx::routeable_message &routeableMsg)
	{
		PLOG(logDEBUG2) << "Sending: " << routeableMsg;

		IvpMessage *ivpMsg = routeableMsg.get_message();
		ivp_broadcastMessage(_plugin, ivpMsg);
		ivpMsg_destroy(ivpMsg);
		ivpMsg = NULL;
	}

	/// Function for determining if a IvpMessage received from the core is a specific type.
	/// Validates the type category and subtype category.
	/// @param message The message to compare against the message type specified.
	/// @return true if the message is the message type specified.
	template <typename MsgType>
	bool IsMessageOfType(IvpMessage *message)
	{
		return (message->type && strcmp(message->type, MsgType::MessageType) == 0 &&
				message->subtype && strcmp(message->subtype, MsgType::MessageSubType) == 0);
	}

	/// Function for determining if a routeable_message received from the core is a specific type.
	/// Validates the type category and subtype category.
	/// @param message The routeable message to compare against the message type specified.
	/// @return true if the message is the message type specified.
	template <typename MsgType>
	bool IsMessageOfType(tmx::routeable_message &message)
	{
		return (message.get_type().compare(MsgType::MessageType) == 0 &&
				message.get_subtype().compare(MsgType::MessageSubType) == 0);
	}

	template <typename MsgType>
	bool IsJ2735Message(MsgType &message)
	{
		return message.get_type().compare(TMX_J2735_MESSAGE_TYPE) == 0;
	}

	/// Main method of the plugin that should not return until the plugin exits.
	virtual int Main();

	/// Handle an exception thrown in the plugin.  The requirement is to log the message
	/// in the event log.  By default, the program also terminates
	/// @param ex The exception to handle
	/// @param abort Terminate the process.  Default is true.
	void HandleException(std::exception &ex, bool abort = true);
protected:
	virtual void OnConfigChanged(const char *key, const char *value);
	virtual void OnError(IvpError err);
	virtual void OnMessageReceived(IvpMessage *msg);
	virtual void OnStateChange(IvpPluginState state);

	// The default message handler. This logs an error for an unhandled message, but
	// also serves as the prototype function declaration for message handlers.
	// @param The specific message payload
	// @param The source TMX routeable message from which the payload was taken
	void handleMessage(tmx::message &msg, tmx::routeable_message &src);

	// Get a configuration value for this plugin.
	// @param key The name of the configuration value.
	// @param value The returned value.
	// @param lock If non-NULL, this mutex is locked while value is set.
	// @return true on success; false if the value could not be retrieved.
	template <typename T>
	bool GetConfigValue(const std::string &key, T &value, std::mutex *lock = NULL)
	{
		bool success = false;
		char *text = ivp_getCopyOfConfigurationValue(_plugin, key.c_str());

		if (lock != NULL)
			lock->lock();

		if (text != NULL)
		{
			try
			{
				value = boost::lexical_cast<T>(text);
				free(text);
				success = true;
			}
			catch (boost::bad_lexical_cast const &ex)
			{
				PLOG(logERROR) << "Unable to convert config value from \"" << text << "\": " << ex.what();
				success = false;
			}
		}

		if (lock != NULL)
			lock->unlock();

		return success;
	}

	// Get a configuration value for this plugin and store the result in an atomic container.
	// @param key The name of the configuration value.
	// @param value The returned value stored in an atomic type.
	// @return true on success; false if the value could not be retrieved.
	template <typename T>
	bool GetConfigValue(const std::string &key, std::atomic<T> &value)
	{
		T tempValue;
		bool success = GetConfigValue<T>(key, tempValue);
		if (success)
			value = tempValue;
		return success;
	}

	// Set a status item.
	// The status is only set if the string representation of the value is not the same as the last
	// time this method was called.
	// @return true if the status string is new and it was set; false otherwise.
	template<typename T>
	bool SetStatus(const char *key, T value, std::streamsize precision = 2)
	{
		std::ostringstream ss;
		ss.setf(std::ios::boolalpha);
		ss.precision(precision);
		ss << std::fixed << value;

		bool isNewValue = false;

		std::map<const char*, std::string>::iterator pair = _statusMap.find(key);
		if (pair == _statusMap.end())
		{
			_statusMap.insert(std::pair<const char*, std::string>(key, ss.str()));
			isNewValue = true;
		}
		else
		{
			if (ss.str().compare(pair->second) != 0)
			{
				pair->second = ss.str();
				isNewValue = true;
			}
		}

		if (isNewValue)
		{
			//PLOG(logINFO) << "New Status. " << key << ": " << ss.str();
			ivp_setStatusItem(_plugin, key, ss.str().c_str());
		}

		return isNewValue;
	}

	std::string NewGuid();

	std::string _name;
	std::string _logPrefix;
	IvpPlugin* _plugin;

private:
	IvpMsgFilter* _msgFilter;

	// Map a plugin status key to the last value set for that key.
	std::map<const char *, std::string> _statusMap;

	// Code for message handler registration and invoking
	struct handler_allocator {
		virtual ~handler_allocator() {}

		virtual std::string get_messageType() = 0;
		virtual void invokeHandler(tmx::routeable_message &routeableMsg) = 0;
	};

	template <typename MsgType, class PluginType, class HandlerType>
	struct handler_allocator_impl: public handler_allocator {
		typedef MsgType type;

		handler_allocator_impl(PluginType *plugin,
				void (HandlerType::*handler)(MsgType &, tmx::routeable_message &)):
					instance(plugin), fn(handler) {}

		std::string get_messageType()
		{
			return battelle::attributes::type_id_name<MsgType>();
		}

		void invokeHandler(tmx::routeable_message &routeableMsg)
		{
			MsgType msg = routeableMsg.template get_payload<MsgType>();
			if (fn)
				(instance->*fn)(msg, routeableMsg);
			else
				throw std::runtime_error("Missing handler for " + get_messageType());
		}
	private:
		PluginType *instance;
		void (HandlerType::*fn)(MsgType &, tmx::routeable_message &);
	};

	// Map for message handlers
	std::map<std::pair<std::string, std::string>, handler_allocator *> _msgHandlers;

	template <typename MsgType, class PluginType, class HandlerType>
	void register_handler(PluginType *plugin, std::string messageType, std::string messageSubType,
			void (HandlerType::*handler)(MsgType &, tmx::routeable_message &))
	{
		static handler_allocator_impl<MsgType, PluginType, HandlerType> *allocator =
				new handler_allocator_impl<MsgType, PluginType, HandlerType>(plugin, handler);

		_msgHandlers[std::pair<std::string, std::string>(messageType, messageSubType)] = allocator;
	}

	bool invoke_handler(std::string, std::string, tmx::routeable_message &);
};

template<>
inline bool PluginClient::GetConfigValue(const std::string &key, boost::property_tree::ptree &value, std::mutex *lock)
{
	bool success = false;
	std::string string_val;
	success = GetConfigValue<std::string>(key, string_val, lock);
	if (!success) return success;

	std::stringstream ss;
	ss << string_val;

	try
	{
		boost::property_tree::read_json(ss, value);
	}
	catch (boost::property_tree::json_parser_error const &ex)
	{
		PLOG(logERROR) << "Unable to read JSON config value from \"" << string_val <<
				"\" into property tree: " << ex.what();
		success = false;
	}

	return success;
}

template<>
inline bool PluginClient::GetConfigValue<bool>(const std::string &key, bool &value, std::mutex *lock)
{
	std::string strValue;
	bool success = GetConfigValue<std::string>(key, strValue, lock);
	if (!success)
		return false;

	if (boost::iequals(strValue, "1")
		|| boost::iequals(strValue, "true")
		|| boost::iequals(strValue, "t")
		|| boost::iequals(strValue, "on"))
	{
		value = true;
	}
	else
		value = false;

	return true;
}

template<>
inline void PluginClient::BroadcastMessage<tmx::messages::TmxEventLogMessage>(tmx::messages::TmxEventLogMessage &message,
		std::string source, unsigned int sourceId, IvpMsgFlags flags)
{
	PLOG(logDEBUG2) << "Sending Event Log Message: " << message;

	ivp_addEventLog(_plugin, message.get_level(), message.get_description().c_str());
}

template <typename Plugin>
int run_plugin(std::string name, int argc, char *argv[])
{
	std::string title = USAGE;
	if (argc > 0 || argv[1] != NULL)
	{
		// Add program name to the usage
		title += ": ";
		title += argv[0];
	}

	boost::program_options::options_description desc(title);

	desc.add_options()
			("help,h", "This help screen")
			//("manifest,m", boost::program_options::value<std::string>()->default_value(IVPREGISTER_MANIFEST_FILE_NAME), "Plugin manifest file")
			("level,l", boost::program_options::value<std::string>()->default_value(DEFAULT_LOG_LEVEL), "Log level, i.e. ERROR, WARNING, INFO, DEBUG, DEBUGn where n=1-4")
			("output,o", boost::program_options::value<std::string>()->default_value("-"), "Log output file.  Use - for standard output");

	boost::program_options::variables_map opts;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), opts);
	boost::program_options::notify(opts);

	if (opts.count("help"))
	{
		std::cerr << desc << std::endl;
		exit(-1);
	}

	boost::property_tree::ptree cfg;

	if (opts.count("manifest"))
	{
		std::string mf = opts["manifest"].as<std::string>();
		try
		{
			boost::property_tree::read_json(mf, cfg);
		}
		catch (std::exception &ex)
		{
			std::cerr << "Unable to read " << mf << ": " << ex.what() << std::endl;
			exit(-1);
		}
	}

	if (opts.count("level"))
	{
		FILELog::ReportingLevel() = FILELog::FromString(opts["level"].as<std::string>());
	}

	FILE *logF = NULL;
	if (opts.count("output"))
	{
		std::string fn = opts["output"].as<std::string>();
		if (fn != "-")
		{
			logF = fopen(fn.c_str(), "w");
			if (logF == NULL)
				std::cerr << "Could not open log file: " << strerror(errno) << ".  Logging to standard output." << std::endl;
		}

		if (logF == NULL)
			FILELog::Stream() = stdout;
		else
			FILELog::Stream() = logF;
	}


	Plugin plugin(name);

	try
	{
		int Ret = plugin.Main();
		if (logF != NULL)
			fclose(logF);
		return Ret;
	}
	catch (std::exception &ex)
	{
		plugin.HandleException(ex, true);
		if (logF != NULL)
			fclose(logF);
		return -1;
	}
}

}} // namespace tmx::utils

#endif /* SRC_PLUGINCLIENT_H_ */
