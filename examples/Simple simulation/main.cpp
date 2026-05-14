#include <iostream>
#include "CPPFX.h"

using namespace std;

/**************
 * Simple mockup simulation controlled by a simple UI.
 *************/

int main()
{
    //initialisation, common with raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(0, 0, "Simple GUI test");
    SetTargetFPS(0);
    Camera2D camera;
    camera = {0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    camera.target = (Vector2){0, 0};
    CPPFX::GUI gui;

    //gui initialisation
    auto speedPicker = gui.AddSpinner("SpeedPicker");
    speedPicker->SetX(100);
    speedPicker->SetY(100);
    speedPicker->hasMin = true;
    speedPicker->SetMin(1);
    speedPicker->hasMax = true;
    speedPicker->SetMax(10);
    speedPicker->SetValue(1);

    auto startButton = gui.AddPressedButton("StartButton");
    startButton->pressedColour.SetColour(RED);
    startButton->unPressedColour.SetColour(DARKGREEN);
    startButton->font.colour.SetColour(RAYWHITE);
    startButton->SetText("START");
    startButton->SetPressedText("STOP");
    startButton->SetX(100);
    startButton->SetY(300);

    //sim initialisation
    const float Ox = 1000, Oy = 500, radius = 250;
    float endX = Ox + radius, endY = Oy;
    float dt = 0; //stands upright without a jump

    while (!WindowShouldClose()) {
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(WHITE);

        //calculate line
        if (startButton->pressed) {
            dt += GetFrameTime() * speedPicker->GetValue();
            float angle = fmod(dt * 360.0f, 360.0f);
            endX = Ox + cos(angle * DEG2RAD) * radius;
            endY = Oy + sin(angle * DEG2RAD) * radius;
        }
        //draw line
        DrawLineEx({Ox, Oy}, {endX, endY}, 10, BLACK);

        //draw ui and update it
        gui.DoUI(camera);

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
