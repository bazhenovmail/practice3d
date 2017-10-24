#pragma once

#include <Menu.h>
#include <functional>

class InGameMenu
{
public:
    void initialize( UI&,
                     BallInput::Input&,
                     std::function<void()> onResume,
                     std::function<void()> onExit ) noexcept;

    void enter();
private:
    BallInput::Input* input_;
    std::function<void()> onResume_;
    Menu menu_;
};

