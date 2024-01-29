#include "Shader.h"
#include "Scene.h"
#include "Graphics.h"
#include "Tools.h"

VShaderOut VertexShader_Base(VShaderIn *in, VShaderGlobals *globals)
{
    // Le vertex shader est une fonction ex�cut�e pour chacun des sommets
    // d'un objet � rendre.
    // Il r�cup�re des donn�es globales � l'objet comme les matrices de conversion.
    // Il se charge principalement de projeter le sommet dans le "clip space"
    // et de convertir la normale/tangente associ�e dans le r�f�rentiel monde.
    // Le vertex shader est ex�cut� pour chacun des sommets d'un objet � rendre.
    VShaderOut out = {0};

    // Coordonn�es homog�nes du sommet (w = 1 car c'est un point)
    Vec4 vertex = Vec4_From3(in->vertex, 1.0f);

    // Coordonnées homogénes de la normale (w = 0 car c'est une direction)
    Vec4 normal = Vec4_From3(in->normal, 0.0f);

    Vec4 tangent = Vec4_From3(in->tangent, 0.0f);

    // Transformation du sommet dans le rep�re cam�ra
    Vec4 vertexCamSpace = Mat4_MulMV(globals->objToView, vertex);

    // Projection du sommet dans le "clip space"
    Vec4 vertexClipSpace = Mat4_MulMV(globals->objToClip, vertex); // OBLIGATOIRE (ne pas modifier)

    // Pour la lumi�re diffuse, il faut transformer la normale dans le rep�re monde
    // Pour le mod�le de Blinn-Phong, il faut calculer la position du sommet
    // dans le r�f�rentiel monde et ajouter l'information au VShaderOut

    // Définit la sortie du vertex shader
    out.clipPos = Vec3_From4(vertexClipSpace);          // OBLIGATOIRE (ne pas modifier)
    out.invDepth = vertexCamSpace.w / vertexCamSpace.z; // OBLIGATOIRE (ne pas modifier)
    out.normal = Vec3_Normalize(Vec3_From4(Mat4_MulMV(globals->objToWorld, normal)));
    out.tangent = Vec3_Normalize(Vec3_From4(Mat4_MulMV(globals->objToWorld, tangent)));
    out.textUV = in->textUV;
    out.worldPos = Vec3_From4(Mat4_MulMV(globals->objToWorld, vertex));
    out.bitangent = Vec3_Cross(out.normal, out.tangent);

    VShaderOut v0 = out;
    VShaderOut v1 = out;
    VShaderOut v2 = out;

    v0.textUV = in->textUV;
    v1.textUV = in->textUV;
    v2.textUV = in->textUV;

    Vec3 edge1 = Vec3_Sub(v1.worldPos, v0.worldPos);
    Vec3 edge2 = Vec3_Sub(v2.worldPos, v0.worldPos);
    Vec2 deltaUV1 = Vec2_Sub(v1.textUV, v0.textUV);
    Vec2 deltaUV2 = Vec2_Sub(v2.textUV, v0.textUV);
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    return out;
}

