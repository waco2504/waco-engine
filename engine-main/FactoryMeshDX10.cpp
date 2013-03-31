#include "FactoryMeshDX10.hpp"

struct EMESHFILEHEADER {
	char fileid[10];
	unsigned int groupCount;
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
		strcpy_s(materials[ob.mats[i].name].matName, 32, ob.mats[i].name.c_str());
		materials[ob.mats[i].name].Kd = ob.mats[i].Kd;
		if(ob.mats[i].map_Kd.size() > 0) {
			resMen->loadTexture2DFromFile(ob.mats[i].map_Kd, ob.mats[i].map_Kd.c_str());
			materials[ob.mats[i].name].diffuseMap = resMen->getData(ob.mats[i].map_Kd)->srv;
		} else {
			materials[ob.mats[i].name].diffuseMap = NULL;
		}
		if(ob.mats[i].map_bump.size() > 0) {
			resMen->loadTexture2DFromFile(ob.mats[i].map_bump, ob.mats[i].map_bump.c_str());
			materials[ob.mats[i].name].normalMap = resMen->getData(ob.mats[i].map_bump)->srv;
		} else {
			materials[ob.mats[i].name].normalMap = NULL;
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
		strcpy_s(meshes[ob.groups[i].name].meshName, 32, ob.groups[i].name.c_str());
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
	/*FILE* file = NULL;
	EMESHFILEHEADER head;
	EGROUPDATA* groups;
	unsigned int* indices;
	EVERTEX* vertices;

	if(fopen_s(&file, filePath, "r")) return false;
	
	fread(&head, sizeof(EMESHFILEHEADER), 1, file);

	if(strcmp(head.fileid, "EMESHFILE")) return false;
	
	groups = new EGROUPDATA[head.groupCount];
	indices = new unsigned int[head.indexCount];
	vertices = new EVERTEX[head.verticesCount];

	fread(groups, sizeof(EGROUPDATA), head.groupCount, file);
	fread(indices, sizeof(unsigned int), head.indexCount, file);
	fread(vertices, sizeof(EVERTEX), head.verticesCount, file);


	std::string n1 = getUniqueName();
	std::string n2 = getUniqueName();
	resMen->createIndexBuffer(n1, head.indexCount*sizeof(unsigned int), 
		indices);
	resMen->createVertexBuffer(n2, head.verticesCount*sizeof(EVERTEX), 
		vertices);

	for(unsigned int i = 0; i < head.groupCount; ++i) {
		std::string name(groups[i].gName);
		strcpy_s(meshes[name].meshName, 32, groups[i].gName); 
		meshes[name].pdxindexBuffer = resMen->getData(n1)->buf;
		meshes[name].pdxvertexBuffer = resMen->getData(n2)->buf;
		meshes[name].indexCount = head.indexCount;
		meshes[name].indexStart = groups[i].indexStart;
		meshes[name].vertexStart = 0;

		for(unsigned int v = 0; v < 8; ++v) {
			meshes[name].BB[v] = groups[i].BB[v];
		}

		// tutaj dodac moze do jakiegos logera ze nie wczytano poprawnie danych
		batches.push_back(RENDERABLE());
		if(meshes.count(name))
			batches.back().Mesh = &meshes[name];
		else 
			batches.back().Mesh = NULL;
		batches.back().Material = NULL;
		batches.back().World.identity();
	}

	delete [] groups;
	delete [] indices;
	delete [] vertices;
	fclose(file);*/
	MessageBox(NULL, "loadMeshesFromMeshFile nie dziala", "", MB_OK);
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


	
	return true;
}

MESHDX10* FactoryMeshDX10::getMesh(const char* meshName) {
	char mna[32] = {0};
	unsigned int len = strlen(meshName);
	if(len > 31) len = 31;
	memcpy_s(mna,32,meshName,len);
	return &meshes[mna];
}

MATERIALDX10* FactoryMeshDX10::getMaterial(const char* matName) {
	char mna[32] = {0};
	unsigned int len = strlen(matName);
	if(len > 31) len = 31;
	memcpy_s(mna,32,matName,len);
	return &materials[mna];
}

unsigned int FactoryMeshDX10::getLoadedBatchSize() {
	return batches.size();
}

RENDERABLE FactoryMeshDX10::getLoadedBatch(unsigned int i) {
	if(i >= batches.size()) return RENDERABLE();
	return  batches[i];
}

