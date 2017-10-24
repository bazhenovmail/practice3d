#pragma once
#include <directxmath.h>
#include "Camera.h"

namespace BallGraphics
{

class ProjectionCamera : public Camera
{
public:
    virtual void render() noexcept override;

    void initialize( float fieldOfViewAngleRad, float screenAspect, float screenNear, float screenDepth, float zPos ) noexcept;
    void rotateX( float deg ) noexcept;
    void rotateY( float deg ) noexcept;
    void rotateZ( float deg ) noexcept;
protected:
    DirectX::XMFLOAT3 position_;
    float pitch_{ -10.f };
    float yaw_{ 0.f };
    float roll_{ 0.f };

    float maxPitch_ = 15.f;
    float minPitch_ = -75.f;
    float maxYaw_ = 30.f;
    float minYaw_ = -30.f;
    float maxRoll_ = 30.f;
    float minRoll_ = -30.f;
};

} //namespace