#pragma once

#include "Vector.h"
#include <unordered_map>
#include <unordered_set>
#include "Body.h"
#include <memory>
#include <array>

namespace BallPhysics
{

using Id = unsigned;

class Line;
class Rectangle;
class Circle;

class World
{
public:
    World();

    void initialize(float width, float height);

    Vector getPosition(Id) const noexcept;
	Vector getVelocity(Id) const noexcept;

    Id addLine(const ShapelessObject&, const Line& line);
    Id addRectangle(const ShapelessObject&, const Rectangle& rect);
    Id addCircle(const ShapelessObject&, const Circle& circle);
	void setLine(Id, const Line& line);

    void removeBody(Id);
	void clear();

    Id addCollisionGroup(bool manualGrid = false);
    void addToCollisionGroup(Id groupId, Id bodyId);
	void removeFromCollisionGroup(Id groupId, Id bodyId) noexcept;

    using CollisionFunc = std::function<bool(Id activeBody, Id passiveBody)>;
	void addCollisionGroupDependency(Id group1, Id group2, CollisionFunc = nullptr, bool resolveCollision = true);
    void fillGroupGrid(Id);
    struct CellRange_
    {
        size_t minX;
        size_t minY;
        size_t maxX;
        size_t maxY;
    };
    CellRange_ getCellRange_(const Body&, float dt);

	void affectBody(Id bodyId, const Body::VelocityChanger& changer);

    void frame(float dt);

private:
    struct FullResult_
    {
        Id id1;
        Id id2;
        CollisionResult result;
    };

    struct BodyInGroup_
    {
        BodyInGroup_(const ShapelessObject& obj, bool userControlled = false): body(obj, userControlled) {}

        Body body;
        Id groupId;
        bool inGroup{false};
    };

    Id nextId_{Id()};
    std::unordered_map<Id, BodyInGroup_> bodies_;

    //std::unordered_map<Id, Body::VelocityChanger> velocityChangers_;

    static constexpr unsigned gridWidth_{35};//48};
    static constexpr unsigned gridHeight_{21};// 27
    static constexpr size_t cellDefaultCapacity_{10};
    Vector cellSize_;
    Vector worldSize_;

	struct CollisionDependency_
	{
		CollisionFunc func;
		bool resolveCollision;
	};
    struct CollisionGroup_
    {
        CollisionGroup_(bool manualGrid);
        std::unordered_set<Id> bodies;
        std::unordered_map<Id, CollisionDependency_> groups;
        std::array<std::vector<Id>, gridWidth_ * gridHeight_> grid;
        bool manualGridFill{false};
        bool withGroup(Id id) const { return groups.find(id) != groups.end(); }
    };

    Id nextCollisionGroupId_{Id()};
    std::unordered_map<Id, CollisionGroup_> collisionGroups_;

    void fillGroup_(CollisionGroup_&, float dt);
    void createPartition_(float dt);
    void locomotion_(float dt);

	//returns true if all locomotion has to be stopped immediately
    bool collisionResponse_(const FullResult_&);
};

} //namespace BallPhysics