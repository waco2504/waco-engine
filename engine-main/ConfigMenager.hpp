#pragma once

#ifndef _INC_CONFIGMENAGER_
#define _INC_CONFIGMENAGER_

#include <string>
#include <map>

class ConfigFile {
	std::map<std::string, std::string> file;
public:
	int open(std::string& filePath);
	int readConfigI(std::string& name); 
	int readConfigI(const char* name); 
	float readConfigF(std::string& name);
	float readConfigF(const char* name);
	std::string readConfigS(std::string& name);
	std::string readConfigS(const char* name);
	void close();
};

#endif