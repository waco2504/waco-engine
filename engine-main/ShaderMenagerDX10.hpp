#pragma once

#ifndef _INC_SHADERMENAGERDX10_
#define _INC_SHADERMENAGERDX10_

#include <map>
#include <vector>

#include "DXClass.hpp"
#include "Math.hpp"

#ifndef SHADERFILE
#define SHADERFILE "..\\engine-main\\shader.hlsl"
#endif

struct SHADERSETDX10 {
	enum TYPE : int { //  do indeksowania shadermecros
		SHADOWMAP2D,
		SHADOWMAP2DDIST,
		SHADOWMAPCUBE,
		SOLIDCOLOR,
		TEXCOLOR,
		SPOTLIGHT,
		POINTLIGHT,
		SPOTLIGHTSHADOW,
		POINTLIGHTSHADOW,
		POINTLIGHTBUMP,
		POINTLIGHTBUMPSHADOW,
		SPOTLIGHTBUMP,
		SPOTLIGHTBUMPSHADOW,
		BLUR,
		BLURCUBE,
		SSNORMALS,
		SSAO,
		FINALMERGE,

		SPOTLIGHTSPEC,	
		POINTLIGHTSPEC,		
		SPOTLIGHTSPECSHADOW,
		POINTLIGHTSPECSHADOW,
		POINTLIGHTSPECBUMP,			
		POINTLIGHTSPECBUMPSHADOW,
		SPOTLIGHTSPECBUMP,
		SPOTLIGHTSPECBUMPSHADOW,
	};
	TYPE Type;
	ID3D10SamplerState* SamplerState;
	ID3D10InputLayout* InputLayout;
	ID3D10VertexShader* VertexShader;	
	ID3D10GeometryShader* GeometryShader;	
	ID3D10PixelShader* PixelShader;
};

class ShaderMenagerDX10 : public DXClass {
public:
	struct LIGHT_DESCREPTION {
		EVECTOR4 Position;
		EVECTOR4 Color;
		EVECTOR4 Direction;
		EVECTOR4 Parameters;
		EMATRIX View;
		EMATRIX Proj;
	};
private:
	struct WORLD_CONST_BUFFER {
		EMATRIX View;
		EMATRIX Proj;
		EMATRIX CubeView[6];
	} worldConst;

	struct OBJECT_CONST_BUFFER {
		EVECTOR4 CameraPosition;
		EVECTOR4 Color;
		EMATRIX WorldViewProj;
		EMATRIX World;
		EMATRIX LWorldViewProj;
	} objectConst;

	struct LIGHT_CONST_BUFFER {
		EVECTOR4 LPosition;
		EVECTOR4 LColor;
		EVECTOR4 LDirection;
		EVECTOR4 LParameters;
		EMATRIX LView;
		EMATRIX LProj;
	} lightConst;

	struct POSTPROCESS_CONST_BUFFER {
		EVECTOR4 TexelSize;
	} postProcessConst;

	struct SSAO_CONST_BUFFER {
		float totStrength;
		float strength;
		float offset;
		float falloff;
		float rad;
		float emptyShit[3];
	} sSAOBuffer;

	ID3D10Buffer* WorldConst;
	ID3D10Buffer* ObjectConst;
	ID3D10Buffer* LightConst;
	ID3D10Buffer* PostProcessConst;
	ID3D10Buffer* SSAOBuffer;

	ID3D10SamplerState* defaultSampler;
	ID3D10SamplerState* linearSampler;
	ID3D10SamplerState* pointSampler;

	std::vector<ID3D10ShaderResourceView*> activeTextures;
	
	std::string uncompiledShader;
	std::map<SHADERSETDX10::TYPE, SHADERSETDX10> shaderSets;
	SHADERSETDX10::TYPE activeShader;
	
	std::string readShaderFile(const char*);
	void procCompilerOutput(HRESULT hr, ID3D10Blob* compilerErrors, 
		const char* info);
	SHADERSETDX10 compileShaderSet(SHADERSETDX10::TYPE shaderType, 
		const D3D10_SHADER_MACRO* macros);
	//SHADERSETDX10 loadPrecompiledShaderSet(SHADERSETDX10::TYPE shaderType);

	void initConstantBuffers();
	void initShaders();

	void updateSamplers();
	void updateTextures();
	void updateWorldBuffer();
	void updateObjectBuffer();
	void updateLightBuffer();
	void updatePostProcessBuffer();
	void updateSSAOBuffer();	
public:
	void init();
	
	void setCameraPosition(const EVECTOR4& camPos);
	void setObjectColor(const EVECTOR4& col);
	void setWorldMatrix(const EMATRIX& mat);
	void setViewMatrix(const EMATRIX& mat);
	void setCubeViewMatrix(UINT idx, const EMATRIX& mat);
	void setProjMatrix(const EMATRIX& mat);
	void setLightDesc(const LIGHT_DESCREPTION* desc);
	void setTextureSRV(unsigned int size, ID3D10ShaderResourceView** srv);

	void onBeginFrame(const SHADERSETDX10::TYPE shaderType);
	void onRender(const SHADERSETDX10::TYPE shaderType);
	void onEndFrame();
};

bool operator==(const ShaderMenagerDX10::LIGHT_DESCREPTION&, const ShaderMenagerDX10::LIGHT_DESCREPTION&);

#endif