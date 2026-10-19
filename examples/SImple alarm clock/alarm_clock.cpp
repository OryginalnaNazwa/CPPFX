#include <CPPFX/CPPFX.hpp>
#include <raylib.h> // for colours and music
#include <string> // for to_string

/**
 *  @file Simple alarm clock
 *  @brief A simple alarm clock that fires after the specified time has passed.
 *  @details Set of six buttons to change waiting time, a button to start or stop the clock. Above it a double-digitted time display.
 */

/**
 * @brief Advances time.
 */
void DoTime(float& elapsed, int& minutes, int& hours) {
    if (elapsed >= 60.0f) {
        elapsed -= 60.0f;
        minutes--;
        if (minutes < 0) {
            hours--;
            minutes = 59;
        }
    }
}

/**
 *  @brief Keeps time in bounds and advances it.
 */
void NormaliseTime(int& minutes, int& hours) {
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
}

/**
 *  @brief Plays the alarm sound.
 */
void DoSound(int minutes, int hours, Music& fxAlarm) {
    if (hours == 0 && minutes == 0 && !IsMusicStreamPlaying(fxAlarm)) {
        PlayMusicStream(fxAlarm);
    }

    if (IsMusicStreamPlaying(fxAlarm)) {
        UpdateMusicStream(fxAlarm);
    }
}

/**
 *  @brief Creates a text display of current set time.
 *  @returns a string with the time set.
 */
std::string GetTimerText(int minutes, int hours) {
    std::string time = "";
    if (hours < 10) {
        time += '0';
    }
    time += std::to_string(hours);
    time += " : ";
    if (minutes < 10) {
        time += '0';
    }
    time += std::to_string(minutes);

    return time;
}

int main() {
    int minutes = 0; ///< currently set minutes of wait
    int hours = 0; ///< currently set hours of wait
    float elapsed = 0; ///< elapsed frame times. In seconds.

    const int screenWidth = 1600;
    const int screenHeight = 1000;
    Camera2D camera = {};
    camera.zoom = 1.0f;
    CPPFX::GUI gui;

    InitWindow(screenWidth, screenHeight, "Alarm Timer");
    SetTargetFPS(60);

    //load loud buzzer
    InitAudioDevice();
    Music fxAlarm = LoadMusicStream("alarm.wav");

    // time display
    auto timerLabel = gui.AddLabel("Timer");
    timerLabel->font.SetFontSize(40.0f);

    // start/stop button
    auto startButton = gui.AddPressedButton("Start Button");
    startButton->SetPressedText("STOP"); startButton->pressedColour.SetColour(RED);
    startButton->SetText("START"); startButton->colour.SetColour(DARKGREEN);
    startButton->onClick = [&fxAlarm, &elapsed]() { StopMusicStream(fxAlarm); elapsed = 0.0f;}; // stops the alarm and restarts the timer
    startButton->font.colour.SetColour(RAYWHITE);

    // set up button interface
    auto btn1 = gui.AddButton("-1h Button");
    btn1->SetText("-1h");
    btn1->onClick = [&hours]() { hours--; };
    auto btn2 = gui.AddButton("-5m Button");
    btn2->SetText("-5m");
    btn2->onClick = [&minutes]() { minutes -= 5; };
    auto btn3 = gui.AddButton("-1m Button");
    btn3->SetText("-1m");
    btn3->onClick = [&minutes]() { minutes --; };
    auto btn4 = gui.AddButton("+1m Button");
    btn4->SetText("+1m");
    btn4->onClick = [&minutes]() { minutes++; };
    auto btn5 = gui.AddButton("+5m Button");
    btn5->SetText("+5m");
    btn5->onClick = [&minutes]() { minutes += 5; };
    auto btn6 = gui.AddButton("+1h Button");
    btn6->SetText("+1h");
    btn6->onClick = [&hours]() { hours++; };

    // arrange layout
    auto column = gui.AddVBox();
    column->SetXY(screenWidth / 3.0f + 150.0f, screenHeight / 3.0f); // yeah, it's bad
    column->alignment.SetAlignment(CPPFX::Alignment::TOP_CENTRE);
    column->AddItem(timerLabel);
    auto buttonRow = gui.AddHBox();
    buttonRow->AddItem(btn1); buttonRow->AddItem(btn2); buttonRow->AddItem(btn3); buttonRow->AddItem(btn4); buttonRow->AddItem(btn5); buttonRow->AddItem(btn6);
    column->AddItem(buttonRow);
    column->AddItem(startButton);

    // main loop
    while (!WindowShouldClose()) {
        // update
        if (startButton->IsPressed()) { // is running
            elapsed += GetFrameTime();

            DoTime(elapsed, minutes, hours);

            DoSound(minutes, hours, fxAlarm);
        }

        NormaliseTime(minutes, hours);

        timerLabel->SetText(GetTimerText(minutes, hours)); // update label

        // draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        gui.DoUI(camera); // and also do UI

        EndDrawing();
    }

    // cleanup
    gui.Close();
    UnloadMusicStream(fxAlarm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
