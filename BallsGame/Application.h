#pragma once

#include "IApplication.h"
#include <array>
#include "World.h"
#include <utility>
#include <Menu.h>

#include "Game.h"
#include "MainMenu.h"

#include "MenuNode.h"

namespace BallsGame
{

class Application :
    public Balls::IApplication
{
public:
    virtual Balls::Params getParams() const override;
    virtual void initialize( BallPhysics::World&, UI& ui, BallInput::Input& ) noexcept override;
    virtual Balls::FrameResult frame() override;
private:
    BallPhysics::World* world_{ nullptr };
    UI* ui_{ nullptr };
    BallInput::Input* input_{ nullptr };

    bool termination_{ false };
    static constexpr float defaultDt_{ 1.f / 60 };
    float dt_{ defaultDt_ };

    Game game_;

    MainMenu mainMenu_;

    MenuNode<Application, std::string, std::string> node_{ *this, "ololo1", "ololo2" };
};

} //namespace