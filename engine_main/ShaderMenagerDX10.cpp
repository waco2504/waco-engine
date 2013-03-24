#include "ShaderMenagerDX10.hpp"
#include "ErrorAssert.hpp"

#include <string>
#include <fstream>

	
const D3D10_SHADER_MACRO M1[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"PSCOLDEPTH", "1", NULL, NULL };
	
const D3D10_SHADER_MACRO M2[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"PINTEX1", "1", "VSOLVEC", "1", "PSCOLDIST", "1", NULL, NULL };
	
const D3D10_SHADER_MACRO M3[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"PINTEX1", "1", "PINRTAI", "1", "INCGS", "1", "GSOLVEC", "1", "PSCOLDIST", "1", 
	NULL, NULL };

const D3D10_SHADER_MACRO M4[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	NULL, NULL };

const D3D10_SHADER_MACRO M5[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"TEX2DC", "1", "TEXSAMC", "1", "PINTEX0", "1", "PSCOLSTEX0", "1", NULL, NULL };

const D3D10_SHADER_MACRO M6[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"PINTEX0", "1", "PINTEX1", "1", "PINTEX2", "1", "VSNORM", "1", "VSOBPOS", "1", 
	"VSLWVPPOS", "1", "PSLIGHTSPOT", "1", NULL, NULL };

const D3D10_SHADER_MACRO M7[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"PINTEX0", "1", "PINTEX1", "1", "VSNORM", "1", "VSOBPOS", "1", "PSLIGHTPOINT", "1", 
	NULL, NULL };

const D3D10_SHADER_MACRO M8[] = { "TEX2DC", "1", "TEXSAMC", "1", "VINNORM", "1", 
	"VINTAN", "1", "VINTEX1", "1", "PINTEX0", "1", "PINTEX1", "1", "PINTEX2", "1", 
	"VSNORM", "1", "VSOBPOS", "1", "VSLWVPPOS", "1", "PSLIGHTSPOT", "1", 
	"PSSHADOW", "1", NULL, NULL };

const D3D10_SHADER_MACRO M9[] = { "TEXCUBEC", "1", "TEXSAMC", "1", "VINNORM", "1", 
	"VINTAN", "1", "VINTEX1", "1", "PINTEX0", "1", "PINTEX1", "1", "VSNORM", "1", 
	"VSOBPOS", "1", "PSLIGHTPOINT", "1", "PSSHADOW", "1", NULL, NULL };

const D3D10_SHADER_MACRO M10[] = { "TEX2DC", "1", "TEXSAMC", "1", "VINNORM", "1", 
	"VINTAN", "1", "VINTEX1", "1", "VSTBN", "1", "PINTEX0", "1", "PINTEX1", "1", 
	"PINTEX3", "1", "PINTEX4", "1", "PINTEX5", "1", "VSNORM", "1", 
	"VSOBPOS", "1", "PSLIGHTPOINT", "1", "PSBUMPMAP", "1", NULL, NULL };

const D3D10_SHADER_MACRO M11[] = { "TEXCUBEC", "1", "TEX2DC", "1", "TEXSAMC", "1", 
	"VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", "VSTBN", "1", "PINTEX0", "1", 
	"PINTEX3", "1", "PINTEX4", "1", "PINTEX5", "1", 
	"PINTEX1", "1", "VSNORM", "1", "VSOBPOS", "1", "PSLIGHTPOINT", "1", 
	"PSSHADOW", "1", "PSBUMPMAP", "1", NULL, NULL };

const D3D10_SHADER_MACRO M12[] = { "TEX2DC", "1", "TEXSAMC", "1", "VINNORM", "1", 
	"VINTAN", "1", "VINTEX1", "1", "PINTEX0", "1", "PINTEX1", "1", "PINTEX3", "1", 
	"PINTEX4", "1", "PINTEX5", "1", "PINTEX2", "1", "VSNORM", "1", 
	"VSOBPOS", "1", "VSTBN", "1", "VSLWVPPOS", "1", "PSLIGHTSPOT", "1", 
	"PSBUMPMAP", "1", NULL, NULL };

const D3D10_SHADER_MACRO M13[] = { "TEX2DC", "2", "TEXSAMC", "1", "VINNORM", "1", 
	"VINTAN", "1", "VINTEX1", "1", "PINTEX0", "1", "PINTEX1", "1", "PINTEX2", "1", 
	"VSNORM", "1", "VSOBPOS", "1", "PINTEX3", "1", "PINTEX4", "1", "PINTEX5", "1", 
	"VSTBN", "1","VSLWVPPOS", "1", "PSLIGHTSPOT", "1", "PSSHADOW", "1", 
	"PSBUMPMAP", "1", NULL, NULL };

