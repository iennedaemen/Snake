//https://roystan.net/articles/grass-shader.html
//http://nickvanheer.com/project/NickVanheer_GrassGeometryShader.pdf

float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

float m_WindVelocity;
float m_Time : TIME;

SamplerComparisonState cmpSampler
{
    // sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;

    // sampler comparison state
    ComparisonFunc = LESS_EQUAL;
};


SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
    FillMode = SOLID;
    CullMode = FRONT;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float4 lPos : TEXCOORD1;
};

struct GS_DATA
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
    CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_INPUT VS(VS_INPUT input)
{
    VS_INPUT output = (VS_INPUT)0;

    //TODO: complete Vertex Shader
    //Hint: Don't forget to project our position to light clip space and store it in lPos
    output.pos = input.pos;
    output.normal = input.normal;
    output.texCoord = input.texCoord;
    output.lPos = mul(float4(input.pos, 1.0f), gWorldViewProj_Light);

    return output;
}

float2 texOffset(int u, int v)
{
    //TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
    return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

float EvaluateShadowMap(float4 lpos)
{
    //TODO: complete

        // homogenous coordinates
    lpos.xyz /= lpos.w;

    // compare: clip space coordinates ranges from [-1, 1]  for the x and y coords 
    //											and [0,1] for the z
    if (lpos.x < -1.0f || lpos.x > 1.0f ||
        lpos.y < -1.0f || lpos.y > 1.0f ||
        lpos.z < 0.0f || lpos.z > 1.0f)
        return 0.5f;

    lpos.x = lpos.x / 2 + 0.5;
    lpos.y = lpos.y / -2 + 0.5;
    lpos.z -= gShadowMapBias;

    //PCF sampling for shadow map
    float sum = 0;
    float x, y;

    //perform PCF filtering on a 4 x 4 texel neighborhood
    for (y = -1.5; y <= 1.5; y += 1.0)
    {
        for (x = -1.5; x <= 1.5; x += 1.0)
        {
            sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
        }
    }

    float shadowFactor = sum / 16.0f;

    if (shadowFactor + 0.5f > 1) return 1;
    return shadowFactor + 0.5f;
}

void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal, float2 texCoord, float4 lPos)
{
    //Step 1. Create a GS_DATA object
    GS_DATA gsData;

    //Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4)
    gsData.pos = mul(float4(pos, 1), gWorldViewProj);

    //Step 3. Transform the normal using the World Matrix and assign it to (GS_DATA object).Normal (Only Rotation, No translation!)
    gsData.normal = mul(normal, (float3x3) gWorld);

    //Step 4. Assign texCoord to (GS_DATA object).TexCoord
    gsData.texCoord = texCoord;

    gsData.lPos = lPos;

    //Step 5. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
    triStream.Append(gsData);
}

float random(float2 p)
{
    float2 K1 = float2(
        23.14069263277926, // e^pi (Gelfond's constant)
        2.665144142690225 // 2^sqrt(2) (Gelfond–Schneider constant)
        );
    return frac(cos(dot(p, K1)) * 12345.6789);
}

float3 rotatePosition(float3 axis, float degrees, float3 p, float3 basePoint)
{
    float3 n = axis; // the axis to rotate about
    float angle = degrees * 3.14 / 180.0;
    // Specify the rotation transformation matrix:
    float3 orPos = p;
    p.x -= basePoint.x;
    p.z -= basePoint.z;

    float3x3 m = float3x3(
        n.x * n.x * (1.0f - cos(angle)) + cos(angle),       // column 1 of row 1
        n.x * n.y * (1.0f - cos(angle)) + n.z * sin(angle), // column 2 of row 1
        n.x * n.z * (1.0f - cos(angle)) - n.y * sin(angle), // column 3 of row 1

        n.y * n.x * (1.0f - cos(angle)) - n.z * sin(angle), // column 1 of row 2
        n.y * n.y * (1.0f - cos(angle)) + cos(angle),       // ...
        n.y * n.z * (1.0f - cos(angle)) + n.x * sin(angle), // ...

        n.z * n.x * (1.0f - cos(angle)) + n.y * sin(angle), // column 1 of row 3
        n.z * n.y * (1.0f - cos(angle)) - n.x * sin(angle), // ...
        n.z * n.z * (1.0f - cos(angle)) + cos(angle)        // ...
        );

    // Apply the rotation to our 3D position:
    float3 q = mul(m, p);
    q.x += basePoint.x;
    q.z += basePoint.z;
    return q;
}

