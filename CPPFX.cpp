#include "CPPFX.h"

using namespace CPPFX;

const std::unordered_set<std::string> GUI::FXIDs = {"Label", "Button", "TextField", "CheckBox", "DropDown", "AnchorPane", "VBox", "HBox", "Workspace", "Spinner", "EditableSpinner",
    "PasswordField", "ProgressBar", "ProgressIndicator", "PressedButton", "List", "Chart", "PieChart"};

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
            if (((screenBased || item->IsScreenBased()) && item->WasIClicked(mousePos, camera)) || ((!screenBased && !item->IsScreenBased()) && item->WasIClicked(GetScreenToWorld2D(mousePos, camera)))) {
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
                item->DrawMyself(elapsedTime, camera);
            } else item->DrawMyself(elapsedTime);
        }
    }
}

void GUI::DoItemsActions(const Vector2& mousePos, const Camera2D& camera) {
    for (auto& item : ItemsInDrawingOrder) {
        if (!item->IsInactive()) {
            if (screenBased || item->IsScreenBased()) {
                item->DoPassiveAction(elapsedTime, camera);
                if (item->WasIClicked(mousePos, camera)) {
                    if (item->onHover) {
                        item->onHover();
                    }
                }
            } else {
                item->DoPassiveAction(elapsedTime);
                if (item->WasIClicked(GetScreenToWorld2D(mousePos, camera))) {
                    if (item->onHover) {
                        item->onHover();
                    }
                }
            }
            if (item->IsFocused()) {
                if (screenBased || item->IsScreenBased()) {
                    item->DoFocusAction(elapsedTime, camera);
                } else item->DoFocusAction(elapsedTime);
            }
        }
    }
}

void GUI::DoClickedItemsActions(const Vector2& mousePos, const Camera2D& camera) {
    for (auto& item : ItemsInDrawingOrder) {
        if (!item->IsInactive()) {
            if (screenBased || item->IsScreenBased()) {
                item->DoPassiveAction(elapsedTime, camera);
            } else {
                item->DoPassiveAction(elapsedTime);
            }
            if (item->IsFocused()) {
                if (screenBased || item->IsScreenBased()) {
                    item->DoFocusAction(elapsedTime, mousePos, camera);
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
        size_t currentCount = ItemsCounter.at(item->GetFxID());
        std::string newID = currentCount == 0 ? item->GetFxID()
                                    : item->GetFxID() + std::to_string(currentCount);
        item->SetID("GUI_AUTO_" + newID);
    } else item->SetID(ID);
    ItemsCounter[item->GetFxID()]++;
}

TextField* GUI::AddTextField(const std::string& ID) {
    std::unique_ptr<Item> textField = std::make_unique<TextField>();
    CreateItemID(textField, ID);
    auto pointer = static_cast<TextField*>(textField.get());
    CreateItem(textField);
    return pointer;
}

Label* GUI::AddLabel(const std::string& ID) {
    std::unique_ptr<Item> label = std::make_unique<Label>();
    CreateItemID(label, ID);
    auto pointer = static_cast<Label*>(label.get());
    CreateItem(label);
    return pointer;
}

Button* GUI::AddButton(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<Button>();
    CreateItemID(item, ID);
    auto pointer = static_cast<Button*>(item.get());
    CreateItem(item);
    return pointer;
}

CheckBox* GUI::AddCheckBox(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<CheckBox>();
    CreateItemID(item, ID);
    auto pointer = static_cast<CheckBox*>(item.get());
    CreateItem(item);
    return pointer;
}

AnchorPane* GUI::AddAnchorPane(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<AnchorPane>();
    CreateItemID(item, ID);
    auto pointer = static_cast<AnchorPane*>(item.get());
    CreateItem(item);
    return pointer;
}

VBox* GUI::AddVBox(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<VBox>();
    CreateItemID(item, ID);
    auto pointer = static_cast<VBox*>(item.get());
    CreateItem(item);
    return pointer;
}

HBox* GUI::AddHBox(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<HBox>();
    CreateItemID(item, ID);
    auto pointer = static_cast<HBox*>(item.get());
    CreateItem(item);
    return pointer;
}

Workspace* GUI::AddWorkspace(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<Workspace>();
    CreateItemID(item, ID);
    auto pointer = static_cast<Workspace*>(item.get());
    CreateItem(item);
    return pointer;
}

Spinner* GUI::AddSpinner(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<Spinner>();
    CreateItemID(item, ID);
    auto pointer = static_cast<Spinner*>(item.get());
    CreateItem(item);
    return pointer;
}

EditableSpinner* GUI::AddEditableSpinner(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<EditableSpinner>();
    CreateItemID(item, ID);
    auto pointer = static_cast<EditableSpinner*>(item.get());
    CreateItem(item);
    return pointer;
}

PasswordField* GUI::AddPasswordField(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<PasswordField>();
    CreateItemID(item, ID);
    auto pointer = static_cast<PasswordField*>(item.get());
    CreateItem(item);
    return pointer;
}

ProgressIndicator* GUI::AddProgressIndicator(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<ProgressIndicator>();
    CreateItemID(item, ID);
    auto pointer = static_cast<ProgressIndicator*>(item.get());
    CreateItem(item);
    return pointer;
}

ProgressBar* GUI::AddProgressBar(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<ProgressBar>();
    CreateItemID(item, ID);
    auto pointer = static_cast<ProgressBar*>(item.get());
    CreateItem(item);
    return pointer;
}

PressedButton* GUI::AddPressedButton(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<PressedButton>();
    CreateItemID(item, ID);
    auto pointer = static_cast<PressedButton*>(item.get());
    CreateItem(item);
    return pointer;
}

PieChart* GUI::AddPieChart(const std::string& ID) {
    std::unique_ptr<Item> item = std::make_unique<PieChart>();
    CreateItemID(item, ID);
    auto pointer = static_cast<PieChart*>(item.get());
    CreateItem(item);
    return pointer;
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
        throw std::out_of_range("Item " + ID + " doesn't exist, cannot delete");
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
        } else throw std::out_of_range("Cannot delete Item " + item->GetID() + ": doesn't exist in this GUI instance.");
    } else {
        throw std::invalid_argument("Cannot delete Item: doesn't exist");
    }
}

bool GUI::IsIDTaken(const std::string& ID) const {
    return Items.contains(ID);
}

bool GUI::IsItemContainer(const std::string& ID) const {
    if (!IsIDTaken(ID)) {
        throw std::out_of_range("No item with " + ID + " found when trying to check whether it's a container.");
    }
    return IsContainer(Items.at(ID)->GetFxID());
}

bool GUI::IsContainer(const std::string& fxID) const {
    static const std::unordered_set<std::string> CONTAINERS = {"AnchorPane", "VBox", "HBox", "Workspace"};
    return CONTAINERS.contains(fxID);
}

bool GUI::IsFxID(const std::string& fxID) const {
    return FXIDs.contains(fxID);
}

std::vector<std::string> GUI::GetItemsIDs() const {
    std::vector<std::string> ids;
    for (const auto& [key, value] : Items) {
        ids.push_back(key);
    }
    return ids;
}

//--- Getters ---

TextField* GUI::GetTextField(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<TextField*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a TextField");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No TextField with the ID " + ID + " exists");
    }
}

