#ifndef XML_PTLM_PARSER_H
#define XML_PTLM_PARSER_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>
#include <stdexcept>

#include "SPaTData.h"

// Error codes

enum {
	ERROR_ARGS = 1, ERROR_XERCES_INIT, ERROR_PARSE, ERROR_EMPTY_DOCUMENT
};

class XmlPtlmParser {
public:
	XmlPtlmParser();
	~XmlPtlmParser();

	bool ReadFile(std::string& filePath);

	char *IntersectionName;
	int IntersectionId;

	int NumPTLMRecords;
	PTLM *PTLMTable;

private:
	void ReadRoot(xercesc::DOMElement* root);
	void ReadIntersection(xercesc::DOMElement* element);
	void ReadSPATMovement(xercesc::DOMElement* element, PTLM *ptlm);

	xercesc::XercesDOMParser *_parser;
};

#endif
