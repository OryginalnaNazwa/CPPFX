# CPPFX
A retained mode GUI library for raylib, inspired by JavaFX, written in C++.
v.0.10.0

## Based on raylib
https://www.raylib.com/
https://github.com/raysan5/raylib

## Requirements
raylib 6.0, mingw-w64 64bit or GNU C++ Compiler (those were tested), C++23

## Setup
Compiler path: ../CPPFX/include

Linker path: ../CPPFX/lib

Linker flags:

Windows:      -lCPPFX -lraylib -lopengl32 -lgdi32 -lwinmm

Linux Ubuntu: -lCPPFX -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

Remember to also link raylib!
raylib's paths should be after CPPFX's

## Quick example
This is a basic example that will display a label surrounded by a black border, containing the text "Hello world!" around the middle of the window.

```cpp
#include "CPPFX.hpp"

int main() {
  //raylib initialisation
  InitWindow(800, 800, "CPPFX basic example");
  Camera2D camera = {0};
  camera.zoom = 1.0f;

  CPPFX::GUI gui; // this is used for managing widgets

  auto helloLabel = gui.AddLabel(); // returns a pointer
  helloLabel->SetXY(400.0f, 400.0f); // or use SetX and SetY
  helloLabel->SetText("Hello world!");
  helloLabel->border.SetThickness(10.0f); // border draws only if the thickness is greater than 0.0f

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(camera); // this is required if you are going to mix screen based and world based widgets in one GUI instance. Otherwise you can skip it.

    gui.DoUI(camera); //this renders items in the gui and handles interaction;

    EndMode2D();
    EndDrawing();
  }
  
  CloseWindow();
  return 0;
}
```

See raylib for full explanation of how main loop works.

Check out the examples provided in the Examples folder for more.

## Widget list
- Label
- TextField
- PasswordField
- Button
- PressedButton
- VBox
- HBox
- Workspace
- AnchorPane
- Spinner
- EditableSpinner
- ProgressIndicator
- ProgressBar
- Dropdown
- Checkbox (RadioButton)
- RadioGroup
- List
- PieChart
- Line
- Square
- Rectangle
- Circle
- Sprite

## Properties in some widgets
- Border
- Font
- Colour
- Shape
- Alignment

## License
CPPFX is licensed under zlib license, same as raylib. See raylib and LICENSE.txt for more information.
