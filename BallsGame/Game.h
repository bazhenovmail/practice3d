#pragma once
#include <Mode.h>

#include <array>
#include <vector>
#include "World.h"
#include "Brick.h"
#include "Racket.h"
#include <utility>
#include <functional>

#include "InGameMenu.h"
#include "GameOver.h"
#include "Level.h"

#include "Texture.h"
#include "Font.h"
#include <ProjectionCamera.h>

namespace BallsGame
{

class Game: public Mode
{
public:
    void initialize(UI&,
                    BallInput::Input&,
                    BallPhysics::World&,
                    std::function<void()> onPause,
                    std::function<void()> onResume,
                    std::function<void()> onExit) noexcept;
	void enter();
	void leave();
private:
    BallPhysics::World* world_{nullptr};

	std::shared_ptr<BallGraphics::ProjectionCamera> camera_;

	BallGraphics::Texture ballTexture_;
	std::unordered_map<unsigned, BallGraphics::Texture> brickTextures_;
	std::unordered_map<LevelBrickContent, BallGraphics::Texture> bonusTextures_;
	std::unordered_map<LevelBrickContent, BallGraphics::Id> bonusMeshes_;
	
	BallPhysics::Id boxId_;
	
	BallPhysics::Id boxGroup_;
	
	BallPhysics::Id ballsGroupWithoutRacket_;
	BallPhysics::Id ballsGroupWithRacket_;

	BallPhysics::Id toRacketGroup_;

	std::unordered_map<BallPhysics::Id, BallGraphics::Id> balls_;

	struct Bonus_
	{
		BallGraphics::Id graphicsId;
		LevelBrickContent content;
	};
	std::unordered_map<BallPhysics::Id, Bonus_> bonuses_;

	static constexpr float screenDepth_{ 3700.0f };
	static constexpr float screenNear_{ 2200.f };

	static constexpr float bonusRadius_ = 16.f;
	static constexpr float ballRadius_ = 10.f;
	
	std::unordered_map<unsigned, BallGraphics::Id> brickMeshes_;
	BallGraphics::Id fieldMesh_;
	BallGraphics::Id brickBackMesh_;
	BallGraphics::Id ballMesh_;

	BallGraphics::Id fieldMeshInstance_;

    std::vector<Brick> field_;
    std::unordered_map<BallPhysics::Id, size_t> idToBrickIndex_;
	Vector brickSize_;// { 40.f, 25.f };
	const Vector gameSize_{ 800.f, 800.f };
    BallPhysics::Id bricksCollisionGroup_;
	BallPhysics::Id bonusGroup_;

	Racket racket_;
    InGameMenu inGameMenu_;
	GameOver gameOver_;
    std::function<void()> onPause_;
    std::function<void()> onResume_;
	std::function<void()> onExit_;

	Level level_;


	void unbindKeys_();
    void bindKeys_();
    void addBrick_(unsigned index);
	void addBrickForeground_(unsigned index);

	LevelBrickContent removeBonus_(BallPhysics::Id);
	void consumeBonus_(LevelBrickContent);
	bool ballCrossedTheLine_(BallPhysics::Id activeId);

	BallPhysics::Id addMeshInstance_(BallGraphics::Id id, std::function<Vector()> worldFunc) noexcept;
	void initBonusMesh_(LevelBrickContent content, const std::string & meshFile, const std::string& textureFile) noexcept;
};

}//namespace BallsGame