#include "MainMenu.h"
#include <Input.h>

namespace BallsGame
{

void MainMenu::initialize(
    UI& ui,
    BallInput::Input &input,
    std::function<void()> onNewGame,
    std::function<void()> onExit ) noexcept
{
    __super::initialize( ui, input );
    onNewGame_ = onNewGame;
    onExit_ = onExit;
    menu_.initialize( ui, input );

    menu_.addItem( "New game", [this]()
    {
        menu_.enable( false );
        if ( onNewGame_ )
            onNewGame_();
    } );
    menu_.addItem( "Exit", [this]()
    {
        menu_.enable( false );
        if ( onExit_ )
            onExit_();
    } );
}

void MainMenu::enter()
{
    menu_.enable( true );
}

} //namespace