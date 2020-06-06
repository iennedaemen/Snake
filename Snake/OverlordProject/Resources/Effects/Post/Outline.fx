
//https://github.com/James-Jones/HLSLCrossCompiler/blob/master/tests/apps/shaders/generic/postProcessing/sobel.hlsl
//https://en.wikipedia.org/wiki/Sobel_operator#Pseudocode_implementation
//http://www.neatware.com/lbstudio/web/hlsl.html

//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepthWriting
{
    DepthEnable = true;
    DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState BackfaceCulling
{
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 texCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 texCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
    output.Position = float4(input.Position, 1.0f);
	// Set the TexCoord
    output.texCoord = input.texCoord;

	return output;
}

float4 PS1(PS_INPUT input) : SV_Target
{
	return float4(gTexture.Sample(samPoint, input.texCoord));
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// SOBEL FILTER
	const float threshold = 0.05;

	// offsets for pixels X around current pixel P
	// |  X  |  X  |  X  |
	// -------------------
	// |  X  |  P  |  X  |
	// -------------------
	// |  X  |  X  |  X  |
	const float2 Offsets[8] = 
	{
		float2(-1,  1),
		float2(0,  1),
		float2(1,  1),
		float2(1,  0),
		float2(-1,  0),
		float2(1, -1),
		float2(0, -1),
		float2(-1, -1),
	};

	float lum[8];
	float3 Luminance = { 0.2125f, 0.7154f, 0.0721f };

	float2 texDimensions;
	gTexture.GetDimensions(texDimensions.x, texDimensions.y);

	for (int i = 0; i < 8; i++) {
		// sample = UV coord, load = texel coord
	  float3 color = gTexture.Load(float3(float2(input.texCoord * float2(texDimensions.x, texDimensions.y)) + Offsets[i], 0));
	  lum[i] = dot(color, Luminance);
	}

	// New Value P by sum of weighted neighbours
	float x = lum[0] + (2 * lum[3]) + lum[5] - lum[2] - (2 * lum[4]) - lum[7];
	float y = lum[0] + (2 * lum[1]) + lum[2] - lum[5] - (2 * lum[6]) - lum[7];

	float edge = sqrt(x * x + y * y);

	// Light colour = edge, dark color = no edge
	// If not an edge -> return sampled color
	if(edge < 0.3f)
	return float4(gTexture.Sample(samPoint, input.texCoord));
	else
	return float4(0, 0, 0, 1);
}


//TECHNIQUE
//---------
technique11 Grayscale
{
    pass P0
    {          
        // Set states...
        SetRasterizerState(BackfaceCulling);
        SetDepthStencilState(EnableDepthWriting, 0);
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

	//pass P1
	//{
	//	// Set states...
	//	SetRasterizerState(BackfaceCulling);
	//	SetDepthStencilState(EnableDepthWriting, 0);
	//	SetVertexShader(CompileShader(vs_4_0, VS()));
	//	SetGeometryShader(NULL);
	//	SetPixelShader(CompileShader(ps_4_0, PS()));
	//}
}

