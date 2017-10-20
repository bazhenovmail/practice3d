#pragma once

class UI;

namespace BallInput
{
class Input;
}

class Mode
{
public:
    void initialize(UI&, BallInput::Input&) noexcept;
protected:
	UI* ui_;
    BallInput::Input* input_;
};

