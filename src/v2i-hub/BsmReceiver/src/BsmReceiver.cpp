/*
 * BsmReceiver.cpp
 *
 *  Created on: May 10, 2016
 *      Author: ivp
 */

#include "BsmReceiver.h"
#include <FrequencyThrottle.h>
#include <mutex>
#include <stdexcept>
#include <thread>

#include <tmx/apimessages/TmxEventLog.hpp>
#include <tmx/j2735_messages/J2735MessageFactory.hpp>

using namespace std;
using namespace boost::asio;
using namespace tmx;
using namespace tmx::messages;
using namespace tmx::utils;

// BSMs may be 10 times a second, so only send errors at most every 2 minutes
#define ERROR_WAIT_MS 120000
#define STATUS_WAIT_MS 2000

namespace BsmReceiver {

mutex syncLock;
FrequencyThrottle<int> errThrottle;
FrequencyThrottle<int> statThrottle;

BsmReceiver::BsmReceiver(std::string name): PluginClient(name)
{
	//Don't need to subscribe to messages
	//SubscribeToMessages();
	errThrottle.set_Frequency(std::chrono::milliseconds(ERROR_WAIT_MS));
	statThrottle.set_Frequency(std::chrono::milliseconds(STATUS_WAIT_MS));
}

BsmReceiver::~BsmReceiver()
{
	delete server;
}

void BsmReceiver::recvBytes(const byte_stream &bytes)
{
	static uint64_t totalBytes = 0;
	static uint32_t totalCount = 0;

	totalBytes += bytes.size();

	static J2735MessageFactory factory;
	routeable_message *msg = factory.NewMessage(const_cast<byte_stream &>(bytes));

	if (msg)
	{
		PLOG(logDEBUG) << *msg;

		// Only forward BSMs
		if (msg->get_subtype().compare(BsmMessage::get_messageType()) == 0)
		{
			PLOG(logDEBUG) << "Received BSM.  Routing.";
			if (routeDsrc)
				msg->set_flags(IvpMsgFlags_RouteDSRC);
			else
				msg->set_flags(IvpMsgFlags_None);

			BroadcastMessage(*msg);

			totalCount ++;
		}
	}

	if (statThrottle.Monitor(1))
	{
		SetStatus("Total BSM Messages Received", totalCount);
		SetStatus("Total Bytes Received", totalBytes);
		//SetStatus("Bytes Received per Second", 1.0 * totalBytes / STATUS_WAIT_SEC);
	}
}

void BsmReceiver::UpdateConfigSettings()
{
	lock_guard<mutex> lock(syncLock);

	GetConfigValue("IP", ip);
	GetConfigValue("Port", port);
	GetConfigValue("RouteDSRC", routeDsrc);
}

void BsmReceiver::OnConfigChanged(const char *key, const char *value)
{
	PluginClient::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

void BsmReceiver::OnStateChange(IvpPluginState state)
{
	PluginClient::OnStateChange(state);

	if (state == IvpPluginState_registered)
	{
		UpdateConfigSettings();
	}
}

int BsmReceiver::Main()
{
	PLOG(logINFO) << "Starting plugin.";

	//byte_stream incoming(4000);
	char bytes[4000];

	while (_plugin->state != IvpPluginState_error)
	{
		syncLock.lock();
		if (server && (server->GetAddress() != ip || server->GetPort() != port))
			delete server;

		if (!server && port > 0)
		{
			PLOG(logDEBUG) <<"Creating UDPServer ip "<<ip<< " port "<< port;
			server = new UdpServer(ip, port);
		}
		syncLock.unlock();

		if (server)
		{
			try
			{
				PLOG(logDEBUG) << "Ready To Receive";
				int len = server->Receive(bytes, 4000);
				PLOG(logDEBUG) << "Received "  << len << " bytes.";

				if (len > 0)
				{
					byte_stream recv(len);
					memcpy(recv.data(), bytes, len);

					PLOG(logDEBUG) << "Copied into byte_stream and calling recvBytes";
					recvBytes(recv);
				}
				else if (len < 0)
				{
					PLOG(logERROR) << "ErrorNo: " << errno;
					errno = -EINVAL;
					throw runtime_error("Receive returned error");
				}

			}
			catch (exception &ex)
			{
				if (errno <= 0 || errThrottle.Monitor(1000 * errno))
				{
					TmxEventLogMessage elm(ex, "Unable to receive from socket: ");
					PLOG(logERROR) << elm.get_description();
					BroadcastMessage(elm);
				}
			}
		}

		// Wait a bit longer to try again
		this_thread::sleep_for(chrono::milliseconds(1));
	}

	return 0;
}

} /* namespace SunGuide */

int main(int argc, char *argv[])
{
	return run_plugin<BsmReceiver::BsmReceiver>("BsmReceiver", argc, argv);
}