const D3D10_SHADER_MACRO M14[] = { "TEX2DC", "1", "TEXSAMC", "1", "PINTEX0", "1", 
	"VSCPYPOS", "1", "PSBLUR2D", "1", NULL, NULL };

const D3D10_SHADER_MACRO M15[] = { "TEXCUBEC", "1", "TEXSAMC", "1", "GINTEX0", "1", 
	"PINTEX0", "1", "PINRTAI", "1", "INCGS", "1", "VSCPYPOS", "1", "PSBLURCUBE", "1", 
	NULL, NULL };

const D3D10_SHADER_MACRO M16[] = { "VINNORM", "1", "VINTAN", "1", "VINTEX1", "1", 
	"PINTEX0", "1", "VSSSNORM", "1", "PSCOLNORM", "1", NULL, NULL };

const D3D10_SHADER_MACRO M17[] = { "TEX2DC", "2", "TEXSAMC", "1", "VSCPYPOS", "1", 
	"PINTEX0", "1", "PSCOLSSAO", "1", NULL, NULL };

const D3D10_SHADER_MACRO M18[] = { "TEX2DC", "3", "TEXSAMC", "1", "VSCPYPOS", "1", 
	"PINTEX0", "1", "PSFINALMERGE", "1", NULL, NULL };


const D3D10_SHADER_MACRO* ShaderMacros[] = { 
	M1, M2, M3, M4, M5, M6, M7, M8, M9, M10, M11, M12, M13, M14, M15, M16, M17, M18
};


