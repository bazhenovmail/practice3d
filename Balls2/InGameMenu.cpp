#include "InGameMenu.h"
#include "Input.h"

void InGameMenu::initialize( UI& ui,
                             BallInput::Input & input,
                             std::function<void()> onResume,
                             std::function<void()> onExit ) noexcept
{
    input_ = &input;
    menu_.initialize( ui, input );
    onResume_ = onResume;
    menu_.addItem( "Resume", [this]()
    {
        menu_.enable( false );
        input_->clearOnKeyDown( VK_ESCAPE );
        if ( onResume_ )
            onResume_();
    } );
    menu_.addItem( "Exit to main menu", [this, onExit]()
    {
        menu_.enable( false );
        input_->clearOnKeyDown( VK_ESCAPE );
        if ( onExit )
            onExit();
    } );
}

void InGameMenu::enter()
{
    menu_.enable( true );
    input_->setOnKeyDown( VK_ESCAPE, [this]()
    {
        auto onResume = onResume_;
        this->menu_.enable( false );
        if ( onResume )
            onResume();
        return true;
    } );
}
