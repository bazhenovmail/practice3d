#include "Game.h"
#include <Input.h>
#include <World.h>
#include <Graphics.h>
#include "LevelReader.h"
#include <DirectXMath.h>
#include <VectorToMatrix.h>

using namespace BallPhysics;

namespace
{
std::unordered_map<unsigned, DirectX::XMVECTOR> brickColors
{
    { 1,{ 0.f, 0.5f, 0.5f, 1.0f } },
    { 2,{ 0.3f, 0.5f, 0.5f, 1.0f } },
    { 3,{ 0.5f, 0.5f, 0.5f, 1.0f } }
};
}

namespace BallsGame
{

void Game::initialize( UI& ui,
                       BallInput::Input & input,
                       BallPhysics::World & world,
                       std::function<void()> onPause,
                       std::function<void()> onResume,
                       std::function<void()> onExit ) noexcept
{
    __super::initialize( ui, input );
    world_ = &world;
    onPause_ = onPause;
    onResume_ = onResume;
    onExit_ = onExit;

    level_ = readLevel( "../../BallsGame/Resources/Levels/0.txt" );
    field_.resize( level_.width * level_.height );

    brickSize_.x = gameSize_.x / level_.width;
    brickSize_.y = gameSize_.y / level_.height;
    const float gameSizeMul = gameSize_.y / 1080.;

    camera_ = std::make_shared<BallGraphics::ProjectionCamera>();
    camera_->initialize( DirectX::XM_PIDIV4 / 2, float( ui_->getGraphics().getScreenWidth() ) / ui_->getGraphics().getScreenHeight(),
                         screenNear_ * gameSizeMul, screenDepth_ * gameSizeMul, -3000 * gameSizeMul );
    ui_->getGraphics().addCamera( camera_ );

    racket_.initialize( ui, *camera_, input, world, gameSize_ );

    inGameMenu_.initialize( ui, input, [this]()
    {
        bindKeys_();
        if ( onResume_ )
            onResume_();
    }, [this]()
    {
        leave();
        if ( onExit_ )
            onExit_();
    } );
    gameOver_.initialize( ui, input, [this]()
    {
        leave();
        enter();
        if ( onResume_ )
            onResume_();
    }, [this]()
    {
        leave();
        if ( onExit_ )
            onExit_();
    } );

    ballTexture_ = ui_->getGraphics().createTexture( "../../BallsGame/Resources/brick1.tga" );
    initBonusMesh_( LevelBrickContent::RacketWidthPlus, "../../BallsGame/Resources/BonusExtension.3dobj", "../../BallsGame/Resources/brick1.tga" );
    initBonusMesh_( LevelBrickContent::RacketWidthMinus, "../../BallsGame/Resources/BonusContraction.3dobj", "../../BallsGame/Resources/brick1.tga" );

    brickTextures_.emplace( 1, ui_->getGraphics().createTexture( "../../BallsGame/Resources/brick_1.tga" ) );
    brickTextures_.emplace( 2, ui_->getGraphics().createTexture( "../../BallsGame/Resources/brick_2.tga" ) );
    brickTextures_.emplace( 3, ui_->getGraphics().createTexture( "../../BallsGame/Resources/brick_3.tga" ) );

    {
        auto mesh = ui_->getEffectColor().createMesh();
        mesh->load( brickSize_.x, brickSize_.y, 0.1f,
                    0., 0.f, 0., 1.0 );
        brickBackMesh_ = ui_->getGraphics().addMesh( std::move( mesh ) );
    }

    {
        auto mesh = ui_->getEffectTexture().createMesh( ballTexture_ );
        BallGraphics::ObjFile objFile;
        bool loaded = objFile.load( "../../BallsGame/Resources/Field.3dobj" );
        assert( loaded );
        loaded = mesh->load( objFile, 20.f );
        assert( loaded );
        fieldMesh_ = ui_->getGraphics().addMesh( std::move( mesh ) );
    }

    {
        auto mesh = ui_->getEffectTexture().createMesh( ballTexture_ );
        BallGraphics::ObjFile objFile;
        bool loaded = objFile.load( "../../BallsGame/Resources/Ball.3dobj" );
        assert( loaded );
        loaded = mesh->load( objFile, ballRadius_ );
        assert( loaded );
        ballMesh_ = ui_->getGraphics().addMesh( std::move( mesh ) );
    }

    for ( auto& brTexture : brickTextures_ )
    {
        auto mesh = ui_->getEffectTexture().createMesh( brTexture.second );
        BallGraphics::ObjFile objFile;
        bool loaded = objFile.load( "../../BallsGame/Resources/Brick2.3dobj" );
        assert( loaded );
        loaded = mesh->load( objFile );
        assert( loaded );
        brickMeshes_.emplace( brTexture.first, ui_->getGraphics().addMesh( std::move( mesh ) ) );
    }
}

void Game::enter()
{
    bindKeys_();
    racket_.enter();

    input_->setOnMouseDown( BallInput::Input::MouseButtonFlag::Right, [this]()
    {
        input_->setOnMouseMove( [this]( LONG x, LONG y )
        {
            camera_->rotateX( y / 20.f );
            camera_->rotateY( x / 20.f );
            camera_->render();
        } );
        racket_.stopMotion();
        return false;
    } );
    input_->setOnMouseUp( BallInput::Input::MouseButtonFlag::Right, [this]()
    {
        input_->setOnMouseMove( [this]( LONG x, LONG y )
        {
            racket_.processMouse( x, y );
        } );
        return false;
    } );
    input_->setOnMouseMove( [this]( LONG x, LONG y )
    {
        racket_.processMouse( x, y );
    } );

    boxId_ = world_->addRectangle( BallPhysics::ShapelessObject{ Vector{ 0.f, -gameSize_.y / 2.f },{ 0, 0 },
                                   std::numeric_limits<float>::infinity() },
                                   BallPhysics::Rectangle( gameSize_.x, 2 * gameSize_.y, true ) );

    auto gameOverLineId = world_->addLine( BallPhysics::ShapelessObject{ Vector{ 0.f, -gameSize_.y / 2.f - ballRadius_ * 2 },{ 0, 0 },
                                           std::numeric_limits<float>::infinity() },
                                           BallPhysics::Line( { Vector{ -gameSize_.x / 2.f, 0. }, Vector{ ui_->getGraphics().getScreenWidth() / 2.f, 0. } } ) );

    auto toRacketLineId = world_->addLine( BallPhysics::ShapelessObject{ Vector{ 0.f, -gameSize_.y / 2.f + 80.f },{ 0, 0 },
                                           std::numeric_limits<float>::infinity() },
                                           BallPhysics::Line( { Vector{ -gameSize_.x / 2.f, 0. }, Vector{ ui_->getGraphics().getScreenWidth() / 2.f, 0. } } ) );

    toRacketGroup_ = world_->addCollisionGroup();
    boxGroup_ = world_->addCollisionGroup();
    ballsGroupWithoutRacket_ = world_->addCollisionGroup();
    ballsGroupWithRacket_ = world_->addCollisionGroup();
    bricksCollisionGroup_ = world_->addCollisionGroup( true );
    bonusGroup_ = world_->addCollisionGroup();
    auto gameOverGroup = world_->addCollisionGroup();

    world_->addToCollisionGroup( toRacketGroup_, toRacketLineId );
    world_->addToCollisionGroup( boxGroup_, boxId_ );
    world_->addToCollisionGroup( gameOverGroup, gameOverLineId );

    world_->addCollisionGroupDependency( racket_.getBoundaryGroup(), boxGroup_, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        racket_.stopMotion();
        return false;
    }, false );
    world_->addCollisionGroupDependency( ballsGroupWithoutRacket_, toRacketGroup_, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        world_->removeFromCollisionGroup( ballsGroupWithoutRacket_, passiveId );
        world_->addToCollisionGroup( ballsGroupWithRacket_, passiveId );
        return false;
    }, false );
    world_->addCollisionGroupDependency( ballsGroupWithoutRacket_, ballsGroupWithoutRacket_ );
    world_->addCollisionGroupDependency( ballsGroupWithRacket_, ballsGroupWithRacket_ );
    world_->addCollisionGroupDependency( ballsGroupWithoutRacket_, ballsGroupWithRacket_ );
    world_->addCollisionGroupDependency( ballsGroupWithRacket_, racket_.getGroup(), [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        world_->removeFromCollisionGroup( ballsGroupWithRacket_, passiveId );
        world_->addToCollisionGroup( ballsGroupWithoutRacket_, passiveId );
        return false;
    } );
    world_->addCollisionGroupDependency( ballsGroupWithoutRacket_, boxGroup_ );
    world_->addCollisionGroupDependency( ballsGroupWithRacket_, boxGroup_ );
    world_->addCollisionGroupDependency( ballsGroupWithoutRacket_, bricksCollisionGroup_, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        auto it = idToBrickIndex_.find( passiveId );
        assert( it != idToBrickIndex_.end() );
        assert( it->second < level_.bricks.size() );
        auto& brick = field_[it->second];
        assert( brick.durability > 0 );
        assert( brick.physicsId == passiveId );

        brick.durability--;
        ui_->getGraphics().removeMeshInstance( brick.graphicsIdForeground );
        if ( brick.durability == 0 )
        {
            auto bonusMeshIt = bonusMeshes_.find( level_.bricks[it->second].content );
            if ( bonusMeshIt != bonusMeshes_.end() )
            {
                auto pos = world_->getPosition( passiveId );
                auto physId = world_->addCircle( { pos,{ 0.f, -50.f }, 1.f }, { bonusRadius_ } );

                auto graphicsId = addMeshInstance_( bonusMeshIt->second, [this, physId]()
                {
                    return Vector{ world_->getPosition( physId ) };
                } );

                bonuses_.emplace( physId, Bonus_{ graphicsId, level_.bricks[it->second].content } );
                world_->addToCollisionGroup( bonusGroup_, physId );
            }
            world_->removeBody( passiveId );
            ui_->getGraphics().removeMeshInstance( brick.graphicsIdBackground );
        }
        else
        {
            addBrickForeground_( it->second );
        }
        return false;
    } );

    world_->addCollisionGroupDependency( racket_.getGroup(), bonusGroup_, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        consumeBonus_( removeBonus_( passiveId ) );
        return false;
    } );

    world_->addCollisionGroupDependency( gameOverGroup, bonusGroup_, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        removeBonus_( activeId );
        return false;
    } );

    world_->addCollisionGroupDependency( ballsGroupWithRacket_, gameOverGroup, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        return ballCrossedTheLine_( activeId );
    } );
    world_->addCollisionGroupDependency( ballsGroupWithoutRacket_, gameOverGroup, [this]( BallPhysics::Id activeId, BallPhysics::Id passiveId )
    {
        return ballCrossedTheLine_( activeId );
    } );

    for ( int i = 0; i < 1; i++ )
    {
        auto ballId = world_->addCircle( ShapelessObject{ Vector{ float( -gameSize_.x / 2. + 45 * ( i + 1 ) ),
                                         float( -250 + ( i % 7 ) ) }, Vector{ 100.0, 150.0 },
                                         float( 1. ) }, Circle( ballRadius_ ) );

        balls_.emplace( ballId, addMeshInstance_( ballMesh_, [this, ballId]()
        {
            return Vector{ world_->getPosition( ballId ) };
        } ) );

        world_->addToCollisionGroup( ballsGroupWithoutRacket_, ballId );
    }

    for ( auto i = 0; i < level_.width * level_.height; i++ )
        addBrick_( i );
    world_->fillGroupGrid( bricksCollisionGroup_ );

    fieldMeshInstance_ = addMeshInstance_( fieldMesh_, []()
    {
        return Vector();
    } );

    if ( onResume_ )
        onResume_();
}

