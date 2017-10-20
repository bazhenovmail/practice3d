#pragma once

#include <Menu.h>
#include <functional>
#include "Mode.h"

class GameOver: public Mode
{
public:
	void initialize(UI&,
					BallInput::Input&,
					std::function<void()> onNewGame,
					std::function<void()> onExit) noexcept;

	void enter();
private:
	std::function<void()> onNewGame_;
	std::function<void()> onExit_;
	Menu menu_;
	BallGraphics::Id gameOverTextMesh_;
	BallGraphics::Id gameOverTextInst_;

	void leave_();
};

