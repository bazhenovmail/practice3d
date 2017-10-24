#pragma once
#include <UI.h>
#include <Input.h>
#include <World.h>
#include <array>
#include <Position.h>

namespace BallGraphics
{
class Camera;
}

class Racket
{
public:
    void initialize(
        UI&,
        BallGraphics::Camera& camera,
        BallInput::Input&,
        BallPhysics::World&,
        Vector gameSize ) noexcept;
    void enter() noexcept;
    void leave() noexcept;
    void increaseWidth() noexcept;
    void decreaseWidth() noexcept;
    BallPhysics::Id getGroup() const noexcept;
    BallPhysics::Id getBoundaryGroup() const noexcept;
    void stopMotion() noexcept;
    void processMouse( LONG x, LONG y ) noexcept;
private:
    UI* ui_{ nullptr };
    BallGraphics::Camera* camera_{ nullptr };
    BallInput::Input* input_{ nullptr };
    BallPhysics::World* world_{ nullptr };
    Vector gameSize_;

    std::vector<BallPhysics::Id> racketIds_;
    BallPhysics::Id boundaryId_;
    BallGraphics::Id meshInstId_;
    BallPhysics::Id racketGroup_;
    BallPhysics::Id boundaryGroup_;
    BallGraphics::Texture texture_;

    static constexpr size_t levelNumber_ = 0x10;
    std::array<BallGraphics::Id, levelNumber_> meshes_;
    static constexpr size_t defaultLevel_ = 5;
    size_t currentLevel_ = defaultLevel_;

    struct RacketLine_
    {
        std::vector<Vector> line;
        float halfWidth{ 0.f };
    };
    std::array<RacketLine_, levelNumber_> lines_;
    static constexpr float racketHeight_ = 40.f;
    static constexpr float racketWidthMul_ = 1.2f;
    static constexpr float maxEllipticalWidth_ = 320.f;

    void initLevel_( size_t level, const std::function<Vector( const Vector& )>&, float zMultiplier,
                     const std::vector<Vector>&, const BallGraphics::ObjFile& objFile ) noexcept;
    void affect_( BallPhysics::Body::VelocityChanger ) noexcept;
    void addMeshInst_( BallPhysics::Id physId ) noexcept;
    void removeMeshInst_() noexcept;
};