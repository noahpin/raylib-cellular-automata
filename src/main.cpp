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
    const int pixelSize = 4;
    const int virtualWidth = screenWidth / pixelSize;
    const int virtualHeight = screenHeight / pixelSize;
    const double virtualRatio = (double)virtualWidth / (double)screenHeight;
    InitWindow(screenWidth, screenHeight, "Cellular Automata!");

    Camera2D worldSpaceCamera = {0};
    worldSpaceCamera.zoom = 1.0f;

    Camera2D screenSpaceCamera = {0};
    screenSpaceCamera.zoom = 1.0f;

    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);

    Rectangle sourceRec = {0.0f, 0.0f, (double)target.texture.width, -(double)target.texture.height};
    Rectangle destRec = {-virtualRatio, -virtualRatio, screenWidth + (virtualRatio * 2), screenHeight + (virtualRatio * 2)};

    Vector2 origin = {0.0f, 0.0f};

    ParticleWorld particleWorld(virtualWidth, virtualHeight);

    double previousTime = GetTime();
    int targetFPS = -10;
    float deltaTime = 0.0f;
    double currentTime = 0.0;
    double updateDrawTime = 0.0;
    double waitTime = 0.0;

    Mat_Type drawType = t_sand;

    while (!WindowShouldClose())
    {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        int virtualMouseX = mouseX / pixelSize;
        int virtualMouseY = mouseY / pixelSize;
        bool click = IsMouseButtonDown(0);

        particleWorld.setDeltaTime(deltaTime);
        particleWorld.setCurrentTime(GetTime());

        if (click)
        {
            for (int i = 0; i < 100; i++)
            {
                int r = 50;
                int xO = rand() % r - ( r / 2);
                int yO = rand() % r - ( r / 2);
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
            DrawText(TextFormat("deltatime: %f", deltaTime), GetScreenWidth() - 220, 70, 20, LIME);
        }
        EndDrawing();

        currentTime = GetTime();
        updateDrawTime = currentTime - previousTime;
        if (targetFPS > 0) // We want a fixed frame rate
        {
            waitTime = (1.0f / (float)targetFPS) - updateDrawTime;
            if (waitTime > 0.0)
            {
                WaitTime((float)waitTime);
                currentTime = GetTime();
                deltaTime = (float)(currentTime - previousTime);
            }
        }
        else
            deltaTime = (float)updateDrawTime;
        previousTime = currentTime;
    }

    UnloadRenderTexture(target);

    CloseWindow();

    return 0;
}