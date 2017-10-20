#include "GameOver.h"
#include "Input.h"

void GameOver::initialize(UI& ui,
						  BallInput::Input &input,
						  std::function<void()> onNewGame,
						  std::function<void()> onExit) noexcept
{
	__super::initialize(ui, input);
	menu_.initialize(ui, input);
	onNewGame_ = onNewGame;
	onExit_ = onExit;
	menu_.addItem("New game", [this]()
	{
		leave_();
		if (onNewGame_)
			onNewGame_();
	});
	menu_.addItem("Exit to main menu", [this, onExit]()
	{
		leave_();
		if (onExit_)
			onExit_();
	});
}

void GameOver::enter()
{
	menu_.enable(true);
	float h = menu_.itemHeight * menu_.size();

	gameOverTextMesh_ = ui_->addUITextMesh("GAME OVER", { 1.0, 0.0, 0.0, 1.0 });
	gameOverTextInst_ = ui_->addUIMeshInst(gameOverTextMesh_, 2, [h]() {return Vector{ 0., h }; });
}

void GameOver::leave_()
{
	menu_.enable(false);
	ui_->getGraphics().removeMeshInstance(gameOverTextInst_);
	ui_->getGraphics().removeMesh(gameOverTextMesh_);
}
