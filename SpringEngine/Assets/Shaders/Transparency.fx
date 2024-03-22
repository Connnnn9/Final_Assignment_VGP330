// For final project that is showcasing transparency

cbuffer TransparencyBuffer : register(b0)
{
    float transparencyValue;
    float3 padding;
};

cbuffer MatrixBuffer : register(b1)
{
    matrix worldViewProj;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

struct VS_INPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};


//struct PS_INTPUT
//{
//    float4 position : POSITION; 
//    float2 texCoord : TEXCOORD0; 
//};
//
//struct PS_OUTPUT
//{
//    float4 color : TARGET;
//};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_INPUT output;
    output.position = mul(input.position, worldViewProj);
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(VS_INPUT input) : SV_Target
{
    VS_OUTPUT output;
    float4 color = ObjTexture.Sample(ObjSampler, input.texCoord);
    color.a  *= transparencyValue;
    //output.color = ObjTexture.Sample(ObjSampler, input.texCoord);
    //output.color.a *= transparencyValue;
    return color;
}

