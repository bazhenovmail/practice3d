#include "Camera.h"

using namespace DirectX;

namespace BallGraphics
{

const DirectX::XMMATRIX & Camera::getViewMatrix() const noexcept
{
	return viewMatrix_;
}

const DirectX::XMMATRIX & Camera::getProjectionMatrix() const noexcept
{
	return projectionMatrix_;
}

}//namespace BallGraphics