Vec4 FragmentShader_Base(FShaderIn *in, FShaderGlobals *globals)
{
    // Le fragment shader est une fonction exécutée pour chaque pixel
    // de tous les triangles visibles.
    // Il récupère un fragment qui contient des données interpolées
    // (position, normale, tangente, uv).
    // Il retourne la couleur du pixel.
    //
    // Le vecteur (x,y,z,w) renvoyé par le fragment shader est interprété comme suit :
    // - x : intensité du rouge (entre 0 et 1)
    // - y : intensité du vert  (entre 0 et 1)
    // - z : intensité du bleu  (entre 0 et 1)
    // - w : opacité            (entre 0 et 1)

    // Récupération du matériau associé au pixel (albedo/normal map)
    Material *material = globals->material;
    assert(material);

    // Récupération de la texture (albedo) associée au pixel
    MeshTexture *albedoTex = Material_GetAlbedo(material);
    assert(albedoTex);

    // Récupération des coordonnées (u,v) associée au pixel.
    // Les coordonnées (u,v) sont dans [0,1]^2.
    // - u représente l'abscisse du point sur une texture 2D.
    // - v représente l'ordonnée du point sur une texture 2D.
    // (0,0) représente le coin en bas à gauche.
    // (1,1) représente le coin en haut à droite.
    float u = in->textUV.x;
    float v = in->textUV.y;

    // Recupération de la couleur du pixel dans la texture
    Vec3 albedo = MeshTexture_GetColorVec3(albedoTex, Vec2_Set(u, v));

    // Récupère les lumières de la scène
    Vec3 lightVector = Scene_GetLight(globals->scene);
    Vec3 lightColor = Scene_GetLightColor(globals->scene);
    Vec3 ambiant = Scene_GetAmbiantColor(globals->scene);

    // Récupère la normale interpolée (non normalisée)
    Vec3 normal = in->normal;

    // Application de la lumière ambiante à l'albedo
    albedo = Vec3_Mul(albedo, ambiant);
    float lambert = Vec3_Dot(normal, lightVector);
    lambert = Float_Clamp01(lambert);
    Vec3 lightScale = Vec3_Scale(lightColor, lambert);
    Vec3 lightTotal = Vec3_Add(lightScale, ambiant);
    albedo = Vec3_Mul(albedo, lightTotal);
    albedo = Vec3_Scale(albedo, 3.0f);
    // chaufe un peu la couleur
    albedo = Vec3_Set(albedo.x * 1.25f, albedo.y * 1.15f, albedo.z * 1.15f);

    return Vec4_From3(albedo, 1.0f);
}

// Tableau de hachage pour le bruit de Perlin
static int hash[] = {8, 4, 12, 6, 1, 9, 5, 13, 2, 10, 6, 14, 3, 11, 7, 15, 0, 8, 4, 12, 6, 1, 9, 5, 13, 2, 10, 6, 14, 3, 11, 7, 15};

// Fonction de hachage pour le bruit de Perlin
int hashFunction(int x, int y)
{
    return hash[(hash[x % 32] + y) % 32];
}

static float cache[1024][1024];
int cacheInitialized = 0;

// Fonction rapide pour la génération de nombres pseudo-aléatoires
unsigned int fastRandom()
{
    static unsigned int seed = 1337;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return seed;
}

// Version optimisée du bruit de Perlin avec mise en cache
float simplifiedPerlinNoise(float x, float y)
{
    float x0 = floorf(x);
    float x1 = x0 + 1.0f;
    float y0 = floorf(y);
    float y1 = y0 + 1.0f;

    if (!cacheInitialized)
    {
        for (int i = 0; i < 1024; i++)
        {
            for (int j = 0; j < 1024; j++)
            {
                cache[i][j] = -1.0f;
            }
        }
        cacheInitialized = 1;
    }

    int cacheX0 = (int)x0 & 1023;
    int cacheY0 = (int)y0 & 1023;

    if (cache[cacheX0][cacheY0] != -1.0f)
        return cache[cacheX0][cacheY0];

    float sx = x - (float)x0 * (3.0 - 2.0 * x + (float)x0);
    float sy = y - (float)y0 * (3.0 - 2.0 * y + (float)y0);
    float n0, n1, ix0, ix1, value;

    n0 = hashFunction(x0, y0) / 31.0f;
    n1 = hashFunction(x1, y0) / 31.0f;
    ix0 = Float_Lerp(n0, n1, sx);

    n0 = hashFunction(x0, y1) / 31.0f;
    n1 = hashFunction(x1, y1) / 31.0f;
    ix1 = Float_Lerp(n0, n1, sx);

    value = Float_Lerp(ix0, ix1, sy);
    cache[cacheX0][cacheY0] = value;
    return value;
}

