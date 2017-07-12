/*
 * SpatPlugin.h
 *
 *  Created on: April 20, 2017
 *      Author: zinkg
 */

#ifndef SPATPLUGIN_H_
#define SPATPLUGIN_H_

#include <atomic>
#include <array>
#include <map>
#include <mutex>
#include <vector>
#include "PluginClient.h"
#include "UdpClient.h"
#include "signalController.h"
#include "utils/PerformanceTimer.h"

#include <tmx/messages/IvpSignalControllerStatus.h>
#include <asn_j2735_r41/SPAT.h>
#include <asn_j2735_r41/UPERframe.h>
#include <tmx/messages/IvpJ2735.h>
#include <boost/chrono.hpp>
#include <FrequencyThrottle.h>

using namespace std;
using namespace tmx;
using namespace tmx::utils;

namespace SpatPlugin {

class SpatPlugin: public PluginClient {

public:

	SpatPlugin(string name);
	virtual ~SpatPlugin();
	virtual int Main();

protected:

	void UpdateConfigSettings();

	// Virtual method overrides.
	void OnConfigChanged(const char *key, const char *value);
	void OnMessageReceived(IvpMessage *msg);
	void OnStateChange(IvpPluginState state);

private:


	unsigned char derEncoded[4000];
	unsigned int derEncodedBytes;

	SignalController sc;
	int _actionNumber = -1;

	std::mutex data_lock;
	char* localIp;
	char* localUdpPort;
	char* tscIp;
	char* tscRemoteSnmpPort;

	char* intersectionName;

	int intersectionId;

	bool isConfigurationLoaded = false;
	bool isConfigured = false;

	bool encodeSpat();
	bool createUPERframe_DERencoded_msg();
};
} /* namespace SpatPlugin */

#endif /* SPATPLUGIN_H_ */
