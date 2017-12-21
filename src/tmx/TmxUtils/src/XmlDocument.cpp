/*
 * XmlDocument.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: ivp
 */

#include "XmlDocument.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace pugi;

#define HEX(x) setw(2) << setfill('0') << hex << (int)( x ) << dec

namespace tmx
{
namespace utils
{

XmlDocument::XmlDocument()
{
}

XmlDocument::~XmlDocument()
{
}

void XmlDocument::LoadXml(const char* docXml, const char* rootPath)
{
	_rootPath = rootPath;

	pugi::xml_parse_result result = _doc.load_string(docXml);
	if (!result)
	{
		stringstream ss;
		ss << "Unable to parse BasePsmMessageXml: " << result.description();
		throw std::runtime_error(ss.str().c_str());
	}

	_rootNode = _doc.select_node(_rootPath).node();
}

std::string XmlDocument::GetOctetHexString(uint16_t value)
{
	stringstream ss;
	ss << HEX((value >> 8) & 0xFF) << " ";
	ss << HEX(value & 0xFF);
	return ss.str();
}

std::string XmlDocument::GetOctetHexString(uint32_t value)
{
	stringstream ss;
	ss << HEX((value >> 24) & 0xFF) << " ";
	ss << HEX((value >> 16) & 0xFF) << " ";
	ss << HEX((value >> 8) & 0xFF) << " ";
	ss << HEX(value & 0xFF);
	return ss.str();
}

std::string XmlDocument::ToXml()
{
	// Create a copy of the document so that invalid nodes can be removed before converting to XML.
	xml_document docCopy;
	docCopy.reset(_doc);

	// Remove invalid nodes.
	xml_node rootNode = docCopy.select_node(_rootPath).node();
	RemoveInvalidNodes(rootNode);

	// The XML must be formatted without spaces/newlines (raw) or the XML to ASN decoder will not work for BOOL fields.
	std::stringstream ss;
	docCopy.save(ss, "", format_raw);
	return ss.str();
}

void XmlDocument::AddValidNode(const std::string& name)
{
	if (std::find(_validNodes.begin(), _validNodes.end(), name) == _validNodes.end())
		_validNodes.push_back(name);
}

void XmlDocument::RemoveInvalidNodes(pugi::xml_node &rootNode)
{
	vector<xml_node> deleteList;

	for (xml_node child = rootNode.first_child(); child; child = child.next_sibling())
	{
		if (std::find(_validNodes.begin(), _validNodes.end(), child.name()) == _validNodes.end())
			deleteList.push_back(child);
	}

	for (uint i=0; i < deleteList.size(); i++)
		rootNode.remove_child(deleteList[i]);
}

void XmlDocument::RemoveAllChildren(pugi::xml_node& node)
{
	vector<xml_node> deleteList;

	for (xml_node child = node.first_child(); child; child = child.next_sibling())
	{
		deleteList.push_back(child);
	}

	for (uint i=0; i < deleteList.size(); i++)
		node.remove_child(deleteList[i]);
}

uint64_t XmlDocument::GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)((double)(tv.tv_sec) * 1000 + (double)(tv.tv_usec) / 1000);
}

void XmlDocument::SetDateNode(const std::string nodeName, uint64_t time_ms)
{
	AddValidNode(nodeName);
	xml_node childNode = _rootNode.select_node(nodeName.c_str()).node();
	SetDateNode(childNode, time_ms);
}

void XmlDocument::SetDateNode(const std::string nodeName, int year, const int month, int day, int hour, int minute, int second)
{
	AddValidNode(nodeName);
	xml_node childNode = _rootNode.select_node(nodeName.c_str()).node();
	SetDateNode(childNode, year, month, day, hour, minute, second);
}

void XmlDocument::SetDateNode(pugi::xml_node childNode, uint64_t time_ms)
{
	time_t time_sec = time_ms / 1000;
	uint64_t remaining_ms = time_ms%1000;
	struct tm *tm;
	tm = gmtime(&time_sec);

	SetDateNode(childNode, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec*1000+remaining_ms);
}

void XmlDocument::SetDateNode(pugi::xml_node childNode, int year, const int month, int day, int hour, int minute, int second)
{
	childNode.select_node("year").node().text().set(year);
	childNode.select_node("month").node().text().set(month);
	childNode.select_node("day").node().text().set(day);
	childNode.select_node("hour").node().text().set(hour);
	childNode.select_node("minute").node().text().set(minute);

	// A DFullDate type does not have a second, but DDateTime does.
	xml_node secondNode = childNode.select_node("second").node();
	if (!secondNode.empty())
		secondNode.text().set(second);
}


} /* namespace utils */
} /* namespace tmx */
