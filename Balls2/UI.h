#pragma once
#include "Graphics.h"
#include "Vector.h"
#include "UICamera.h"
#include "string"

namespace BallGraphics
{
class EffectText;
class EffectColor;
class EffectTexture;
class Font;
class Texture;
}

class UI
{
public:
	void initialize(BallGraphics::Graphics& graphics,
					BallGraphics::EffectText& effText,
					BallGraphics::EffectColor& effColor,
					BallGraphics::EffectTexture& effTexture,
					BallGraphics::Font& font,
					BallGraphics::Texture& fontTexture) noexcept;

	BallGraphics::Id addMenuRectInst(std::function<Vector()>&& worldFunc) noexcept;
	BallGraphics::Id addUITextMesh(const std::string& text, const DirectX::XMVECTOR& color) noexcept;
	BallGraphics::Id addUIMeshInst(BallGraphics::Id id, BallGraphics::Id layer, std::function<Vector()>&& worldFunc) noexcept;

	BallGraphics::Graphics& getGraphics() noexcept;
	BallGraphics::EffectTexture& getEffectTexture() noexcept;
	BallGraphics::EffectColor& getEffectColor() noexcept;
	BallGraphics::UICamera& getUICamera() noexcept;

private:
	BallGraphics::Graphics* graphics_{ nullptr };
	BallGraphics::EffectText* effectText_{ nullptr };
	BallGraphics::EffectColor* effectColor_{ nullptr };
	BallGraphics::EffectTexture* effectTexture_{ nullptr };
	BallGraphics::Font* font_{ nullptr };
	BallGraphics::Texture* fontTexture_{ nullptr };
	std::shared_ptr<BallGraphics::UICamera> camera_{ nullptr };

	BallGraphics::Id menuColorRectMesh_{ BallGraphics::Id() };
};