Vec4 FragmentShader_CelShading(FShaderIn *in, FShaderGlobals *globals)
{
    Material *material = globals->material;
    assert(material);

    MeshTexture *albedoTex = Material_GetAlbedo(material);
    assert(albedoTex);

    float u = in->textUV.x;
    float v = in->textUV.y;

    Vec3 albedo = MeshTexture_GetColorVec3(albedoTex, Vec2_Set(u, v));
    Vec3 lightVector = Scene_GetLight(globals->scene);
    Vec3 lightColor = Scene_GetLightColor(globals->scene);
    Vec3 ambiant = Scene_GetAmbiantColor(globals->scene);
    Vec3 normal = in->normal;
    normal = Vec3_Normalize(normal);

    float lambert = Vec3_Dot(normal, lightVector);
    lambert = Float_Clamp01(lambert);

    float lowerLimit = 0.5f;
    float upperLimit = 0.55f;
    float lowerValue = 0.3f;
    float upperValue = 0.7f;
    float contourThreshold = 0.5f; // Valeur pour déterminer les contours
    float contourWidth = 0.05f;    // Largeur des contours
    float diffuseIntensity = 1.0f; // Intensité de la couleur diffuse

    if (lambert < lowerLimit)
        lambert = lowerValue;
    else if (lambert > upperLimit)
        lambert = upperValue;
    else
        lambert = Float_Lerp(lowerValue, upperValue, (lambert - lowerLimit) / (upperLimit - lowerLimit));

    // Ajouter des contours
    float contour = Float_Clamp01((lambert - contourThreshold) / contourWidth);
    contour = Float_Lerp(1.0f, 0.0f, contour);

    // Ajouter variations subtiles de couleur
    Vec3 colorVariation = Vec3_Set(0.0f, 0.0f, 0.0f);
    float colorVariationIntensity = 0.1f;
    colorVariation = Vec3_Set(colorVariationIntensity, colorVariationIntensity, colorVariationIntensity);
    colorVariation = Vec3_Mul(colorVariation, Vec3_Set((float)fastRandom() / RAND_MAX, (float)fastRandom() / RAND_MAX, (float)fastRandom() / RAND_MAX));

    Vec3 lightScale = Vec3_Scale(lightColor, lambert);
    Vec3 lightTotal = Vec3_Add(lightScale, ambiant);

    float perlinNoise = simplifiedPerlinNoise(u, v);
    perlinNoise = perlinNoise * 0.5;

    Vec3 noise = Vec3_Set(perlinNoise, perlinNoise, perlinNoise);

    // Mélange le bruit de Perlin avec la couleur de base
    albedo = Vec3_Add(albedo, noise);
    albedo = Vec3_Mul(albedo, lightTotal);
    return Vec4_From3(albedo, 1.0f);
}

