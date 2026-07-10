#include "CPPFX.hpp"
#include <algorithm>   // for remove_if, sort
#include <compare>     // for operator<
#include <functional>  // for function
#include <utility>     // for pair, move
#include "bases.hpp"     // for Container
//#include "items.h"     // for widgets

using namespace CPPFX;

const char* CPPFX::CPPFX_VERSION_STRING = "@(#)CPPFX 0.9.2"; //sanity check for version

const std::unordered_set<std::string> GUI::FXIDs = {"Label", "Button", "TextField", "CheckBox", "DropDown", "AnchorPane", "VBox", "HBox", "Workspace", "Spinner", "EditableSpinner",
    "PasswordField", "ProgressBar", "ProgressIndicator", "PressedButton", "List", "RadioGroup", "Chart", "PieChart", "Line", "Square", "Rectangle", "Circle", "Sprite"};

const std::string GUI::AUTOMATIC_ID_PREFIX = "GUI_AUTO_";

const std::unordered_set<std::string> GUI::CONTAINERS = {"AnchorPane", "VBox", "HBox", "Workspace"};

//--- Main loop ---

void GUI::DoUI(const Camera2D& camera) {
    elapsedTime += GetFrameTime();
    if (needsSorting) {
        SortOrder();
        needsSorting = false;
    }
    Vector2 mousePos = {(float)GetMouseX(), (float)GetMouseY()};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        onMouseClick(mousePos, camera);
        DoClickedItemsActions(mousePos, camera);
    } else {
        DoItemsActions(mousePos, camera);
    }

    DrawUI(camera);
}

void GUI::onMouseClick(const Vector2& mousePos, const Camera2D& camera) {
    for (auto it = ItemsInDrawingOrder.rbegin(); it != ItemsInDrawingOrder.rend(); ++it) {
        Item* item = *it;
        if (!item->IsInactive())  {
            if (((screenBased || item->IsScreenBased()) && item->WasIClicked(mousePos)) || ((!screenBased && !item->IsScreenBased()) && item->WasIClicked(GetScreenToWorld2D(mousePos, camera)))) {
                if (!item->IsFocused()) {
                    DefocusItems();
                    item->Focus();
                }
                if (item->onClick) item->onClick();
                if (item->DoesEatClicks()) {
                    return;
                }
            }
        }
    }
    DefocusItems();
}

void GUI::DrawUI(const Camera2D& camera) const {
    for (auto& item : ItemsInDrawingOrder) {
        if (item->IsVisible()) {
            if (screenBased || item->IsScreenBased()) {
                EndMode2D(); // THIS WILL CRASH IF THE MODE HAS NOT BEEN BEGUN EARLIER!!!
                item->DrawMyself(elapsedTime);
                BeginMode2D(camera);
            } else item->DrawMyself(elapsedTime);
        }
    }
}

void GUI::DoItemsActions(const Vector2& mousePos, const Camera2D& camera) {
    for (auto& item : ItemsInDrawingOrder) {
        if (!item->IsInactive()) {
            if (screenBased || item->IsScreenBased()) {
                if (item->WasIClicked(mousePos)) {
                    if (item->onHover) {
                        item->onHover();
                    }
                }
            } else {
                if (item->WasIClicked(GetScreenToWorld2D(mousePos, camera))) {
                    if (item->onHover) {
                        item->onHover();
                    }
                }
            }
            item->DoPassiveAction(elapsedTime);
            if (item->IsFocused()) {
                item->DoFocusAction(elapsedTime);
            }
        }
    }
}

void GUI::DoClickedItemsActions(const Vector2& mousePos, const Camera2D& camera) {
    for (auto& item : ItemsInDrawingOrder) {
        if (!item->IsInactive()) {
            item->DoPassiveAction(elapsedTime);
            if (item->IsFocused()) {
                if (screenBased || item->IsScreenBased()) {
                    item->DoFocusAction(elapsedTime, mousePos);
                } else item->DoFocusAction(elapsedTime, GetScreenToWorld2D(mousePos, camera));
            }
        }
    }
}

void GUI::DefocusItems() {
    for (auto& [key, item] : Items) {
        item->Defocus();
    }
}

//--- Adders ---

