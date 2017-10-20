#include "VectorToMatrix.h"

namespace BallUtils
{

DirectX::XMMATRIX vectorToMatrix(const Vector& pos)
{
	return DirectX::XMMATRIX(1.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 1.0f, 0.0f,
							 (pos.x), (pos.y), 0.0f, 1.0f);
}

}//namespace BallUtils