std::string ShaderMenagerDX10::readShaderFile(const char* path) {
	std::string out;
	std::ifstream file(path);

	if(file.fail()) {
		return out;
	}

	file.seekg(0, std::ios::end);   
	out.reserve((unsigned int)file.tellg());
	file.seekg(0, std::ios::beg); 

	out.assign((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

	file.close();
	return out;
}

void ShaderMenagerDX10::procCompilerOutput(HRESULT hr, ID3D10Blob* compilerErrors, 
	const char* info) {
	if(FAILED(hr)) {
		std::string text;
		text += "Errors in shader compilation";
		text += "\nCompile errors: ";
		if(compilerErrors != NULL) {
			text += (const char*)compilerErrors->GetBufferPointer();
		}
		text += "\n";
		if(info != NULL) {
			text += info;
		}
		
		MessageBox(NULL, text.c_str(), "Error", MB_OK);
		exit(0);
	}
}

SHADERSETDX10 ShaderMenagerDX10::compileShaderSet(SHADERSETDX10::TYPE shaderType, 
	const char* filePath, const D3D10_SHADER_MACRO* macros) {
	SHADERSETDX10 ret;
	ID3D10Blob* pvertexShaderBlob = NULL;
	ID3D10Blob* pgeometryShaderBlob = NULL;
	ID3D10Blob* ppixelShaderBlob = NULL;
	HRESULT hr = 0;
	ID3D10Blob* compilerErrors = NULL;
	std::string info;
	std::string sdata = readShaderFile(filePath);

	ZeroMemory(&ret, sizeof(SHADERSETDX10));
	ret.Type = shaderType;

	hr = D3D10CompileShader(sdata.c_str(), sdata.length(), NULL, macros, NULL, "VS", "vs_4_0", 
		D3D10_SHADER_OPTIMIZATION_LEVEL1|D3D10_SHADER_WARNINGS_ARE_ERRORS, &pvertexShaderBlob,
		&compilerErrors);

	// nie dzia³a, rzuca naruszeniem pamiêci
	//hr = D3DCompile(sdata.c_str(), sdata.length(), NULL, macros, NULL, NULL, "vs_4_0", 
	//	D3D10_SHADER_OPTIMIZATION_LEVEL1|D3D10_SHADER_WARNINGS_ARE_ERRORS, 0, &pvertexShaderBlob, 
	//	&compilerErrors);

	info.clear();
	info += "D3DX10CompileFromFile vs_4_0 Macro: ";
	info += macros[0].Name;
	procCompilerOutput(hr, compilerErrors, info.c_str());
	compilerErrors = NULL;

	pd3dDevice->CreateVertexShader(pvertexShaderBlob->GetBufferPointer(), 
		pvertexShaderBlob->GetBufferSize(), &ret.VertexShader);
	
	
	if(shaderType == SHADERSETDX10::SHADOWMAPCUBE || shaderType == SHADERSETDX10::BOXBLURCUBE) {
		D3D10CompileShader(sdata.c_str(), sdata.length(), NULL, macros, NULL, "GS", "gs_4_0", 
			D3D10_SHADER_OPTIMIZATION_LEVEL1|D3D10_SHADER_WARNINGS_ARE_ERRORS, &pgeometryShaderBlob,
			&compilerErrors);

		info.clear();
		info += "D3DX10CompileFromFile gs_4_0 Macro: ";
		info += macros[0].Name;
		procCompilerOutput(hr, compilerErrors, info.c_str());
		compilerErrors = NULL;
	
		pd3dDevice->CreateGeometryShader(pgeometryShaderBlob->GetBufferPointer(),
			pgeometryShaderBlob->GetBufferSize(), &ret.GeometryShader);
	}

	D3D10CompileShader(sdata.c_str(), sdata.length(), NULL, macros, NULL, "PS", "ps_4_0", 
		D3D10_SHADER_OPTIMIZATION_LEVEL1|D3D10_SHADER_WARNINGS_ARE_ERRORS, &ppixelShaderBlob,
		&compilerErrors);

	info.clear();
	info += "D3DX10CompileFromFile ps_4_0 Macro: ";
	info += macros[0].Name;
	procCompilerOutput(hr, compilerErrors, info.c_str());
	compilerErrors = NULL;

	pd3dDevice->CreatePixelShader(ppixelShaderBlob->GetBufferPointer(),
		ppixelShaderBlob->GetBufferSize(), &ret.PixelShader);
	

	if(		shaderType != SHADERSETDX10::FINALMERGE 
		&&	shaderType != SHADERSETDX10::SSAO 
		&&	shaderType != SHADERSETDX10::BOXBLUR 
		&&	shaderType != SHADERSETDX10::BOXBLURCUBE) {

		D3D10_INPUT_ELEMENT_DESC desc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
			D3D10_INPUT_PER_VERTEX_DATA, 0, }, 
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
				D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0, }, 
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
				D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0, }, 
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
				D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0, }, 
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
				D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0, }, 
		};

		hr = pd3dDevice->CreateInputLayout(desc, sizeof(desc)/sizeof(D3D10_INPUT_ELEMENT_DESC), 
			pvertexShaderBlob->GetBufferPointer(),pvertexShaderBlob->GetBufferSize(), 
			&ret.InputLayout);
		DXASSERT(S_OK != hr);
	} 
	else {
		D3D10_INPUT_ELEMENT_DESC desc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
				D3D10_INPUT_PER_VERTEX_DATA, 0, },  
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
				D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0, }, 
		};

		hr = pd3dDevice->CreateInputLayout(desc, 
			sizeof(desc)/sizeof(D3D10_INPUT_ELEMENT_DESC), 
			pvertexShaderBlob->GetBufferPointer(),pvertexShaderBlob->GetBufferSize(), 
			&ret.InputLayout);
		DXASSERT(S_OK != hr);
	}

	return ret;
}

void ShaderMenagerDX10::initConstantBuffers() {
	HRESULT hr = S_OK;
	D3D10_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(OBJECT_CONST_BUFFER);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

	hr = pd3dDevice->CreateBuffer(&cbDesc, NULL, &ObjectConst);
    DXASSERT(S_OK != hr);

    cbDesc.ByteWidth = sizeof(WORLD_CONST_BUFFER);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

	hr = pd3dDevice->CreateBuffer(&cbDesc, NULL, &WorldConst);
    DXASSERT(S_OK != hr);

	cbDesc.ByteWidth = sizeof(LIGHT_CONST_BUFFER);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

	hr = pd3dDevice->CreateBuffer(&cbDesc, NULL, &LightConst);
    DXASSERT(S_OK != hr);

	cbDesc.ByteWidth = sizeof(POSTPROCESS_CONST_BUFFER);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

	hr = pd3dDevice->CreateBuffer(&cbDesc, NULL, &PostProcessConst);
    DXASSERT(S_OK != hr);

	cbDesc.ByteWidth = sizeof(SSAO_CONST_BUFFER);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

	hr = pd3dDevice->CreateBuffer(&cbDesc, NULL, &SSAOBuffer);
    DXASSERT(S_OK != hr);

	pd3dDevice->VSSetConstantBuffers(0, 1, &WorldConst);
	pd3dDevice->VSSetConstantBuffers(1, 1, &ObjectConst);
	pd3dDevice->VSSetConstantBuffers(2, 1, &LightConst);
	pd3dDevice->VSSetConstantBuffers(3, 1, &PostProcessConst);
	pd3dDevice->VSSetConstantBuffers(4, 1, &SSAOBuffer);

	pd3dDevice->GSSetConstantBuffers(0, 1, &WorldConst);
	pd3dDevice->GSSetConstantBuffers(1, 1, &ObjectConst);
	pd3dDevice->GSSetConstantBuffers(2, 1, &LightConst);
	pd3dDevice->GSSetConstantBuffers(3, 1, &PostProcessConst);
	pd3dDevice->GSSetConstantBuffers(4, 1, &SSAOBuffer);

	pd3dDevice->PSSetConstantBuffers(0, 1, &WorldConst);
	pd3dDevice->PSSetConstantBuffers(1, 1, &ObjectConst);
	pd3dDevice->PSSetConstantBuffers(2, 1, &LightConst);
	pd3dDevice->PSSetConstantBuffers(3, 1, &PostProcessConst);
	pd3dDevice->PSSetConstantBuffers(4, 1, &SSAOBuffer);
}

