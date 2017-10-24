#pragma once
#include "Texture.h"
#include <array>

namespace BallGraphics
{

class Font
{
public:
    struct FontType
    {
        float left, right;
        int size;
    };

    bool load( const std::string& fileName );
    std::pair<bool, FontType> getData( wchar_t symbol ) const noexcept;

private:
    static constexpr size_t fontSize_ = 95;
    static constexpr size_t diff_ = 32;
    std::array<FontType, fontSize_> font_;

};

} //namespace