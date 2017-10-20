#include "UICamera.h"

using namespace DirectX;

namespace BallGraphics
{

void UICamera::initialize(float screenWidth, float screenHeight, float screenNear, float screenDepth) noexcept
{
	projectionMatrix_ = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, screenNear, screenDepth);
	viewMatrix_ = DirectX::XMMatrixIdentity();
}

void UICamera::render() noexcept
{

}

}//namespace BallGraphics