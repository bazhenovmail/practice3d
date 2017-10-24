#include "LevelReader.h"
#include <unordered_map>
#include <fstream>
#include <assert.h>

std::unordered_map<char, LevelBrickContent> contentMap
{
    { '.', LevelBrickContent::None },
    { 'O', LevelBrickContent::BallSizePlus },
    { 'o', LevelBrickContent::BallSizeMinus },
    { 'W', LevelBrickContent::RacketWidthPlus },
    { 'w', LevelBrickContent::RacketWidthMinus },
    { '*', LevelBrickContent::Fireball },
    { 'B', LevelBrickContent::BallPlus },
    { 'H', LevelBrickContent::HPPlus },
    { 'h', LevelBrickContent::HPMinus },
};

Level readLevel( const std::string& fileName )
{
    std::ifstream file( fileName );
    assert( file.is_open() );

    Level result;

    assert( file.good() );
    file >> result.width;
    assert( file.good() );
    file >> result.height;

    result.bricks.resize( result.width * result.height );

    char ch;
    for ( size_t i = 0; i < result.width * result.height; )
    {
        assert( file.good() );
        file >> ch;

        if ( ch != '.' )
        {
            assert( ch >= '0' && ch <= '9' );
            result.bricks[i].durability = ch - '0';
        }

        assert( file.good() );
        file >> ch;
        auto it = contentMap.find( ch );
        assert( it != contentMap.end() );
        if ( result.bricks[i].durability == 0 )
        {
            assert( it->second == LevelBrickContent::None );
        }
        result.bricks[i].content = it->second;

        i++;
    }
    file >> ch;
    assert( file.eof() );

    return result;
}