void GUI::CreateItem(std::unique_ptr<Item>& item) {
    ItemsInDrawingOrder.push_back(item.get());
    Items.insert({item->GetID(), std::move(item)});
    needsSorting = true;
}

void GUI::CreateItemID(std::unique_ptr<Item>& item, const std::string& ID) {
    if (ID.empty()) {
        try {
            size_t currentCount = ItemsCounter.at(item->GetFxID());
            std::string newID = currentCount == 0 ? item->GetFxID()
                                    : item->GetFxID() + std::to_string(currentCount);
            item->SetID(AUTOMATIC_ID_PREFIX + newID);
        } catch (std::out_of_range& e) {
            CPPFX_THROW_GUI(std::out_of_range, "Invalid creation of item with the fxid " + item->GetFxID() + " while trying to give it id " + ID + "or " + item->GetID());
            //this shouldn't happen, left it for debug of virtual inheritance
        }

    } else item->SetID(ID);
    ItemsCounter[item->GetFxID()]++;
}

TextField* GUI::AddTextField(const std::string& ID) {
    return AddItem<TextField>(ID);
}

Label* GUI::AddLabel(const std::string& ID) {
    return AddItem<Label>(ID);
}

Button* GUI::AddButton(const std::string& ID) {
    return AddItem<Button>(ID);
}

CheckBox* GUI::AddCheckBox(const std::string& ID) {
    return AddItem<CheckBox>(ID);
}

AnchorPane* GUI::AddAnchorPane(const std::string& ID) {
    return AddItem<AnchorPane>(ID);
}

VBox* GUI::AddVBox(const std::string& ID) {
    return AddItem<VBox>(ID);
}

HBox* GUI::AddHBox(const std::string& ID) {
    return AddItem<HBox>(ID);
}

Workspace* GUI::AddWorkspace(const std::string& ID) {
    return AddItem<Workspace>(ID);
}

Spinner* GUI::AddSpinner(const std::string& ID) {
    return AddItem<Spinner>(ID);
}

EditableSpinner* GUI::AddEditableSpinner(const std::string& ID) {
    return AddItem<EditableSpinner>(ID);
}

PasswordField* GUI::AddPasswordField(const std::string& ID) {
    return AddItem<PasswordField>(ID);
}

ProgressIndicator* GUI::AddProgressIndicator(const std::string& ID) {
    return AddItem<ProgressIndicator>(ID);
}

ProgressBar* GUI::AddProgressBar(const std::string& ID) {
    return AddItem<ProgressBar>(ID);
}

PressedButton* GUI::AddPressedButton(const std::string& ID) {
    return AddItem<PressedButton>(ID);
}

PieChart* GUI::AddPieChart(const std::string& ID) {
    return AddItem<PieChart>(ID);
}

Line* GUI::AddLine(const std::string& ID) {
    return AddItem<Line>(ID);
}

CPPFX::Square* GUI::AddSquare(const std::string& ID) {
    return AddItem<CPPFX::Square>(ID);
}

CPPFX::Rectangle* GUI::AddRectangle(const std::string& ID) {
    return AddItem<CPPFX::Rectangle>(ID);
}

Circle* GUI::AddCircle(const std::string& ID) {
    return AddItem<Circle>(ID);
}

RadioButton* GUI::AddRadioButton(const std::string& ID) {
    return AddItem<RadioButton>(ID);
}

RadioGroup* GUI::AddRadioGroup(const std::string& ID) {
    return AddItem<RadioGroup>(ID);
}

Sprite* GUI::AddSprite(const std::string& ID) {
    return AddItem<Sprite>(ID);
}

//--- Remover ---

void GUI::RemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        for (auto& [key, item] : Items) {
            if (IsContainer(item->GetFxID())) {
                GetContainer(item->GetID())->SafeRemoveItem(ID);
            }
        }
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&ID](const Item* item) { return item->GetID() == ID; }),ItemsInDrawingOrder.end());
        Items.erase(ID);
    } else {
        CPPFX_THROW_GUI(std::out_of_range, "Item " + ID + " doesn't exist, cannot delete");
    }
}

