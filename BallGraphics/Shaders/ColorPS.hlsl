struct PixelInputType{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PixelInputType input): SV_TARGET
{
    //return float4(0.9f, 0.1f, 1.0f, 0.1f);

    if(input.color.a == 0.0)
        discard;

    return input.color;
}