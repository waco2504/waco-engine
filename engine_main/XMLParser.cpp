#include "XMLParser.hpp"


void XMLParser::getXMLDesc() {
	std::string line;
	std::getline(file, line, '>');
}

bool XMLParser::openFile(const std::string& filePath) {
	if(file.is_open()) file.close();
	file.open(filePath.c_str());
	if(file.bad()) return false;
	getXMLDesc();
	curElement.tagType = ENDTAG;
	return true;
}

void XMLParser::closeFile() {
	if(file.is_open()) file.close();
}

bool XMLParser::readElementName(std::string& out) {
	out = curElement.name;
	return true;
}

bool XMLParser::readElementContent(std::string& out) {
	return true;
}

bool XMLParser::readElementContent(float&) {
	return true;
}

bool XMLParser::readAttributeName(std::string&) {
	return true;
}

bool XMLParser::readAttributeContent(std::string&) {
	return true;
}

bool XMLParser::readAttributeContent(float&) {
	return true;
}

bool XMLParser::gotoNextAttribute() {
	return true;
}

bool XMLParser::gotoNextChildElement() {
	return true;
}

bool XMLParser::gotoNextElement() { 
	std::string line; 
	unsigned int eon = 0, ie;

	for(char c = 0; c != '<' && file.good(); c = file.get());
	if(file.bad()) return false;
	
	std::getline(file, line, '>'); // w line tagname i attributy

	eon = line.find_first_of(" /", 0);
	ie = line.find_last_of("/");

	if(line[eon] == ' ') {
		ATTRIBUTE temp;
		unsigned int b = 0, e = eon;

		curElement.name = line.substr(0, eon);
		
		while(e < line.size()) {
			for(b = e; line[b] != ' '; ++b); ++b;	// b wskazuje na poczatek attribu
			for(e = b; line[e] != '='; ++e);		// e to koneic nazwy attribu
			temp.name = line.substr(b, e-b);
			for(b = e; line[b] != '\"'; ++b); ++b;	// b wskazuje na poczatek wartosci attribu
			for(e = b; line[e] != '\"'; ++e);		// e wskazuje na koniec wartosci attribu
			temp.content = line.substr(b, e-b);
			++e;
		}

		if(ie == line.size()-1) {
			curElement.tagType = EMPTYTAG;
		} else {
			curElement.tagType = STARTTAG;
		}
	} 
	else if(ie == eon) {
		curElement.name = line.substr(0, eon);
		curElement.tagType = EMPTYTAG;
	}
	else if(eon == std::string::npos) {
		curElement.name = line;
		curElement.tagType = STARTTAG;
	}

	// tzn ze jakies content mozna by zebrac
	if(curElement.tagType == STARTTAG) {
		// niech laduje caly az do napotkania endtaga o tej samej nazwie?
	}

	return true;
}
	
bool XMLParser::gotoParentElement() {
	return true;
}



