/*
 * @file PluginClient.cpp
 *
 *  Created on: Feb 25, 2016
 *      Author: ivp
 */

// Include header file for self first to test that it has all includes it needs for inclusion anywhere.
#include "PluginClient.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <uuid/uuid.h>

#include "PluginUtil.h"

using namespace std;
using namespace tmx;

namespace tmx {
namespace utils {

// Define static instance members.
std::map<IvpPlugin*, PluginClient*> PluginClient::_instanceMap;

PluginClient::PluginClient(std::string name) :
	_name(name),
	_logPrefix(name + " - "),
	_msgFilter(NULL)
{
	PLOG(logDEBUG2) << "Constructing the plugin";

	// Populate the info for the plugin by specifying the callback functions.
	// Since info.manifestLocation is not specified below, the default value of
	// "manifest.json" is assumed.
	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onConfigChanged = StaticOnConfigChanged;
	info.onError = StaticOnError;
	info.onMsgReceived = StaticOnMessageReceived;
	info.onStateChange = StaticOnStateChange;

	PLOG(logDEBUG2) << "Creating the plugin in IVP";

	// Create the plugin.
	_plugin = ivp_create(info);

	if (!_plugin)
	{
		PLOG(logERROR) << "Error creating plugin.";
		throw std::runtime_error("Error creating plugin: " + name);
	}

	// Pull the name from the manifest, if possible
	if (_plugin->jsonManifest)
	{
		IvpManifest *manifest = ivpRegister_getManifestFromJson(_plugin->jsonManifest);
		if (manifest->name && strlen(manifest->name) > 0)
			_name = string(manifest->name);
		ivpRegister_destroyManifest(manifest);
	}

	PLOG(logDEBUG2) << "Registering the IVP plugin instance";
	PluginClient::_instanceMap[_plugin] = this;
}

PluginClient::~PluginClient()
{
}

// static wrapper for OnConfigChanged.
void PluginClient::StaticOnConfigChanged(IvpPlugin *plugin, const char *key, const char *value)
{
	PluginClient *p = PluginClient::_instanceMap[plugin];
	if (p)
	{
		try
		{
			p->OnConfigChanged(key, value);
		}
		catch (exception &ex)
		{
			p->HandleException(ex, false);
		}
	}
}

// TMX API calls this method whenever a configuration value changes.
void PluginClient::OnConfigChanged(const char *key, const char *value)
{
	PLOG(logINFO) << "Config Changed. " << key << ": " << value;
}

// static wrapper for OnError.
void PluginClient::StaticOnError(IvpPlugin *plugin, IvpError err)
{
	PluginClient *p = PluginClient::_instanceMap[plugin];
	if (p)
	{
		try
		{
			p->OnError(err);
		}
		catch (exception &ex)
		{
			p->HandleException(ex, false);
		}
	}
}

// TMX API calls this method to pass errors back to the plugin application.
void PluginClient::OnError(IvpError err)
{
	PLOG(logERROR) << "Error: " << err.error << ", Level: " << err.level <<
			", Sys Error: " << err.sysErrNo;
}

// static wrapper for OnMessageReceived.
void PluginClient::StaticOnMessageReceived(IvpPlugin *plugin, IvpMessage *msg)
{
	PluginClient *p = PluginClient::_instanceMap[plugin];
	if (p)
	{
		try
		{
			p->OnMessageReceived(msg);
		}
		catch (exception &ex)
		{
			p->HandleException(ex, false);
		}
	}
}

// TMX API calls this method to pass received message to the plugin application.
// Only message that have been requested (using IvpMsgFilter) are received.
// Note that the API destroys the IvpMessage after this callback returns.
void PluginClient::OnMessageReceived(IvpMessage *msg)
{
	// Count the number of messages received and output the message details.
	static unsigned int count = 1;
	count++;

	routeable_message routeableMsg(msg);

	PLOG(logDEBUG1) << "Received Message. Type: " << routeableMsg.get_type() <<
			", Subtype: " << routeableMsg.get_subtype() <<
			", Source: " << routeableMsg.get_source() <<
			", Count: " << count;

	invoke_handler(routeableMsg.get_type(), routeableMsg.get_subtype(), routeableMsg);
}

// static wrapper for OnStateChange.
void PluginClient::StaticOnStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	PluginClient *p = PluginClient::_instanceMap[plugin];
	if (p)
	{
		try
		{
			p->OnStateChange(state);
		}
		catch (exception &ex)
		{
			p->HandleException(ex, false);
		}
	}
}

// TMX API calls this method whenever the plugin state changes.
void PluginClient::OnStateChange(IvpPluginState state)
{
	PLOG(logDEBUG1) << "State Changed: " << PluginUtil::IvpPluginStateToString(state);
}

void PluginClient::AddMessageFilter(const char *type, const char *subtype, IvpMsgFlags flags)
{
	_msgFilter = ivpSubscribe_addFilterEntryWithFlagMask(_msgFilter, type, subtype, flags);
}

void PluginClient::SubscribeToMessages()
{
	if (_msgFilter == NULL)
		throw runtime_error("Error subscribing to messages.  No message filters were added.");

	ivp_subscribe(_plugin, _msgFilter);
	ivpSubscribe_destroyFilter(_msgFilter);
	_msgFilter = NULL;
}

int PluginClient::Main()
{
	PLOG(logINFO) << "Starting plugin.";

	while (_plugin->state != IvpPluginState_error)
	{
		PLOG(logDEBUG4) << "Sleeping 1 ms" << endl;

		this_thread::sleep_for(chrono::milliseconds(1000));
	}

	PLOG(logINFO) << "Plugin terminating gracefully.";
	return EXIT_SUCCESS;
}

bool PluginClient::invoke_handler(string messageType, string messageSubType,
		routeable_message &routeableMsg)
{
	try
	{
		handler_allocator *allocator =
				_msgHandlers[pair<string, string>(messageType, messageSubType)];
		if (allocator)
			allocator->invokeHandler(routeableMsg);
		else
			return false;
	}
	catch (exception &ex)
	{
		HandleException(ex, false);
		return false;
	}

	return true;
}

void PluginClient::handleMessage(message &msg, routeable_message &src)
{
	throw runtime_error(this->_name + " received unhandled message of Type=" +
			src.get_type() + ", SubType=" + src.get_subtype());
}

string PluginClient::NewGuid()
{
	char returnStr[37];

	try
	{
		uuid_t id;
		uuid_generate(id);
		uuid_unparse(id,returnStr);
	}
	catch (exception &ex)
	{
		HandleException(ex, false);
	}

    return string(returnStr);
}

void PluginClient::HandleException(exception &ex, bool abort)
{
	std::string message = GetName() + " encountered unhandled exception: ";
	if (abort)
		message = GetName() + " terminating from unhandled exception: ";

	tmx::messages::TmxEventLogMessage elm(ex, message, abort);
	PLOG(logERROR) << elm.get_description();
	BroadcastMessage(elm);

	if (abort)
		std::terminate();
}


}} // namespace tmx::utils
