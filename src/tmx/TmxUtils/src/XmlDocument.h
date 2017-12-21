/*
 * XmlDocument.h
 *
 *  Created on: Nov 17, 2016
 *      Author: ivp
 */

#ifndef XMLDOCUMENT_H_
#define XMLDOCUMENT_H_

#include <vector>
#include <tmx/pugixml/pugixml.hpp>

namespace tmx
{
namespace utils
{

/**
 * Base class used for creation of J2735 documents from base XML strings for manipulation using xpath.
 *
 * Note that tmx/messages/message_document.hpp could not be used because std11 is not supported on MIPS.
 */
class XmlDocument
{
public:
	XmlDocument();
	virtual ~XmlDocument();

	std::string ToXml();

protected:
	void LoadXml(const char* docXml, const char* rootPath);
	std::string GetOctetHexString(uint16_t value);
	std::string GetOctetHexString(uint32_t value);
	void AddValidNode(const std::string& name);
	static void RemoveAllChildren(pugi::xml_node& node);

	/**
	 * Remove all optional nodes that have not been set.
	 */
	void RemoveInvalidNodes(pugi::xml_node &rootNode);

	uint64_t GetMsTimeSinceEpoch();
	void SetDateNode(pugi::xml_node node, uint64_t time_ms);
	void SetDateNode(pugi::xml_node childNode, int year, const int month, int day, int hour, int minute, int second);
	void SetDateNode(const std::string nodeName, uint64_t time_ms);
	void SetDateNode(const std::string nodeName, int year, const int month, int day, int hour, int minute, int second = 0);

	pugi::xml_document _doc;
	pugi::xml_node _rootNode;
	const char* _rootPath;

private:
	std::vector<std::string> _validNodes;
};

} /* namespace utils */
} /* namespace tmx */

#endif /* XMLDOCUMENT_H_ */
