cbuffer PostProcessBuffer : register(b0)
{
    int mode;
    float params0;
    float params1;
    float params2;
}

Texture2D textureMap0 : register(t0);
Texture2D textureMap1 : register(t1);
Texture2D textureMap2 : register(t2);
Texture2D textureMap3 : register(t3);

SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 finalColor = 0.0f;
    if(mode == 0)
    {
        finalColor = textureMap0.Sample(textureSampler, input.texCoord);
    }
    else if (mode == 1) // monochrome
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        finalColor = (color.r + color.g + color.b) / 3.0f;
    }
    else if (mode == 2) // invert
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        finalColor = 1.0f - color;
    }
    else if (mode == 3) // mirror
    {
        float2 texCoord = input.texCoord;
        texCoord.x *= params0;
        texCoord.y *= params1;
        finalColor = textureMap0.Sample(textureSampler, texCoord);
    }
    else if (mode == 4)// blur
    {
        float u = input.texCoord.x;
        float v = input.texCoord.y;
        finalColor =
            textureMap0.Sample(textureSampler, float2(u, v)) +
            textureMap0.Sample(textureSampler, float2(u + params0, v)) +
            textureMap0.Sample(textureSampler, float2(u - params0, v)) +
            textureMap0.Sample(textureSampler, float2(u, v + params1)) +
            textureMap0.Sample(textureSampler, float2(u, v - params1)) +
            textureMap0.Sample(textureSampler, float2(u + params0, v + params1)) +
            textureMap0.Sample(textureSampler, float2(u - params0, v + params1)) +
            textureMap0.Sample(textureSampler, float2(u - params0, v - params1)) +
            textureMap0.Sample(textureSampler, float2(u + params0, v - params1));
        finalColor *= 0.12;
    }
    else if (mode == 5)// combine 2
    {
        float4 color0 = textureMap0.Sample(textureSampler, input.texCoord);
        float4 color1 = textureMap1.Sample(textureSampler, input.texCoord);
        finalColor = (color0 + color1) * 0.5f;
        //finalColor = (color0 * (1.0f - color1.r)) + (color1 * color1.r);
    }
    else if (mode == 6)// motionBlur
    {
        float u = input.texCoord.x;
        float v = input.texCoord.y;
        float dist = distance(input.texCoord, float2(0.5f, 0.5f));
        float weight = saturate(lerp(0.0f, 1.0f, (dist - 0.25f) / 0.25f));
        finalColor =
            textureMap0.Sample(textureSampler, float2(u, v)) +
            textureMap0.Sample(textureSampler, float2(u + params0 * weight, v)) +
            textureMap0.Sample(textureSampler, float2(u - params0 * weight, v)) +
            textureMap0.Sample(textureSampler, float2(u, v + params1 * weight)) +
            textureMap0.Sample(textureSampler, float2(u, v - params1 * weight)) +
            textureMap0.Sample(textureSampler, float2(u + params0 * weight, v + params1* weight)) +
            textureMap0.Sample(textureSampler, float2(u - params0 * weight, v + params1* weight)) +
            textureMap0.Sample(textureSampler, float2(u - params0 * weight, v - params1* weight)) +
            textureMap0.Sample(textureSampler, float2(u + params0 * weight, v - params1* weight));
        finalColor *= 0.12;
    }
    else if (mode == 7)// ChromaticAberration
    {
        float2 distortion = float2 (params0, -params1);
        float4 redChannel = textureMap0.Sample(textureSampler, input.texCoord + distortion.x * input.texCoord);
        float4 greenChannel = textureMap0.Sample(textureSampler, input.texCoord);
        float4 blueChannel = textureMap0.Sample(textureSampler, input.texCoord + distortion.y * input.texCoord);
        finalColor = float4(redChannel.r, greenChannel.g, blueChannel.b, 1.0f);
    }
    else if (mode == 8) // gradient transparency
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        float gradientAlpha = lerp(1.0f, 0.0f, input.texCoord.y * params2);
        finalColor = float4(color.rgb, color.a * gradientAlpha);
    }
    return finalColor;
}