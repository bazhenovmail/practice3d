#pragma once
#include <directxmath.h>

namespace BallGraphics
{

class Camera
{
public:
	virtual void render() noexcept = 0;
	virtual ~Camera() {}

	const DirectX::XMMATRIX& getViewMatrix() const noexcept;
	const DirectX::XMMATRIX& getProjectionMatrix() const noexcept;
protected:
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX projectionMatrix_;
};

}//namespace BallGraphics