void ShaderMenagerDX10::initShaders() {
	unsigned int scount = sizeof(ShaderMacros)/sizeof(D3D10_SHADER_MACRO*);

	for(unsigned int si = 0; si < scount; ++si) {
		shaderSets[(SHADERSETDX10::TYPE)si] = 
			compileShaderSet((SHADERSETDX10::TYPE)si, 
				SHADERFILE,
				ShaderMacros[si]);
	}
}

void ShaderMenagerDX10::updateSamplers() {
	if(activeShader == SHADERSETDX10::SSAO) {
		pd3dDevice->PSSetSamplers(0, 1, &pointSampler);
	} else {
		pd3dDevice->PSSetSamplers(0, 1, &linearSampler);
	}
}

void ShaderMenagerDX10::updateTextures() {
	if(activeTextures.size() > 0)
		pd3dDevice->PSSetShaderResources(0, activeTextures.size(), activeTextures.data());
}

void ShaderMenagerDX10::updateWorldBuffer() {
	void* data = NULL;
	WorldConst->Map(D3D10_MAP_WRITE_DISCARD, 0, &data);
	CopyMemory(data, &worldConst, sizeof(WORLD_CONST_BUFFER));
	WorldConst->Unmap();
}

void ShaderMenagerDX10::updateObjectBuffer() {
	void* data = NULL;
	EMATRIX coord;
	coord.identity();
	coord.scale(0.5f,-0.5f,1.0f);
	coord.translate(0.5f,0.5f,0.0f);

	objectConst.WorldViewProj = objectConst.World;
	objectConst.WorldViewProj = worldConst.View * objectConst.WorldViewProj;
	objectConst.WorldViewProj = worldConst.Proj * objectConst.WorldViewProj;

	objectConst.LWorldViewProj = objectConst.World;
	objectConst.LWorldViewProj = lightConst.LView * objectConst.LWorldViewProj;
	objectConst.LWorldViewProj = lightConst.LProj * objectConst.LWorldViewProj;
	objectConst.LWorldViewProj = coord * objectConst.LWorldViewProj; 
		// z zakresu -1 1 do 0 1

	ObjectConst->Map(D3D10_MAP_WRITE_DISCARD, 0, &data);
	CopyMemory(data, &objectConst, sizeof(OBJECT_CONST_BUFFER));
	ObjectConst->Unmap();
}

void ShaderMenagerDX10::updateLightBuffer() {
	void* data = NULL;

	LightConst->Map(D3D10_MAP_WRITE_DISCARD, 0, &data);
	CopyMemory(data, &lightConst, sizeof(LIGHT_CONST_BUFFER));
	LightConst->Unmap();
}

void ShaderMenagerDX10::updatePostProcessBuffer() {
	void* data = NULL;
	PostProcessConst->Map(D3D10_MAP_WRITE_DISCARD, 0, &data);
	CopyMemory(data, &postProcessConst, sizeof(POSTPROCESS_CONST_BUFFER));
	PostProcessConst->Unmap();
}

void ShaderMenagerDX10::updateSSAOBuffer() {
	void* data = NULL;
	
	sSAOBuffer.totStrength = 1.38f;
	sSAOBuffer.strength = 0.07f;
	sSAOBuffer.offset = 18.0f;
	sSAOBuffer.falloff = 0.000002f;
	sSAOBuffer.rad = 0.006f;
	
	SSAOBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &data);
	CopyMemory(data, &sSAOBuffer, sizeof(SSAO_CONST_BUFFER));
	SSAOBuffer->Unmap();
}

