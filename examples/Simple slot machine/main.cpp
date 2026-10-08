#include <CPPFX/CPPFX.hpp> // for UI
#include <raylib.h> // for drawing
#include <vector> // for vector
#include <string> // for string

/***************************************************************************************
 * @file Slot Machine
 * @brief A simple simulation of a slot machine.
 * Three reels with symbols represented by coloured squares. Push the lever to roll.
 **************************************************************************************/

enum PHASE { ///< phase of the game
    NOTHING = 1,
    ROLLING = 2,
    COUNT = 3,
    LOSS = 4
};

static const std::vector<Color> REEL = {  GOLD, RED, GREEN, BLUE, GRAY, PURPLE, PINK, MAROON, BLACK }; ///< symbols on the reel in order
static const std::vector<int> rewards = { 1000, 500, 250,   100,  50,   25,     10,   5,      1 };

/**
 *  @brief Draws the Reel
 *  Draws a static reel with symbols based on the current indexes.
 *  @param reelsIndexes which symbols are currently shown
 */
void DrawReel(const std::vector<int>& reelsIndexes) {
    const float REELS_Y = 400.0f, REEL_WIDTH = 200.0f, REEL_HEIGHT = 300.0f;
    const float REELS_X_START = 500.0f;
    const size_t NUMBER_OF_REELS = 3;
    const float SYMBOL_WIDTH = 50.0f;

    float currentReelX = REELS_X_START;
    for (size_t i = 0; i < NUMBER_OF_REELS; ++i) {
        DrawRectangleLines(currentReelX, REELS_Y, REEL_WIDTH, REEL_HEIGHT, BLACK);
        DrawRectangle(currentReelX + (REEL_WIDTH * 0.5f) - (SYMBOL_WIDTH * 0.5f), REELS_Y + (REEL_HEIGHT * 0.5f) - (SYMBOL_WIDTH * 0.5f),
                      SYMBOL_WIDTH, SYMBOL_WIDTH, REEL[reelsIndexes[i]]);
        currentReelX += REEL_WIDTH;
    }
}

/**
 *  @brief Draws the Reel while it rolls
 *  Draws a reel while it's rolling. Randomly chooses each symbol each frame.
 */
void DrawRollingReel() {
    const std::vector<int> indexes = {GetRandomValue(0, REEL.size() - 1), GetRandomValue(0, REEL.size() - 1), GetRandomValue(0, REEL.size() - 1)};
    DrawReel(indexes);
}

const Vector2 END_UP = {1600.0f, 400.0f}; ///< where the head of the lever is when it's not toggled
const float HEAD_RADIUS = 50.0f; ///< size of the lever's head

/**
 *  @brief Draws the shared part of the lever
 *  Used by specialised lever drawing functions.
 *  @see DrawLeverUp
 *  @see DrawLeverDown
 *  @param END where the lever should end
 */
void DrawLever(const Vector2& END) {
    const Vector2 BEGIN = {1500.0f, 700.0f};
    const float LEVER_THICKNESS = 25.0f;
    DrawLineEx(BEGIN, END, LEVER_THICKNESS, GRAY);
    DrawCircle(END.x, END.y, HEAD_RADIUS, MAROON);
}

/**
 *  @brief Draws the lever facing up.
 */
void DrawLeverUp() {
    DrawLever(END_UP);
}

/**
 *  @brief Draws the lever facing down.
 */
void DrawLeverDown() {
    const Vector2 END_DOWN = {1600.0f, 900.0f};
    DrawLever(END_DOWN);
}

/**
 *  @brief Changes phase to rolling.
 *  Sets indexes of each reel to a random value, locks the lever, subtracts the cost of play and moves over to the next phase.
 *  Does nothing if the player doesn't have enough money, just in case. Used by the button.
 *  @param phase current phase, will be changed.
 *  @param reelsIndexes current symbols on reels
 *  @param lever pointer to the button
 *  @param money player's current money
 *  @param COST_OF_ROLL cost of roll
 */
