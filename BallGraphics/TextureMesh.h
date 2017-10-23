#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "EffectTexture.h"
#include "ObjFile.h"
#include <tuple>
#include <Position.h>

namespace BallGraphics
{

class TextureMesh : public Mesh
{
private:
    struct ModelType
    {
        ModelType( Position position, float itu, float itv, float inx, float iny, float inz );
        Position position;
        float tu, tv;
        float nx, ny, nz;
    };
    struct VertexType
    {
        VertexType( const ModelType& arg );
        DirectX::XMFLOAT3 position;	//really?
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };
public:

    TextureMesh( const D3D& d3d, EffectTexture& et, Texture& texture ) : Mesh( d3d ), effectTexture_{ et }, texture_{ texture }
    {
    };
    TextureMesh( const TextureMesh& arg ) = delete;
    TextureMesh& operator = ( const TextureMesh& arg ) = delete;
    TextureMesh( TextureMesh&& arg );
    TextureMesh& operator = ( TextureMesh&& arg );

    virtual ~TextureMesh();

    virtual void render( const GetWorldFunc& worldFunc, const Camera* camera ) noexcept override;

    UINT get_index_count();

    bool load( const ObjFile& objFile, float mul = 1.f ) noexcept;
    bool load( const ObjFile& objFile, const std::function<Position( float, float, float )>& ) noexcept;

    float xSize{ 0.f };
    float ySize{ 0.f };
    float zSize{ 0.f };

private:
    std::vector<ModelType> vertices_;
    EffectTexture& effectTexture_;
    Texture& texture_;
    D3D_PRIMITIVE_TOPOLOGY topology_;


    void render_( const EffectTexture::MatrixBufferType& params, const EffectTexture::LightBufferType& lightParams ) noexcept;
    void initializeBuffers_() noexcept;
};

} //namespace