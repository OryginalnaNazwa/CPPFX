# CPPFX
A retained mode GUI library for raylib, inspired by JavaFX, written in C++.

## Based on raylib
https://www.raylib.com/
https://github.com/raysan5/raylib

## Requirements
raylib 6.0, mingw-w64 64bit or GNU C++ Compiler (those were tested), C++20

## Setup
Compiler path: ../CPPFX/include

Linker path: ../CPPFX/lib

Linker flags:

Windows:      -lCPPFX -lraylib -lopengl32 -lgdi32 -lwinmm

Linux Ubuntu: -lCPPFX -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

## Quick example
This is a basic example that will display a label surrounded by a black border, containing the text "Hello world!" around the middle of the window.

```cpp
#include "CPPFX.h"

int main() {
  InitWindow(800, 800, "CPPFX basic example");

  Camera2D camera = {0};
  camera.zoom = 1.0f;

  CPPFX::GUI gui;
  auto helloLabel = gui.AddLabel();
  helloLabel->SetX(400);
  helloLabel->SetY(400);
  helloLabel->SetText("Hello world!");
  helloLabel->border.SetThickness(10);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    gui.DoUI(camera); //this renders items in the gui and handles interaction;
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
- Button
- VBox
- HBox
- Workspace
- AnchorPane
- Spinner
- EditableSpinner
- ProgressIndicator
- ProgressBar
- PasswordField
- Dropdown
- Checkbox
- List
- PieChart
- PressedButton

## Properties in some widgets
- Border
- Font
- Colour
- Shape
- Alignment

## License
CPPFX is licensed under zlib license, same as raylib. See raylib and LICENSE.txt for more information.
