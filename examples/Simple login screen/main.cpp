#include "CPPFX.hpp"

using namespace std;
using namespace CPPFX;

/************************************************************************************************************************************************************************
 *  Simple login screen mockup. v2.1
 *  A label with welcome text and two text fields, one masked for password.
 *  Clicking a button or pressing enter is a login attempt. Incorrect credentials end with error appearing. Correct ("Login", "Password") end in welcome label changing.
 * ======================================================================================================================================================================
 *  v2.1 - made up to date with convenience fixes in Label and Fields.
 *  v2 - switched to Hide and Show for error label
 ************************************************************************************************************************************************************************/

//simple function mimicking simple login behaviour
void LoginLogic(const TextField* loginField, const PasswordField* passwordField, Label* welcomeLabel, Label* errorLabel) {
    if (loginField->GetText() == "Login" && passwordField->GetText() == "Password") {//dummy logic
        welcomeLabel->SetText("Logged in!"); //dummy logic
        errorLabel->Hide();
    } else {
        errorLabel->Show();
        welcomeLabel->SetText("Hello!");
    }
}

int main() {
    //initialisation
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Simple login screen example");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.zoom = 1.0f;

    GUI gui;

    //VBox to hold other items in a neat column
    auto fields = gui.AddVBox("Fields");
    fields->SetX(GetScreenWidth() / 2.0f - 100.0f);
    fields->SetY(GetScreenHeight() / 2.0f - 200.0f);

    auto welcomeLabel = gui.AddLabel("WelcomeLabel");
    welcomeLabel->SetText("Hello!");

    auto errorLabel = gui.AddLabel("ErrorLabel");
    errorLabel->Hide(); // makes it invisible and not interactable
    errorLabel->SetText("Incorrect login or password.");
    errorLabel->font.colour.SetColour(RED);

    auto loginField = gui.AddTextField("LoginField");
    loginField->SetPromptText("Login");

    auto passwordField = gui.AddPasswordField("PasswordField"); // by default prompts "Password"

    auto loginButton = gui.AddButton("LoginButton");
    loginButton->SetText("Log in");
    loginButton->onClick = [&loginField, &passwordField, &welcomeLabel, &errorLabel]() { LoginLogic(loginField, passwordField, welcomeLabel, errorLabel); }; //callback

    //add items to the VBox
    fields->AddItem(welcomeLabel);
    fields->AddItem(loginField);
    fields->AddItem(passwordField);
    fields->AddItem(loginButton);
    fields->AddItem(errorLabel);

    //main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);

        //log-in logic
        if (IsKeyPressed(KEY_ENTER)) { //raylib key press catch
            LoginLogic(loginField, passwordField, welcomeLabel, errorLabel);
        }

        //draw and update ui
        gui.DoUI(camera);

        EndMode2D();
        EndDrawing();
      }

    CloseWindow();

    return 0;
}
