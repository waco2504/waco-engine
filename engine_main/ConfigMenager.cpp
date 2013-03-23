#include "ConfigMenager.hpp"
#include <fstream>
#include <algorithm>


int ConfigFile::open(std::string& filePath) {
	std::string buf;
	std::ifstream _file(filePath);

	if(_file.fail()) {
		return 1; // plik nie znaleziony
	}

	while(!_file.eof()) {
		std::getline(_file, buf);
		
		buf.erase(remove_if(buf.begin(), buf.end(), isspace), buf.end());

		file[buf.substr(0, buf.find_first_of(':'))] = buf.substr(buf.find_first_of(':')+1);
	}

	_file.close();
	return 0;
}

int ConfigFile::readConfigI(std::string& name) {
	return atoi(file[name].c_str());
}

int ConfigFile::readConfigI(const char* name) {
	return this->readConfigI(std::string(name));
}

float ConfigFile::readConfigF(std::string& name) {
	return (float)atof(file[name].c_str());
}

float ConfigFile::readConfigF(const char* name) {
	return this->readConfigF(std::string(name));
}

std::string ConfigFile::readConfigS(std::string& name) {
	return file[name];
}

std::string ConfigFile::readConfigS(const char* name) {
	return this->readConfigS(std::string(name));
}

void ConfigFile::close() {
	file.clear();
}