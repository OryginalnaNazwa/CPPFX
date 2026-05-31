#include "CPPFX.h"

/**************
 * Simple mockup simulation controlled by a simple UI.
 * A clock-arm like spins around. It's speed is controlled by a Spinner and the simulation can be stopped with a PressedButton.
 *************/

int main()
{
    //initialisation, common with raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(0, 0, "Simple simulation - clock");
    SetTargetFPS(60);
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
    speedPicker->SetMin();
    speedPicker->SetMinValue(1);
    speedPicker->SetMax();
    speedPicker->SetMaxValue(10);
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
        if (startButton->IsPressed()) {
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
