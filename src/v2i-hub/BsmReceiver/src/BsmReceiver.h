/*
 * BsmReceiver.h
 *
 *  Created on: May 10, 2016
 *      Author: ivp
 */

#ifndef SRC_BSMRECEIVER_H_
#define SRC_BSMRECEIVER_H_

#include <PluginClient.h>
#include <UdpClient.h>
#include <UdpServer.h>
#include <boost/asio.hpp>
#include <tmx/j2735_messages/BasicSafetyMessage.hpp>

#define UDP "UDP"

namespace BsmReceiver {

class BsmReceiver: public tmx::utils::PluginClient {
public:
	BsmReceiver(std::string);
	virtual ~BsmReceiver();

	void recvBytes(const tmx::byte_stream &);

	int Main();
protected:
	void UpdateConfigSettings();

	// Virtual method overrides.
	void OnConfigChanged(const char *key, const char *value);
	void OnStateChange(IvpPluginState state);
private:
	std::string ip;
	unsigned short port = 0;
	bool routeDsrc = false;

	tmx::utils::UdpServer *server = NULL;
};

} /* namespace SunGuide */

#endif /* SRC_BSMRECEIVER_H_ */
