#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Font.h"
#include "EffectText.h"

namespace BallGraphics
{

class EffectText;

class TextMesh: public Mesh
{

private:
    struct ModelType
    {
        ModelType(float ix, float iy, float iz, float itu, float itv);
        float x, y, z;
        float tu, tv;
    };
    struct VertexType
    {
        VertexType(const ModelType& arg);
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };
public:

    TextMesh(const D3D& d3d, EffectText& et, Texture& texture, const Font& font): Mesh(d3d), effectText_{et}, texture_{texture}, font_{font} {}
    TextMesh(const TextMesh& arg) = delete;
    TextMesh& operator = (const TextMesh& arg) = delete;
    TextMesh(TextMesh&& arg);
    TextMesh& operator = (TextMesh&& arg);

    virtual ~TextMesh();

    virtual void render(const GetWorldFunc& worldFunc, const Camera* camera) noexcept override;

    UINT get_index_count();

    void load(const std::string& text, const DirectX::XMVECTOR& color, float depth) noexcept;
	void updateText(const std::string& text) noexcept;

private:
    std::vector<ModelType> vertices_;
    EffectText& effectText_;
    Texture& texture_;
    const Font& font_;
    DirectX::XMVECTOR color_;
	float depth_{ 0.0f };
	bool initialized_{ false };

    void render_(const EffectText::MatrixBufferType& params);
	void initializeBuffers_() noexcept;
};

}//namespace BallGraphics