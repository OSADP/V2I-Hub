//============================================================================
// Name        : UIProxyPlugin.h
// Author      : Battelle Memorial Institute - Ben Paselsky (paselsky@battelle.org)
// Version     :
// Copyright   : Copyright (c) 2017 Battelle Memorial Institute. All rights reserved.
// Description : Plugin that listens for websocket connections from UI and
//				 sends UI messages
//============================================================================

#ifndef TMX_PLUGINS_UIProxyPlugin_H_
#define TMX_PLUGINS_UIProxyPlugin_H_

#include <atomic>
#include <thread>
#include <queue>
#include <chrono>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <libwebsockets.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "PluginClient.h"
#include <tmx/messages/message_document.hpp>

#include <Base64.h>

#include <TmxControl.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lockfree/spsc_queue.hpp>


using namespace std;
using namespace tmx;
using namespace tmx::utils;
using namespace tmx::messages;
using namespace std;
using namespace tmxctl;
using namespace boost::property_tree;

namespace UIProxyPlugin
{

#define READ_BUFFER_SIZE 4096
#define CORE_CONNECT_PORT 24601

/**
 * This plugin listens for websocket connections from the TMX admin portal
 * and processes commands
 */
class UIProxyPlugin: public PluginClient
{
public:
	UIProxyPlugin(std::string);
	virtual ~UIProxyPlugin();
	int Main();
protected:
	// Virtual method overrides.
	void OnConfigChanged(const char *key, const char *value);
	void OnStateChange(IvpPluginState state);
	static uint64_t GetMsTimeSinceEpoch();
	void UpdateConfigSettings();
	static int WSCallbackHTTP(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
	static int WSCallbackBASE64(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);


private:

	struct PerSessionDataBASE64
	{
		uint64_t session;
		int coreSockfd;
		bool coreConnected;
	};

	atomic<bool> _newConfigValues{false};
	std::atomic<uint64_t> _sleepMS{10}; // sleep in milliseconds
	atomic<bool> _sslEnabled{false};
	string _sslPath = "";

	// locks
	mutex _configLock;

	//statics
	static uint64_t _nextSession;
	static uint64_t _connectionCount;
};

} /* namespace UIProxyPlugin */

#endif /* TMX_PLUGINS_UIProxyPlugin_H_ */
