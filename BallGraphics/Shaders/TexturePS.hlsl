Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

    textureColor = shaderTexture.Sample(SampleType, input.tex);
    if (textureColor.a == 0.0)
        discard;

	float4 color = ambientColor;
	float3 lightDir = -lightDirection;
	float lightIntensity = saturate(dot(input.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
	}
	color = saturate(color);
	color *= textureColor;
	color = saturate(color);

    return color;
}