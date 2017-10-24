#include "Font.h"
#include <assert.h>
#include <fstream>

namespace BallGraphics
{

bool Font::load( const std::string& fileName )
{
    std::ifstream fin;

    fin.open( fileName );
    assert( !fin.fail() );

    char temp;
    for ( unsigned i = 0; i < fontSize_; i++ )
    {
        fin.get( temp );
        while ( temp != ' ' )
        {
            fin.get( temp );
        }
        fin.get( temp );
        while ( temp != ' ' )
        {
            fin.get( temp );
        }

        fin >> font_[i].left;
        fin >> font_[i].right;
        fin >> font_[i].size;
    }
    fin.close();

    return true;
}

std::pair<bool, Font::FontType> Font::getData( wchar_t symbol ) const noexcept
{
    if ( symbol <= diff_ || symbol > diff_ + fontSize_ )
    {
        return{ false, FontType() };
    }
    return{ true, font_[symbol - diff_] };
}

} //namespace