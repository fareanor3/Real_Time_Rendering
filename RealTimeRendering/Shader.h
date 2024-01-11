#ifndef _SHADER_H_
#define _SHADER_H_

#include "Settings.h"
#include "Vector.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Material.h"

typedef struct Scene_s Scene;

/// @brief Structure repr�sentant les donn�es globales fournies au vertex shader.
typedef struct VShaderGlobals_s
{
    Vec3 cameraPos;
    Mat4 objToWorld;
    Mat4 viewToWorld;
    Mat4 objToView;
    Mat4 objToClip;
} VShaderGlobals;

/// @brief Structure repr�sentant les donn�es d'un sommet fournies au vertex shader.
typedef struct VShaderIn_s
{
    /// @brief Position du sommet dans le r�f�rentiel objet.
    Vec3 vertex;

    /// @brief Position de la normale associ�e au sommet dans le r�f�rentiel objet.
    Vec3 normal;

    /// @brief Position de la tangente associ�e au sommet dans le r�f�rentiel objet.
    Vec3 tangent;

    //J'ai rajouté ça pour les bonus
    Vec3 bitangent;

    /// @brief Coordonn�es uv associ�es au sommet.
    Vec2 textUV;
} VShaderIn;

/// @brief Structure repr�sentant la sortie du vertex shader
typedef struct VShaderOut_s
{
    /// @brief Position du sommet apr�s la projection en perspective.
    /// Les coordonn�es des points visibles sont entre -1.0f et 1.0f (sur chaque axe).
    Vec3 clipPos;

    /// @brief Inverse de la coordonn�e z du sommet dans le r�f�rentiel cam�ra.
    float invDepth;

    /// @brief Normale associ�e au sommet exprim�e dans le r�f�rentiel monde.
    Vec3 normal;

    /// @brief Coordonn�es uv associ�es au sommet.
    Vec2 textUV;

    // TODO
    // Pour les bonus

    /// @brief Position du sommet dans le r�f�rentiel monde.
    Vec3 worldPos;

    /// @brief Tangente associ�e au sommet exprim�e dans le r�f�rentiel monde.
    Vec3  tangent;

    /// @brief Bitangente associ�e au sommet exprim�e dans le r�f�rentiel monde.
    Vec3 bitangent;

} VShaderOut;

/// @brief Structure repr�sentant les donn�es globales fournies au fragment shader.
typedef struct FShaderGlobals_s
{
    /// @brief Pointeur vers la sc�ne.
    Scene *scene;

    /// @brief Mat�riau utilis� pour le triangle.
    Material *material;

    /// @brief Position de la cam�ra.
    Vec3 cameraPos;
} FShaderGlobals;

/// @brief Structure repr�sentant les donn�es associ�e � un pixel (fragment)
/// fournies au fragment shader.
/// Elle sont obtenues par interpolation des sorties du vertex shader.
typedef struct FShaderIn_s
{
    /// @brief Normale associ�e au pixel exprim�e dans le r�f�rentiel monde.
    /// Le vecteur est obtenu par interpolation, donc n'est pas n�cessairement unitaire.
    Vec3 normal;

    /// @brief Coordonn�es uv associ�es au pixel.
    Vec2 textUV;

    // TODO
    // Pour les bonus

    /// @brief Position associ�e au pixel exprim�e dans le r�f�rentiel monde.
    /// N�cessaire pour la lumi�re sp�culaire de Blinn-Phong
    Vec3 worldPos;

    /// @brief Tangente associ�e au pixel exprim�e dans le r�f�rentiel monde.
    /// N�cessaire pour l'utilisation d'une "normal map".
    /// Le vecteur est obtenu par interpolation, donc n'est pas n�cessairement unitaire
    /// ou perpendiculaire � la normale.
    Vec3 tangent;
} FShaderIn;

typedef VShaderOut VertexShader(VShaderIn *in, VShaderGlobals *globals);
typedef Vec4 FragmentShader(FShaderIn *in, FShaderGlobals *globals);

VShaderOut VertexShader_Base(VShaderIn *in, VShaderGlobals *globals);

Vec4 FragmentShader_Base(FShaderIn *in, FShaderGlobals *globals);
Vec4 FragmentShader_CelShading(FShaderIn *in, FShaderGlobals *globals);
Vec4 FragmentShader_Blinn_Phong(FShaderIn *in, FShaderGlobals *globals);
Vec4 FragmentShader_NormalMap(FShaderIn *in, FShaderGlobals *globals);

#endif
