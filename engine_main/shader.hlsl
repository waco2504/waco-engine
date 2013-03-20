/* TEX2DC - liczba textur 2d */
/* TEXCUBEC - liczba textur cube */
/* TEXSAMC - liczba smaplerow */
/* VINNORM - czy vertex shader dostaje normalna */
/* VINTAN - czy vertex shader dostaje tangent */
/* VINTEX1 - czy vertex shader dostaje drugi texcoord */
/* GINTEX0 - czy geometry shader ma dostac texcoord */
/* PINTEXx - 0 <= x <= 2 czy pixel shader dostaje texcoordy */
/* INCGS - jest geometry shader */
/* VSCPYPOD - skopiuj pozycje z wejscia na wyjscie bez transformacji */
/* VSNORM - wylicz normalna w world space */
/* VSSSNORM - wylicz nromalna w screen space */
/* VSOBPOS - wylicz pozycje pixela w world space */
/* VSLWVPPOS - wylicz pozycje pixela w light space */
/* GSOLVEC - wylicz obiekt do swiatla vector */
/* PSCOLDEPTH */
/* PSCOLDIST */
/* PSCOLSTEX0 */
/* PSCOLSTEX1 */
/* PSCOLNORM */
/* PSLIGHTSPOT */
/* PSLIGHTPOINT */
/* PSLIGHTDIR */
/* PSSHADOW */
/* PSBLUR2D */
/* PSBLURCUBE */
/* PSFINALMERGE */
/* PSCOLSSAO */
/* VSTANGENT */
/* PSBUMPMAP */


/* const buffers */
cbuffer WorldConst : register(b0) {
	row_major float4x4 View;
	row_major float4x4 Proj;
	row_major float4x4 CubeView[6];
};

cbuffer ObjectConst : register(b1) {
	float4 CameraPosition;
	float4 OColor;
	row_major float4x4 WorldViewProj;
	row_major float4x4 World;
	row_major float4x4 LWorldViewProj;
};

cbuffer LightConst : register(b2) {
	float4 LPosition;
	float4 LColor;
	float4 LDirection;
	float4 LParameters;
	row_major float4x4 LView;
	row_major float4x4 LProj;
};

cbuffer PostProcessConst : register(b3) {
	float4 TexelSize;
	//float4 ambientFactor;
};

cbuffer SSAOBuffer : register(b4) {
	float totStrength;
	float strength;
	float offset;
	float falloff;
	float rad;
};



/* textures */
#if TEX2DC > 0
	Texture2D MTexture2D[TEX2DC];
#endif
#if TEXCUBEC > 0
	TextureCube MTextureCube[TEXCUBEC];
#endif


#if TEXSAMC > 0
	SamplerState MSSSampler : register(ps, s0);
#endif



/* in/out structures */
struct VIN {	// wejscie dla vertexa
	float3 pos : POSITION0;
#ifdef VINNORM
	float3 norm : NORMAL0;
#endif
#ifdef VINTAN
	float4 tan : TANGENT0;
#endif
	float3 tex0 : TEXCOORD0;
#ifdef VINTEX1
	float3 tex1 : TEXCOORD1;
#endif
};


struct GIN {	// wejscie dla geometry
	float4 pos : POSITION0;
#ifdef GINTEX0
	float3 tex0 : TEXCOORD0;
#endif
};


struct PIN {	// wejscie dla pixela
	float4 pos : SV_Position;
#ifdef PINTEX0
	float3 tex0 : TEXCOORD0;
#endif
#ifdef PINTEX1
	float3 tex1 : TEXCOORD1;
#endif
#ifdef PINTEX2
	float4 tex2 : TEXCOORD2;
#endif
#ifdef PINTEX3
	float3 tex3 : TEXCOORD3;
#endif
#ifdef PINTEX4
	float3 tex4 : TEXCOORD4;
#endif
#ifdef PINTEX5
	float3 tex5 : TEXCOORD5;
#endif
#ifdef PINRTAI
	uint rTIndex : SV_RenderTargetArrayIndex;
#endif
};


struct POUT {
	float4 col : SV_Target0;
};