Label* GUI::GetLabel(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<Label*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a Label");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No Label with the ID " + ID + " exists");
    }
}

Button* GUI::GetButton(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<Button*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a Button");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No Button with the ID " + ID + " exists");
    }
}

CheckBox* GUI::GetCheckBox(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<CheckBox*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a CheckBox");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No CheckBox with the ID " + ID + " exists");
    }
}

AnchorPane* GUI::GetAnchorPane(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<AnchorPane*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a AnchorPane");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No AnchorPane with the ID " + ID + " exists");
    }
}

VBox* GUI::GetVBox(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<VBox*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a VBox");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No VBox with the ID " + ID + " exists");
    }
}

HBox* GUI::GetHBox(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<HBox*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a HBox");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No HBox with the ID " + ID + " exists");
    }
}

Container* GUI::GetContainer(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<Container*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a Container");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No Container with the ID " + ID + " exists");
    }
}

Workspace* GUI::GetWorkspace(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<Workspace*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a Workspace");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No Workspace with the ID " + ID + " exists");
    }
}

Spinner* GUI::GetSpinner(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<Spinner*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a Spinner");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No Spinner with the ID " + ID + " exists");
    }
}

EditableSpinner* GUI::GetEditableSpinner(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<EditableSpinner*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a EditableSpinner");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No EditableSpinner with the ID " + ID + " exists");
    }
}

PasswordField* GUI::GetPasswordField(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<PasswordField*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a PasswordField");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No PasswordField with the ID " + ID + " exists");
    }
}

ProgressIndicator* GUI::GetProgressIndicator(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<ProgressIndicator*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a ProgressIndicator");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No ProgressIndicator with the ID " + ID + " exists");
    }
}

ProgressBar* GUI::GetProgressBar(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<ProgressBar*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a ProgressBar");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No ProgressBar with the ID " + ID + " exists");
    }
}

PressedButton* GUI::GetPressedButton(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<PressedButton*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a PressedButton");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No PressedButton with the ID " + ID + " exists");
    }
}

PieChart* GUI::GetPieChart(const std::string& ID) {
    try {
        auto* ptr = dynamic_cast<PieChart*>(Items.at(ID).get());
        if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a PieChart");
        return ptr;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No PieChart with the ID " + ID + " exists");
    }
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
        throw std::out_of_range("No item with the ID " + ID + " exists, cannot set it priority to highest");
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
        throw std::out_of_range("No item with the ID " + ID + " exists, cannot set it priority to highest - 1");
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
        throw std::out_of_range("No item with the ID " + ID + " exists, cannot set it priority to lowest");
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
        throw std::out_of_range("No item with the ID " + ID + " exists, cannot set it priority to lowest + 1");
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
        throw std::invalid_argument("No item with internal ID " + fxID + " found. Check spelling.");
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