void StartRoll(PHASE& phase, std::vector<int>& reelsIndexes, CPPFX::Button* lever, float& money, float COST_OF_ROLL) {
    if (money < COST_OF_ROLL) return;

    phase = ROLLING;
    for (auto& index : reelsIndexes) {
        index = GetRandomValue(0, REEL.size() - 1);
    }
    lever->MakeInactive(); // no changing phase while its already changed
    money -= COST_OF_ROLL;
}

int main() {
    // setup
    std::vector<int> reelsIndexes = {0, 0, 0}; ///< current symbols on each reel
    PHASE phase = NOTHING; ///< current phase of the game
    float dt; ///< frame time
    const float ROLLING_TIME = 3.0f; ///< time of rolling the reels in seconds
    float rollingTimer = ROLLING_TIME; ///< timer for the roll
    float money = 20.0f;
    const float COST_OF_ROLL = 10.0f; ///< how much does one roll cost

    //raylib setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Slot Machine");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.zoom = 1.0f;

    // UI configuration
    CPPFX::GUI gui;
    auto lever = gui.AddButton("Lever"); // button that will handle lever
    lever->MakeInvisible(); // invisible, lever is the visible part, button only handles clicks.
    lever->onClick = [&phase, &reelsIndexes, &lever, &money, &COST_OF_ROLL]() { // set callback to change phase and roll
        StartRoll(phase, reelsIndexes, lever, money, COST_OF_ROLL);
    };
    lever->SetXY(END_UP.x - HEAD_RADIUS, END_UP.y - HEAD_RADIUS);
    lever->SetDimensions(HEAD_RADIUS * 2.0f, HEAD_RADIUS * 2.0f);

     // main loop
    while (!WindowShouldClose()) {
        BeginDrawing(); // raylib loop start
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);

        dt = GetFrameTime();

        DrawText((std::string("$") + TextFormat("%.2f", money)).c_str(), 0, 0, 60, GOLD); // show money

        if (phase == NOTHING) { // stationary
            if (lever->IsInactive()) lever->MakeActive();
            DrawReel(reelsIndexes);
            DrawLeverUp();
        } else if (phase == ROLLING && rollingTimer >= 0.0f) { // rolls
            DrawLeverDown();
            DrawRollingReel();
            rollingTimer -= dt;
            if (rollingTimer <= 0.0f) {
                phase = COUNT;
                rollingTimer = ROLLING_TIME;
            }
        } else if (phase == COUNT) { // count rewards
            DrawReel(reelsIndexes);
            DrawLeverUp();
            const float REWARD_MULTIPLIER_NOT_FULL = 0.1f;
            float addedMoney = 0.0f;
            if (reelsIndexes[0] == reelsIndexes[1] && reelsIndexes[1] == reelsIndexes[2]) {
                addedMoney = rewards[reelsIndexes[0]];
            } else if (reelsIndexes[0] == reelsIndexes[1] || reelsIndexes[0] == reelsIndexes[2]) {
                addedMoney = rewards[reelsIndexes[0]] * REWARD_MULTIPLIER_NOT_FULL;
            } else if (reelsIndexes[1] == reelsIndexes[2]) {
                addedMoney = rewards[reelsIndexes[1]] * REWARD_MULTIPLIER_NOT_FULL;
            }
            money += addedMoney;
            if (money < COST_OF_ROLL) phase = LOSS;
            else phase = NOTHING;
        } else if (phase == LOSS) {
            DrawReel(reelsIndexes);
            DrawLeverUp();

            DrawText("YOU LOST ALL YOUR MONEY!!!", 200, 300, 80, RED);
        }

        gui.DoUI(camera); // does the button

        EndMode2D(); // raylib loop end
        EndDrawing();
    }
    gui.Close();
    CloseWindow();

    return 0;
}
