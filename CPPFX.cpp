#include "FX.h"

using namespace CPPFX;

//--- Main loop ---

void GUI::DoUI(const Camera2D& camera) {
    dt += GetFrameTime();
    if (needsSorting) {
        SortOrder();
        needsSorting = false;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetScreenToWorld2D({(float)GetMouseX(), (float)GetMouseY()}, camera);
        onMouseClick(mousePos);
        DoItemsActions(mousePos);
    } else {
        DoItemsActions();
    }

    DrawUI();
}

void GUI::onMouseClick(const Vector2& mousePos) {
    for (auto it = ItemsInDrawingOrder.rbegin(); it != ItemsInDrawingOrder.rend(); ++it) {
        Item* item = *it;
        if (item->WasIClicked(mousePos)) {
            if (!item->active) {
                DeactivateItems();
                item->active = true;
            }
            if (item->onClick) item->onClick();
            return;
        }
    }
    DeactivateItems();
}

void GUI::DrawUI() const {
    for (auto& item : ItemsInDrawingOrder) {
        item->DrawMyself(dt);
    }
}

void GUI::DoItemsActions() {
    for (auto& item : ItemsInDrawingOrder) {
        item->currentFrameTime = dt;
        if (item->active) {
            item->DoActiveAction(dt);
        }
    }
}

void GUI::DoItemsActions(const Vector2& mousePos) {
    for (auto& item : ItemsInDrawingOrder) {
        item->currentFrameTime = dt;
        if (item->active) {
            item->DoActiveAction(dt, mousePos);
        }
    }
}

void GUI::DeactivateItems() {
    for (auto& [key, item] : Items) {
        item->Deactivate();
    }
}

//--- Adders ---

void GUI::AddItem(const std::string& fxID, const std::string& ID) {
    if (fxID == "TextField") {
        AddTextField(ID);
    } else if (fxID == "Label") {
        AddLabel(ID);
    } else if (fxID == "Button") {
        AddButton(ID);
    } else if (fxID == "CheckBox") {
        AddCheckBox(ID);
    } else if (fxID == "DropDown") {
        AddDropDown<std::string>(ID);
    } else if (fxID == "AnchorPane") {
        AddAnchorPane(ID);
    } else if (fxID == "VBox") {
        AddVBox(ID);
    } else if (fxID == "HBox") {
        AddHBox(ID);
    } else if (fxID == "Workspace") {
        AddWorkspace(ID);
    } else if (fxID == "Spinner") {
        AddSpinner(ID);
    } else if (fxID == "EditableSpinner") {
        AddEditableSpinner(ID);
    } else if (fxID == "PasswordField") {
        AddPasswordField(ID);
    } else if (fxID == "ProgressIndicator") {
        AddProgressIndicator(ID);
    } else if (fxID == "ProgressBar") {
        AddProgressBar(ID);
    } else {
        throw std::invalid_argument("Invalid ID: " + fxID);
    }
}

void GUI::CreateItem(std::unique_ptr<Item>& item) {
    ItemsInDrawingOrder.push_back(item.get());
    Items.insert({item->ID, std::move(item)});
    needsSorting = true;
}

void GUI::CreateItemID(std::unique_ptr<Item>& item, const std::string& ID) {
    if (ID.empty()) {
        size_t currentCount = ItemsCounter.at(item->GetFxID());
        std::string newID = currentCount == 0 ? item->GetFxID()
                                    : item->GetFxID() + std::to_string(currentCount);
        item->ID = "GUI_AUTO_" + newID;
    } else item->ID = ID;
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

//--- Remover ---

void GUI::RemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        for (auto& [key, item] : Items) {
            if (IsContainer(item->GetFxID())) {
                GetContainer(item->ID)->SafeRemoveItem(ID);
            }
        }
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&ID](const Item* item) { return item->ID == ID; }),ItemsInDrawingOrder.end());
        Items.erase(ID);
    } else {
        throw std::invalid_argument("Item " + ID + " doesn't exist");
    }
}

bool GUI::IsIDTaken(const std::string& ID) const {
    return Items.contains(ID);
}

bool GUI::IsItemContainer(const std::string& ID) const {
    if (!IsIDTaken(ID)) {
        throw std::out_of_range("No item with " + ID + " found!");
    }
    return IsContainer(Items.at(ID)->GetFxID());
}

bool GUI::IsContainer(const std::string& fxID) const {
    static const std::unordered_set<std::string> CONTAINERS = {"AnchorPane", "VBox", "HBox", "Workspace"};
    return CONTAINERS.contains(fxID);
}

bool GUI::IsFxID(const std::string& fxID) const {
    static const std::unordered_set<std::string> FXIDs = {"Label", "Button", "TextField", "CheckBox", "DropDown", "AnchorPane", "VBox", "HBox", "Workspace", "Spinner", "EditableSpinner",
    "PasswordField", "ProgressBar", "ProgressIndicator"};
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

//--- Priority helpers ---

void GUI::SortOrder() {
    std::sort(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [](const Item* a, const Item* b) {
        return a->priority > b->priority;
    });
}

void GUI::SetHighestPriority(const std::string& ID) {
    Item* item = nullptr;
    try {
        item = Items.at(ID).get();
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No item with the ID " + ID + " exists");
    }
    size_t highestPriority = ItemsInDrawingOrder.back()->priority;
    item->SetPriority(highestPriority);
    needsSorting = true;
}

void GUI::SetAboveHighestPriority(const std::string& ID) {
    Item* item = nullptr;
    try {
        item = Items.at(ID).get();
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No item with the ID " + ID + " exists");
    }
    size_t highestPriority = ItemsInDrawingOrder.back()->priority;
    item->SetPriority(highestPriority);
    item->MoveUpPriority();
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
