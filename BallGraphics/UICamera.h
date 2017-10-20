#pragma once
#include <directxmath.h>
#include "Camera.h"

namespace BallGraphics
{

class UICamera: public Camera
{
public:
	void initialize(float screenWidth, float screenHeight, float screenNear, float screenDepth) noexcept;

	virtual void render() noexcept override;
};

}//namespace BallGraphics