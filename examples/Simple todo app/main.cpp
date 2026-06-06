#include "CPPFX.hpp"
#include <fstream>
#include <string>
#include <unordered_set>
#include <stdexcept>

/****************************************************************************************************************************************************************************************
 * Simple todo app.
 * Type task in the field and accept with the button. Saves and loads from plain text file. Camera is movable for convenience. Should probably be better, but it's just a quick example.
 ****************************************************************************************************************************************************************************************/

const std::string path = "todos_list.txt";

void SaveToFile(CPPFX::GUI& gui) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open the file during saving.");
    }
    const std::unordered_set<std::string> todos = gui.GetItemsIDs();
    for (const auto& todo : todos) {
        if (todo.contains(gui.GetAutomaticIDPrefix())) { // since all interface widgets were not created with ID, the only ones with not automatic ID are todos
            continue;
        }
        file << todo << std::endl;
        file << gui.GetItem<CPPFX::CheckBox>(todo)->IsPressed() << std::endl;
    }
    file.close();
}

void LoadFromFile(CPPFX::GUI& gui) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open the file during loading.");
    }

    std::string todo, press;
    while (getline(file, todo)) {
        getline(file, press);
        auto added = gui.AddCheckBox(todo);
        added->SetText(todo);
        if (press == "1") { // theres probably a better way to do it, but that's what I got to work
            added->SetPressed(true);
        } else added->SetPressed(false);
        gui.GetVBox("GUI_AUTO_VBox")->AddItem(added); // automatic prefix + class name is the auto-generated ID for the first widget of this type. Next one would be GUI_AUTO_VBox1 and so on.
    }
}

// a little hack. You can move the camera to see all the items. It should probably be in multiple columns instead.
void MoveTheCamera(Camera2D& camera) {
    if (IsKeyDown(KEY_RIGHT)) camera.target.x += 5;
    if (IsKeyDown(KEY_LEFT))  camera.target.x -= 5;
    if (IsKeyDown(KEY_DOWN))  camera.target.y += 5;
    if (IsKeyDown(KEY_UP))    camera.target.y -= 5;

    // Right mouse drag moves camera
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        camera.target.x -= delta.x / camera.zoom;
        camera.target.y -= delta.y / camera.zoom;
    }
}

int main() {
    // initialise raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(2000, 2000, "TODO app");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    CPPFX::GUI gui;
    bool closing = false; // to safely close the loop from inside it

    auto todoList = gui.AddVBox(); // column with the todos

    auto todoField = gui.AddTextField(); // input field
    todoField->ClearText();
    todoField->SetPromptText("Put todo here");
    todoField->ExpandToText();
    todoField->border.SetThickness(10.0f);

    auto addButton = gui.AddButton();
    addButton->SetText("Add a todo");
    addButton->ExpandToText();
    addButton->onClick = [&todoField, &todoList, &gui](){ // calback
        const std::string newTodo = todoField->GetText();
        if (!gui.IsIDTaken(newTodo)) {
            auto todo = gui.AddCheckBox(newTodo); // creates a new item with the ID the same as the text
            todo->SetText(newTodo);
            todoList->AddItem(todo);
            todoField->ClearText();
        }
    };
    addButton->border.SetThickness(10.0f);

    auto saveButton = gui.AddButton();
    saveButton->SetText("Save and quit");
    saveButton->ExpandToText();
    saveButton->onClick = [&gui, &closing](){
        SaveToFile(gui);
        closing = true;
    };
    saveButton->border.SetThickness(10.0f);

    auto ui = gui.AddHBox();
    ui->SetXY(800.0f, 800.0f);
    ui->AddItem(todoField);
    ui->AddItem(addButton);
    ui->AddItem(saveButton);
    ui->SetPadding(40.0f);

    LoadFromFile(gui);

    while (!WindowShouldClose()) {
        MoveTheCamera(camera);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        gui.DoUI(camera);

        EndMode2D();
        EndDrawing();
        if (closing) break; // gets out of the loop
    }

    CloseWindow();

    return 0;
}
