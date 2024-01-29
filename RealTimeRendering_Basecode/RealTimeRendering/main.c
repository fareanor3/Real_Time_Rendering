#include "Settings.h"
#include "Window.h"
#include "Camera.h"
#include "Matrix.h"
#include "Vector.h"
#include "Scene.h"
#include "Graphics.h"
#include "Renderer.h"
#include "Shader.h"
#include "Tools.h"
#include "Mesh.h"
#include "Material.h"

int main(int argc, char *argv[])
{
    Window *window = NULL;
    Renderer *renderer = NULL;
    Scene *scene = NULL;
    Mesh *mesh = NULL;

    system("dir");

    // Initialise la SDL et crée la fenêtre
    int exitStatus = Settings_InitSDL();
    if (exitStatus != EXIT_SUCCESS)
        goto ERROR_LABEL;

    window = Window_New();
    if (!window)
        goto ERROR_LABEL;

    renderer = Window_getRenderer(window);

    g_time = Timer_New();
    if (!g_time)
        goto ERROR_LABEL;

    // Crée la scène
    scene = Scene_New(window);
    if (!scene)
        goto ERROR_LABEL;

    mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/Jaxy", "Jaxy.obj");
    // mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/CaptainToad", "CaptainToad.obj");
    if (!mesh)
        goto ERROR_LABEL;

    // Arbre de scène
    Object *root = Scene_GetRoot(scene);
    Camera *camera = Scene_GetCamera(scene);

    // Crée l'objet
    Object *object = Scene_CreateObject(scene, sizeof(Object));
    if (!object)
        goto ERROR_LABEL;

    exitStatus = Object_Init(object, scene, Mat4_Identity, root);
    if (exitStatus != EXIT_SUCCESS)
        goto ERROR_LABEL;

    Object_SetMesh(object, mesh);

    // Calcule une échelle normalisée pour l'objet
    Vec3 meshCenter = mesh->m_center;
    Vec3 meshMin = mesh->m_min;
    Vec3 meshMax = mesh->m_max;
    float xSize = fabsf(meshMax.x - meshMin.x);
    float ySize = fabsf(meshMax.y - meshMin.y);
    float zSize = fabsf(meshMax.z - meshMin.z);
    float objectSize = fmaxf(xSize, fmaxf(ySize, zSize));
    float scale = 3.0f / objectSize;

    // Centre l'objet en (0,0,0) et applique l'échelle
    Mat4 objectTransform = Mat4_Identity;
    objectTransform = Mat4_GetTranslationMatrix(Vec3_Neg(mesh->m_center));
    objectTransform = Mat4_MulMM(Mat4_GetScaleMatrix(scale), objectTransform);
    Object_SetLocalTransform(object, objectTransform);

    // Lancement du temps global
    Timer_Start(g_time);

    // Paramètre initiaux de la caméra
    float camDistance = 7.3f;
    float angleY = 0.0f;
    float angleX = 0.0f;
    float angleZ = 0.0f;
    float coeffRotate = 0.0f;

    // Crée une annimation de la caméra
    int zoomFrames = 100;                                              // Nombre de frames pour l'animation de zoom
    float startDistance = 20.0f;                                       // Distance de départ
    float endDistance = 7.3f;                                          // Distance finale
    int currentFrame = 0;                                              // frame actuelle
    float rotationSpeedY = 9.9f;                                       // Vitesse de rotation
    Vec3 lightChange = Vec3_Set(0.01f, 0.01f, 0.01f);                  // Changement de lumière
    Vec3 maxLight = Vec3_Set(8.0f, 8.0f, 8.0f);                        // Lumière maximale
    Scene_SetLightColor(scene, Vec3_Set(0.0f, 0.0f, 0.0f));            // Lumierre à 0
    Vec3 currentLight = Scene_GetLightColor(scene);                    // Lumière actuelle
    Scene_SetDefaultFragmentShader(scene, FragmentShader_Blinn_Phong); // Shader de base pour que la lumière soit visible
    float oldAngleY = angleY;                                          // Ancien angle de rotation

    float fpsAccu = 0.0f;
    int frameCount = 0;
    float LumAngle = 0.0f;
    bool quit = false;
    bool turn = false;
    bool lumrotation = false;
    bool lCtrl = false;
    bool MeshToad = false;

    while (!quit)
    {
        // annimation de zoom
        if (currentFrame < zoomFrames)
        {
            camDistance = startDistance - (startDistance - endDistance) * ((float)currentFrame / zoomFrames);
            currentFrame++;
            angleY += rotationSpeedY;
            if (angleY > 2 * M_PI) // Si l'angle dépasse 360 degrés (2*PI radians), on le ramène à 0
            {
                angleY -= 2 * M_PI;
            }
            currentLight = Vec3_Add(currentLight, lightChange);
            Scene_SetLightColor(scene, currentLight);
        }

        SDL_Event evt;

        // Met à jour le temps global
        Timer_Update(g_time);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
            case SDL_KEYDOWN:
                if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    quit = true;
                    break;
                }

                if (evt.key.repeat)
                    break;

                switch (evt.key.keysym.scancode)
                {
                case SDL_SCANCODE_Z:
                    Scene_SetWireframe(scene, !Scene_GetWireframe(scene));
                    break;

                case SDL_SCANCODE_B:
                    Scene_SetDefaultFragmentShader(scene, FragmentShader_Blinn_Phong);
                    break;

                case SDL_SCANCODE_M: // sur azeerty, "," qui est utilisée
                    Scene_SetDefaultFragmentShader(scene, FragmentShader_NormalMap);
                    break;

                case SDL_SCANCODE_T:
                    Scene_SetDefaultFragmentShader(scene, FragmentShader_CelShading);
                    break;

                case SDL_SCANCODE_N:
                    Scene_SetDefaultFragmentShader(scene, FragmentShader_Base);
                    break;

                case SDL_SCANCODE_P:
                    Scene_SetDefaultFragmentShader(scene, FragmentShader_PerlinNoise);
                    break;

                case SDL_SCANCODE_L:
                    lumrotation = !lumrotation;
                    break;

                case SDL_SCANCODE_SEMICOLON:
                    if (!MeshToad)
                    {
                        mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/Jaxy", "Jaxy.obj");
                        MeshToad = !MeshToad;
                    }
                    else
                    {
                        mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/CaptainToad", "CaptainToad.obj");
                        MeshToad = !MeshToad;
                    }

                    if (!mesh)
                        goto ERROR_LABEL;
                    Vec3 meshCenter = mesh->m_center;
                    Vec3 meshMin = mesh->m_min;
                    Vec3 meshMax = mesh->m_max;
                    float xSize = fabsf(meshMax.x - meshMin.x);
                    float ySize = fabsf(meshMax.y - meshMin.y);
                    float zSize = fabsf(meshMax.z - meshMin.z);
                    float objectSize = fmaxf(xSize, fmaxf(ySize, zSize));
                    if (!MeshToad)
                        scale = 3.0f / objectSize;
                    else
                        scale = 9.0f;

                    // Centre l'objet en (0,0,0) et applique l'échelle
                    Mat4 objectTransform = Mat4_Identity;
                    objectTransform = Mat4_GetTranslationMatrix(Vec3_Neg(mesh->m_center));
                    objectTransform = Mat4_MulMM(Mat4_GetScaleMatrix(scale), objectTransform);
                    Object_SetLocalTransform(object, objectTransform);
                    Object_SetMesh(object, mesh);
                    break;

                case SDL_SCANCODE_SPACE:
                    turn = !turn;
                    break;

                case SDL_SCANCODE_LCTRL:
                    lCtrl = true;
                    break;

                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_DOWN:
                    printf(evt.key.keysym.scancode == SDL_SCANCODE_UP ? "up" : "down");
                    Vec3 currentLight = Scene_GetLightColor(scene);
                    currentLight = evt.key.keysym.scancode == SDL_SCANCODE_UP ? Vec3_Add(currentLight, lightChange) : Vec3_Sub(currentLight, lightChange);
                    Scene_SetLightColor(scene, currentLight);
                    break;

                default:
                    break;
                }
                break;

            case SDL_MOUSEMOTION:
                if (evt.motion.state & SDL_BUTTON_LMASK)
                {
                    angleY = fmodf(angleY - evt.motion.xrel * 0.25f, 360.0f);
                    if (angleY < -180.0f)
                        angleY += 360.0f;
                    coeffRotate = ((180.0f - fabsf(angleY)) / 90.0f) - 1.0f;
                    angleX = Float_Clamp(angleX - evt.motion.yrel * 0.25f, -90, 90);
                    angleZ = (angleY >= 0.0f) ? -angleX : angleX;
                }
                if (evt.motion.state & SDL_BUTTON_RMASK)
                    Scene_SetLightDirection(scene, Vec3_Set(-evt.motion.xrel * 0.1f, evt.motion.yrel * 0.1f, 0.0f));
                break;

            case SDL_MOUSEWHEEL:
                camDistance -= evt.wheel.y * 0.25f;
                break;

            case SDL_KEYUP:
                if (evt.key.keysym.scancode == SDL_SCANCODE_LCTRL)
                    lCtrl = false;
                break;

            default:
                break;
            }
        }
        // Calcule la rotation de la caméra
        if (turn)
            angleY -= 360.f / 10.f * Timer_GetDelta(g_time);

        if (lumrotation)
        {
            // fait tourner la lumière au tour de l'objet
            Scene_SetLightDirection(scene, Vec3_Set(cosf(LumAngle), sinf(LumAngle), 0.0f));
            LumAngle += 360.f / 700.f * Timer_GetDelta(g_time);
        }

        // Calcule la matrice locale de la caméra
        Mat4 cameraModel = Mat4_Identity;
        cameraModel = Mat4_MulMM(
            Mat4_GetTranslationMatrix(Vec3_Set(0.f, 0.f, camDistance)),
            cameraModel);

        cameraModel = Mat4_MulMM(
            Mat4_GetYRotationMatrix(angleY),
            cameraModel);

        cameraModel = Mat4_MulMM(
            Mat4_GetXRotationMatrix(angleX * coeffRotate),
            cameraModel);

        cameraModel = Mat4_MulMM(
            Mat4_GetZRotationMatrix(angleZ * (1 - fabsf(coeffRotate))),
            cameraModel);

        // Applique la matrice locale de la caméra
        Object_SetTransform(((Object *)camera), Scene_GetRoot(scene), cameraModel);

        // Calcule le rendu de la scène dans un buffer
        Scene_Render(scene);

        // Met à jour le rendu (affiche le buffer précédent)
        Renderer_Update(renderer);

        // Calcule les FPS
        fpsAccu += Timer_GetDelta(g_time);
        frameCount++;
        if (fpsAccu > 1.0f)
        {
            printf("FPS = %.1f\n", (float)frameCount / fpsAccu);
            fpsAccu = 0.0f;
            frameCount = 0;
        }
    }

    Scene_Free(scene);
    Timer_Free(g_time);
    Window_Free(window);

    Settings_QuitSDL();

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - main()\n");
    assert(false);
    Scene_Free(scene);
    Timer_Free(g_time);
    Window_Free(window);
    return EXIT_FAILURE;
}