#include "CPPFX.h"
#include <vector>
#include <cmath>

class Node {
public:
    float length;   // renamed from radius — clearer
    float baseX, baseY;
    float endX, endY;
    float angle;    // in degrees, 0 = pointing up

    Node(float len, float bx, float by, float ang)
        : length(len), baseX(bx), baseY(by), angle(ang)
    {
        UpdateEnd();
    }
    Node() {}

    // Recompute endX/endY from base + angle + length
    void UpdateEnd() {
        endX = baseX + length * sinf(angle * DEG2RAD);
        endY = baseY - length * cosf(angle * DEG2RAD);
    }

    void DrawMyself() const {
        DrawCircle((int)baseX, (int)baseY, 10, BLACK);
        DrawLineEx({baseX, baseY}, {endX, endY}, 6, DARKGRAY);
    }
};

void UpdateChain(std::vector<Node>& nodes) {
    for (size_t i = 0; i < nodes.size(); i++) {
        nodes[i].UpdateEnd();
        if (i + 1 < nodes.size()) {
            nodes[i + 1].baseX = nodes[i].endX;
            nodes[i + 1].baseY = nodes[i].endY;
        }
    }
}

void InitialiseNodes(std::vector<Node>& nodes) {
    // Only node 0 has a fixed base; the rest are chained
    nodes.emplace_back(100.0f, 800.0f, 500.0f, 0.0f);
    nodes.emplace_back(100.0f, 0.0f,   0.0f,   0.0f); // base overwritten by chain
    nodes.emplace_back( 80.0f, 0.0f,   0.0f,  30.0f);
    UpdateChain(nodes);
}

void AddNode(std::vector<Node>& nodes) {
    nodes.emplace_back(100.0f, 0.0f, 0.0f, 0.0f);
    UpdateChain(nodes);
}

void RemoveNode(std::vector<Node>& nodes) {
    if (nodes.size() > 1) {
        nodes.pop_back();
    }
}

int main() {
    std::vector<Node> nodes;
    InitialiseNodes(nodes);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    InitWindow(800, 600, "Robotic arm");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    CPPFX::GUI gui;

    auto nodeChoice   = gui.AddSpinner();
    nodeChoice->SetMin(); nodeChoice->SetMinValue(0);
    nodeChoice->SetMax();

    auto lengthChoice = gui.AddEditableSpinner();
    lengthChoice->SetMin(); lengthChoice->SetMinValue(1);

    auto angleChoice  = gui.AddEditableSpinner();
    angleChoice->SetMin();  angleChoice->SetMinValue(0);
    angleChoice->SetMax();  angleChoice->SetMaxValue(360);
    angleChoice->AllowWrap();

    auto nodeLabel   = gui.AddLabel();  nodeLabel->SetText("Active node index");    nodeLabel->ExpandToText();
    auto lengthLabel = gui.AddLabel();  lengthLabel->SetText("Length of the arm");  lengthLabel->ExpandToText();
    auto angleLabel  = gui.AddLabel();  angleLabel->SetText("Angle of the arm");    angleLabel->ExpandToText();

    auto nodePick   = gui.AddHBox();
    nodePick->AddItem(nodeLabel);
    nodePick->AddItem(nodeChoice);

    auto lengthPick = gui.AddHBox();
    lengthPick->AddItem(lengthLabel);
    lengthPick->AddItem(lengthChoice);

    auto anglePick = gui.AddHBox();
    anglePick->AddItem(angleLabel);
    anglePick->AddItem(angleChoice);

    auto addButton = gui.AddButton();
    addButton->SetText("Add node to the top");  addButton->ExpandToText();
    addButton->onClick = [&nodes]() {AddNode(nodes);};
    addButton->border.SetThickness(5);

    auto removeButton = gui.AddButton();
    removeButton->SetText("Remove the top node");   removeButton->ExpandToText();
    removeButton->onClick = [&nodes]() {RemoveNode(nodes);};
    removeButton->border.SetThickness(5);

    auto ui = gui.AddVBox();
    ui->AddItem(nodePick);
    ui->AddItem(lengthPick);
    ui->AddItem(anglePick);
    ui->AddItem(addButton);
    ui->AddItem(removeButton);
    ui->SetPriority(11); //must go before the previous ones to set them first, by default containers have priority of 10.

    int prevIdx = 0;
    lengthChoice->SetValue(nodes[0].length); //sets the base node display correctly
    angleChoice->SetValue(nodes[0].angle);

    while (!WindowShouldClose()) {
        nodeChoice->SetMaxValue((float)nodes.size() - 1);
        int idx = (int)nodeChoice->GetValue();

        if (idx != prevIdx) {
            // Immediately overwrite spinners AND their committed value
            lengthChoice->SetValue(nodes[idx].length);
            angleChoice->SetValue(nodes[idx].angle);
            prevIdx = idx;
        }

        nodes[idx].length = lengthChoice->GetValue();
        nodes[idx].angle  = angleChoice->GetValue();
        UpdateChain(nodes);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        for (auto& node : nodes)
            node.DrawMyself();
        gui.DoUI(camera); //also sorts on the first frame after getting dirty
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
