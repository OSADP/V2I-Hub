#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "XmlPtlmParser.h"

using namespace xercesc;
using namespace std;

bool _debugOutput = true;

XmlPtlmParser::XmlPtlmParser()
{
	try
	{
		// Initialize Xerces infrastructure.
		XMLPlatformUtils::Initialize();
	}
	catch (XMLException& e)
	{
		char* message = XMLString::transcode(e.getMessage());
		cerr << "XML toolkit initialization error: " << message << endl;
		XMLString::release(&message);
		// throw exception here to return ERROR_XERCES_INIT
	}

	_parser = new XercesDOMParser;
}

XmlPtlmParser::~XmlPtlmParser()
{
	// Free memory

	delete _parser;

	// Terminate Xerces framework.

	try
	{
		XMLPlatformUtils::Terminate();
	}
	catch (xercesc::XMLException& e)
	{
		char* message = xercesc::XMLString::transcode(e.getMessage());

		cerr << "XML xerces termination error: " << message << endl;
		XMLString::release(&message);
	}
}

string GetFileError()
{
	// errno is declared by include file errno.h.
	if ( errno == ENOENT)
		return "File does not exist, or path is an empty string.";
	if ( errno == ENOTDIR)
		return "A component of the path is not a directory.";
	if ( errno == ELOOP)
		return "Too many symbolic links encountered while traversing the path.";
	if ( errno == EACCES)
		return "Permission denied.";
	if ( errno == ENAMETOOLONG)
		return "Filename is too long.";

	return "Unknown error reading file.";
}

bool XmlPtlmParser::ReadFile(string& filePath)
{
	// Test the file status.
	struct stat fileStatus;
	cout << "Reading File: " << filePath << endl;
	errno = 0;
	// ok == 0, error == -1.
	if (stat(filePath.c_str(), &fileStatus) == -1)
	{
		cout << GetFileError() << " File: " << filePath << endl;
		return false;
	}

	try
	{
		_parser->parse(filePath.c_str());

		// Get the DOM document.
		// It is owned by the parser, so there is no need to free it later.
		DOMDocument* xmlDoc = _parser->getDocument();

		// Get the top-level root element.
		DOMElement* root = xmlDoc->getDocumentElement();
		if (!root)
		{
			cout << "Root element not found.  Is XML document empty?" << " File: " << filePath << endl;
			return false;
		}

		ReadRoot(root);
	}
	catch (xercesc::XMLException& e)
	{
		char* message = xercesc::XMLString::transcode(e.getMessage());
		ostringstream errBuf;
		errBuf << "Error parsing file: " << message << flush;
		XMLString::release(&message);
		return false;
	}

	return true;
}

// Compare lower case versions of the elements tag name and a char* string.
bool MatchTagName(DOMElement* element, const char* name)
{
	XMLCh* xmlName = XMLString::transcode(name);
	bool isMatch = XMLString::compareIString(xmlName, element->getTagName()) == 0;
	XMLString::release(&xmlName);
	return isMatch;
}

// Compare lower case versions of the elements text content and a char* string.
bool MatchTextContent(DOMElement* element, const char* str)
{
	XMLCh* xmlStr = XMLString::transcode(str);
	bool isMatch = XMLString::compareIString(element->getTextContent(), xmlStr) == 0;
	XMLString::release(&xmlStr);
	return isMatch;
}

// Compare lower case versions of 2 strings (1 XMLCh* and 1 char*).
bool MatchString(const XMLCh* xmlCh, const char* str)
{
	XMLCh* xmlStr = XMLString::transcode(str);
	bool isMatch = XMLString::compareIString(xmlCh, xmlStr) == 0;
	XMLString::release(&xmlStr);
	return isMatch;
}

char* GetAttribute(DOMElement* element, const char* name)
{
	XMLCh* xmlName = XMLString::transcode(name);
	const XMLCh* attribute = element->getAttribute(xmlName);
	XMLString::release(&xmlName);
	return XMLString::transcode(attribute);
}

void XmlPtlmParser::ReadRoot(DOMElement* root)
{
	// Get all children of root.
	DOMNodeList* children = root->getChildNodes();
	const XMLSize_t childCount = children->getLength();

	NumPTLMRecords = 0;

	// Loop through children of root.
	for (XMLSize_t i = 0; i < childCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if (currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;

		// This node is an Element.
		DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);

		if (MatchTagName(currentElement, "Intersection"))
		{
			ReadIntersection(currentElement);
		}
		else if (MatchTagName(currentElement, "SPATMovement"))
		{
			// The first time through the loop, just count the number of records.
			NumPTLMRecords++;
		}
	}

	if (_debugOutput)
		cout << "NumPTLMRecords: " << NumPTLMRecords << endl;

	// Allocate the table now that it is known how many are in the XML file.
	PTLMTable = (PTLM *) calloc (NumPTLMRecords, sizeof(PTLM));

	int recordIndex = 0;

	// Loop through children of root again to read the PTLM entries.
	for (XMLSize_t i = 0; i < childCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if (currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;

		// This node is an Element.
		DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);

		if (MatchTagName(currentElement, "SPATMovement"))
		{
			ReadSPATMovement(currentElement, &PTLMTable[recordIndex++]);
		}
	}
}

