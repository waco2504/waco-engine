#include "../engine_main/Math.hpp"
#include "../engine_main/OBJLoader.hpp"

/*
EFILESTRUCT:
header;
groupdata;
indices;
vertices;
*/

struct EMESHFILEHEADER {
	char fileid[10];
	unsigned int groupCount;
	unsigned int indexCount;
	unsigned int verticesCount;
};

struct GROUPDATA {
	char gName[16];
	char matName[16];
	unsigned int indexOffset;
	unsigned int verticesCount;
	EVECTOR BB[8];
};

struct EVERTEX {
	EVECTOR3 position;
	EVECTOR3 normal;
	EVECTOR4 tangent;
	EVECTOR3 texture0;
};

void load(const char* fname);

const EVECTOR3 floatMax(FLT_MAX, FLT_MAX, FLT_MAX);
const EVECTOR3 floatMin(-FLT_MAX, -FLT_MAX, -FLT_MAX);
EVECTOR3 cgmax, cgmin;
OBJLOADER_OBJECT ob;
std::vector<unsigned int> egoffsets;
std::vector<unsigned int> eindices;
std::vector<EVERTEX> evertices;

int main() {
	FILE* out = NULL;
	EMESHFILEHEADER head;
	GROUPDATA gdata;
	char ifn[32] = {0};
	char ofn[32] = {0};

	scanf_s("%s", ifn, 32);
	scanf_s("%s", ofn, 32);

	load(ifn);
	fopen_s(&out, ofn, "wb");

	strcpy(head.fileid, "EMESHFILE");
	head.groupCount = ob.groups.size();
	head.indexCount = eindices.size();
	head.verticesCount = evertices.size();

	fwrite(&head, sizeof(EMESHFILEHEADER), 1, out);

	for(unsigned int gi = 0; gi < head.groupCount; ++gi) {
		strcpy_s(gdata.gName, ob.groups[gi].name.c_str());
		strcpy_s(gdata.matName, ob.groups[gi].matName.c_str());
		gdata.indexOffset = egoffsets[gi];
		gdata.verticesCount = ob.groups[gi].faces.size() / 3;

		cgmax = floatMin;
		cgmin = floatMax;
		for(unsigned int vi = 0; vi < ob.groups[gi].faces.size()*3; ++vi) {
			for(unsigned int f = 0; f < 3; ++f) {
				if(evertices[eindices[vi+egoffsets[gi]]].position[f] > cgmax[f]) 
					cgmax[f] = evertices[eindices[vi+egoffsets[gi]]].position[f];
				if(evertices[eindices[vi+egoffsets[gi]]].position[f] < cgmin[f]) 
					cgmin[f] = evertices[eindices[vi+egoffsets[gi]]].position[f];
			}
		}

		gdata.BB[0] = cgmax;
		gdata.BB[1] = EVECTOR3(cgmin.x, cgmax.y, cgmax.z);
		gdata.BB[2] = EVECTOR3(cgmin.x, cgmax.y, cgmin.z);
		gdata.BB[3] = EVECTOR3(cgmax.x, cgmax.y, cgmin.z);
		gdata.BB[4] = EVECTOR3(cgmax.x, cgmin.y, cgmin.z);
		gdata.BB[5] = EVECTOR3(cgmax.x, cgmin.y, cgmax.z);
		gdata.BB[6] = EVECTOR3(cgmin.x, cgmin.y, cgmax.z);
		gdata.BB[7] = cgmin;
		
		fwrite(&gdata, sizeof(GROUPDATA), 1, out);
	}

	fwrite(eindices.data(), sizeof(unsigned int), eindices.size(), out);
	fwrite(evertices.data(), sizeof(EVERTEX), evertices.size(), out);


	fclose(out);
	return 0;
}

void load(const char* fname) {
	const EVECTOR3 floatMax(FLT_MAX, FLT_MAX, FLT_MAX);
	const EVECTOR3 floatMin(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	OBJLoader obLoader;
	unsigned int curIndex = 0;
	EVERTEX curVertex;
	EVECTOR3 cgmax;
	EVECTOR3 cgmin;
	EVECTOR4 v1, v2;
	EVECTOR3 st1, st2;

	obLoader.loadObjectFromOBJ(&ob, fname);

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
}