void GUI::RemoveItem(Item*& item) {
    if (item) {
        if (IsIDTaken(item->GetID())) {
            if (IsContainer(item->GetFxID())) {
                GetContainer(item->GetID())->SafeRemoveItem(item);
            }
            ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&item](const Item* i) { return i->GetID() == item->GetID(); }),ItemsInDrawingOrder.end());
            Items.erase(item->GetID());
            item = nullptr;
        } else CPPFX_THROW_GUI(std::out_of_range, "Item with the ID " + item->GetID() + " is not in the GUI.");
    } else CPPFX_THROW_GUI(std::invalid_argument, "Cannot delete Item: doesn't exist");
}

bool GUI::IsIDTaken(const std::string& ID) const {
    return Items.contains(ID);
}

bool GUI::HasItem(const std::string& ID) const {
    return IsIDTaken(ID);
}

bool GUI::IsItemContainer(const std::string& ID) const {
    if (!IsIDTaken(ID)) {
        CPPFX_THROW_GUI(std::out_of_range, "No item with " + ID + " found when trying to check whether it's a container.");
    }
    return IsContainer(Items.at(ID)->GetFxID());
}

bool GUI::IsContainer(const std::string& fxID) {
    return CONTAINERS.contains(fxID);
}

const std::unordered_set<std::string>& GUI::GetContainersFxIDs() {
    return CONTAINERS;
}

bool GUI::IsFxID(const std::string& fxID) {
    return FXIDs.contains(fxID);
}

const std::unordered_set<std::string>& GUI::GetFxIDs() {
    return FXIDs;
}

const std::unordered_set<std::string> GUI::GetItemsIDs() const {
    std::unordered_set<std::string> ids;
    for (const auto& [key, value] : Items) {
        ids.insert(key);
    }
    return ids;
}

const std::string& GUI::GetAutomaticIDPrefix() {
    return AUTOMATIC_ID_PREFIX;
}

bool GUI::IsIDAutomatic(const std::string& ID) {
    return ID.contains(AUTOMATIC_ID_PREFIX);
}

//--- Getters ---

TextField* GUI::GetTextField(const std::string& ID) {
    return GetItem<TextField>(ID);
}

Label* GUI::GetLabel(const std::string& ID) {
    return GetItem<Label>(ID);
}

Button* GUI::GetButton(const std::string& ID) {
    return GetItem<Button>(ID);
}

CheckBox* GUI::GetCheckBox(const std::string& ID) {
    return GetItem<CheckBox>(ID);
}

AnchorPane* GUI::GetAnchorPane(const std::string& ID) {
    return GetItem<AnchorPane>(ID);
}

VBox* GUI::GetVBox(const std::string& ID) {
    return GetItem<VBox>(ID);
}

HBox* GUI::GetHBox(const std::string& ID) {
    return GetItem<HBox>(ID);
}

Container* GUI::GetContainer(const std::string& ID) {
    return GetItem<Container>(ID);
}

Workspace* GUI::GetWorkspace(const std::string& ID) {
    return GetItem<Workspace>(ID);
}

Spinner* GUI::GetSpinner(const std::string& ID) {
    return GetItem<Spinner>(ID);
}

EditableSpinner* GUI::GetEditableSpinner(const std::string& ID) {
    return GetItem<EditableSpinner>(ID);
}

PasswordField* GUI::GetPasswordField(const std::string& ID) {
    return GetItem<PasswordField>(ID);
}

ProgressIndicator* GUI::GetProgressIndicator(const std::string& ID) {
    return GetItem<ProgressIndicator>(ID);
}

ProgressBar* GUI::GetProgressBar(const std::string& ID) {
    return GetItem<ProgressBar>(ID);
}

PressedButton* GUI::GetPressedButton(const std::string& ID) {
    return GetItem<PressedButton>(ID);
}

PieChart* GUI::GetPieChart(const std::string& ID) {
    return GetItem<PieChart>(ID);
}

Line* GUI::GetLine(const std::string& ID) {
    return GetItem<Line>(ID);
}

CPPFX::Square* GUI::GetSquare(const std::string& ID) {
    return GetItem<Square>(ID);
}

CPPFX::Rectangle* GUI::GetRectangle(const std::string& ID) {
    return GetItem<Rectangle>(ID);
}

Circle* GUI::GetCircle(const std::string& ID) {
    return GetItem<Circle>(ID);
}

RadioButton* GUI::GetRadioButton(const std::string& ID) {
    return GetItem<RadioButton>(ID);
}

