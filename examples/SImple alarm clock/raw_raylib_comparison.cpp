#include "raylib.h"
#include <string>

int main() {
    const int screenWidth = 600;
    const int screenHeight = 400;

    InitWindow(screenWidth, screenHeight, "Alarm Timer");
    SetTargetFPS(60);

    int minutes = 0;
    int hours = 0;

    // Button dimensions
    int btnWidth = 80;
    int btnHeight = 50;
    int spacing = 10;

    // Button positions (centered layout)
    int startY = 150;
    int centerX = screenWidth / 2;

    bool isRunning = false;

    int frame = 0;

    // Plus buttons
    Rectangle btnPlus1Min = {centerX - btnWidth*2 - spacing*1.5f, startY, btnWidth, btnHeight};
    Rectangle btnPlus5Min = {centerX - btnWidth - spacing*0.5f, startY, btnWidth, btnHeight};
    Rectangle btnPlus10Min = {centerX + spacing*0.5f, startY, btnWidth, btnHeight};
    Rectangle btnPlus1Hour = {centerX + btnWidth + spacing*1.5f, startY, btnWidth, btnHeight};

    // Minus buttons
    Rectangle btnMinus1Min = {centerX - btnWidth*2 - spacing*1.5f, startY + btnHeight + spacing, btnWidth, btnHeight};
    Rectangle btnMinus5Min = {centerX - btnWidth - spacing*0.5f, startY + btnHeight + spacing, btnWidth, btnHeight};
    Rectangle btnMinus10Min = {centerX + spacing*0.5f, startY + btnHeight + spacing, btnWidth, btnHeight};
    Rectangle btnMinus1Hour = {centerX + btnWidth + spacing*1.5f, startY + btnHeight + spacing, btnWidth, btnHeight};

    // Enter button
    Rectangle btnEnter = {centerX - 100, startY + (btnHeight + spacing) * 2 + 20, 200, 60};

    InitAudioDevice();
    Music fxAlarm = LoadMusicStream("alarm.wav");

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // Plus button logic
        if (mousePressed && CheckCollisionPointRec(mousePos, btnPlus1Min)) {
            minutes += 1;
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnPlus5Min)) {
            minutes += 5;
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnPlus10Min)) {
            minutes += 10;
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnPlus1Hour)) {
            hours += 1;
        }

        // Minus button logic
        if (mousePressed && CheckCollisionPointRec(mousePos, btnMinus1Min)) {
            minutes -= 1;
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnMinus5Min)) {
            minutes -= 5;
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnMinus10Min)) {
            minutes -= 10;
        }
        if (mousePressed && CheckCollisionPointRec(mousePos, btnMinus1Hour)) {
            hours -= 1;
        }

        if (isRunning) {
            frame++;

            if (frame >= 3600) {  // One minute at 60 FPS
                frame = 0;
                minutes--;

                if (minutes < 0 && hours > 0) {
                    hours--;
                    minutes = 59;
                }
            }

            if (hours == 0 && minutes == 0 && !IsMusicStreamPlaying(fxAlarm)) {
                PlayMusicStream(fxAlarm);
            }

            if (IsMusicStreamPlaying(fxAlarm)) {
                UpdateMusicStream(fxAlarm);
            }
        }

        // Normalize time (handle overflow/underflow)
        if (minutes >= 60) {
            hours += minutes / 60;
            minutes = minutes % 60;
        }
        if (minutes < 0) {
            hours -= 1;
            minutes += 60;
        }
        if (hours < 0) {
            hours = 0;
            minutes = 0;
        }

        // Enter button logic
        if (mousePressed && CheckCollisionPointRec(mousePos, btnEnter)) {
            isRunning = !isRunning;
            StopMusicStream(fxAlarm);
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Display time
        std::string timeStr = std::to_string(hours) + "h " + std::to_string(minutes) + "m";
        int textWidth = MeasureText(timeStr.c_str(), 40);
        DrawText(timeStr.c_str(), centerX - textWidth/2, 60, 40, DARKGRAY);

        // Draw plus buttons
        DrawRectangleRec(btnPlus1Min, CheckCollisionPointRec(mousePos, btnPlus1Min) ? LIGHTGRAY : GRAY);
        DrawText("+1m", btnPlus1Min.x + 18, btnPlus1Min.y + 15, 20, WHITE);

        DrawRectangleRec(btnPlus5Min, CheckCollisionPointRec(mousePos, btnPlus5Min) ? LIGHTGRAY : GRAY);
        DrawText("+5m", btnPlus5Min.x + 18, btnPlus5Min.y + 15, 20, WHITE);

        DrawRectangleRec(btnPlus10Min, CheckCollisionPointRec(mousePos, btnPlus10Min) ? LIGHTGRAY : GRAY);
        DrawText("+10m", btnPlus10Min.x + 12, btnPlus10Min.y + 15, 20, WHITE);

        DrawRectangleRec(btnPlus1Hour, CheckCollisionPointRec(mousePos, btnPlus1Hour) ? LIGHTGRAY : GRAY);
        DrawText("+1h", btnPlus1Hour.x + 18, btnPlus1Hour.y + 15, 20, WHITE);

        // Draw minus buttons
        DrawRectangleRec(btnMinus1Min, CheckCollisionPointRec(mousePos, btnMinus1Min) ? LIGHTGRAY : GRAY);
        DrawText("-1m", btnMinus1Min.x + 20, btnMinus1Min.y + 15, 20, WHITE);

        DrawRectangleRec(btnMinus5Min, CheckCollisionPointRec(mousePos, btnMinus5Min) ? LIGHTGRAY : GRAY);
        DrawText("-5m", btnMinus5Min.x + 20, btnMinus5Min.y + 15, 20, WHITE);

        DrawRectangleRec(btnMinus10Min, CheckCollisionPointRec(mousePos, btnMinus10Min) ? LIGHTGRAY : GRAY);
        DrawText("-10m", btnMinus10Min.x + 14, btnMinus10Min.y + 15, 20, WHITE);

        DrawRectangleRec(btnMinus1Hour, CheckCollisionPointRec(mousePos, btnMinus1Hour) ? LIGHTGRAY : GRAY);
        DrawText("-1h", btnMinus1Hour.x + 20, btnMinus1Hour.y + 15, 20, WHITE);

        // Draw enter button
        if (!isRunning) {
            DrawRectangleRec(btnEnter, CheckCollisionPointRec(mousePos, btnEnter) ? DARKGREEN : GREEN);
            DrawText("START", btnEnter.x + 60, btnEnter.y + 18, 30, WHITE);
        } else {
            DrawRectangleRec(btnEnter, CheckCollisionPointRec(mousePos, btnEnter) ? MAROON : RED);
            DrawText("STOP", btnEnter.x + 60, btnEnter.y + 18, 30, WHITE);
        }

        EndDrawing();
    }

    UnloadMusicStream(fxAlarm);

    CloseAudioDevice();

    CloseWindow();
    return 0;
}