void XmlPtlmParser::ReadIntersection(DOMElement* parentElement)
{
	// Get all children.
	DOMNodeList* children = parentElement->getChildNodes();
	const XMLSize_t childCount = children->getLength();

	// Loop through children.
	for (XMLSize_t i = 0; i < childCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if (currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;

		// This node is an Element.
		DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);

		if (MatchTagName(currentElement, "Name"))
		{
			IntersectionName = XMLString::transcode(currentElement->getTextContent());
			if (_debugOutput)
				cout << "IntersectionId: " << IntersectionName << endl;
		}
		else if (MatchTagName(currentElement, "ID"))
		{
			IntersectionId = atoi(XMLString::transcode(currentElement->getTextContent()));
			if (_debugOutput)
				cout << "IntersectionId: " << IntersectionId << endl;
		}
	}
}

void XmlPtlmParser::ReadSPATMovement(DOMElement* parentElement, PTLM *ptlm)
{
	ptlm->TSCEntityType = unavailableTSCEntity;

	// Get all children.
	DOMNodeList* children = parentElement->getChildNodes();
	const XMLSize_t childCount = children->getLength();

	// Loop through children.
	for (XMLSize_t i = 0; i < childCount; i++)
	{
		DOMNode* currentNode = children->item(i);
		if (currentNode->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;

		// This node is an Element.
		DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);

		if (MatchTagName(currentElement, "Movement"))
		{
			if (MatchTextContent(currentElement, "ped"))
				ptlm->Movement = man_pedestrian;
			else if (MatchTextContent(currentElement, "straight"))
				ptlm->Movement = man_straight;
			else if (MatchTextContent(currentElement, "left"))
				ptlm->Movement = man_leftturn;
			else if (MatchTextContent(currentElement, "right"))
				ptlm->Movement = man_rightturn;
			else
				ptlm->Movement = man_uturn;

			if (_debugOutput)
				cout << "\nMovement: " << ptlm->Movement << endl;
		}
		else if (MatchTagName(currentElement, "MovementType") || MatchTagName(currentElement, "PhaseType"))
		{
			if (MatchTextContent(currentElement, "protected"))
				ptlm->PhaseType = protectedPhase;
			else if (MatchTextContent(currentElement, "permitted"))
				ptlm->PhaseType = permittedPhase;

			if (_debugOutput)
				cout << "PhaseType: " << ptlm->PhaseType << endl;
		}
		else if (MatchTagName(currentElement, "Lanes"))
		{
			ptlm->Lanes = XMLString::transcode(currentElement->getTextContent());

			if (_debugOutput)
				cout << "Lanes: " << ptlm->Lanes << endl;
		}
		else if (MatchTagName(currentElement, "LanesType"))
		{
			if (MatchTextContent(currentElement, "ped"))
				ptlm->LanesType = pedLane;
			else if (MatchTextContent(currentElement, "vehicle"))
				ptlm->LanesType = vehicleLane;
			else if (MatchTextContent(currentElement, "special"))
				ptlm->LanesType = specialLane;
			else
				ptlm->LanesType = unavailableLaneType;

			if (_debugOutput)
				cout << "LanesType: " << ptlm->LanesType << endl;
		}
		else if (MatchTagName(currentElement, "Phase"))
		{
			ptlm->TSCEntityType = phase;
			ptlm->TSCPhsOvlp = atoi(XMLString::transcode(currentElement->getTextContent()));
			if (_debugOutput)
				cout << "Phase TSCPhsOvlp: " << ptlm->TSCPhsOvlp << endl;
		}
		else if (MatchTagName(currentElement, "Overlap"))
		{
			ptlm->TSCEntityType = overlap;
			ptlm->TSCPhsOvlp = atoi(XMLString::transcode(currentElement->getTextContent()));
			if (_debugOutput)
				cout << "Overlap TSCPhsOvlp: " << ptlm->TSCPhsOvlp << endl;
		}
		else if (MatchTagName(currentElement, "pedCalls"))
		{
			ptlm->PedCalls = XMLString::transcode(currentElement->getTextContent());

			if (_debugOutput)
				cout << "PedCalls: " << ptlm->PedCalls << endl;
		}
		else if (MatchTagName(currentElement, "pedDetectors"))
		{
			ptlm->PedDetectors = XMLString::transcode(currentElement->getTextContent());

			if (_debugOutput)
				cout << "PedDetectors: " << ptlm->PedDetectors << endl;
		}
		else if (MatchTagName(currentElement, "SignalGroup"))
		{
			ptlm->SignalGroup = atoi(XMLString::transcode(currentElement->getTextContent()));

			if (_debugOutput)
				cout << "Signal Group: " << ptlm->SignalGroup << endl;
		}

	}
}