Vec4 FragmentShader_Blinn_Phong(FShaderIn *in, FShaderGlobals *globals)
{
    // Le fragment shader est une fonction ex�cut�e pour chaque pixel
    // de tous les triangles visibles.
    // Il r�cup�re un fragment qui contient des donn�es interpol�es
    // (position, normale, tangente, uv).
    // Il retourne la couleur du pixel.
    //
    // Le vecteur (x,y,z,w) renvoy� par le fragment shader est interpr�t� comme suit :
    // - x : intensit� du rouge (entre 0 et 1)
    // - y : intensit� du vert  (entre 0 et 1)
    // - z : intensit� du bleu  (entre 0 et 1)
    // - w : opacit�            (entre 0 et 1)

    // R�cup�ration du mat�riau associ� au pixel (albedo/normal map)
    Material *material = globals->material;
    assert(material);

    // R�cup�ration de la texture (albedo) associ�e au pixel
    MeshTexture *albedoTex = Material_GetAlbedo(material);
    // MeshTexture* normalTex = Material_GetNormalMap(material);
    assert(albedoTex);
    // assert(normalTex);

    // R�cup�ration des coordonn�es (u,v) associ�e au pixel.
    // Les coordonn�es (u,v) sont dans [0,1]^2.
    // - u repr�sente l'abscisse du point sur une texture 2D.
    // - v repr�sente l'ordonn�e du point sur une texture 2D.
    // (0,0) repr�sente le coin en bas � gauche.
    // (1,1) repr�sente le coin en haut � droite.
    float u = in->textUV.x;
    float v = in->textUV.y;

    // Recup�ration de la couleur du pixel dans la texture
    Vec3 albedo = MeshTexture_GetColorVec3(albedoTex, Vec2_Set(u, v));
    // Vec3 normal = MeshTexture_GetColorVec3(normalTex, Vec2_Set(u, v));

    // R�cup�re les lumi�res de la sc�ne
    Vec3 lightVector = Scene_GetLight(globals->scene);
    Vec3 lightColor = Scene_GetLightColor(globals->scene);
    Vec3 ambiant = Scene_GetAmbiantColor(globals->scene);

    // R�cup�re la normale interpol�e (non normalis�e)
    Vec3 normal = in->normal;
    normal = Vec3_Normalize(normal);
    Vec3 camera = globals->cameraPos;

    // Application de la lumi�re ambiante � l'albedo
    // albedo = Vec3_Mul(albedo, ambiant);

    // Pour la lumi�re diffuse, il faut utiliser la normale.
    float lambert = Float_Clamp01(Vec3_Dot(normal, lightVector));
    Vec3 lightScale = Vec3_Scale(lightColor, lambert);
    Vec3 lightTotal = Vec3_Add(lightScale, ambiant);

    // Pour la lumi�re sp�culaire de Blinn-Phong, il faut :
    Vec3 view = Vec3_Normalize(Vec3_Sub(camera, in->worldPos));
    Vec3 half = Vec3_Normalize(Vec3_Add(lightVector, view));
    Vec3 reflect = Vec3_Sub(lightVector, Vec3_Scale(normal, -2.0f * Vec3_Dot(lightVector, normal)));

    float specular = Vec3_Dot(half, normal);
    specular = powf(specular, 50);
    Vec3 specularScale = Vec3_Scale(lightColor, specular);
    // lightTotal = Vec3_Add(lightTotal, specularColor);

    // - r�cup�rer l'interpolation de la position dans le monde du pixel ;
    // - calculer le vecteur de vue, le vecteur moiti�
    // Vous devez donc modifier le vertex shader, puis modifier la fonction
    // Graphics_RenderTriangle() pour initialiser l'interpolation puis pour
    // calculer l'interpolation.
    // Utilisez les macros VEC3_INIT_INTERPOLATION() et VEC3_INTERPOLATE().

    //.............................................................................................
    // Quelques exemples de debug (� d�commenter)

    // Debug : couleur unique en RGBA (bleu ESIEA)
    // return Vec4_Set(0.21f, 0.66f, 0.88f, 1.0f);

    // Debug : coordonn�es (uv) de texture
    // return Vec4_From2(in->textUV, 0.0, 1.0f);

    // Debug : normales
    // return Vec4_From3(in->normal, 1.0f);

    // Debug : normales V2
    // return Vec4_From3(Vec3_Scale(Vec3_Add(in->normal, Vec3_One), 0.5f), 1.0f);
    //.............................................................................................

    // Retourne la couleur (albedo) associ�e au pixel dans la texture.

    albedo = Vec3_Mul(albedo, lightTotal); // Ajout de l'�clairage ambiant et diffuse
    // Vec3 specular = Vec3_Set(0.0f, 0.0f, 0.0f);
    albedo = Vec3_Abs(albedo, specularScale); // Ajout de la lumi�re sp�culaire

    return Vec4_From3(albedo, 1.0f);
}