void ShaderMenagerDX10::init() {
	initConstantBuffers();
	initShaders();

	D3D10_SAMPLER_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(D3D10_SAMPLER_DESC));
	sDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
	sDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	sDesc.MinLOD = 0.0f;
	sDesc.MaxLOD = FLT_MAX;
	sDesc.MipLODBias = 0.0f;
	sDesc.MaxAnisotropy = 16;
	sDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	for(unsigned int i = 0; i < 4; ++i)
		sDesc.BorderColor[i] = 0.0f;

	defaultSampler = NULL;
	pd3dDevice->CreateSamplerState(&sDesc, &linearSampler);
	
	sDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
	sDesc.MaxAnisotropy = 1;
	pd3dDevice->CreateSamplerState(&sDesc, &pointSampler);
}

void ShaderMenagerDX10::setCameraPosition(const EVECTOR4& camPos) {
	objectConst.CameraPosition = camPos;
}

void ShaderMenagerDX10::setObjectColor(const EVECTOR4& col) {
	objectConst.Color = col;
}

void ShaderMenagerDX10::setWorldMatrix(const EMATRIX& mat) {
	objectConst.World = mat;
}

void ShaderMenagerDX10::setViewMatrix(const EMATRIX& mat) {
	worldConst.View = mat;
}

void ShaderMenagerDX10::setCubeViewMatrix(UINT idx, const EMATRIX& mat) {
	worldConst.CubeView[idx] = mat;
}

void ShaderMenagerDX10::setProjMatrix(const EMATRIX& mat) {
	worldConst.Proj = mat;
}

void ShaderMenagerDX10::setLightDesc(const LIGHT_DESCREPTION* desc) {
	lightConst.LColor = desc->Color;
	lightConst.LDirection = desc->Direction;
	lightConst.LPosition = desc->Position;
	lightConst.LParameters = desc->Parameters;
	lightConst.LParameters.x = cos(desc->Parameters.x/2.0f);
	lightConst.LParameters.y = cos(desc->Parameters.y/2.0f);
	lightConst.LProj = desc->Proj;
	lightConst.LView = desc->View;
}

void ShaderMenagerDX10::setTextureSRV(unsigned int size, ID3D10ShaderResourceView** srv) {
	D3D10_TEXTURE2D_DESC desc;
	ID3D10Texture2D* res = NULL;

	activeTextures.clear();
	for(unsigned int i = 0; i < size; ++i) {
		activeTextures.push_back(srv[i]);
	}

	if(activeTextures[0] != NULL) {
		activeTextures[0]->GetResource((ID3D10Resource**)&res);
		res->GetDesc(&desc);
		postProcessConst.TexelSize.x = 1.0f / desc.Width;
		postProcessConst.TexelSize.y = 1.0f / desc.Height;
		res->Release();
	}
}

void ShaderMenagerDX10::onBeginFrame(const SHADERSETDX10::TYPE shaderType) {
	if(shaderSets.count(shaderType)) {
		if(activeShader != shaderType) {
			activeShader = shaderType;
			pd3dDevice->IASetInputLayout(shaderSets[shaderType].InputLayout);
			pd3dDevice->VSSetShader(shaderSets[shaderType].VertexShader);
			pd3dDevice->GSSetShader(shaderSets[shaderType].GeometryShader);
			pd3dDevice->PSSetShader(shaderSets[shaderType].PixelShader);
		}
	} else {
		std::string msg("ShaderMenagerDX10: SHADERSETDX10::TYPE not found");
		MessageBox(NULL, msg.c_str(), "Error", MB_OK);
		exit(1);
	}
	
	updateWorldBuffer();
	updatePostProcessBuffer();
	updateSSAOBuffer();
	updateSamplers();
}

void ShaderMenagerDX10::onRender(const SHADERSETDX10::TYPE shaderType) {
	updateLightBuffer();
	updateObjectBuffer();
	updateTextures();
}

void ShaderMenagerDX10::onEndFrame() {
	ID3D10ShaderResourceView* zero = NULL;
	for(unsigned int i = 0; i < activeTextures.size(); ++i) {
		pd3dDevice->PSSetShaderResources(i,1,&zero);
	}
	activeTextures.clear();
}

bool operator==(const ShaderMenagerDX10::LIGHT_DESCREPTION& l, const ShaderMenagerDX10::LIGHT_DESCREPTION& r) {
	return l.Color == r.Color && l.Direction == r.Direction && l.Parameters == r.Parameters && l.Position == r.Position && l.Proj == r.Proj && l.View == r.View;
}





