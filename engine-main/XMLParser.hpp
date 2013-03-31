#pragma once

#ifndef _INC_XMLPARSER_
#define _INC_XMLPARSER_

#include <string>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>

class XMLParser {
	enum TAGTYPE : int {
		STARTTAG,
		ENDTAG,
		EMPTYTAG
	};
	struct ATTRIBUTE {
		std::string name;
		std::string content;
	};
	struct ELEMENT {
		TAGTYPE tagType;
		std::string name;
		std::deque<ATTRIBUTE> attribs;
		std::string content;
	};

	std::ifstream file;
	ELEMENT curElement;

	void getXMLDesc();
public:
	bool openFile(const std::string& filePath);
	void closeFile();

	bool readElementName(std::string&);
	bool readElementContent(std::string&);
	bool readElementContent(float&);

	bool readAttributeName(std::string&);
	bool readAttributeContent(std::string&);
	bool readAttributeContent(float&);

	bool gotoNextAttribute();

	bool gotoNextChildElement();			// idzie glebiej w hierarchii
	bool gotoNextElement();					// idzie do nastepnego rownowaznego
	bool gotoParentElement();				// idzie do nastpenego wyzej w hierarchii
};

#endif