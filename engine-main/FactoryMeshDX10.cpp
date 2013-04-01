#include "FactoryMeshDX10.hpp"

#include <fstream>

struct EMESHFILEHEADER {
	char fileid[16];
	char mtlFile[64];
	unsigned int groupCount;
	unsigned int matsCount;
	unsigned int indexCount;
	unsigned int verticesCount;
};

struct EGROUPDATA {
	char gName[32];
	char matName[32];
	unsigned int indexStart;
	unsigned int indexCount;
	EVECTOR BB[8];
};

struct EMATERIALDATA {
	char matName[32];
	EVECTOR diffuseCol;
	char diffuseMapFile[64];
	char normalMapFile[64];
	char specularMapFile[64];
};


typedef MESHDX10::EVERTEX EVERTEX;

const std::string FactoryMeshDX10::getUniqueName() {
	// kazda kolejna nazwa buffora zawiera ta liczbe
	static int namegen = 0;
	char num[128] = {0};
	std::string ret("FM");

	_itoa_s(++namegen, num, 128, 10);
	
	ret += num;

	return std::string(ret);
}

FactoryMeshDX10::FactoryMeshDX10() : resMen(NULL) {
	pobjLoader = new OBJLoader();
}

FactoryMeshDX10::~FactoryMeshDX10() {
	delete pobjLoader;
}

void FactoryMeshDX10::init(ResourceMenagerDX10* rM) {
	resMen = rM;
}

