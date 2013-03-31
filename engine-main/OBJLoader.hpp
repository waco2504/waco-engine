#pragma once

#ifndef _INC_OBJLOADER_
#define _INC_OBJLOADER_

#include "Math.hpp"

#include <vector>
#include <string>


struct OBJLOADER_MATERIAL {
	std::string name;
	float Ns;
	float Ni;
	float d;
	float Tr;
	EVECTOR3 Tf;
	float illum;
	EVECTOR3 Ka;
	EVECTOR3 Kd;
	EVECTOR3 Ks;
	EVECTOR3 Ke;
	std::string map_Ka;
	std::string map_Kd;
	std::string map_d;
	std::string map_bump;
	std::string bump;
};

struct OBJLOADER_INDEX {
	unsigned int vi;
	unsigned int vti;
	unsigned int vni;
};

struct OBJLOADER_FACE {
	std::vector<OBJLOADER_INDEX> vertices;
};

struct OBJLOADER_GROUP {
	std::string name;
	std::string matName;
	std::vector<OBJLOADER_FACE> faces;
};

struct OBJLOADER_OBJECT {
	std::vector<EVECTOR3> poss;
	std::vector<EVECTOR3> norms;
	std::vector<EVECTOR3> coords;
	std::vector<OBJLOADER_MATERIAL> mats;
	std::vector<OBJLOADER_GROUP> groups;
};


class OBJLoader {
public:
	void loadObjectFromOBJ(OBJLOADER_OBJECT* out, std::string& filepath);
};

#endif