void Game::leave()
{
    ui_->getGraphics().removeMeshInstance( fieldMeshInstance_ );

    racket_.leave();
    world_->clear();

    for ( const auto& ball : balls_ )
    {
        ui_->getGraphics().removeMeshInstance( ball.second );
    }
    balls_.clear();
    for ( const auto& bonus : bonuses_ )
    {
        ui_->getGraphics().removeMeshInstance( bonus.second.graphicsId );
    }
    bonuses_.clear();
    for ( const auto& brick : field_ )
    {
        if ( brick.durability )
        {
            ui_->getGraphics().removeMeshInstance( brick.graphicsIdBackground );
            ui_->getGraphics().removeMeshInstance( brick.graphicsIdForeground );
        }
    }
}

void Game::unbindKeys_()
{
    input_->clearOnKeyDown( VK_ESCAPE );
}

void Game::bindKeys_()
{
    input_->setOnKeyDown( VK_ESCAPE, [this]()
    {
        inGameMenu_.enter();
        if ( onPause_ )
            onPause_();
        return true;
    } );
}

void Game::addBrick_( unsigned index )
{
    Vector brickPos{ index % level_.width * brickSize_.x + brickSize_.x / 2 - gameSize_.x / 2,
        gameSize_.y / 2 - ( index / level_.width * brickSize_.y + brickSize_.y / 2 ) };

    field_[index].durability = level_.bricks[index].durability;
    if ( field_[index].durability )
    {
        auto brickId = world_->addRectangle( BallPhysics::ShapelessObject{ brickPos, Vector(), std::numeric_limits<float>::infinity() },
                                             BallPhysics::Rectangle( brickSize_.x, brickSize_.y, false ) );
        field_[index].physicsId = brickId;
        auto idToIndexResult = idToBrickIndex_.emplace( brickId, index );
        assert( idToIndexResult.second );
        world_->addToCollisionGroup( bricksCollisionGroup_, brickId );

        addBrickForeground_( index );

        field_[index].graphicsIdBackground = addMeshInstance_( brickBackMesh_, [this, brickId]()
        {
            return Vector{ world_->getPosition( brickId ) };
        } );
    }
}

