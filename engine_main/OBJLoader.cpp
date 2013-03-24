#include "OBJLoader.hpp"

#include <assert.h>
#include <fstream>

#include <cstdlib>
#include <cstdio>
#include <deque>

void OBJLoader::loadObjectFromOBJ(OBJLOADER_OBJECT* ret, const char* fP) {
	FILE* objFile = NULL;
	FILE* mtlFile = NULL;
	char mtlFilePath[256] = {0};
	char opt[64] = {0};
	char data[256] = {0};
	EVECTOR3 vec;
	unsigned int spcs = 0;
	unsigned int cRe = 0;
	OBJLOADER_GROUP curgroup; 
	OBJLOADER_FACE face, face1;
	OBJLOADER_INDEX idx;
	OBJLOADER_MATERIAL mat;

	fopen_s(&objFile, fP, "r");
	assert(objFile != NULL && "OBJ file doesn't exist");

	while(!feof(objFile)) {
		fscanf_s(objFile, "%s ", opt, 64);

		if(opt[0] == '#') {
			continue;
		}
		else if(!strcmp(opt, "mtllib")) { // nazwa pliku z materialem
			fscanf_s(objFile, "%s", mtlFilePath, 256);
		}
		else if(!strcmp(opt, "v")) {
			fscanf_s(objFile, "%f %f %f\n", &vec.x, &vec.y, &vec.z);
			ret->poss.push_back(vec);
		}
		else if(!strcmp(opt, "vn")) {
			fscanf_s(objFile, "%f %f %f\n", &vec.x, &vec.y, &vec.z);
			ret->norms.push_back(vec);
		}
		else if(!strcmp(opt, "vt")) {
			fscanf_s(objFile, "%f %f %f\n", &vec.x, &vec.y, &vec.z);
			vec.y *= -1.0f;
			ret->coords.push_back(vec);
		}
		else if(!strcmp(opt, "g")) { 
			fscanf_s(objFile, "%s\n", data, 256);	
			if(curgroup.name.length() != 0) { 
				ret->groups.push_back(curgroup);
				curgroup.faces.clear();
			}
			curgroup.name = data;
		}
		else if(!strcmp(opt, "usemtl")) {
			fscanf_s(objFile, "%s\n", data, 256);
			curgroup.matName = data;
		}
		else if(!strcmp(opt, "f")) { // indeksy face'a
			spcs = 0;
			cRe = 0;
			fgets(data, 256, objFile);
			for(unsigned int i = 0, k = 0; i < strnlen_s(data, 256); ++i) {
				if(data[i] == '/') ++spcs;
				if(spcs == 2) {
					spcs = 0;
					sscanf_s(&data[cRe], "%u/%u/%u", &idx.vi, &idx.vti, &idx.vni);
					face.vertices.push_back(idx);
					for(; i < strnlen_s(data, 256); ++i) {
						if(isspace(data[i])) { 
							cRe = i;
							break;
						}
					}
				}
			}

			if(face.vertices.size() == 4) {
				face1.vertices.push_back(face.vertices[0]);
				face1.vertices.push_back(face.vertices[1]);
				face1.vertices.push_back(face.vertices[2]);
				
				curgroup.faces.push_back(face1);
				face1.vertices.clear();
				
				face1.vertices.push_back(face.vertices[0]);
				face1.vertices.push_back(face.vertices[2]);
				face1.vertices.push_back(face.vertices[3]);

				curgroup.faces.push_back(face1);
				face1.vertices.clear();
			} else {
				curgroup.faces.push_back(face);
			}
			face.vertices.clear();
		}
	}
	fclose(objFile);

	if(curgroup.name.size() != 0) { 
		ret->groups.push_back(curgroup);
	}

	if(mtlFilePath[0] == NULL) {
		return;
	}

	fopen_s(&mtlFile, mtlFilePath, "r");
	assert(mtlFile != NULL && "MTL file doesn't exist");

	while(!feof(mtlFile)) {
		fscanf_s(objFile, "%s ", opt, 64);

		if(opt[0] == '#') {
			continue; 
		}
		else if(!strcmp(opt, "newmtl")) {
			fscanf_s(mtlFile, "%s\n", data, 256);

			if(mat.name.size() != NULL) { 
				ret->mats.push_back(mat);
			}

			mat.name = data;
			mat.map_Kd.clear();
			mat.map_bump.clear();
			mat.map_d.clear();
			mat.map_Ka.clear();
		}
		else if(!strcmp(opt, "Ns")) {
			fscanf_s(mtlFile, "%f\n", &mat.Ns);
		}
		else if(!strcmp(opt, "Ni")) {
			fscanf_s(mtlFile, "%f\n", &mat.Ni);
		}
		else if(!strcmp(opt, "d")) {
			fscanf_s(mtlFile, "%f\n", &mat.d);
		}
		else if(!strcmp(opt, "Ns")) {
			fscanf_s(mtlFile, "%f\n", &mat.Tr);
		}
		else if(!strcmp(opt, "Tf")) {
			fscanf_s(mtlFile, "%f %f %f\n", &mat.Tf.x, &mat.Tf.y, &mat.Tf.z);
		}
		else if(!strcmp(opt, "illum")) {
			fscanf_s(mtlFile, "%f\n", &mat.illum);
		}
		else if(!strcmp(opt, "Ka")) {
			fscanf_s(mtlFile, "%f %f %f\n", &mat.Ka.x, &mat.Ka.y, &mat.Ka.z);
		}
		else if(!strcmp(opt, "Kd")) {
			fscanf_s(mtlFile, "%f %f %f\n", &mat.Kd.x, &mat.Kd.y, &mat.Kd.z);
		}
		else if(!strcmp(opt, "Ks")) {
			fscanf_s(mtlFile, "%f %f %f\n", &mat.Ks.x, &mat.Ks.y, &mat.Ks.z);
		}
		else if(!strcmp(opt, "Ke")) {
			fscanf_s(mtlFile, "%f %f %f\n", &mat.Ke.x, &mat.Ke.y, &mat.Ke.z);
		}
		else if(!strcmp(opt, "map_Ka")) {
			fscanf_s(mtlFile, "%s\n", data, 256);
			mat.map_Ka = data;
		}
		else if(!strcmp(opt, "map_Kd")) {
			fscanf_s(mtlFile, "%s\n", data, 256);
			mat.map_Kd = data;
		}
		else if(!strcmp(opt, "map_d")) {
			fscanf_s(mtlFile, "%s\n", data, 256);
			mat.map_d = data;
		}
		else if(!strcmp(opt, "map_bump")) {
			fscanf_s(mtlFile, "%s\n", data, 256);
			mat.map_bump = data;
		}
		else if(!strcmp(opt, "bump")) {
			fscanf_s(mtlFile, "%s\n", data, 256);
			mat.bump = data;
		}
	}

	if(mat.name.size() != NULL) { 
		ret->mats.push_back(mat);
	}
}


