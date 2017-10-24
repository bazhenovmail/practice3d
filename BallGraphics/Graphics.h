#pragma once

#include "D3D.h"
#include "EffectColor.h"
#include "EffectTexture.h"
#include "EffectText.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include "Texture.h"
#include "ColorMesh.h"
#include "TextureMesh.h"
#include "TextMesh.h"
#include "Font.h"
#include "Camera.h"

namespace BallGraphics
{

using Id = unsigned;

class Graphics
{
public:

    struct MeshInstance
    {
        Id meshId{ Id() };
        const Camera* camera{ nullptr };
        std::function<DirectX::XMMATRIX()> worldFunc{ nullptr };
        Id layer{ Id() };
    };

    Graphics();
    void initialize( int screenWidth,
                     int screenHeight,
                     bool vsync,
                     HWND hwnd,
                     bool full_screen );
    void frame();
    void shutdown();

    Texture createTexture( const std::string& fileName ) noexcept;

    Id addEffect( Effect* effect ) noexcept;
    Id addMesh( std::unique_ptr<Mesh>&& mesh ) noexcept;
    const std::unique_ptr<Mesh>& adjustMesh( Id ) noexcept;
    Id addMeshInstance( MeshInstance&& instance ) noexcept;
    Id addCamera( std::shared_ptr<Camera> camera ) noexcept;

    void removeMesh( Id id ) noexcept;
    void removeMeshInstance( Id id ) noexcept;

    int getScreenWidth() const noexcept;
    int getScreenHeight() const noexcept;

private:
    int screenWidth_{ 0 };
    int screenHeight_{ 0 };
    D3D d3d_;

    Id nextEffectId_{ Id() };
    std::unordered_map<Id, Effect*> effects_;

    Id nextMeshId_{ Id() };
    struct Mesh_
    {
        std::unique_ptr<Mesh> mesh;
        unsigned usages{ 0u };
    };
    std::unordered_map<Id, Mesh_> meshes_;

    Id nextMeshInstanceId_{ Id() };
    std::unordered_map<Id, MeshInstance> meshInstances_;
    std::map<Id, std::unordered_set<Id>> layers_;	//layer vs id vs inst

    Id nextCameraId_{ Id() };
    std::unordered_map<Id, std::shared_ptr<Camera>> cameras_;
};

} //namespace