[maxvertexcount(12)]
void GrassGenerator(triangle VS_INPUT vertices[3], inout TriangleStream<GS_DATA> triStream)
{
    //Use these variable names
    float3 basePoint, spikeNormal, top, left, right, midLeft, midRight, midLeftTop, midRightTop, midLeftBottom, midRightBottom;
    //Step 1. Calculate The basePoint
    basePoint = (vertices[0].pos + vertices[1].pos + vertices[2].pos) / 3;

    //Step 2. Calculate The normal of the basePoint
    spikeNormal = (vertices[0].normal + vertices[1].normal + vertices[2].normal) / 3;

    //Step 3. Calculate The Spike's Top Position
    float height = random(basePoint.x + basePoint.z) * 20;
    if (height < 10) height = 10;
    top = basePoint + (height * spikeNormal);

    float width = 8;

    float3 spikeDirection = float3(width * 0.5f, 0, 0);
    left = basePoint - spikeDirection;
    right = basePoint + spikeDirection;

    midLeft = float3((left.x + top.x) / 2, (left.y + top.y) / 2, (left.z + top.z) / 2);
    midRight = float3((right.x + top.x) / 2, (right.y + top.y) / 2, (right.z + top.z) / 2);

    midLeftTop = float3((midLeft.x + top.x) / 2, (midLeft.y + top.y) / 2, (midLeft.z + top.z) / 2);
    midRightTop = float3((midRight.x + top.x) / 2, (midRight.y + top.y) / 2, (midRight.z + top.z) / 2);

    midLeftBottom = float3((left.x + midLeft.x) / 2, (left.y + midLeft.y) / 2, (left.z + midLeft.z) / 2);
    midRightBottom = float3((right.x + midRight.x) / 2, (right.y + midRight.y) / 2, (right.z + midRight.z) / 2);

    spikeNormal = cross((top - left), (top - right));

    // float2 uv = basePoint.xz * m_WindDistortionMap.xy + m_WindDistortionMap.zw + m_WindFrequency * time;
     //float3 wind = normalize(float3(windSample.x, windSample.y, 0));

     // Rotate
    top = rotatePosition(float3(1, 0, 0), 35, top, basePoint);
    midLeftTop = rotatePosition(float3(1, 0, 0), 25, midLeftTop, basePoint);
    midRightTop = rotatePosition(float3(1, 0, 0), 25, midRightTop, basePoint);
    midLeft = rotatePosition(float3(1, 0, 0), 15, midLeft, basePoint);
    midRight = rotatePosition(float3(1, 0, 0), 15, midRight, basePoint);
    midLeftBottom = rotatePosition(float3(1, 0, 0), 10, midLeftBottom, basePoint);
    midRightBottom = rotatePosition(float3(1, 0, 0), 10, midRightBottom, basePoint);

    float i = random(basePoint.x + basePoint.z) * 453.145;

    top = rotatePosition(float3(0, 1, 0), i, top, basePoint);
    midLeftTop = rotatePosition(float3(0, 1, 0), i, midLeftTop, basePoint);
    midRightTop = rotatePosition(float3(0, 1, 0), i, midRightTop, basePoint);
    midLeft = rotatePosition(float3(0, 1, 0), i, midLeft, basePoint);
    midRight = rotatePosition(float3(0, 1, 0), i, midRight, basePoint);
    midLeftBottom = rotatePosition(float3(0, 1, 0), i, midLeftBottom, basePoint);
    midRightBottom = rotatePosition(float3(0, 1, 0), i, midRightBottom, basePoint);
    left = rotatePosition(float3(0, 1, 0), i, left, basePoint);
    right = rotatePosition(float3(0, 1, 0), i, right, basePoint);

    float3 grassDirection = float3(1, 0, -0.7f);

    float weight = random(basePoint.x + basePoint.z * 0.5f);
    if (weight < 0.1f) weight = 0.1f;

    float windRand = random(basePoint.x + basePoint.z * 125.454f);
    float wave = sin(m_Time - windRand);

    top += grassDirection * ((m_WindVelocity * weight * 0.5) * (wave)) + grassDirection * 0.5f;
    midLeftTop += grassDirection * ((m_WindVelocity * weight * 0.4) * (wave)) + grassDirection * 0.4f;
    midRightTop += grassDirection * ((m_WindVelocity * weight * 0.4) * (wave)) + grassDirection * 0.4f;
    midLeft += grassDirection * ((m_WindVelocity * weight * 0.2) * (wave)) + grassDirection * 0.2f;
    midRight += grassDirection * ((m_WindVelocity * weight * 0.2) * (wave)) + grassDirection * 0.2f;
    midLeftBottom += grassDirection * ((m_WindVelocity * weight * 0.1) * (wave)) + grassDirection * 0.1f;
    midRightBottom += grassDirection * ((m_WindVelocity * weight * 0.1) * (wave)) + grassDirection * 0.1f;


    // Create Existing Geometry
    CreateVertex(triStream, vertices[0].pos, vertices[0].normal, vertices[0].texCoord, vertices[0].lPos);
    CreateVertex(triStream, vertices[1].pos, vertices[1].normal, vertices[1].texCoord, vertices[0].lPos);
    CreateVertex(triStream, vertices[2].pos, vertices[2].normal, vertices[2].texCoord, vertices[0].lPos);

    //Restart the strip so we can add another (independent) triangle!
    triStream.RestartStrip();

    //Create Spike Geometry

    //normalize(cross(B - A, C - A))
    spikeNormal = normalize(cross(midLeftTop - top, midRightTop - top));
    CreateVertex(triStream, top, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRightTop - midLeftTop, top - midLeftTop));
    CreateVertex(triStream, midLeftTop, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(top - midRightTop, midLeftTop - midRightTop));
    CreateVertex(triStream, midRightTop, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRightTop - midLeft, midLeftTop - midLeft));
    CreateVertex(triStream, midLeft, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRightTop - midRight, midLeft - midRight));
    CreateVertex(triStream, midRight, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRight - midLeftBottom, midLeft - midLeftBottom));
    CreateVertex(triStream, midLeftBottom, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRight - midRightBottom, midLeftBottom - midRightBottom));
    CreateVertex(triStream, midRightBottom, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRightBottom - left, midLeftBottom - left));
    CreateVertex(triStream, left, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

    spikeNormal = normalize(cross(midRightBottom - right, left - right));
    CreateVertex(triStream, right, spikeNormal, float2(0, 0), float4(0, 0, 0, 0));

}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(GS_DATA input) : SV_TARGET
{
    float shadowValue = EvaluateShadowMap(input.lPos);

    float4 diffuseColor = gDiffuseMap.Sample(samLinear,input.texCoord);
    float3 color_rgb = diffuseColor.rgb;
    float color_a = diffuseColor.a;

    //HalfLambert Diffuse :)
    float diffuseStrength = dot(input.normal, -gLightDirection);
    diffuseStrength = diffuseStrength * 0.5 + 0.5;
    diffuseStrength = saturate(diffuseStrength);
    color_rgb = color_rgb * diffuseStrength;

    return float4(color_rgb * shadowValue , color_a);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
        SetRasterizerState(NoCulling);
        SetDepthStencilState(EnableDepth, 0);

        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(CompileShader(gs_4_0, GrassGenerator()));
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
