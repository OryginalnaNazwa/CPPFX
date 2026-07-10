#include "bases.hpp"
#include <algorithm>  // for remove_if, find, sort
#include <compare>    // for operator<
#include <iterator>   // for distance
#include <stdexcept>  // for out_of_range, invalid_argument
#include <numeric>    // for numberic_limits

using namespace CPPFX;

// --- Text Item ---

std::string TextItem::Truncate(const std::string& text) const {
        if (text.empty()) return text;
        if (MeasureText(text.c_str(), font.GetFontSize()) + textMargin > width) {
            std::string truncated = "";
            for (const auto& c : text) {
                const std::string characterString(1,c);
                if ((float)(MeasureText((truncated + "..." + characterString).c_str(), font.GetFontSize())) > width) {//may have to switch to MeasureTextEx with fonts
                    return truncated + "...";
                }
                truncated += c;
            }
            return truncated;
        } else return text;
    }

void TextItem::ExpandToText() {
    float textWidth = MeasureText(text.c_str(), font.GetFontSize());
    if (textWidth > width) {
        width = textWidth + textMargin;
    }

    if (font.GetFontSize() > height) {
        height += font.GetFontSize() - height + textMargin;
    }
}

void TextItem::FitToText() {
    float textWidth = MeasureText(text.c_str(), font.GetFontSize());
    if (textWidth != width) {
        width = textWidth + textMargin;
    }

    if (font.GetFontSize() != height) {
        height = font.GetFontSize() + textMargin;
    }
}

void TextItem::SetText(const std::string& text) {
    this->text = text;
}

void TextItem::ClearText() {
    text = "";
}

std::string TextItem::GetText() const {
    return text;
}

void TextItem::SetTextMargin(float margin) {
    if (margin < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative text margin.");
    } else textMargin = margin;
}

float TextItem::GetTextMargin() const {
    return textMargin;
}

const std::string TextItem::GetClassID() {
    return "TextItem";
}

//--- Container ---

const std::unordered_set<std::string> Container::CONTAINERS = {"AnchorPane", "VBox", "HBox", "Workspace"};

bool Container::IsContainer(const std::string& fxID) const {
    return CONTAINERS.contains(fxID);
}

void Container::DoNotOrder() {
    needsOrdering = false;
}

void Container::Order() {
    needsOrdering = true;
}

void Container::SetX(float x) {
    Item::SetX(x);
    needsOrdering = true;
}
void Container::SetY(float y) {
    Item::SetY(y);
    needsOrdering = true;
}

void Container::SetWidth(float value) {
    Item::SetWidth(value);
    needsOrdering = true;
}

void Container::SetHeight(float value) {
    Item::SetHeight(value);
    needsOrdering = true;
}

void Container::MakeInvisible() {
    Item::MakeInvisible();
    for (auto& [key, item] : Items) {
        item->MakeInvisible();
    }
}

void Container::MakeVisible() {
    Item::MakeVisible();
    for (auto& [key, item] : Items) {
        item->MakeVisible();
    }
}

void Container::MakeActive() {
    Item::MakeActive();
    for (auto& [key, item] : Items) {
        item->MakeActive();
    }
}

void Container::MakeInactive() {
    Item::MakeInactive();
    for (auto& [key, item] : Items) {
        item->MakeInactive();
    }
}

void Container::Show() {
    Item::Show();
    for (auto& [key, item] : Items) {
        item->Show();
    }
}

void Container::Hide() {
    Item::Hide();
    for (auto& [key, item] : Items) {
        item->Hide();
    }
}

void Container::SetVisible(bool flag) {
    Item::SetVisible(flag);
    for (auto& [key, item] : Items) {
        item->SetVisible(flag);
    }
}

void Container::SetInactive(bool flag) {
    Item::SetInactive(flag);
    for (auto& [key, item] : Items) {
        item->SetInactive(flag);
    }
}

void Container::ConsumeClicks() {
    Item::ConsumeClicks();
    for (auto& [key, item] : Items) {
        item->ConsumeClicks();
    }
}
void Container::LetClicksThrough() {
    Item::LetClicksThrough();
    for (auto& [key, item] : Items) {
        item->LetClicksThrough();
    }
}

void Container::SetToWorld() {
    Item::SetToWorld();
    for (auto& [key, item] : Items) {
        item->SetToWorld();
    }
}

void Container::SetToScreen() {
    Item::SetToScreen();
    for (auto& [key, item] : Items) {
        item->SetToScreen();
    }
}

void Container::AddItem(Item* item) {
    if (!item) {
        CPPFX_THROW(std::invalid_argument, "Null pointer - cannot add item.");
    }
    if (!IsIDTaken(item->GetID())) {
        Items.insert({item->GetID(), item});
        ItemsInDrawingOrder.push_back(item);
    } else {
        CPPFX_THROW(std::invalid_argument, "Item already in container - cannot add item.");
    }
    needsOrdering = true;
}

