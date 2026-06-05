#include "bases.hpp"
#include <algorithm>  // for remove_if, find, sort
#include <compare>    // for operator<
#include <iterator>   // for distance
#include <stdexcept>  // for out_of_range, invalid_argument

using namespace CPPFX;

// --- Text Item ---

std::string TextItem::Truncate(const std::string& text) const {
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
        throw std::invalid_argument("Negative text margin value in " + ID + " type " + fxID);
    } else textMargin = margin;
}

float TextItem::GetTextMargin() const {
    return textMargin;
}

//--- Container ---

void Container::AddItem(Item* item) {
    if (!item) {
        throw std::invalid_argument("In container " + ID + ": null pointer, cannot add item.");
    }
    if (!IsIDTaken(item->GetID())) {
        Items.insert({item->GetID(), item});
        ItemsInDrawingOrder.push_back(item);
    } else {
        throw std::invalid_argument("Item " + item->GetID() + " is already in the container " + ID);
    }
    needsSorting = true;
    needsOrdering = true;
}

void Container::RemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&ID](const Item* item) { return item->GetID() == ID; }),ItemsInDrawingOrder.end());
        Items.erase(ID);
    } else {
        throw std::invalid_argument("Item " + ID + " is not in the container " + this->ID + " or doesn't exist");
    }
    needsOrdering = true;
}

void Container::RemoveItem(const Item* item) {
    if (item) {
        if (IsIDTaken(item->GetID())) {
            ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&item](const Item* i) { return i->GetID() == item->GetID(); }),ItemsInDrawingOrder.end());
            Items.erase(item->GetID());
        } else {
            throw std::out_of_range("In container " + this->ID + ": cannot delete item " + item->GetID() + " as it's not in the container.");
        }
    } else throw std::invalid_argument("In container " + this->ID + ": cannot delete item - doesn't exist.");
    needsOrdering = true;
}

void Container::SafeRemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [&ID](const Item* item) { return item->GetID() == ID; }), ItemsInDrawingOrder.end());
        Items.erase(ID);
        needsOrdering = true;
    }
}

void Container::SafeRemoveItem(const Item* item) {
    if (item && IsIDTaken(item->GetID())) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [&item](const Item* i) { return i->GetID() == item->GetID(); }), ItemsInDrawingOrder.end());
        Items.erase(item->GetID());
        needsOrdering = true;
    }
}

bool Container::IsIDTaken(const std::string& ID) const {
    return Items.contains(ID);
}

void Container::SortOrder() {
    std::sort(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [](const Item* a, const Item* b) {
        return a->GetPriority() > b->GetPriority();
    });
    needsSorting = false;
}

void Container::DoPassiveAction(float elapsedTime) {
    if (needsSorting) {
        SortOrder();
    }
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

    // --- Box ---

void Box::SetPadding(float value) {
    Padded::SetPadding(value);
    needsOrdering = true;
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
    } else throw std::out_of_range("Chart " + this->ID + " at element getting: does not contain a label " + label);
}

double Chart::GetElement(int index) const {
    if (index < 0 || size_t(index) >= values.size()) {
        throw std::out_of_range("Chart " + this->ID + ": invalid index at getting");
    } else return values[index];
}

void Chart::SetElement(const std::string& label, double newValue) {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        values[index] = newValue;
    } else throw std::out_of_range("Chart " + this->ID + " in element setting: does not contain a label " + label);
}

void Chart::SetElement(int index, double newValue) {
    if (index < 0 || size_t(index) >= values.size()) {
        throw std::out_of_range("Chart " + this->ID + ": invalid index at setting element");
    } else values[index] = newValue;
}

void Chart::RemoveElement(const std::string& label) {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        values.erase(values.begin() + index);
        labels.erase(it);
    } else throw std::out_of_range("Chart " + this->ID + " at element removal: does not contain a label " + label);
}

void Chart::RemoveElement(int index) {
    if (index < 0 || size_t(index) >= values.size()) {
        throw std::out_of_range("Chart " + this->ID + ": invalid index at removal");
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

// --- Shape ---

void Shape::DoFocusAction(float elapsedTime) {
    return;
}
