#include "CPPFX.h"

using namespace std;
using namespace CPPFX;

/*********************
 * Simple login screen mockup
 ********************/

//simple function mimicking simple login behaviour
void LoginLogic(const TextField* loginField, const PasswordField* passwordField, Label* welcomeLabel, Label* errorLabel) {
    if (loginField->GetText() == "Login" && passwordField->GetText() == "Password") {//dummy logic
        welcomeLabel->SetText("Logged in!"); //dummy logic
        errorLabel->ClearText();
    } else {
        errorLabel->SetText("Wrong login or password.");
        welcomeLabel->SetText("Hello!");
    }
}

int main()
{
    //initialisation
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Simple login screen example");
    SetTargetFPS(60);
    Camera2D camera;
    camera = {0};
    camera.zoom = 1.0f;

    GUI gui;

    //VBox to hold other items in a neat column
    auto fields = gui.AddVBox("Fields");
    fields->SetX(GetScreenWidth() / 2 - 100);
    fields->SetY(GetScreenHeight() / 2 - 200);

    auto welcomeLabel = gui.AddLabel("WelcomeLabel");
    welcomeLabel->SetText("Hello!");
    welcomeLabel->alignment.SetAlignment(_Alignment::CENTRE);

    auto errorLabel = gui.AddLabel("ErrorLabel");
    errorLabel->ClearText(); //makes it invisible
    errorLabel->font.colour.SetColour(RED);

    auto loginField = gui.AddTextField("LoginField");
    loginField->SetPromptText("Login");
    loginField->ClearText(); //makes sure the prompt text will be displayed instead

    auto passwordField = gui.AddPasswordField("PasswordField");
    passwordField->SetPromptText("Password");
    passwordField->ClearText(); //makes sure the prompt text will be displayed instead

    auto loginButton = gui.AddButton("LoginButton");
    loginButton->SetText("Log in");
    loginButton->onClick = [loginField, passwordField, welcomeLabel, errorLabel]() { LoginLogic(loginField, passwordField, welcomeLabel, errorLabel); }; //callback

    //add items to the VBox
    fields->AddItem(welcomeLabel);
    fields->AddItem(loginField);
    fields->AddItem(passwordField);
    fields->AddItem(loginButton);
    fields->AddItem(errorLabel);
    fields->SetPositionsOfItems(); //actually orders the items around

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