bool FactoryMeshDX10::loadMeshesFromOBJ(std::string& filePath) {
	if(resMen == NULL) return false;
	OBJLOADER_OBJECT ob;
	pobjLoader->loadObjectFromOBJ(&ob, filePath);
	
	unsigned int curIndex = 0;
	EVERTEX curVertex;

	std::vector<unsigned int> egoffsets;
	std::vector<unsigned int> eindices;
	std::vector<EVERTEX> evertices;

	const EVECTOR3 floatMax(FLT_MAX, FLT_MAX, FLT_MAX);
	const EVECTOR3 floatMin(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	EVECTOR3 cgmax;
	EVECTOR3 cgmin;
	EVECTOR4 v1, v2;
	EVECTOR3 st1, st2;

	for(unsigned int p = 0; p < ob.poss.size(); ++p) {
		curVertex.position = ob.poss[p];
		evertices.push_back(curVertex);
	}
	
	egoffsets.push_back(0);
	for(unsigned int gi = 0; gi < ob.groups.size(); ++gi) {
		for(unsigned int fi = 0; fi < ob.groups[gi].faces.size(); ++fi) {
			for(unsigned int ii = 0; ii < ob.groups[gi].faces[fi].vertices.size(); ++ii) { // 3
				unsigned int vi = ob.groups[gi].faces[fi].vertices[ii].vi-1;		
				
				if(!(evertices[vi].normal == EVECTOR3(0.0f,0.0f,0.0f))) {
					curVertex = evertices[vi];
					curVertex.normal = ob.norms[ob.groups[gi].faces[fi].vertices[ii].vni-1];
					curVertex.texture0 = ob.coords[ob.groups[gi].faces[fi].vertices[ii].vti-1];

					evertices.push_back(curVertex);
					eindices.push_back(evertices.size()-1);
				} else {
					evertices[vi].normal = ob.norms[ob.groups[gi].faces[fi].vertices[ii].vni-1];
					evertices[vi].texture0 = ob.coords[ob.groups[gi].faces[fi].vertices[ii].vti-1];
				
					eindices.push_back(vi);
				}
			}
		}
		egoffsets.push_back(eindices.size());
	}

	for(unsigned int i = 0; i < ob.mats.size(); ++i) {
		materials[ob.mats[i].name].matName = ob.mats[i].name;
		materials[ob.mats[i].name].Kd = ob.mats[i].Kd;

		// ladowanie mapy koloru diffuse
		if(ob.mats[i].map_Kd.length() > 0) {
			resMen->loadTexture2DFromFile(ob.mats[i].map_Kd, ob.mats[i].map_Kd.c_str());
			materials[ob.mats[i].name].diffuseMap = resMen->getData(ob.mats[i].map_Kd)->srv;
		} else {
			materials[ob.mats[i].name].diffuseMap = NULL;
		}
		// ladownaie mapy normalnych
		if(ob.mats[i].map_bump.length() > 0) {
			resMen->loadTexture2DFromFile(ob.mats[i].map_bump, ob.mats[i].map_bump.c_str());
			materials[ob.mats[i].name].normalMap = resMen->getData(ob.mats[i].map_bump)->srv;
		} else {
			materials[ob.mats[i].name].normalMap = NULL;
		}
		// ladownie mapy koloru (czy wspolczynnika?) specular
		if(ob.mats[i].map_Ks.length() > 0) {
			resMen->loadTexture2DFromFile(ob.mats[i].map_Ks, ob.mats[i].map_Ks.c_str());
			materials[ob.mats[i].name].specularMap = resMen->getData(ob.mats[i].map_Ks)->srv;
		} else {
			materials[ob.mats[i].name].specularMap = NULL;
		}
	}

	/// liczenie tangentow
	EVECTOR3 *tan1 = new EVECTOR3[evertices.size() * 2];
    EVECTOR3 *tan2 = tan1 + evertices.size();
    ZeroMemory(tan1, evertices.size() * sizeof(EVECTOR3) * 2);
    
	for(unsigned int gi = 0; gi < ob.groups.size(); ++gi) {
		for(unsigned int fi = 0; fi < ob.groups[gi].faces.size(); ++fi) {
			unsigned int i1 = eindices[0+3*fi+egoffsets[gi]];
			unsigned int i2 = eindices[1+3*fi+egoffsets[gi]];
			unsigned int i3 = eindices[2+3*fi+egoffsets[gi]];

			const EVECTOR3& v1 = evertices[i1].position;
			const EVECTOR3& v2 = evertices[i2].position;
			const EVECTOR3& v3 = evertices[i3].position;

			const EVECTOR3& w1 = evertices[i1].texture0;
			const EVECTOR3& w2 = evertices[i2].texture0;
			const EVECTOR3& w3 = evertices[i3].texture0;

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			EVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			EVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;
        
			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}
	}
    
	for(unsigned int vi = 0; vi < evertices.size(); ++vi) {
		const EVECTOR3& n = evertices[vi].normal;
		const EVECTOR3& t = tan1[vi];
		EVECTOR3 tangent;
		// Gram-Schmidt orthogonalize
		tangent = (t-n * n.dotProduct(t));
		tangent.normalize();
		evertices[vi].tangent = tangent;
		// Calculate handedness
		evertices[vi].tangent.w = (n.crossProduct(t).dotProduct(tan2[vi]) < 0.0f) ? -1.0F : 1.0F;
	}

	delete [] tan1;
	///

	std::string n1 = getUniqueName();
	std::string n2 = getUniqueName();
	resMen->createIndexBuffer(n1, eindices.size()*sizeof(unsigned int), 
		eindices.data());
	resMen->createVertexBuffer(n2, evertices.size()*sizeof(EVERTEX), 
		evertices.data());

	for(unsigned int i = 0; i < ob.groups.size(); ++i) {
		meshes[ob.groups[i].name].meshName = ob.groups[i].name;
		meshes[ob.groups[i].name].pdxindexBuffer = resMen->getData(n1)->buf;
		meshes[ob.groups[i].name].pdxvertexBuffer = resMen->getData(n2)->buf;
		meshes[ob.groups[i].name].indexCount = ob.groups[i].faces.size()*3;
		meshes[ob.groups[i].name].indexStart = egoffsets[i];
		meshes[ob.groups[i].name].vertexStart = 0;

		cgmax = floatMin;
		cgmin = floatMax;
		for(unsigned int vi = 0; vi < ob.groups[i].faces.size()*3; ++vi) {
			for(unsigned int f = 0; f < 3; ++f) {
				if(evertices[eindices[vi+egoffsets[i]]].position[f] > cgmax[f]) 
					cgmax[f] = evertices[eindices[vi+egoffsets[i]]].position[f];
				if(evertices[eindices[vi+egoffsets[i]]].position[f] < cgmin[f]) 
					cgmin[f] = evertices[eindices[vi+egoffsets[i]]].position[f];
			}
		}

		meshes[ob.groups[i].name].BB[0] = cgmax;
		meshes[ob.groups[i].name].BB[1] = EVECTOR3(cgmin.x, cgmax.y, cgmax.z);
		meshes[ob.groups[i].name].BB[2] = EVECTOR3(cgmin.x, cgmax.y, cgmin.z);
		meshes[ob.groups[i].name].BB[3] = EVECTOR3(cgmax.x, cgmax.y, cgmin.z);
		meshes[ob.groups[i].name].BB[4] = EVECTOR3(cgmax.x, cgmin.y, cgmin.z);
		meshes[ob.groups[i].name].BB[5] = EVECTOR3(cgmax.x, cgmin.y, cgmax.z);
		meshes[ob.groups[i].name].BB[6] = EVECTOR3(cgmin.x, cgmin.y, cgmax.z);
		meshes[ob.groups[i].name].BB[7] = cgmin;

		// tutaj dodac moze do jakiegos logera ze nie wczytano poprawnie danych
		batches.push_back(RENDERABLE());
		batches.back().Mesh = NULL;
		batches.back().Material = NULL;
		batches.back().World.identity();

		if(meshes.count(ob.groups[i].name))
			batches.back().Mesh = &meshes[ob.groups[i].name];
		if(materials.count(ob.groups[i].matName))
			batches.back().Material = &materials[ob.groups[i].matName];
	}

	return true;
}

bool FactoryMeshDX10::loadMeshesFromMeshFile(std::string& filePath) {
	std::ifstream file;
	EMESHFILEHEADER head;
	EGROUPDATA group;
	EMATERIALDATA material;
	unsigned int* indices = NULL;
	EVERTEX* vertices = NULL;
	
	file.open(filePath.c_str());

	file.read((char*)&head, sizeof(EMESHFILEHEADER));

	if(strcmp(head.fileid, "WACOMESH") != 0) {
		file.close();
		return false;
	}

	indices = new unsigned int[head.indexCount];
	vertices = new EVERTEX[head.verticesCount];

	for(unsigned int i = 0; i < head.indexCount; ++i)
		file.read((char*)&indices[i], sizeof(unsigned int));
	for(unsigned int i = 0; i < head.verticesCount; ++i)
		file.read((char*)&vertices[i], sizeof(EVERTEX));

	std::string n1 = getUniqueName();
	std::string n2 = getUniqueName();
	resMen->createIndexBuffer(n1, head.indexCount*sizeof(unsigned int), 
		indices);
	resMen->createVertexBuffer(n2, head.verticesCount*sizeof(EVERTEX), 
		vertices);

	delete [] indices;
	delete [] vertices;


	for(unsigned int i = 0; i < head.matsCount; ++i) {
		file.read((char*)&material, sizeof(EMATERIALDATA));

		materials[material.matName].matName = material.matName;
		materials[material.matName].Kd = material.diffuseCol;

		// ladowanie mapy koloru diffuse
		if(strlen(material.diffuseMapFile) > 0) {
			resMen->loadTexture2DFromFile(std::string(material.diffuseMapFile), material.diffuseMapFile);
			materials[material.matName].diffuseMap = 
				resMen->getData(std::string(material.diffuseMapFile))->srv;
		} else {
			materials[material.matName].diffuseMap = NULL;
		}
		// ladownaie mapy normalnych
		if(strlen(material.normalMapFile) > 0) {
			resMen->loadTexture2DFromFile(std::string(material.normalMapFile), material.normalMapFile);
			materials[material.matName].normalMap = 
				resMen->getData(std::string(material.normalMapFile))->srv;
		} else {
			materials[material.matName].normalMap = NULL;
		}
		// ladownie mapy koloru (czy wspolczynnika?) specular
		if(strlen(material.specularMapFile) > 0) {
			resMen->loadTexture2DFromFile(std::string(material.specularMapFile), material.specularMapFile);
			materials[material.matName].specularMap = 
				resMen->getData(std::string(material.specularMapFile))->srv;
		} else {
			materials[material.matName].specularMap = NULL;
		}
	}


	for(unsigned int i = 0; i < head.groupCount; ++i) {
		file.read((char*)&group, sizeof(EGROUPDATA));

		meshes[group.gName].meshName = group.gName;
		meshes[group.gName].pdxindexBuffer = resMen->getData(n1)->buf;
		meshes[group.gName].pdxvertexBuffer = resMen->getData(n2)->buf;
		meshes[group.gName].indexCount = group.indexCount;
		meshes[group.gName].indexStart = group.indexStart;
		meshes[group.gName].vertexStart = 0; 

		for(unsigned int j = 0; j < 8; ++j) {
			meshes[group.gName].BB[j] = group.BB[j];
		}

		// tutaj dodac moze do jakiegos logera ze nie wczytano poprawnie danych
		batches.push_back(RENDERABLE());
		batches.back().Mesh = NULL;
		batches.back().Material = NULL;
		batches.back().World.identity();

		if(meshes.count(group.gName))
			batches.back().Mesh = &meshes[group.gName];
		if(materials.count(group.matName))
			batches.back().Material = &materials[group.matName];
	}

	file.close();

	return true;
}

bool FactoryMeshDX10::convertOBJToMeshFile(std::string& inFile, std::string& outFile) {
	if(resMen == NULL) return false;
	OBJLOADER_OBJECT ob;
	pobjLoader->loadObjectFromOBJ(&ob, inFile);
	
	unsigned int curIndex = 0;
	EVERTEX curVertex;

	std::vector<unsigned int> egoffsets;
	std::vector<unsigned int> eindices;
	std::vector<EVERTEX> evertices;

	const EVECTOR3 floatMax(FLT_MAX, FLT_MAX, FLT_MAX);
	const EVECTOR3 floatMin(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	EVECTOR3 cgmax;
	EVECTOR3 cgmin;
	EVECTOR4 v1, v2;
	EVECTOR3 st1, st2;

	for(unsigned int p = 0; p < ob.poss.size(); ++p) {
		curVertex.position = ob.poss[p];
		evertices.push_back(curVertex);
	}
	
	egoffsets.push_back(0);
	for(unsigned int gi = 0; gi < ob.groups.size(); ++gi) {
		for(unsigned int fi = 0; fi < ob.groups[gi].faces.size(); ++fi) {
			for(unsigned int ii = 0; ii < ob.groups[gi].faces[fi].vertices.size(); ++ii) { // 3
				unsigned int vi = ob.groups[gi].faces[fi].vertices[ii].vi-1;		
				
				if(!(evertices[vi].normal == EVECTOR3(0.0f,0.0f,0.0f))) {
					curVertex = evertices[vi];
					curVertex.normal = ob.norms[ob.groups[gi].faces[fi].vertices[ii].vni-1];
					curVertex.texture0 = ob.coords[ob.groups[gi].faces[fi].vertices[ii].vti-1];

					evertices.push_back(curVertex);
					eindices.push_back(evertices.size()-1);
				} else {
					evertices[vi].normal = ob.norms[ob.groups[gi].faces[fi].vertices[ii].vni-1];
					evertices[vi].texture0 = ob.coords[ob.groups[gi].faces[fi].vertices[ii].vti-1];
				
					eindices.push_back(vi);
				}
			}
		}
		egoffsets.push_back(eindices.size());
	}

	/// liczenie tangentow
	EVECTOR3 *tan1 = new EVECTOR3[evertices.size() * 2];
    EVECTOR3 *tan2 = tan1 + evertices.size();
    ZeroMemory(tan1, evertices.size() * sizeof(EVECTOR3) * 2);
    
	for(unsigned int gi = 0; gi < ob.groups.size(); ++gi) {
		for(unsigned int fi = 0; fi < ob.groups[gi].faces.size(); ++fi) {
			unsigned int i1 = eindices[0+3*fi+egoffsets[gi]];
			unsigned int i2 = eindices[1+3*fi+egoffsets[gi]];
			unsigned int i3 = eindices[2+3*fi+egoffsets[gi]];

			const EVECTOR3& v1 = evertices[i1].position;
			const EVECTOR3& v2 = evertices[i2].position;
			const EVECTOR3& v3 = evertices[i3].position;

			const EVECTOR3& w1 = evertices[i1].texture0;
			const EVECTOR3& w2 = evertices[i2].texture0;
			const EVECTOR3& w3 = evertices[i3].texture0;

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			EVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			EVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;
        
			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}
	}
    
	for(unsigned int vi = 0; vi < evertices.size(); ++vi) {
		const EVECTOR3& n = evertices[vi].normal;
		const EVECTOR3& t = tan1[vi];
		EVECTOR3 tangent;
		// Gram-Schmidt orthogonalize
		tangent = (t-n * n.dotProduct(t));
		tangent.normalize();
		evertices[vi].tangent = tangent;
		// Calculate handedness
		evertices[vi].tangent.w = (n.crossProduct(t).dotProduct(tan2[vi]) < 0.0f) ? -1.0F : 1.0F;
	}

	delete [] tan1;
	///

	EMESHFILEHEADER fileHeader;
	EGROUPDATA group;
	EMATERIALDATA material;
	ZeroMemory(&fileHeader, sizeof(EMESHFILEHEADER));
	
	strcpy_s(fileHeader.fileid, "WACOMESH");
	strcpy_s(fileHeader.mtlFile, ob.mtlFileName.c_str());
	fileHeader.groupCount = ob.groups.size();
	fileHeader.matsCount = ob.mats.size();
	fileHeader.indexCount = eindices.size();
	fileHeader.verticesCount = evertices.size();

	std::ofstream ofile(outFile);
	ofile.write((char*)&fileHeader, sizeof(EMESHFILEHEADER));

	for(unsigned int i = 0; i < eindices.size(); ++i) {
		ofile.write((char*)&eindices[i], sizeof(unsigned int));
	}

	for(unsigned int i = 0; i < evertices.size(); ++i) {
		ofile.write((char*)&evertices[i], sizeof(EVERTEX));
	}

	for(unsigned int i = 0; i < ob.mats.size(); ++i) {
		ZeroMemory(&material, sizeof(EMATERIALDATA));

		strcpy_s(material.matName, ob.mats[i].name.c_str());
		material.diffuseCol = ob.mats[i].Kd;

		if(ob.mats[i].map_Kd.length() > 0)
			strcpy_s(material.diffuseMapFile, ob.mats[i].map_Kd.c_str());

		if(ob.mats[i].bump.length() > 0)
			strcpy_s(material.normalMapFile, ob.mats[i].bump.c_str());

		if(ob.mats[i].map_Ks.length() > 0)
			strcpy_s(material.specularMapFile, ob.mats[i].map_Ks.c_str());

		ofile.write((char*)&material, sizeof(EMATERIALDATA));
	}

	for(unsigned int i = 0; i < ob.groups.size(); ++i) {
		ZeroMemory(&group, sizeof(EGROUPDATA));
		strcpy_s(group.gName, ob.groups[i].name.c_str());
		strcpy_s(group.matName, ob.groups[i].matName.c_str());
		group.indexStart = egoffsets[i];
		group.indexCount = ob.groups[i].faces.size()*3;

		cgmax = floatMin;
		cgmin = floatMax;
		for(unsigned int vi = 0; vi < ob.groups[i].faces.size()*3; ++vi) {
			for(unsigned int f = 0; f < 3; ++f) {
				if(evertices[eindices[vi+egoffsets[i]]].position[f] > cgmax[f]) 
					cgmax[f] = evertices[eindices[vi+egoffsets[i]]].position[f];
				if(evertices[eindices[vi+egoffsets[i]]].position[f] < cgmin[f]) 
					cgmin[f] = evertices[eindices[vi+egoffsets[i]]].position[f];
			}
		}

		group.BB[0] = cgmax;
		group.BB[1] = EVECTOR3(cgmin.x, cgmax.y, cgmax.z);
		group.BB[2] = EVECTOR3(cgmin.x, cgmax.y, cgmin.z);
		group.BB[3] = EVECTOR3(cgmax.x, cgmax.y, cgmin.z);
		group.BB[4] = EVECTOR3(cgmax.x, cgmin.y, cgmin.z);
		group.BB[5] = EVECTOR3(cgmax.x, cgmin.y, cgmax.z);
		group.BB[6] = EVECTOR3(cgmin.x, cgmin.y, cgmax.z);
		group.BB[7] = cgmin;

		ofile.write((char*)&group, sizeof(EGROUPDATA));
	}
	
	ofile.close();

	return true;
}

MESHDX10* FactoryMeshDX10::getMesh(std::string& meshName) {
	return &meshes[meshName];
}

MATERIALDX10* FactoryMeshDX10::getMaterial(std::string& matName) {
	return &materials[matName];
}

unsigned int FactoryMeshDX10::getLoadedBatchSize() {
	return batches.size();
}

RENDERABLE FactoryMeshDX10::getLoadedBatch(unsigned int i) {
	if(i >= batches.size()) return RENDERABLE();
	return  batches[i];
}

