#pragma once
#include <vector>

namespace BallGraphics
{

class ObjFile
{
public:
	struct ObjVertexType
	{
		float x, y, z;
	};

	struct TextCoord
	{
		float x, y;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	bool load(const std::string & fileName) noexcept;

	std::vector<ObjVertexType> vertices;
	std::vector<TextCoord> texCoords;
	std::vector<ObjVertexType> normals;
	std::vector<FaceType> faces;
};

}//namespace BallGraphics