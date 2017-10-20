cbuffer MatrixBuffer{
    matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
};

struct VertexInputType{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType main(VertexInputType input){
    PixelInputType output;
    /*
    matrix iMatrix =
    {1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
        */
	input.position.w = 1.0f;
    output.position = mul(input.position, world_matrix);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);
    output.color = input.color;
    //output.position = input.position;
    return output;
}