void Container::RemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&ID](const Item* item) { return item->GetID() == ID; }),ItemsInDrawingOrder.end());
        Items.erase(ID);
    } else {
        CPPFX_THROW(std::invalid_argument, "Item is not in container - cannot remove item.");
    }
    needsOrdering = true;
}

void Container::RemoveItem(const Item* item) {
    if (item) {
        if (IsIDTaken(item->GetID())) {
            ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&item](const Item* i) { return i->GetID() == item->GetID(); }),ItemsInDrawingOrder.end());
            Items.erase(item->GetID());
        } else {
            CPPFX_THROW(std::out_of_range, "Item " + item->GetID() + " is not in the container - cannot remove.");
        }
    } else CPPFX_THROW(std::invalid_argument, "Null pointer - cannot remove item.");
    needsOrdering = true;
}

void Container::SafeRemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),
                                [&ID](const Item* item) { return item->GetID() == ID; }), ItemsInDrawingOrder.end());
        Items.erase(ID);
        needsOrdering = true;
    }
}

void Container::SafeRemoveItem(const Item* item) {
    if (item && IsIDTaken(item->GetID())) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),
                                [&item](const Item* i) { return i->GetID() == item->GetID(); }), ItemsInDrawingOrder.end());
        Items.erase(item->GetID());
        needsOrdering = true;
    }
}

bool Container::IsIDTaken(const std::string& ID) const {
    return Items.contains(ID);
}

void Container::DoPassiveAction(float elapsedTime) {
    if (needsOrdering) {
        SetPositionsOfItems();
        needsOrdering = false; // needs to be here, SetPositionsOfItems is fully virtual.
    }
}

void Container::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Container::DoFocusAction(float elapsedTime) {
    Defocus();
    return;
}

void Container::ExpandToChildren() {
    if (Items.empty()) CPPFX_THROW(std::runtime_error, "There are no children to expand to.");

    float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();
    for (const auto& [key, item] : Items) {
        if (item->GetX() + item->GetTotalWidth() > maxX) maxX = item->GetX() + item->GetTotalWidth();
        if (item->GetY() + item->GetTotalHeight() > maxY) maxY = item->GetY() + item->GetTotalHeight();
    }
    if (maxX > xAnchor + width) width = maxX - xAnchor;
    if (maxY > yAnchor + height) height = maxY - yAnchor;
}

void Container::FitToChildren() {
    if (Items.empty()) CPPFX_THROW(std::runtime_error, "There are no children to fit to.");

    float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();
    for (const auto& [key, item] : Items) {
        if (item->GetX() + item->GetTotalWidth() > maxX) maxX = item->GetX() + item->GetTotalWidth();
        if (item->GetY() + item->GetTotalHeight() > maxY) maxY = item->GetY() + item->GetTotalHeight();
    }
    width = maxX - xAnchor;
    height = maxY - yAnchor;
}

const std::string Container::GetClassID() {
    return "Container";
}

    // --- Box ---

void Box::SetPadding(float value) {
    Padded::SetPadding(value);
    needsOrdering = true;
}

const std::string Box::GetClassID() {
    return "Box";
}

// --- Chart ---

void Chart::AddElement(const std::string& label, double value) {
    labels.push_back(label);
    values.push_back(value);
}

void Chart::AddElement(double value) {
    values.push_back(value);
}

double Chart::GetElement(const std::string& label) const {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        return values[index];
    } else CPPFX_THROW(std::out_of_range, "There is no label " + label + " - unable to get element");
}

double Chart::GetElement(int index) const {
    if (index < 0 || size_t(index) >= values.size()) {
        CPPFX_THROW(std::out_of_range, "Invalid index - cannot get element");
    } else return values[index];
}

void Chart::SetElement(const std::string& label, double newValue) {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        values[index] = newValue;
    } else CPPFX_THROW(std::out_of_range, "There is no label " + label + " - unable to set element");
}

void Chart::SetElement(int index, double newValue) {
    if (index < 0 || size_t(index) >= values.size()) {
        CPPFX_THROW(std::out_of_range, "Invalid index - cannot set element");
    } else values[index] = newValue;
}

void Chart::RemoveElement(const std::string& label) {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        values.erase(values.begin() + index);
        labels.erase(it);
    } else CPPFX_THROW(std::out_of_range, "There is no label " + label + " - unable to remove element");
}

void Chart::RemoveElement(int index) {
    if (index < 0 || size_t(index) >= values.size()) {
        CPPFX_THROW(std::out_of_range, "Invalid index - cannot remove element");
    } else {
        values.erase(values.begin() + index);
        if (size_t(index) < labels.size()) {
            labels.erase(labels.begin() + index);
        }
    }
}

size_t Chart::GetLabelsSize() const {
    return labels.size();
}

size_t Chart::GetValuesSize() const {
    return values.size();
}

const std::string Chart::GetClassID() {
    return "Chart";
}

// --- Shape ---

void Shape::DoFocusAction(float elapsedTime) {
    return;
}

const std::string Shape::GetClassID() {
    return "Shape";
}
