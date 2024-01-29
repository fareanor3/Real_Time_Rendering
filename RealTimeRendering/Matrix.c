#include "Matrix.h"
#include "Vector.h"
#include "Tools.h"

const Mat3 Mat3_Identity = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f};
const Mat4 Mat4_Identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f};

//-------------------------------------------------------------------------------------------------

void Mat3_Print(Mat3 mat)
{
    printf("Mat3 : \n");
    for (int i = 0; i < 3; ++i)
    {
        printf("| ");
        for (int j = 0; j < 3; ++j)
        {
            printf("%f ", mat.data[i][j]);
        }
        printf("|\n");
    }
}

Mat3 Mat3_MulMM(Mat3 mat1, Mat3 mat2)
{
    Mat3 mat;
    for (int i = 0; i < 3; ++i)
    {
        float mat1_i0 = mat1.data[i][0];
        float mat1_i1 = mat1.data[i][1];
        float mat1_i2 = mat1.data[i][2];
        for (int j = 0; j < 3; ++j)
        {
            mat.data[i][j] = fmaf(mat1_i0, mat2.data[0][j],
                                  fmaf(mat1_i1, mat2.data[1][j],
                                       mat1_i2 * mat2.data[2][j]));
        }
    }
    return mat;
}

Vec3 Mat3_MulMV(Mat3 mat, Vec3 v)
{
    Vec3 returnV;
    for (int i = 0; i < 3; ++i)
    {
        returnV.data[i] = fmaf(mat.data[i][0], v.data[0],
                               fmaf(mat.data[i][1], v.data[1],
                                    mat.data[i][2] * v.data[2]));
    }
    return returnV;
}
// Sarus algrithm
float Mat3_Det(Mat3 mat)
{
    float det = 0.0f;
    det += mat.data[0][0] * (mat.data[1][1] * mat.data[2][2] - mat.data[1][2] * mat.data[2][1]);
    det -= mat.data[0][1] * (mat.data[1][0] * mat.data[2][2] - mat.data[1][2] * mat.data[2][0]);
    det += mat.data[0][2] * (mat.data[1][0] * mat.data[2][1] - mat.data[1][1] * mat.data[2][0]);

    return det;
}

Mat4 Mat4_Transpose(Mat4 matrix)
{
    Mat4 mat;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            mat.data[j][i] = matrix.data[i][j];
    }
    return mat;
}

Mat4 Mat4_MulMM(Mat4 mat1, Mat4 mat2)
{
    Mat4 mat;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            mat.data[i][j] = fmaf(mat1.data[i][0], mat2.data[0][j],
                                  fmaf(mat1.data[i][1], mat2.data[1][j],
                                       fmaf(mat1.data[i][2], mat2.data[2][j],
                                            mat1.data[i][3] * mat2.data[3][j])));
        }
    }
    return mat;
}

Vec4 Mat4_MulMV(Mat4 mat, Vec4 v)
{
    Vec4 returnV;
    returnV.x = fmaf(mat.data[0][0], v.x, fmaf(mat.data[0][1], v.y, fmaf(mat.data[0][2], v.z, mat.data[0][3] * v.w)));
    returnV.y = fmaf(mat.data[1][0], v.x, fmaf(mat.data[1][1], v.y, fmaf(mat.data[1][2], v.z, mat.data[1][3] * v.w)));
    returnV.z = fmaf(mat.data[2][0], v.x, fmaf(mat.data[2][1], v.y, fmaf(mat.data[2][2], v.z, mat.data[2][3] * v.w)));
    returnV.w = fmaf(mat.data[3][0], v.x, fmaf(mat.data[3][1], v.y, fmaf(mat.data[3][2], v.z, mat.data[3][3] * v.w)));
    return returnV;
}

Mat4 Mat4_Scale(Mat4 mat, float s)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            mat.data[i][j] *= s;
    }
    return mat;
}

