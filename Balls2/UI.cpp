#include "UI.h"
#include "VectorToMatrix.h"

using namespace BallUtils;

void UI::initialize( BallGraphics::Graphics & graphics,
                     BallGraphics::EffectText & effText,
                     BallGraphics::EffectColor & effColor,
                     BallGraphics::EffectTexture& effTexture,
                     BallGraphics::Font& font,
                     BallGraphics::Texture& fontTexture ) noexcept
{
    graphics_ = &graphics;
    effectText_ = &effText;
    effectColor_ = &effColor;
    effectTexture_ = &effTexture;
    font_ = &font;
    fontTexture_ = &fontTexture;
    camera_ = std::make_shared<BallGraphics::UICamera>();
    assert( camera_ );
    camera_->initialize( graphics_->getScreenWidth(), graphics_->getScreenHeight(), 10000.f, 10001.f );
    graphics_->addCamera( camera_ );

    {
        auto mesh = effectColor_->createMesh();
        mesh->load( 100.f, 20.f, 10000.9f, 0.4, 0.4, 0.9, 0.7 );
        menuColorRectMesh_ = graphics_->addMesh( std::move( mesh ) );
    }
}

BallGraphics::Id UI::addMenuRectInst( std::function<Vector()>&& worldFunc ) noexcept
{
    return addUIMeshInst( menuColorRectMesh_, 1, std::move( worldFunc ) );
}

BallGraphics::Id UI::addUITextMesh( const std::string & text, const DirectX::XMVECTOR& color ) noexcept
{
    assert( graphics_ );
    assert( effectText_ );
    assert( font_ );
    assert( fontTexture_ );
    auto mesh = effectText_->createMesh( *fontTexture_, *font_ );
    mesh->load( text, color, 10000.9f );
    return graphics_->addMesh( std::move( mesh ) );
}

BallGraphics::Id UI::addUIMeshInst( BallGraphics::Id id, BallGraphics::Id layer, std::function<Vector()>&& worldFunc ) noexcept
{
    assert( graphics_ );
    assert( camera_ );
    BallGraphics::Graphics::MeshInstance inst;
    inst.camera = camera_.get();
    inst.layer = layer;
    inst.meshId = id;
    inst.worldFunc = [worldFunc{ std::move( worldFunc ) }]() {return BallUtils::vectorToMatrix( worldFunc() ); };
    return graphics_->addMeshInstance( std::move( inst ) );
}

BallGraphics::Graphics & UI::getGraphics() noexcept
{
    return *graphics_;
}

BallGraphics::EffectTexture & UI::getEffectTexture() noexcept
{
    return *effectTexture_;
}

BallGraphics::EffectColor & UI::getEffectColor() noexcept
{
    return *effectColor_;
}

BallGraphics::UICamera & UI::getUICamera() noexcept
{
    return *camera_;
}
