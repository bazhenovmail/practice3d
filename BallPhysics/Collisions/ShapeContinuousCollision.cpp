#include "ShapeContinuousCollision.h"
#include <vector>
#include <unordered_map>
#include "ContinuousCollisions.h"
#include <algorithm>
#include <array>
#include <algorithm>
#include "Vector.h"
#include "../Rectangle.h"
#include "../Circle.h"
#include "../Line.h"
#include <assert.h>
#include "../Intersections/RectangleIntersection.h"
#include "CircleMovementRectangle.h"
#include "../Intersections/BoundingVolumes.h"

namespace BallPhysics
{

struct ShapeCollisionResult
{
    Vector collisionPoint;
    float dt;
    bool occured = false;
};

ShapeCollisionResult continuousCollisionCircleConvexPolygon(
    const Vector& circleOrigin,
    const Vector& circleVelocity,
    float radius,
    float maxT,
    const Vector* polygonVertices,
    unsigned vertexNumber,
    bool closed,
    const Vector& polygonVelocity )
{
    struct PointData
    {
        ContinuousCollisionCirclePointOccured occured;
        float dt;
    };

    struct LineData
    {
        ContinuousCollisionCircleLineOccured occured;
        ContinuousCollisionCircleLineTime dt;
    };

    assert( vertexNumber > 1 );
    auto lineNumber = vertexNumber - 1 * ( !closed );

    std::unordered_map<unsigned, PointData> occuredPoint( vertexNumber );
    std::unordered_map<unsigned, LineData> occuredLine( lineNumber );
    for ( unsigned i = 0; i < vertexNumber; i++ )
    {
        ContinuousCollisionCirclePointOccured pointOccured =
            continuousCollisionCirclePoint(
                polygonVertices[i],
                circleOrigin,
                radius,
                polygonVelocity,
                circleVelocity );
        if ( pointOccured.occured )
            occuredPoint[i].occured = pointOccured;
    }
    for ( unsigned i = 0; i < lineNumber; i++ )
    {
        ContinuousCollisionCircleLineOccured lineOccured =
            continuousCollisionCircleLine(
                polygonVertices[i],
                polygonVertices[( i + 1 ) % vertexNumber],
                circleOrigin,
                radius,
                polygonVelocity,
                circleVelocity );
        if ( lineOccured.occured )
            occuredLine[i].occured = lineOccured;
    }

    struct TimeInfo
    {
        TimeInfo( unsigned ind, bool isL ) : index{ ind }, isLine{ isL }
        {
        }

        unsigned index;
        bool isLine;
    };

    std::vector<TimeInfo> timeInfo;
    timeInfo.reserve( occuredPoint.size() + occuredLine.size() );
    unsigned timeInfoIndex{ 0 };
    for ( auto& it : occuredPoint )
    {
        it.second.dt = continuousCollisionCirclePointTime( it.second.occured );
        if ( it.second.dt >= 0. )
        {
            timeInfo.emplace_back( it.first, false );
            timeInfoIndex++;
        }
    }
    for ( auto& it : occuredLine )
    {
        it.second.dt = continuousCollisionCircleLineTime( it.second.occured, circleOrigin, polygonVertices[it.first], radius );
        if ( it.second.dt.dt >= 0. )
        {
            timeInfo.emplace_back( it.first, true );
            timeInfoIndex++;
        }
    }

    std::sort( timeInfo.begin(), timeInfo.end(), [&occuredPoint, &occuredLine]( const TimeInfo& arg1, const TimeInfo& arg2 )
    {
        float dt1 = arg1.isLine ? occuredLine[arg1.index].dt.dt : occuredPoint[arg1.index].dt;
        float dt2 = arg2.isLine ? occuredLine[arg2.index].dt.dt : occuredPoint[arg2.index].dt;
        return dt1 < dt2;
    } );

    ShapeCollisionResult result;
    for ( auto it = timeInfo.begin(); it != timeInfo.end() && !result.occured; it++ )
    {
        if ( it->isLine )
        {
            Vector r1, r2;
            result.dt = occuredLine[it->index].dt.dt;
            if ( result.dt > maxT )
                break;
            result.collisionPoint =
                continuousCollisionCircleLinePoint(
                    polygonVertices[it->index],
                    polygonVertices[( it->index + 1 ) % vertexNumber],
                    circleOrigin,
                    polygonVelocity,
                    circleVelocity,
                    occuredLine[it->index].dt,
                    r1,
                    r2 );
            result.occured = continuousCollisionCircleLinePointOnTrack( r1, r2, result.collisionPoint );
        }
        else
        {
            result.dt = occuredPoint[it->index].dt;
            if ( result.dt > maxT )
                break;
            result.occured = true;
            result.collisionPoint = polygonVertices[it->index];
        }
    }
    assert( !result.occured || result.dt >= 0. );
    return result;
}

CollisionResult collision( const Circle& circle, const Rectangle& rect, const ShapelessObject& circlePos, const ShapelessObject& rectPos, float maxT )
{
    auto circleBoundary = boundingVolume( circle, circlePos, maxT );
    auto rectBoundary = boundingVolume( rect, rectPos, maxT );

    bool collisionPossible = rectangleIntersection( circleBoundary.first, circleBoundary.second, rectBoundary.first, rectBoundary.second );
    if ( !collisionPossible )
    {
        return CollisionResult{};
    }

    std::array<Vector, 4u> vertices{
        Vector( rectPos.position.x - rect.width / 2, rectPos.position.y + rect.height / 2 ),
        Vector( rectPos.position.x + rect.width / 2, rectPos.position.y + rect.height / 2 ),
        Vector( rectPos.position.x + rect.width / 2, rectPos.position.y - rect.height / 2 ),
        Vector( rectPos.position.x - rect.width / 2, rectPos.position.y - rect.height / 2 ) };
    if ( rect.hollow )
        std::reverse( vertices.begin(), vertices.end() );
    ShapeCollisionResult almostResult =
        continuousCollisionCircleConvexPolygon(
            circlePos.position,
            circlePos.velocity,
            circle.radius,
            maxT,
            vertices.data(),
            4,
            true,
            rectPos.velocity );
    CollisionResult result;
    result.dt = almostResult.dt;
    result.occured = almostResult.occured;
    if ( almostResult.occured )
    {
        Vector relativeVelocity = circlePos.velocity - rectPos.velocity;
        result.normal = ( almostResult.collisionPoint - ( circlePos.position + circlePos.velocity * almostResult.dt ) ) * ( 1 / circle.radius );
    }

    assert( !result.occured || result.dt >= 0. );
    return result;
}

CollisionResult collision( const Circle& circle, const Line& line, const ShapelessObject& circlePos, const ShapelessObject& linePos, float maxT )
{
    std::vector<Vector> vertices( line.vertices_.size() );
    for ( unsigned i = 0; i < line.vertices_.size(); i++ )
    {
        vertices[i] = line.vertices_[i] + linePos.position;
    }
    ShapeCollisionResult almostResult =
        continuousCollisionCircleConvexPolygon(
            circlePos.position,
            circlePos.velocity,
            circle.radius,
            maxT,
            vertices.data(),
            line.vertices_.size(),
            false,
            linePos.velocity );
    CollisionResult result;
    result.dt = almostResult.dt;
    result.occured = almostResult.occured;
    if ( almostResult.occured )
    {
        Vector relativeVelocity = circlePos.velocity - linePos.velocity;
        result.normal = ( almostResult.collisionPoint - ( circlePos.position + circlePos.velocity * almostResult.dt ) ) * ( 1 / circle.radius );
    }

    assert( !result.occured || result.dt > 0. );
    return result;
}

CollisionResult collision( const Circle & circle1, const Circle & circle2, const ShapelessObject & circle1Pos, const ShapelessObject & circle2Pos, float maxT )
{
    ContinuousCollisionCirclePointOccured pointOccured =
        continuousCollisionCircleCircle(
            circle1Pos.position,
            circle2Pos.position,
            circle1.radius,
            circle2.radius,
            circle1Pos.velocity,
            circle2Pos.velocity );

    CollisionResult result;

    if ( pointOccured.occured )
    {
        result.dt = continuousCollisionCirclePointTime( pointOccured );
        if ( result.dt < maxT && result.dt >= 0. )
        {
            result.occured = true;
            result.normal = ( ( circle2Pos.position + circle2Pos.velocity * result.dt ) - ( circle1Pos.position + circle1Pos.velocity * result.dt ) )
                * ( 1 / ( circle1.radius + circle2.radius ) );
        }
    }

    return result;
}

} //namespace