#pragma once
#include "Mesh.h"
#include "Vector.h"
#include "EffectColor.h"

namespace BallGraphics
{

class EffectColor;

class ColorMesh: public Mesh
{

private:
    struct ModelType
	{
        ModelType(float ix, float iy, float iz, float ir, float ig, float ib, float ia);
        float x, y, z;
        float r, g, b, a;
    };
    struct VertexType
	{
        VertexType(const ModelType& arg);
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };
public:


    ColorMesh(const D3D& d3d, EffectColor& ec): Mesh(d3d), m_effect_color{ec}{};
    ColorMesh(const ColorMesh& arg) = delete;
    ColorMesh& operator = (const ColorMesh& arg) = delete;
    ColorMesh(ColorMesh&& arg);
    ColorMesh& operator = (ColorMesh&& arg);

    virtual ~ColorMesh();

    virtual void render(const GetWorldFunc& worldFunc, const Camera* camera) noexcept override;

    UINT get_index_count();

    void load(float width, float height, float depth, float ir, float ig, float ib, float ia);
	void load(const Vector& cPoint, const std::vector<Vector>& points, float depth, float ir, float ig, float ib, float ia);

private:
	D3D_PRIMITIVE_TOPOLOGY topology_;
    std::vector<ModelType> vertices_;
    EffectColor& m_effect_color;

    void render_(const EffectColor::MatrixBufferType& params);
	void initialize_buffers_() noexcept;
};

}//namespace BallGraphics