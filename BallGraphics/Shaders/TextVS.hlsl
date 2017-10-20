cbuffer MatrixBuffer{
    matrix world_matrix;
    matrix view_matrix;
    matrix projection_matrix;
};

struct VertexInputType{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, world_matrix);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    return output;
}