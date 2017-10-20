#pragma once

#include "Vector.h"

namespace BallPhysics
{

struct ContinuousCollisionCirclePointOccured
{
    float a;
    float b;
    float d;
    bool occured;
};

ContinuousCollisionCirclePointOccured continuousCollisionCirclePoint(
    const Vector& pointOrigin,
    const Vector& circleOrigin,
    float radius,
    const Vector& pointVelocity,
    const Vector& circleVelocity);

ContinuousCollisionCirclePointOccured continuousCollisionCircleCircle(
    const Vector& circle1Origin,
    const Vector& circle2Origin,
    float radius1,
    float radius2,
    const Vector& circle1Velocity,
    const Vector& circle2Velocity);

float continuousCollisionCirclePointTime(const ContinuousCollisionCirclePointOccured& occuredResult);

struct ContinuousCollisionCircleLineOccured
{
    Vector dr;
    float denominator;
    bool occured;
};

ContinuousCollisionCircleLineOccured continuousCollisionCircleLine(
    const Vector& linePoint1Origin,
    const Vector& linePoint2Origin,
    const Vector& circleOrigin,
    float radius,
    const Vector& lineVelocity,
    const Vector& circleVelocity);

struct ContinuousCollisionCircleLineTime
{
    float dt;
    float drSquared;
};

ContinuousCollisionCircleLineTime continuousCollisionCircleLineTime(
    const ContinuousCollisionCircleLineOccured& occured,
    const Vector& circleOrigin,
    const Vector& linePoint1Origin,
    float radius);

Vector continuousCollisionCircleLinePoint(
    const Vector& linePoint1Origin,
    const Vector& linePoint2Origin,
    const Vector& circleOrigin,
    const Vector& lineVelocity,
    const Vector& circleVelocity,
    const ContinuousCollisionCircleLineTime& lineTime,
    Vector& r1,
    Vector& r2);

bool continuousCollisionCircleLinePointOnTrack(
    const Vector& r1,
    const Vector& r2,
    const Vector& collisionPoint);

}