Vec4 FragmentShader_NormalMap(FShaderIn *in, FShaderGlobals *globals)
{
    // Le fragment shader est une fonction exécutée pour chaque pixel
    // de tous les triangles visibles.
    // Il récupère un fragment qui contient des données interpolées
    // (position, normale, tangente, uv).
    // Il retourne la couleur du pixel.

    // Récupération du matériau associé au pixel (albedo/normal map)
    Material *currentMaterial = globals->material;
    assert(currentMaterial);

    // Récupération de la texture (albedo) associée au pixel
    MeshTexture *albedoTexture = Material_GetAlbedo(currentMaterial);
    assert(albedoTexture);

    float textureCoordinateU = in->textUV.x;
    float textureCoordinateV = in->textUV.y;

    Vec3 albedoColor = MeshTexture_GetColorVec3(albedoTexture, Vec2_Set(textureCoordinateU, textureCoordinateV));
    Vec3 surfaceNormal = in->normal;
    NormalMap *normalMapTexture = Material_GetNormalMap(currentMaterial);

    if (normalMapTexture)
    {
        // Recupération de la couleur du pixel dans la texture
        Vec3 bitangentVector = Vec3_Cross(in->normal, in->tangent);
        Vec3 normalMapColor = NormalMap_GetNormalVec3(normalMapTexture, Vec2_Set(textureCoordinateU, textureCoordinateV)); // valeur entre -1 et 1

        // Recalcul de la normal en fonction des données de la normal map
        Vec3 tangentNormal = Vec3_Scale(in->tangent, normalMapColor.x);
        Vec3 bitangentNormal = Vec3_Scale(bitangentVector, normalMapColor.y);
        Vec3 normalNormal = Vec3_Scale(in->normal, normalMapColor.z);

        // Recombinaison des composantes pour obtenir la nouvelle normal
        surfaceNormal = Vec3_Add(Vec3_Add(tangentNormal, bitangentNormal), normalNormal);
    }

    surfaceNormal = Vec3_Normalize(surfaceNormal);
    surfaceNormal = Vec3_Scale(surfaceNormal, 1.7f); // intensité de la normal map à ajuster en % (ici 50% bonus)

    // Récupération des informations de la scène
    Vec3 lightDirection = Scene_GetLight(globals->scene);
    Vec3 lightColor = Scene_GetLightColor(globals->scene);
    Vec3 ambientLightColor = Scene_GetAmbiantColor(globals->scene);
    Vec3 cameraPosition = globals->cameraPos;

    // Calcul de la reflection (diffuse)
    float diffuseReflectance = Float_Clamp01(Vec3_Dot(surfaceNormal, lightDirection));
    Vec3 diffuseLightColor = Vec3_Scale(lightColor, diffuseReflectance);
    Vec3 totalLightColor = Vec3_Add(diffuseLightColor, ambientLightColor);

    // Calcul de la reflection spéculaire (Blinn-Phong)
    Vec3 viewDirection = Vec3_Normalize(Vec3_Sub(cameraPosition, in->worldPos));
    Vec3 halfVector = Vec3_Normalize(Vec3_Add(lightDirection, viewDirection));
    float specularReflectance = Vec3_Dot(halfVector, surfaceNormal);
    specularReflectance = powf(specularReflectance, 50);
    Vec3 specularLightColor = Vec3_Scale(lightColor, specularReflectance);

    // Application de l'éclairage à la couleur de l'albedo
    Vec3 finalColor = Vec3_Mul(albedoColor, totalLightColor); // Ajout de l'éclairage ambiant et diffuse
    finalColor = Vec3_Abs(finalColor, specularLightColor);    // Ajout de la lumière spéculaire
    finalColor = Vec3_Scale(finalColor, 0.7f);

    return Vec4_From3(finalColor, 1.0f);
}

Vec4 FragmentShader_PerlinNoise(FShaderIn *in, FShaderGlobals *globals)
{
    Material *material = globals->material;
    assert(material);

    // Récupération de la texture (albedo) associée au pixel
    MeshTexture *albedoTex = Material_GetAlbedo(material);
    assert(albedoTex);
    float u = in->textUV.x;
    float v = in->textUV.y;

    Vec3 albedo = MeshTexture_GetColorVec3(albedoTex, Vec2_Set(u, v));

    // Récupère les lumières de la scène
    Vec3 lightVector = Scene_GetLight(globals->scene);
    Vec3 lightColor = Scene_GetLightColor(globals->scene);
    Vec3 ambiant = Scene_GetAmbiantColor(globals->scene);

    // Récupère la normale interpolée (non normalisée)
    Vec3 normal = in->normal;

    // Application de la lumière ambiante à l'albedo
    albedo = Vec3_Mul(albedo, ambiant);

    // Utilisez la fonction simplifiedPerlinNoise pour obtenir une valeur de bruit de Perlin
    float noiseValue = simplifiedPerlinNoise(u * 10.0f, v * 10.0f); // Adjust frequency for more or less randomness

    // Utiliser le bruit de Perlin pour contrôler la visibilité de la texture
    float threshold = 0.5f;
    if (noiseValue > threshold)
    {
        Vec3 noise = Vec3_Set(noiseValue, noiseValue, noiseValue);

        // Mélange le bruit de Perlin avec la couleur de base
        albedo = Vec3_Mul(albedo, noise);
        albedo = Vec3_Mul(albedo, lightColor);
    }
    else
    {
        // Masquer la texture en dessous du seuil
        albedo = Vec3_Set(0.0f, 0.0f, 0.0f);
    }

    // Convertir Vec3 en Vec4 et renvoyer
    return Vec4_From3(albedo, 1.0f);
}