RadioGroup* GUI::GetRadioGroup(const std::string& ID) {
    return GetItem<RadioGroup>(ID);
}

Sprite* GUI::GetSprite(const std::string& ID) {
    return GetItem<Sprite>(ID);
}

//--- Priority helpers ---

void GUI::SortOrder() {
    std::sort(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [](const Item* a, const Item* b) {
        return a->GetPriority() > b->GetPriority();
    });
}

void GUI::SetHighestPriority(const std::string& ID) {
    Item* item = nullptr;
    try {
        item = Items.at(ID).get();
    } catch (const std::out_of_range& e) {
        CPPFX_THROW_GUI(std::out_of_range, "No item with the ID " + ID + " exists, cannot set it priority to highest");
    }
    if (item->GetPriority() != 0) {
        if (needsSorting) {
            SortOrder();
        }
        size_t highestPriority = ItemsInDrawingOrder.back()->GetPriority();
        item->SetPriority(highestPriority);
        needsSorting = true;
    } else {
        CPPFX_WARN("Item" + ID + " is already at highest priority.");
    }
}

void GUI::SetAboveHighestPriority(const std::string& ID) {
    Item* item = nullptr;
    try {
        item = Items.at(ID).get();
    } catch (const std::out_of_range& e) {
        CPPFX_THROW_GUI(std::out_of_range, "No item with the ID " + ID + " exists, cannot set it priority to highest - 1");
    }
    if (item->GetPriority() != 0) {
        if (needsSorting) {
            SortOrder();
        }
        size_t highestPriority = ItemsInDrawingOrder.back()->GetPriority();
        item->SetPriority(highestPriority);
        if (item->GetPriority() != 0) {
            item->MoveUpPriority();
        } else CPPFX_WARN("Other items already have the highest possible priority (0), setting item" + ID + " priority to 0.");
        needsSorting = true;
    } else {
        CPPFX_WARN("Item" + ID + " is already at highest priority.");
    }
}

void GUI::SetLowestPriority(const std::string& ID) {
    Item* item = nullptr;
    try {
        item = Items.at(ID).get();
    } catch (const std::out_of_range& e) {
        CPPFX_THROW_GUI(std::out_of_range, "No item with the ID " + ID + " exists, cannot set it priority to lowest");
    }
    if (needsSorting) {
        SortOrder();
    }
    size_t lowestPriority = ItemsInDrawingOrder.front()->GetPriority();
    item->SetPriority(lowestPriority);
    needsSorting = true;
}

void GUI::SetBelowLowestPriority(const std::string& ID) {
    Item* item = nullptr;
    try {
        item = Items.at(ID).get();
    } catch (const std::out_of_range& e) {
        CPPFX_THROW_GUI(std::out_of_range, "No item with the ID " + ID + " exists, cannot set it priority to lowest + 1");
    }
    if (needsSorting) {
        SortOrder();
    }
    size_t lowestPriority = ItemsInDrawingOrder.front()->GetPriority();
    item->SetPriority(lowestPriority);
    item->MoveDownPriority();
    needsSorting = true;
}

size_t GUI::GetTotalItemCount(const std::string& fxID) const {
    if (IsFxID(fxID)) {
        return ItemsCounter.at(fxID);
    } else {
        CPPFX_THROW_GUI(std::invalid_argument, "No item with internal ID " + fxID + " found. Check spelling.");
    }
}

void GUI::SetGlobalPrimaryColour(const Color& color) {
    for (auto& [key, item] : Items) {
        item->colour.SetColour(color);
    }
}

void GUI::SetGlobalPrimaryColour(const Colour& colour) {
    for (auto& [key, item] : Items) {
        item->colour.SetColour(colour.GetColour());
    }
}

void GUI::SetGlobalPrimaryColour(const std::string& colour) {
    for (auto& [key, item] : Items) {
        item->colour.SetColour(colour);
    }
}

void GUI::SetGlobalScreenBased() {
    screenBased = true;
}

void GUI::SetGlobalWorldBased() {
    screenBased = false;
}

void GUI::SetGlobalCoordinateBase(const bool& flag) {
    screenBased = flag;
}

bool GUI::IsGlobalScreenBased() const {
    return screenBased;
}
