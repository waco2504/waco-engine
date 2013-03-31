#pragma once

#ifndef _INC_FACTORYMESHDX10_
#define _INC_FACTORYMESHDX10_

#include "RendererDX10.hpp"

class FactoryMeshDX10 : public DXClass {
	OBJLoader* pobjLoader;
	ResourceMenagerDX10* resMen;

	std::map<std::string, MESHDX10> meshes;
	std::map<std::string, MATERIALDX10> materials;
	std::vector<RENDERABLE> batches;

	const std::string getUniqueName();
public:
	FactoryMeshDX10();
	~FactoryMeshDX10();
	void init(ResourceMenagerDX10*);
	bool loadMeshesFromOBJ(std::string& filePath);
	bool loadMeshesFromMeshFile(std::string& filePath);
	bool convertOBJToMeshFile(std::string& inFile, std::string& outFile);
	MESHDX10* getMesh(const char* meshName);
	MATERIALDX10* getMaterial(const char* matName);
	unsigned int getLoadedBatchSize();
	RENDERABLE getLoadedBatch(unsigned int i);
};

#endif