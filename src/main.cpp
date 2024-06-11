#include <raylib.h>
#include <raymath.h>
#include <iostream>
using namespace std;

int main() {
std::cout << "Hello, World!" << std::endl; 
    InitWindow(400, 224, "Cellular Automata!");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
    
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}