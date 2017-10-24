#include "Mode.h"

void Mode::initialize( UI& ui, BallInput::Input& input ) noexcept
{
    ui_ = &ui;
    input_ = &input;
}
