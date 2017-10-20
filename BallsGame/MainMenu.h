#pragma once
#include <Mode.h>
#include <UI.h>
#include <Menu.h>
#include <functional>

namespace BallsGame
{

class MainMenu: public Mode
{
public:
    void initialize(UI&,
					BallInput::Input&,
					std::function<void()> onNewGame,
					std::function<void()> onExit) noexcept;
	void enter();
private:
    Menu menu_;
	std::function<void()> onNewGame_;
	std::function<void()> onExit_;
};

}//namespace BallsGame
