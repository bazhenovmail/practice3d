#include "ProjectionCamera.h"

using namespace DirectX;

namespace BallGraphics
{

void ProjectionCamera::initialize(float fieldOfViewAngleRad, float screenAspect, float screenNear, float screenDepth, float zPos) noexcept
{
	projectionMatrix_ = DirectX::XMMatrixPerspectiveFovLH(fieldOfViewAngleRad, screenAspect, screenNear, screenDepth);
	position_ = { 0.f, 0.f, zPos };
}

void ProjectionCamera::render() noexcept
{
	// Setup the vector that points upwards.
	XMFLOAT3 up{ 0.f, 1.f, 0.f };
	// Load it into a XMVECTOR structure.
	XMVECTOR upVector = XMLoadFloat3(&up);

	// Load it into a XMVECTOR structure.
	XMVECTOR positionVector = XMLoadFloat3(&position_);

	/*
	// Setup where the camera is looking by default.
	XMFLOAT3 lookAt{ 0.f, 0.f, 1.f };
	// Load it into a XMVECTOR structure.
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);
	*/
	XMVECTOR lookAtVector{ 0.f, 0.f, 0.f };

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	const float PIDIV180 = DirectX::XM_PI / 180.f;
	FXMVECTOR angles{ pitch_ * PIDIV180, yaw_ * PIDIV180, roll_ * PIDIV180 };
	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(angles);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	positionVector = XMVector3TransformCoord(positionVector, rotationMatrix);
	//lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	//lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	viewMatrix_ = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void ProjectionCamera::rotateX(float deg) noexcept
{
	pitch_ += deg;
	if (pitch_ > maxPitch_)
		pitch_ = maxPitch_;
	if (pitch_ < minPitch_)
		pitch_ = minPitch_;
}

void ProjectionCamera::rotateY(float deg) noexcept
{
	yaw_ += deg;
	/*
	if (yaw_ > maxYaw_)
		yaw_ = maxYaw_;
	if (yaw_ < minYaw_)
		yaw_ = minYaw_;
		*/
}

void ProjectionCamera::rotateZ(float deg) noexcept
{
	roll_ += deg;
	/*
	if (roll_ > maxRoll_)
		roll_ = maxRoll_;
	if (roll_ < minRoll_)
		roll_ = minRoll_;
		*/
}

}//namespace BallGraphics