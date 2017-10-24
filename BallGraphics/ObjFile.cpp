#include "ObjFile.h"
#include <fstream>

namespace BallGraphics
{

bool ObjFile::load( const std::string & fileName ) noexcept
{
    std::ifstream fin;
    fin.open( fileName );
    if ( fin.fail() )
    {
        return false;
    }

    char input;
    char input2;
    ObjVertexType vertex;
    TextCoord texCoord;
    ObjVertexType normal;
    FaceType face;

    fin.get( input );
    while ( !fin.eof() )
    {
        if ( input == 'v' )
        {
            fin.get( input );

            if ( input == ' ' )
            {
                fin >> vertex.x >> vertex.y >> vertex.z;

                // Right hand to left hand
                vertex.z *= -1.0f;
                vertices.push_back( vertex );
            }

            if ( input == 't' )
            {
                fin >> texCoord.x >> texCoord.y;

                // Right hand to left hand
                texCoord.y = 1.0f - texCoord.y;
                texCoords.push_back( texCoord );
            }

            if ( input == 'n' )
            {
                fin >> normal.x >> normal.y >> normal.z;

                // Right hand to left hand
                normal.z *= -1.f;
                normals.push_back( normal );
            }
        }

        if ( input == 'f' )
        {
            fin.get( input );
            if ( input == ' ' )
            {
                // Reverse order - Right hand to left hand
                fin >> face.vIndex3 >> input2 >> face.tIndex3 >> input2 >> face.nIndex3
                    >> face.vIndex2 >> input2 >> face.tIndex2 >> input2 >> face.nIndex2
                    >> face.vIndex1 >> input2 >> face.tIndex1 >> input2 >> face.nIndex1;
                faces.push_back( face );
            }
        }

        while ( input != '\n' )
        {
            fin.get( input );
        }

        fin.get( input );
    }
    return true;
}

} //namespace