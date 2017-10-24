#pragma once

struct Vector
{
    Vector() = default;
    Vector(const Vector&) = default;
    Vector(float iX, float iY): x{iX}, y{iY} {}
    float lengthSquared() const;

    float x = 0;
    float y = 0;

    Vector& operator += (const Vector& arg);
};

Vector operator - (const Vector& arg);
Vector operator + (const Vector& arg1, const Vector& arg2);
Vector operator - (const Vector& arg1, const Vector& arg2);
Vector operator * (const Vector& vec, float mul);
Vector operator * (float mul, const Vector& vec);
Vector operator / (const Vector& vec, float mul);
float dotProduct(const Vector& arg1, const Vector& arg2);

