Texture2D shaderTexture;
SamplerState SampleType;

cbuffer ColorBuffer
{
    float4 textColor;
};

struct PixelInputType{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    //textureColor.a = 0.0f;
    if(textureColor[3] < 0.5)
    {
        discard;
        //textureColor = float4(0.0, 0.9, 0.9, 0.65);
    }
    else
    {
        //textureColor = float4(1.0, 0.0, 0.0, 1.0);
        textureColor = textColor;
    }
    return textureColor;
}