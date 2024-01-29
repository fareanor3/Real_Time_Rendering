#include "Vector.h"

const Vec2 Vec2_Right = {+1.0f, +0.0f};
const Vec2 Vec2_Left = {-1.0f, +0.0f};
const Vec2 Vec2_Up = {+0.0f, +1.0f};
const Vec2 Vec2_Down = {+0.0f, -1.0f};
const Vec2 Vec2_Zero = {+0.0f, +0.0f};
const Vec2 Vec2_One = {+1.0f, +1.0f};

const Vec3 Vec3_Right = {+1.0f, +0.0f, +0.0f};
const Vec3 Vec3_Left = {-1.0f, +0.0f, +0.0f};
const Vec3 Vec3_Up = {+0.0f, +1.0f, +0.0f};
const Vec3 Vec3_Down = {+0.0f, -1.0f, +0.0f};
const Vec3 Vec3_Front = {+0.0f, +0.0f, +1.0f};
const Vec3 Vec3_Back = {+0.0f, +0.0f, -1.0f};
const Vec3 Vec3_Zero = {+0.0f, +0.0f, +0.0f};
const Vec3 Vec3_One = {+1.0f, +1.0f, +1.0f};

const Vec4 Vec4_RightH = {+1.0f, +0.0f, +0.0f, +1.0f};
const Vec4 Vec4_LeftH = {-1.0f, +0.0f, +0.0f, +1.0f};
const Vec4 Vec4_UpH = {+0.0f, +1.0f, +0.0f, +1.0f};
const Vec4 Vec4_DownH = {+0.0f, -1.0f, +0.0f, +1.0f};
const Vec4 Vec4_FrontH = {+0.0f, +0.0f, +1.0f, +1.0f};
const Vec4 Vec4_BackH = {+0.0f, +0.0f, -1.0f, +1.0f};
const Vec4 Vec4_ZeroH = {+0.0f, +0.0f, +0.0f, +1.0f};
const Vec4 Vec4_Zero = {+0.0f, +0.0f, +0.0f, +0.0f};
const Vec4 Vec4_One = {+1.0f, +1.0f, +1.0f, +1.0f};

//-------------------------------------------------------------------------------------------------
// Fonctions à coder

Vec2 Vec2_Add(Vec2 v1, Vec2 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

inline Vec2 Vec2_Sub(const Vec2 v1, const Vec2 v2)
{
    return (Vec2) { v1.x - v2.x, v1.y - v2.y };
}

inline float Vec2_SignedArea(const Vec2 a, const Vec2 b, const Vec2 c)
{
    Vec2 BA = Vec2_Sub(b, a);
    Vec2 CA = Vec2_Sub(c, a);
    return fmaf(BA.x, CA.y, -fmaf(BA.y, CA.x, 0.0f));
}

float Vec2_Determinant(Vec2 v1, Vec2 v2)
{
    return fmaf(v1.x, v2.y, -fmaf(v1.y, v2.x, 0.0f));
}

bool Vec2_Barycentric(Vec2 *vertices, Vec2 p, float *bary)
{
    memset(bary, 0, 3 * sizeof(float));
    float airePCB = Vec2_SignedArea(p, vertices[1], vertices[2]);
    float airePCA = Vec2_SignedArea(p, vertices[2], vertices[0]);
    float airePAB = Vec2_SignedArea(p, vertices[0], vertices[1]);
    float aireABC = airePCB + airePCA + airePAB;
    bary[0] = airePCB / aireABC;
    bary[1] = airePCA / aireABC;
    bary[2] = airePAB / aireABC;
    if (bary[0] < 0.0f || bary[1] < 0.0f || bary[2] < 0.0f)
        return false;
    return true;
}

Vec3 Vec3_Add(Vec3 v1, Vec3 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

Vec3 Vec3_Sub(Vec3 v1, Vec3 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

Vec3 Vec3_Scale(Vec3 v, float s)
{
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

float Vec3_Dot(Vec3 v1, Vec3 v2)
{
    return fmaf(v1.x, v2.x, fmaf(v1.y, v2.y, v1.z * v2.z));
}

Vec3 Vec3_Cross(Vec3 v1, Vec3 v2)
{
    Vec3 returnV;
    returnV.x = v1.y * v2.z - v1.z * v2.y;
    returnV.y = v1.z * v2.x - v1.x * v2.z;
    returnV.z = v1.x * v2.y - v1.y * v2.x;
    return returnV;
}

float Vec3_Length(Vec3 v)
{
    return sqrtf(fmaf(v.x, v.x, fmaf(v.y, v.y, v.z * v.z)));
}

Vec3 Vec3_Normalize(Vec3 v)
{
    float length = Vec3_Length(v);
    if (length > 1e-7f)
    {
        float inv_length = 1.0f / length;
        v.x *= inv_length;
        v.y *= inv_length;
        v.z *= inv_length;
    }

    return v;
}
Vec3 Vec3_Interpolate(Vec3 v1)
{
    return (Vec3) { 2.0f * v1.x - 1.0f, 2.0f * v1.y - 1.0f, 2.0f * v1.z - 1.0f };
}

Vec3 Vec3_From4(const Vec4 v)
{
    Vec3 returnV = {v.x, v.y, v.z};
    if (v.w != 0 && v.w != 1.0f)
    {
        float inv_w = 1.0f / v.w;
        returnV.x *= inv_w;
        returnV.y *= inv_w;
        returnV.z *= inv_w;
    }
    return returnV;
}

//-------------------------------------------------------------------------------------------------
// Fonctions du basecode

Vec3 Vec3_Mul(Vec3 v1, Vec3 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}

Vec3 Vec3_Max(Vec3 v1, Vec3 v2)
{
    v1.x = fmaxf(v1.x, v2.x);
    v1.y = fmaxf(v1.y, v2.y);
    v1.z = fmaxf(v1.z, v2.z);
    return v1;
}

Vec3 Vec3_Min(Vec3 v1, Vec3 v2)
{
    v1.x = fminf(v1.x, v2.x);
    v1.y = fminf(v1.y, v2.y);
    v1.z = fminf(v1.z, v2.z);
    return v1;
}