/* vertex shader */
#ifndef INCGS
PIN VS(VIN vIn) {
	PIN vOut = (PIN)0;
	#if VSCPYPOS != 0
		vOut.pos = float4(vIn.pos,1.0f);
	#else
		vOut.pos = mul(float4(vIn.pos,1.0f), WorldViewProj);
	#endif
#else
GIN VS(VIN vIn) {
	GIN vOut = (GIN)0;
	#if VSCPYPOS != 0
		vOut.pos = float4(vIn.pos,1.0f);
	#else
		vOut.pos = mul(float4(vIn.pos,1.0f), World);
	#endif
#endif

#ifdef PINTEX0
	#if VSNORM != 0
		vOut.tex0 = mul(vIn.norm, (float3x3)World);
	#elif VSSSNORM != 0
		// normalna ma byc w screena space... 
		float4 tn;
		tn.xyz = vIn.norm;
		tn.w = 0.0f;
		tn = mul(tn, World);
		tn = mul(tn, View);
		tn = mul(tn, Proj); // a moze przez maciez WorldViewProj mnozyc :P

		vOut.tex0 = normalize(tn.xyz);
	#else
		vOut.tex0 = vIn.tex0;
	#endif
#endif
#ifdef PINTEX1
	#if VSOBPOS != 0
		float4 t = mul(float4(vIn.pos,1.0f), World);
		vOut.tex1 = t.xyz / t.w;
	#elif VSOLVEC != 0
		float4 t = mul(float4(vIn.pos,1.0f), World);
		float3 pw = t.xyz / t.w;
		vOut.tex1 = pw - LPosition.xyz;
	#elif !INCGS
		vOut.tex1 = vIn.tex1;
	#endif
#endif
#ifdef PINTEX2
	#if VSLWVPPOS != 0
		vOut.tex2 = mul(float4(vIn.pos,1.0f), LWorldViewProj);
	#else
		vOut.tex2 = float4(0.0f,0.0f,0.0f,0.0f);
	#endif
#endif
#ifdef PINTEX3
	vOut.tex3 = vIn.tex0;
#endif
#ifdef VSTBN
	float3x3 TBN;
	float3 T, B, N, P;
	#ifdef VSOBPOS
		P = vOut.tex1;
	#else 
		float4 tP = mul(float4(vIn.pos.xyz,1.0f), World);
		P = tP.xyz / tP.w;
	#endif

	T = normalize(mul(vIn.tan.xyz, (float3x3)World));
	B = normalize(mul(cross(vIn.norm, vIn.tan.xyz) * vIn.tan.w, (float3x3)World));
	N = normalize(mul(vIn.norm, (float3x3)World));
	T = normalize(cross(B, N));
	TBN = float3x3(T,B,N);
#endif
#ifdef PINTEX4
	float3 OL = (LPosition.xyz / LPosition.w) - P;
	vOut.tex4 = normalize(mul(OL, TBN));
#endif
#ifdef PINTEX5
	float3 OC = CameraPosition.xyz - P;
	vOut.tex5 = normalize(mul(OC, TBN));
#endif

	return vOut;
}


/* geometry shader */
#ifdef INCGS
[maxvertexcount(18)]
void GS(triangle GIN triIn[3], inout TriangleStream<PIN> streamIO) {
	for(int i = 0; i < 6; ++i) {
		PIN gOut;
		gOut.rTIndex = i;

		for(int j = 0; j < 3; ++j) {
			gOut.pos = mul(triIn[j].pos, CubeView[i]);
			gOut.pos = mul(gOut.pos, Proj);
		#ifdef PINTEX0
			gOut.tex0 = triIn[j].tex0;
			gOut.tex0.z = i;
		#endif
		#ifdef PINTEX1
			#if GSOLVEC != 0
				gOut.tex1 = triIn[j].pos.xyz - LPosition.xyz;
			#else 
				gOut.tex1 = float3(0.0f,0.0f,0.0f);
			#endif
		#endif
			streamIO.Append(gOut);
		}

		streamIO.RestartStrip();
	}
}
#endif


/* pixel shader */
POUT PS(PIN pIn) {
	POUT pOut = (POUT)0;
	pOut.col = OColor;

#ifdef PSCOLDEPTH
	pOut.col.x = pIn.pos.z;
	pOut.col.y = pIn.pos.z * pIn.pos.z;
#endif
#ifdef PSCOLDIST
	pOut.col.x = length(pIn.tex1);
	pOut.col.y = dot(pIn.tex1,pIn.tex1);
#endif
#ifdef PSCOLSTEX0
	pOut.col = MTexture2D[0].Sample(MSSSampler, pIn.tex0.xy);
#endif
#ifdef PSCOLSTEX1
	pOut.col =	pOut.col * 0.5f + 
				MTexture2D[1].Sample(MSSSampler, pIn.tex1.xy) * 0.5f;
#endif

#ifdef PSCOLNORM
	pOut.col.xyz = pIn.tex0;
	pOut.col.w = pIn.pos.z;
#endif

#if PSLIGHTSPOT != 0 || PSLIGHTPOINT != 0
	float3 LO = (LPosition.xyz / LPosition.w) - pIn.tex1;
	float3 CS = normalize(LO);
	float3 L, N, K;
	float3 CP = CameraPosition.xyz;
	float3 OP = pIn.tex1;
	float diff = 0.0f;
	float spec = 0.0f;

	#ifdef PSBUMPMAP
		float3 SN = 2.0f * MTexture2D[TEX2DC-1].Sample(MSSSampler, pIn.tex3.xy).xyz - float3(1.0f,1.0f,1.0f); 
		K = pIn.tex5;
		N = SN;
		L = pIn.tex4;
	#else
		K = normalize(CP - OP);
		N = pIn.tex0;
		L = normalize(LO);
	#endif

	diff = saturate(dot(L, N));
	spec = saturate(dot(normalize(L + K), N));

	#ifdef PSSPECMAP
		float sfac = 1.0f;
		#if TEX2DC == 1 // tylko spec mapa
			sfac = MTexture2D[0].Sample(MSSSampler, pIn.tex3.xy).x;
		#else
			sfac = MTexture2D[1].Sample(MSSSampler, pIn.tex3.xy).x;
		#endif
		spec *= sfac;
	#endif

	#ifdef PSLIGHTSPOT
		float3 V = normalize(-LO);
		float cosD = dot(V, LDirection.xyz);
		diff *= smoothstep(LParameters.y, LParameters.x, cosD); 
	#endif

	#ifdef PSSHADOW
		float2 distance;
		float2 moments;
		float lit = 0.0f;
		float bias = 1.0f;

		#if PSLIGHTPOINT != 0
			distance.x = length(LO);
			distance.y = dot(LO,LO);
			moments = MTextureCube[0].Sample(MSSSampler, -CS).xy;
			bias = 0.99f;
			if(distance.x * bias <= moments.x) lit = 1.0f;
		#elif PSLIGHTSPOT != 0
			distance.x = pIn.tex2.z / pIn.tex2.w;
			float2 tcs = pIn.tex2.xy / pIn.tex2.w;
			moments = MTexture2D[0].Sample(MSSSampler, tcs).xy;
			bias = 0.999999f;
			if(distance.x * bias <= moments.x) lit = 1.0f;
		#endif

		diff = diff * lit;
	#endif

	if(spec > diff) spec = diff;
	pOut.col.xyz = spec * LColor.xyz + diff * LColor.xyz;
	pOut.col.xyz *= 0.5f;
	pOut.col.w = 1.0f;
#endif


#if PSBLUR2D != 0 || PSBLURCUBE != 0
	float2 tc = pIn.tex0.xy;
	tc.x -= TexelSize.x;
	tc.y -= TexelSize.y;
	pOut.col = float4(0.0f,0.0f,0.0f,0.0f);

	[unroll]
	for(int i = 0; i < 3; ++i) {
		[unroll]
		for(int j = 0; j < 3; ++j) {
			#if PSBLUR2D != 0
			pOut.col += MTexture2D[0].Sample
				(MSSSampler, 
				float2(tc.x+j*TexelSize.x, tc.y+i*TexelSize.y));
			#elif PSBLURCUBE != 0
			pOut.col += MTextureCube[0].Sample
				(MSSSampler, 
				float3(tc.x+j*TexelSize.x, tc.y+i*TexelSize.y, pIn.tex0.z));
			#endif
		}
	}
	pOut.col /= 9.0f;
#endif

#ifdef PSFINALMERGE
	float4 color = MTexture2D[0].Sample(MSSSampler, pIn.tex0.xy);
	float4 lightFactor = MTexture2D[1].Sample(MSSSampler, pIn.tex0.xy);
	float4 SSAOFactor = MTexture2D[2].Sample(MSSSampler, pIn.tex0.xy);
	float ambientFactor = 0.1f;

	pOut.col.xyz = color.xyz * max(max(lightFactor.xyz, ambientFactor), (1.0f-lightFactor.w)) * SSAOFactor.x;
	pOut.col.w = 1.0f;
#endif

#ifdef PSCOLSSAO
	// these are the random vectors inside a unit sphere
	float3 pSphere[16] = { float3(0.53812504, 0.18565957, -0.43192),
		float3(0.13790712, 0.24864247, 0.44301823),
		float3(0.33715037, 0.56794053, -0.005789503),
		float3(-0.6999805, -0.04511441, -0.0019965635),
		float3(0.06896307, -0.15983082, -0.85477847),
		float3(0.056099437, 0.006954967, -0.1843352),
		float3(-0.014653638, 0.14027752, 0.0762037),
		float3(0.010019933, -0.1924225, -0.034443386),
		float3(-0.35775623, -0.5301969, -0.43581226),
		float3(-0.3169221, 0.106360726, 0.015860917),
		float3(0.010350345, -0.58698344, 0.0046293875),
		float3(-0.08972908, -0.49408212, 0.3287904),
		float3(0.7119986, -0.0154690035, -0.09183723),
		float3(-0.053382345, 0.059675813, -0.5411899),
		float3(0.035267662, -0.063188605, 0.54602677),
		float3(-0.47761092, 0.2847911, -0.0271716) };

	
	float3 fres = normalize((MTexture2D[1].Sample(MSSSampler, pIn.tex0.xy).xyz * 2.0f) - float3(1.0,1.0,1.0));	// grab a normal for reflecting the sample rays later on
	float4 currentPixelSample = MTexture2D[0].Sample(MSSSampler, pIn.tex0.xy);
	float currentPixelDepth = currentPixelSample.w;
	float3 ep = float3(pIn.tex0.x, pIn.tex0.y, currentPixelDepth);											// current fragment coords in screen space
	float3 norm = currentPixelSample.xyz;																	// get the normal of current fragment
	float bl = 0.0;
	float radD = rad/currentPixelDepth;																		// adjust for the depth ( not shure if this is good..)
	float3 ray, se, occNorm;
	float occluderDepth, depthDifference, normDiff;
 
	for(int i=0; i < 16; ++i) {
		// get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
		ray = radD*reflect(pSphere[i],fres);
 
		// if the ray is outside the hemisphere then change direction
		se = ep + sign(dot(ray,norm))*ray;
 
		// get the depth of the occluder fragment
		float4 occluderFragment = MTexture2D[0].Sample(MSSSampler, se.xy);
 
		// get the normal of the occluder fragment
		occNorm = occluderFragment.xyz;
 
		// if depthDifference is negative = occluder is behind current fragment
		depthDifference = currentPixelDepth-occluderFragment.w;
 
		// calculate the difference between the normals as a weight
 
		normDiff = (1.0-dot(occNorm,norm));
		// the falloff equation, starts at falloff and is kind of 1/x^2 falling
		bl += step(falloff,depthDifference)*normDiff*(1.0-smoothstep(falloff,strength,depthDifference));
   }
 
   // output the result
   float ao = 1.0-totStrength*bl*(1.0f/16.0f);
   pOut.col = float4(ao,ao,ao,1.0f);
#endif

	return pOut;
}


