#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "particle.h"
using namespace std;

int main()
{
    cout << "Hello, World!" << endl;
    const int screenWidth = 1200;
    const int screenHeight = 900;
    const int pixelSize = 3;
    const int virtualWidth = screenWidth / pixelSize;
    const int virtualHeight = screenHeight / pixelSize;
    const float virtualRatio = (float)virtualWidth / (float)screenHeight;
    InitWindow(screenWidth, screenHeight, "Cellular Automata!");

    Camera2D worldSpaceCamera = {0};
    worldSpaceCamera.zoom = 1.0f;

    Camera2D screenSpaceCamera = {0};
    screenSpaceCamera.zoom = 1.0f;

    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);

    Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height};
    Rectangle destRec = {-virtualRatio, -virtualRatio, screenWidth + (virtualRatio * 2), screenHeight + (virtualRatio * 2)};

    Vector2 origin = {0.0f, 0.0f};

    ParticleWorld particleWorld(virtualWidth, virtualHeight);

    double previousTime = GetTime();
    double targetFPS = 1000;
    double deltaTime = 0;

    Mat_Type drawType = t_sand;

    while (!WindowShouldClose())
    {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        int virtualMouseX = mouseX / pixelSize;
        int virtualMouseY = mouseY / pixelSize;
        bool click = IsMouseButtonDown(0);

        if (click)
        {
            for (int i = 0; i < 30; i++)
            {
                int r = 10;
                int xO = rand() % r - (r / 2);
                int yO = rand() % r - (r / 2);
                Color drawColor = WHITE;
                switch (drawType)
                {
                case t_air:
                {
                    drawColor = color_air();
                    break;
                }
                case t_sand:
                {
                    drawColor = color_sand();
                    break;
                }
                case t_solid:
                {
                    drawColor = color_solid();
                    break;
                }
                case t_water:
                {
                    drawColor = color_water();
                    break;
                }
                }
                Particle *nP = new Particle{drawType, drawColor};
                particleWorld.SetParticle(Vector2{(float)virtualMouseX + xO, (float)virtualMouseY + yO}, nP);
            }
        }
        if (IsKeyPressed(KEY_A))
        {
            drawType = t_air;
        }
        if (IsKeyPressed(KEY_S))
        {
            drawType = t_sand;
        }
        if (IsKeyPressed(KEY_B))
        {
            drawType = t_solid;
        }
        if (IsKeyPressed(KEY_W))
        {
            drawType = t_water;
        }
        particleWorld.UpdateParticles();
        BeginTextureMode(target);
        {
            ClearBackground(WHITE);
            BeginMode2D(worldSpaceCamera);
            {
                for (int i = 0; i < virtualWidth * virtualHeight; i++)
                {
                    Particle *p = particleWorld.ParticleAtIndex(i);
                    if (p->getType() == t_air)
                        continue;
                    Vector2 v = particleWorld.IndexToCoord(i);
                    int x = v.x;
                    int y = v.y;
                    DrawPixel(x, y, p->getColor());
                }
            }
            EndMode2D();
        }
        EndTextureMode();

        BeginDrawing();
        {
            ClearBackground(RED);
            BeginMode2D(screenSpaceCamera);
            {
                DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
            }
            EndMode2D();

            DrawFPS(GetScreenWidth() - 95, 10);
        }
        EndDrawing();

        double currentTime = GetTime();
        double updateDrawTime = currentTime - previousTime;

        double waitTime = (1.0f / (float)targetFPS) - updateDrawTime;
        if (waitTime > 0.0)
        {
            WaitTime((float)waitTime);
            currentTime = GetTime();
            deltaTime = (float)(currentTime - previousTime);
        }
        previousTime = currentTime;
    }

    UnloadRenderTexture(target);

    CloseWindow();

    return 0;
}