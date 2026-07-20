#include "CPPFX.hpp"
#include <cmath> // for fmod

/*****************************************************************************************************************************
 * Simple mockup simulation controlled by a simple UI. v2
 * A clock-arm like spins around. It's speed is controlled by a Spinner and the simulation can be stopped with a PressedButton.
 * ===========================================================================================================================
 *  v2.1 - small fixes.
 *  v2 - switched to Line for the clock.
 ******************************************************************************************************************************/

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
    speedPicker->SetXY(100.0f);
    speedPicker->SetMin(); // turns on the lower bound
    speedPicker->SetMinValue(1.0f); // sets it
    speedPicker->SetMax(); // the same but the other side
    speedPicker->SetMaxValue(10.0f);
    speedPicker->SetValue(1.0f); // sets the current value

    auto startButton = gui.AddPressedButton("StartButton");
    startButton->pressedColour.SetColour(RED);
    startButton->colour.SetColour(DARKGREEN);
    startButton->font.colour.SetColour(RAYWHITE);
    startButton->SetText("START");
    startButton->SetPressedText("STOP");
    startButton->SetX(100.0f);
    startButton->SetY(300.0f);

    auto arm = gui.AddLine("Arm"); // simple shape
    arm->DrawLengthAndAngle(); // sets drawing based on length and angle, rather than from point to point
    arm->SetLength(250.0f);
    arm->SetXY(1000.0f, 500.0f);
    arm->colour.SetColour(BLACK);

    float dt = 0.0f; // delta time

    while (!WindowShouldClose()) {
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(WHITE);

        //calculate line
        if (startButton->IsPressed()) { // easier than doing it in callback
            dt += GetFrameTime() * speedPicker->GetValue(); // time of move
            float angle = fmod(dt * 360.0f, 360.0f); // calculate angle based on that
            arm->SetAngle(angle);
        }

        //draw ui and update it
        gui.DoUI(camera);

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
