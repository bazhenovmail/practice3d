#include "Vector.h"

float Vector::lengthSquared() const
{
    return x * x + y * y;
}

Vector & Vector::operator+=( const Vector & arg )
{
    x += arg.x;
    y += arg.y;
    return *this;
}

Vector operator - ( const Vector& arg )
{
    return Vector( -arg.x, -arg.y );
}

Vector operator+( const Vector & arg1, const Vector & arg2 )
{
    return Vector( arg1.x + arg2.x, arg1.y + arg2.y );
}

Vector operator-( const Vector & arg1, const Vector & arg2 )
{
    return Vector( arg1.x - arg2.x, arg1.y - arg2.y );
}

Vector operator*( const Vector & vec, float mul )
{
    return Vector( vec.x * mul, vec.y * mul );
}

Vector operator*( float mul, const Vector & vec )
{
    return Vector( vec.x * mul, vec.y * mul );
}

Vector operator/( const Vector & vec, float div )
{
    return Vector{ vec.x / div, vec.y / div };
}

float dotProduct( const Vector & arg1, const Vector & arg2 )
{
    return arg1.x * arg2.x + arg1.y * arg2.y;
}