float Mat4_Cofactor(Mat4 mat, int i, int j)
{
    Mat3 res = {0};
    int x = ((i + j) % 2 == 0) ? 1 : -1;
    int h = 0, k = 0;

    for (int row = 0; row < 4; row++)
    {
        if (row == i)
            continue;
        for (int col = 0; col < 4; col++)
        {
            if (col == j)
                continue;
            res.data[h][k++] = mat.data[row][col];
            if (k == 3)
            {
                k = 0;
                h++;
            }
        }
    }
    return x * Mat3_Det(res);
}

Mat4 Mat4_CofactorMatrix(Mat4 mat)
{
    Mat4 returnMat = Mat4_Identity;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            returnMat.data[i][j] = (float)Mat4_Cofactor(mat, i, j);
    }
    return returnMat;
}

// Algorthme de Laplace
float Mat4_Det(Mat4 mat)
{
    float det = 0;
    for (int i = 0; i < 4; i++)
        det += mat.data[0][i] * Mat4_Cofactor(mat, 0, i);
    return det;
}

Mat4 Mat4_Inv(Mat4 mat)
{
    float det = Mat4_Det(mat);
    if (fabsf(det) < 1e-4)
        return Mat4_Identity;
    return Mat4_Scale(Mat4_Transpose(Mat4_CofactorMatrix(mat)), 1.0f / det);
}

//-------------------------------------------------------------------------------------------------
// Fonctions du basecode

Mat4 Mat4_GetProjectionMatrix(float l, float r, float b, float t, float n, float f)
{
    Mat4 projectionMatrix = Mat4_Identity;

    if (r == l || t == b || f == n)
        return projectionMatrix;

    float rl_diff = r - l;
    float tb_diff = t - b;
    float fn_diff = f - n;

    float two_n = 2.0f * n;
    float rl_sum = r + l;
    float tb_sum = t + b;

    projectionMatrix.data[0][0] = two_n / rl_diff;
    projectionMatrix.data[1][1] = two_n / tb_diff;
    projectionMatrix.data[2][2] = -(f + n) / fn_diff;
    projectionMatrix.data[3][2] = -1.0f;
    projectionMatrix.data[0][2] = -rl_sum / rl_diff;
    projectionMatrix.data[1][2] = -tb_sum / tb_diff;
    projectionMatrix.data[2][3] = -two_n * f / fn_diff;

    return projectionMatrix;
}

Mat4 Mat4_GetScaleMatrix(float s)
{
    Mat4 scaleMatrix = Mat4_Identity;
    for (int i = 0; i < 4; ++i)
        scaleMatrix.data[i][i] = s;

    return scaleMatrix;
}

Mat4 Mat4_GetScale3Matrix(Vec3 v)
{
    Mat4 scaleMatrix = Mat4_Identity;

    scaleMatrix.data[0][0] = v.data[0];
    scaleMatrix.data[1][1] = v.data[1];
    scaleMatrix.data[2][2] = v.data[2];

    return scaleMatrix;
}

Mat4 Mat4_GetTranslationMatrix(Vec3 v)
{
    Mat4 translationMatrix = Mat4_Identity;

    translationMatrix.data[0][3] = v.data[0];
    translationMatrix.data[1][3] = v.data[1];
    translationMatrix.data[2][3] = v.data[2];
    translationMatrix.data[3][3] = 1.f;

    return translationMatrix;
}

Mat4 Mat4_GetXRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[1][1] = c;
    rotationMatrix.data[2][2] = c;

    rotationMatrix.data[1][2] = -s;
    rotationMatrix.data[2][1] = s;

    return rotationMatrix;
}

Mat4 Mat4_GetYRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[0][0] = c;
    rotationMatrix.data[2][2] = c;

    rotationMatrix.data[2][0] = -s;
    rotationMatrix.data[0][2] = s;

    return rotationMatrix;
}

Mat4 Mat4_GetZRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[0][0] = c;
    rotationMatrix.data[1][1] = c;
    rotationMatrix.data[0][1] = -s;
    rotationMatrix.data[1][0] = s;

    return rotationMatrix;
}
