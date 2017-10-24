#include "World.h"
#include <assert.h>
#include "Line.h"
#include "Collisions/ShapeContinuousCollision.h"
#include "Circle.h"
#include "Rectangle.h"
#include <iterator>
#include "Timer.h"

namespace BallPhysics
{

World::CollisionGroup_::CollisionGroup_( bool manualGrid ) :
    manualGridFill{ manualGrid }
{
    for ( auto& cell : grid )
    {
        cell.reserve( cellDefaultCapacity_ );
    }
}

World::World()
{

}

void World::initialize( float width, float height )
{
    cellSize_ = Vector{ width / gridWidth_, height / gridHeight_ };
    worldSize_.x = width;
    worldSize_.y = height;
}

Vector World::getPosition( Id id ) const noexcept
{
    auto it = bodies_.find( id );
    assert( it != bodies_.end() );
    return it->second.body.getPosition();
}

Vector World::getVelocity( Id id ) const noexcept
{
    auto it = bodies_.find( id );
    assert( it != bodies_.end() );
    return it->second.body.getVelocity();
}

Id World::addLine( const ShapelessObject& obj, const Line& line )
{
    auto it = bodies_.emplace( std::piecewise_construct, std::forward_as_tuple( nextId_ ), std::forward_as_tuple( obj, true ) );
    it.first->second.body.setLine( line );
    return nextId_++;
}

Id World::addRectangle( const ShapelessObject& obj, const Rectangle& rect )
{
    auto it = bodies_.emplace( std::piecewise_construct, std::forward_as_tuple( nextId_ ), std::forward_as_tuple( obj ) );
    it.first->second.body.setRectangle( rect );
    return nextId_++;
}

Id World::addCircle( const ShapelessObject& obj, const Circle& circle )
{
    auto it = bodies_.emplace( std::piecewise_construct, std::forward_as_tuple( nextId_ ), std::forward_as_tuple( obj ) );
    it.first->second.body.setCircle( circle );
    return nextId_++;
}

void World::setLine( Id id, const Line& line )
{
    auto it = bodies_.find( id );
    assert( it != bodies_.end() );
    it->second.body.setLine( line );
}

void World::removeBody( Id id )
{
    auto it = bodies_.find( id );
    assert( it != bodies_.end() );
    if ( it->second.inGroup )
    {
        auto groupIt = collisionGroups_.find( it->second.groupId );
        assert( groupIt != collisionGroups_.end() );

        auto cellRange = getCellRange( it->second.body, 0.f );
        for ( size_t i = ( cellRange.minX ); i <= ( cellRange.maxX ); i++ )
        {
            for ( size_t j = cellRange.minY; j <= cellRange.maxY; j++ )
            {
                std::vector<Id>& cell = groupIt->second.grid[j * gridWidth_ + i];
                auto inCellIt = std::find( cell.begin(), cell.end(), id );
                assert( inCellIt != cell.end() );	//if a body existed less than one frame, this will strike
                cell.erase( inCellIt );
            }
        }

        auto groupEraseResult = groupIt->second.bodies.erase( id );
        assert( groupEraseResult == 1 );
    }
    bodies_.erase( it );
}

void World::clear()
{
    bodies_.clear();
    collisionGroups_.clear();
}

Id World::addCollisionGroup( bool manualGrid )
{
    auto addResult = collisionGroups_.emplace( nextCollisionGroupId_, CollisionGroup_{ manualGrid } );
    assert( addResult.second );
    return nextCollisionGroupId_++;
}

void World::addToCollisionGroup( Id groupId, Id bodyId )
{
    auto it = collisionGroups_.find( groupId );
    assert( it != collisionGroups_.end() );

    auto result = it->second.bodies.emplace( bodyId );
    assert( result.second );

    auto bodyIt = bodies_.find( bodyId );
    assert( bodyIt != bodies_.end() );
    assert( !bodyIt->second.inGroup );
    bodyIt->second.inGroup = true;
    bodyIt->second.groupId = groupId;
}

void World::removeFromCollisionGroup( Id groupId, Id bodyId ) noexcept
{
    auto it = collisionGroups_.find( groupId );
    assert( it != collisionGroups_.end() );

    auto bodyIt = bodies_.find( bodyId );
    assert( bodyIt != bodies_.end() );
    assert( bodyIt->second.inGroup );

    auto erased = it->second.bodies.erase( bodyId );
    assert( erased == 1 );
    bodyIt->second.inGroup = false;
}

void World::addCollisionGroupDependency( Id group1, Id group2, CollisionFunc func, bool resolveCollision )
{
    if ( group1 > group2 )
    {
        std::swap( group1, group2 );
    }

    auto it1 = collisionGroups_.find( group1 );
    assert( it1 != collisionGroups_.end() );

    auto it2 = collisionGroups_.find( group2 );
    assert( it2 != collisionGroups_.end() );

    auto straightIt = it1->second.groups.find( group2 );
    assert( straightIt == it1->second.groups.end() );

    auto reverseIt = it2->second.groups.find( group1 );
    assert( reverseIt == it2->second.groups.end() );

    auto result = it1->second.groups.emplace( group2, CollisionDependency_{ func, resolveCollision } );
    assert( result.second );
}

void World::affectBody( Id bodyId, const Body::VelocityChanger& changer )
{
    auto bodyIt = bodies_.find( bodyId );
    assert( bodyIt != bodies_.end() );
    bodyIt->second.body.affectVelocity( changer );
}

void World::frame( float dt )
{

    int counter{ 0 };
#ifndef STEPBYSTEP
    while ( dt > 0. )
#endif
    {
        FullResult_ fullResult;

        auto processBodies = [this, dt, &fullResult]
        ( std::unordered_map<Id, BodyInGroup_>::const_iterator body1It, std::unordered_map<Id, BodyInGroup_>::const_iterator body2It )
        {
            FullResult_ res;
            res.id1 = body1It->first;
            res.id2 = body2It->first;
            res.result = body1It->second.body.collisionResult( body2It->second.body, dt );

            if ( res.result.occured && ( !fullResult.result.occured || res.result.dt < fullResult.result.dt ) )
            {
                assert( res.result.dt >= 0. );
                fullResult = res;
            }
        };

        auto physStart = Timer::Clock::now();

        createPartition_( dt );

        auto physEnd = Timer::Clock::now();
        auto physDiff = std::chrono::duration_cast<std::chrono::milliseconds> ( physEnd - physStart );


        for ( const auto& group1 : collisionGroups_ )
        {
            for ( const auto& group2Data : group1.second.groups )
            {
                if ( group1.first == group2Data.first )
                {
                    for ( const auto& cell : group1.second.grid )
                    {
                        for ( auto idIt1 = cell.begin(); idIt1 != cell.end(); idIt1++ )
                        {
                            auto body1It = bodies_.find( *idIt1 );
                            assert( body1It != bodies_.end() );

                            for ( auto idIt2 = cell.begin(); idIt2 != idIt1; idIt2++ )
                            {
                                auto body2It = bodies_.find( *idIt2 );
                                assert( body2It != bodies_.end() );

                                processBodies( body1It, body2It );
                            }
                        }
                    }
                }
                else
                {
                    auto group2It = collisionGroups_.find( group2Data.first );
                    assert( group2It != collisionGroups_.end() );
                    const auto& group2 = group2It->second;

                    for ( size_t i = 0; i < gridWidth_ * gridHeight_; i++ )
                    {
                        if ( !group1.second.grid[i].empty() && !group2.grid[i].empty() )
                        {

                            for ( auto id1 : group1.second.grid[i] )
                            {
                                auto body1It = bodies_.find( id1 );
                                assert( body1It != bodies_.end() );

                                for ( auto id2 : group2.grid[i] )
                                {
                                    auto body2It = bodies_.find( id2 );
                                    assert( body2It != bodies_.end() );

                                    processBodies( body1It, body2It );
                                }
                            }
                        }
                    }
                }
            }
        }

        if ( fullResult.result.occured )
        {
            assert( fullResult.result.dt >= 0. );
            dt -= fullResult.result.dt;
            locomotion_( fullResult.result.dt );
            if ( collisionResponse_( fullResult ) )
            {
                dt = 0.;
            }
        }
        else
        {
            locomotion_( dt );
            dt = 0.;
        }
        counter++;
    }
}

void World::fillGroupGrid( Id groupId )
{
    auto it = collisionGroups_.find( groupId );
    assert( it != collisionGroups_.end() );

    fillGroup_( it->second, 0.f );
}

World::CellRange World::getCellRange( const Body& body, float dt )
{
    CellRange result;

    std::pair<Rectangle, Vector> boundary = body.boundingVolume( dt );
    Vector pos = boundary.second + worldSize_ / 2.f;

    Vector start = pos;
    start.x -= boundary.first.width / 2;
    start.y -= boundary.first.height / 2;

    Vector end = pos;
    end.x += boundary.first.width / 2;
    end.y += boundary.first.height / 2;

    start.x /= cellSize_.x;
    start.y /= cellSize_.y;

    end.x /= cellSize_.x;
    end.y /= cellSize_.y;

    const float maxError = 1e-6;

    result.minX = start.x < 0 ? 0 : start.x;
    result.minY = start.y < 0 ? 0 : start.y;
    result.maxX = end.x >= gridWidth_ ? gridWidth_ - 1 : end.x;
    result.maxY = std::max( ( end.y >= gridHeight_ ? gridHeight_ - 1 : end.y ), 0.f );

    return result;
}

void World::fillGroup_( CollisionGroup_& group, float dt )
{
    for ( auto& cell : group.grid )
    {
        cell.clear();
        assert( cell.capacity() >= cellDefaultCapacity_ );
    }

    for ( const auto bodyId : group.bodies )
    {
        auto bodyIt = bodies_.find( bodyId );
        assert( bodyIt != bodies_.end() );
        const Body& body = bodyIt->second.body;

        auto cellRange = getCellRange( body, dt );

        for ( size_t i = ( cellRange.minX ); i <= ( cellRange.maxX ); i++ )
        {
            for ( size_t j = cellRange.minY; j <= cellRange.maxY; j++ )
            {
                std::vector<Id>& cell = group.grid[j * gridWidth_ + i];
                cell.push_back( bodyId );
            }
        }
    }
}

void World::createPartition_( float dt )
{
    for ( auto& group : collisionGroups_ )
    {
        if ( !group.second.manualGridFill )
        {
            fillGroup_( group.second, dt );
        }
    }
}

void World::locomotion_( float dt )
{
    for ( auto& body : bodies_ )
    {
        body.second.body.locomote( dt );
    }
}

bool World::collisionResponse_( const FullResult_& result )
{
    auto it1 = bodies_.find( result.id1 );
    assert( it1 != bodies_.end() );
    auto it2 = bodies_.find( result.id2 );
    assert( it2 != bodies_.end() );

    assert( it1->second.inGroup && it2->second.inGroup );

    if ( it1->second.inGroup && it2->second.inGroup )
    {
        auto group1It = collisionGroups_.find( it1->second.groupId );
        assert( group1It != collisionGroups_.end() );

        auto dependency1 = group1It->second.groups.find( it2->second.groupId );

        if ( dependency1 != group1It->second.groups.end() )
        {
            if ( dependency1->second.resolveCollision )
            {
                it1->second.body.processCollision( result.result, it2->second.body );
            }
            if ( dependency1->second.func )
            {
                return dependency1->second.func( result.id1, result.id2 );
            }
        }
    }
    return false;
}

} //namespace