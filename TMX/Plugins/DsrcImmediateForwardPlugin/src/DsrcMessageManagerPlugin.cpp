/*
 * DsrcMessageManagerPlugin.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: ivp
 */

#include "DsrcMessageManagerPlugin.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "UdpClient.h"


using namespace boost::property_tree;
using namespace std;
using namespace tmx;
using namespace tmx::utils;

namespace DsrcMessageManager
{

const char* Key_SkippedNoDsrcMetadata = "Messages Skipped (No DSRC metadata)";
const char* Key_SkippedNoMessageRoute = "Messages Skipped (No route)";
const char* Key_SkippedInvalidUdpClient = "Messages Skipped (Invalid UDP Client)";

DsrcMessageManagerPlugin::DsrcMessageManagerPlugin(std::string name) : PluginClient(name),
	_configRead(false),
	_skippedNoDsrcMetadata(0),
	_skippedNoMessageRoute(0),
	_skippedInvalidUdpClient(0)
{
	for (uint i = 0; i < _udpClientList.size(); i++)
	{
		_udpClientList[i] = NULL;
	}

	AddMessageFilter("J2735", "*", IvpMsgFlags_RouteDSRC);
	AddMessageFilter("Battelle-DSRC", "*", IvpMsgFlags_RouteDSRC);
	SubscribeToMessages();

	_muteDsrc = false;
	SetSystemConfigValue("MuteDsrcRadio", _muteDsrc, false);
}

DsrcMessageManagerPlugin::~DsrcMessageManagerPlugin()
{
	lock_guard<mutex> lock(_mutexUdpClient);

	for (uint i = 0; i < _udpClientList.size(); i++)
	{
		if (_udpClientList[i] != NULL)
			delete _udpClientList[i];
	}
}

void DsrcMessageManagerPlugin::OnConfigChanged(const char *key, const char *value)
{
	PluginClient::OnConfigChanged(key, value);

	UpdateConfigSettings();
}

void DsrcMessageManagerPlugin::OnMessageReceived(IvpMessage *msg)
{
	// Uncomment this line to call the base method, which prints the message received to cout.
	//PluginClient::OnMessageReceived(msg);

	if (!_configRead)
	{
		PLOG(logWARNING) << "Config not read yet.  Message Ignored: " <<
				"Type: " << msg->type << ", Subtype: " << msg->subtype;
		return;
	}

	if (msg->dsrcMetadata == NULL)
	{
		SetStatus<uint>(Key_SkippedNoDsrcMetadata, ++_skippedNoDsrcMetadata);
		PLOG(logWARNING) << "No DSRC metadata.  Message Ignored: " <<
				"Type: " << msg->type << ", Subtype: " << msg->subtype;
		return;
	}

	if(!_muteDsrc)
	{
		SendMessageToRadio(msg);
	}
}

void DsrcMessageManagerPlugin::OnStateChange(IvpPluginState state)
{
	PluginClient::OnStateChange(state);

	if (state == IvpPluginState_registered)
	{
		UpdateConfigSettings();
	}
}

void DsrcMessageManagerPlugin::UpdateConfigSettings()
{
	PLOG(logINFO) << "Updating configuration settings.";

	// Update the configuration setting for all UDP clients.
	// This includes creation/update of _udpClientList and _messageConfigMap.
	{
		lock_guard<mutex> lock(_mutexUdpClient);
		_messageConfigMap.clear();

		_skippedNoDsrcMetadata = 0;
		_skippedNoMessageRoute = 0;
		_skippedInvalidUdpClient = 0;
		SetStatus<uint>(Key_SkippedNoDsrcMetadata, _skippedNoDsrcMetadata);
		SetStatus<uint>(Key_SkippedNoMessageRoute, _skippedNoMessageRoute);
		SetStatus<uint>(Key_SkippedInvalidUdpClient, _skippedInvalidUdpClient);
	}
	for (uint i = 0; i < _udpClientList.size(); i++)
	{
		UpdateUdpClientFromConfigSettings(i);
	}

	// Get the signature setting.
	// The same mutex is used that protects the UDP clients.
	GetConfigValue("Signature", _signature, &_mutexUdpClient);

	GetConfigValue("MuteDsrcRadio", _muteDsrc);
	SetStatus("MuteDsrc", _muteDsrc);
	_configRead = true;
}

// Retrieve all settings for a UDP client, then create a UDP client using those settings.
// Other settings related to the UDP client are also updated (i.e. msg id list, psid list).
bool DsrcMessageManagerPlugin::UpdateUdpClientFromConfigSettings(uint clientIndex)
{
	if (_udpClientList.size() <= clientIndex)
	{
		PLOG(logWARNING) << "Invalid client number. Only " << _udpClientList.size() << " clients available.";
		return false;
	}

	int clientNum = clientIndex + 1;
	string messagesSetting((boost::format("Messages_UDP_Port_%d") % clientNum).str());
	string udpPortSetting((boost::format("UDP_Port_%d") % clientNum).str());
	string radioIpSetting("DSRC_Radio_IP");

	try
	{
		uint32_t udpPort;
		GetConfigValue(udpPortSetting, udpPort);

		string messages;
		GetConfigValue(messagesSetting, messages);

		string radioIp;
		GetConfigValue(radioIpSetting, radioIp);

		// Take the lock while shared data is accessed.
		// A lock_guard will unlock when it goes out of scope (even if an exception occurs).
		lock_guard<mutex> lock(_mutexUdpClient);

		ParseJsonMessageConfig(messages, clientIndex);

		if (_udpClientList[clientIndex] != NULL)
			delete _udpClientList[clientIndex];

		if (udpPort > 0 && radioIp.length() > 0)
		{
			PLOG(logINFO) << "Creating UDP Client " << (clientIndex + 1) <<
					" - Radio IP: " << radioIp << ", Port: " << udpPort;
			_udpClientList[clientIndex] = new UdpClient(radioIp, udpPort);
		}
		else
		{
			_udpClientList[clientIndex] = NULL;
		}
	}
	catch(std::exception const & ex)
	{
		PLOG(logERROR) << "Error getting config settings: " << ex.what();
		return false;
	}

	return true;
}

bool DsrcMessageManagerPlugin::ParseJsonMessageConfig(const std::string& json, uint clientIndex)
{
	if (json.length() == 0)
		return true;

	try
	{
		// Example JSON parsed:
		// { "Messages": [ { "TmxType": "MAP-P", "SendType": "MAP", "PSID": "0x8002" }, { "TmxType": "SPAT-P", "SendType": "SPAT", "PSID": "0x8002" } ] }
		// The strings below (with extra quotes escaped) can be used for testing.
		//string json2 = "{ \"Messages\": [ ] }";
		//string json2 = "{ \"Messages\": [ { \"TmxType\": \"MAP-P\", \"SendType\": \"MAP\", \"PSID\": \"0x8002\" }, { \"TmxType\": \"SPAT-P\", \"SendType\": \"SPAT\", \"PSID\": \"0x8002\" } ] }";

		// Read the JSON into a boost property tree.
		ptree pt;
		istringstream is(json);
		read_json(is, pt);

		// Iterate over the Messages section of the property tree.
		// Note that Messages is at the root of the property tree, otherwise the entire
		// path to the child would have to be specified to get_child.
		BOOST_FOREACH(ptree::value_type &child, pt.get_child("Messages"))
		{
			// Array elements have no names.
			assert(child.first.empty());

			MessageConfig config;
			config.ClientIndex = clientIndex;
			config.TmxType = child.second.get<string>("TmxType");
			config.SendType = child.second.get<string>("SendType");
			config.Psid = child.second.get<string>("PSID");

			PLOG(logINFO) << "Message Config - Client: " << (config.ClientIndex + 1) <<
					", TmxType: " << config.TmxType << ", SendType: " << config.SendType << ", PSID: " << config.Psid;

			// Add the message configuration to the map.
			_messageConfigMap[config.TmxType] = config;
		}
	}
	catch(std::exception const & ex)
	{
		PLOG(logERROR) << "Error parsing Messages: " << ex.what();
		return false;
	}

	return true;
}

void DsrcMessageManagerPlugin::SendMessageToRadio(IvpMessage *msg)
{
	static FrequencyThrottle<std::string> _statusThrottle(chrono::milliseconds(2000));

	lock_guard<mutex> lock(_mutexUdpClient);

	std::map<std::string, MessageConfig>::iterator it = _messageConfigMap.find(msg->subtype);

	int msgCount = 0;

	std::map<std::string, int>::iterator itMsgCount = _messageCountMap.find(msg->subtype);

	if(itMsgCount != _messageCountMap.end())
	{
		msgCount = (int)itMsgCount->second;
		msgCount ++;
	}

	_messageCountMap[msg->subtype] = msgCount;


	if (_statusThrottle.Monitor(msg->subtype)) {
		SetStatus<int>(msg->subtype, msgCount);
	}

	if (it == _messageConfigMap.end())
	{
		SetStatus<uint>(Key_SkippedNoMessageRoute, ++_skippedNoMessageRoute);
		PLOG(logWARNING) << "TMX Subtype not found in configuration.  Message Ignored: " <<
				"Type: " << msg->type << ", Subtype: " << msg->subtype;
		return;
	}

	// Convert the payload to upper case.
	for (int i = 0; i < (int)(strlen(msg->payload->valuestring)); i++)
		msg->payload->valuestring[i] = toupper(msg->payload->valuestring[i]);

	// Format the message using the protocol defined in the
	// USDOT ROadside Unit Specifications Document v 4.0 Appendix C.

	stringstream os;

	os << "Version=0.7" << "\n";
	os << "Type=" << it->second.SendType << "\n" << "PSID=" << it->second.Psid << "\n";
	os << "Priority=7" << "\n" << "TxMode=CONT" << "\n" << "TxChannel=" << msg->dsrcMetadata->channel << "\n";
	os << "TxInterval=0" << "\n" << "DeliveryStart=\n" << "DeliveryStop=\n";
	os << "Signature= "<< _signature << "\n" << "Encryption=False\n";
	os << "Payload=" << msg->payload->valuestring << "\n";

	string message = os.str();

	// Send the message using the configured UDP client.

	cout << _logPrefix << "Sending - TmxType: " << it->second.TmxType << ", SendType: " << it->second.SendType;
	cout << ", PSID: " << it->second.Psid << ", Client: " << it->second.ClientIndex;
	cout << ", Port: " << _udpClientList[it->second.ClientIndex]->GetPort() << endl;
	//cout << message << endl;

	if (_udpClientList[it->second.ClientIndex] != NULL)
	{
		_udpClientList[it->second.ClientIndex]->Send(message);
	}
	else
	{
		SetStatus<uint>(Key_SkippedInvalidUdpClient, ++_skippedInvalidUdpClient);
		PLOG(logWARNING) << "UDP Client Invalid. Cannot send message. TmxType: " << it->second.TmxType;
	}
}


} /* namespace DsrcMessageManager */