void Game::addBrickForeground_( unsigned index )
{
    auto meshIt = brickMeshes_.find( field_[index].durability );
    assert( meshIt != brickMeshes_.end() );
    field_[index].graphicsIdForeground = addMeshInstance_( meshIt->second, [this, index]()
    {
        return Vector{ world_->getPosition( field_[index].physicsId ) };
    } );
}

LevelBrickContent Game::removeBonus_( BallPhysics::Id id )
{
    auto bonusIt = bonuses_.find( id );
    assert( bonusIt != bonuses_.end() );
    LevelBrickContent content = bonusIt->second.content;
    world_->removeBody( id );
    ui_->getGraphics().removeMeshInstance( bonusIt->second.graphicsId );
    bonuses_.erase( bonusIt );
    return content;
}

void Game::consumeBonus_( LevelBrickContent content )
{
    switch ( content )
    {
    case LevelBrickContent::RacketWidthPlus:
        racket_.increaseWidth();
        break;
    case LevelBrickContent::RacketWidthMinus:
        racket_.decreaseWidth();
        break;
    default:
        ;
    }
}

bool Game::ballCrossedTheLine_( BallPhysics::Id activeId )
{
    auto ballIt = balls_.find( activeId );
    assert( ballIt != balls_.end() );

    if ( balls_.size() == 1 )
    {
        unbindKeys_();
        gameOver_.enter();
        if ( onPause_ )
            onPause_();
        return true;
    }
    else
    {
        ui_->getGraphics().removeMeshInstance( ballIt->second );
        world_->removeBody( ballIt->first );
        balls_.erase( ballIt );
        return false;
    }
}

BallPhysics::Id Game::addMeshInstance_( BallGraphics::Id id, std::function<Vector()> worldFunc ) noexcept
{
    assert( ui_ );
    BallGraphics::Graphics::MeshInstance inst;
    inst.camera = camera_.get();
    inst.layer = 0;
    inst.meshId = id;
    inst.worldFunc = [worldFunc{ std::move( worldFunc ) }]() {return BallUtils::vectorToMatrix( worldFunc() ); };
    return ui_->getGraphics().addMeshInstance( std::move( inst ) );
}

void Game::initBonusMesh_( LevelBrickContent content, const std::string & meshFile, const std::string & textureFile ) noexcept
{
    auto textureIt = bonusTextures_.emplace( content, ui_->getGraphics().createTexture( textureFile ) );
    assert( textureIt.second );
    auto mesh = ui_->getEffectTexture().createMesh( textureIt.first->second );

    BallGraphics::ObjFile objFile;
    bool loaded = objFile.load( meshFile );
    assert( loaded );
    loaded = mesh->load( objFile, 16.f );
    assert( loaded );
    bonusMeshes_.emplace( content, ui_->getGraphics().addMesh( std::move( mesh ) ) );
}

} //namespace