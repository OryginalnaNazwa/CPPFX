#include "items.h"

using namespace CPPFX;

// --- Items ---
// --- Item ---

void Item::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
}

bool Item::WasIClicked(const Vector2& mousePosition) const {
    int xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

void Item::DoActiveAction(const float& dt, const Vector2& mousePosition) {
    return DoActiveAction(dt);
}

void Item::DoPassiveAction(const float& dt) {
    return;
}

void Item::Deactivate() {
    active = false;
}

void Item::Hide() {
    inactive = true;
    visible = false;
}

void Item::Show() {
    inactive = false;
    visible = true;
}

// --- Helpers ---


// --- Setters ---
void Item::SetX(const float& x) {
    xAnchor = x;
}
void Item::SetY(const float& y) {
    yAnchor = y;
}

void Item::SetHeight(const float& value) {
    if (value < 0) {
        throw std::range_error("Range error: Negative height.");
    } else {
        height = value;
    }
}

void Item::SetWidth(const float& value) {
    if (value < 0) {
        throw std::range_error("Range error: Negative width.");
    } else {
        width = value;
    }
}

void Item::SetPriority(const int& value) {
    if (value < 0) {
        priority = 0;
        throw std::range_error("Range error: Negative priority.");
    } else {
        priority = value;
    }
}

void Item::MoveUpPriority() {
    if (priority > 0) {
        priority--;
    }
}

void Item::MoveDownPriority() {
    priority++;
}

// --- Getters ---
float Item::GetX() const {
    return xAnchor;
}

float Item::GetY() const {
    return yAnchor;
}

float Item::GetHeight() const {
    return height;
}

float Item::GetWidth() const {
    return width;
}

std::string Item::GetFxID() const {
    return fxID;
}

size_t Item::GetPriority() const {
    return priority;
}

std::string TextItem::Truncate(const std::string& text) const {
    if (MeasureText(text.c_str(), font.fontSize) + MeasureText("|", font.fontSize) + textMargin + 2 > width) {//makes sure that the blinker won't get out
        std::string truncated = "";
        for (const auto& c : text) {
            const std::string characterString(1,c);
            if (MeasureText(truncated.c_str(), font.fontSize) + MeasureText("_...|", font.fontSize) + MeasureText(characterString.c_str(), font.fontSize) + 2 > width) {//may have to switch to MeasureTextEx with fonts //'_' - little hack to make sure the blinker will stay inbound
                return truncated + "...";
            }
            truncated += c;
        }
        return truncated;
    } else return text;
}

void TextItem::ExpandToText() {
    int textWidth = MeasureText(text.c_str(), font.fontSize);
    if (textWidth > width) {
        width = textWidth + textMargin;
    }

    if (font.fontSize > height) {
        height += font.fontSize - height + textMargin;
    }
}

void TextItem::FitToText() {
    int textWidth = MeasureText(text.c_str(), font.fontSize);
    if (textWidth != width) {
        width = textWidth + textMargin;
    }

    if (font.fontSize != height) {
        height = font.fontSize + textMargin;
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

//--- Text Field ---

void TextField::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text == "") {
        if (promptText != "" && !active) {
            DrawText(Truncate(promptText).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, GREY);
        }
    } else {
        std::string truncated = Truncate(text);
        DrawText(truncated.c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
        if (active && fmod(dt, 1.0f) < 0.5f) { //checks whether half a second passed
            DrawText("|", MeasureText(truncated.c_str(), font.fontSize) + xAnchor + textMargin + 2, yAnchor + (height / 2) - (font.fontSize / 2),
                      font.fontSize, font.colour.GetColour());
        }
    }
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void TextField::DoActiveAction(const float& dt) {
    if (active) {
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (text != "") {
                text.erase(text.size() - 1);
            }
        } else if (IsKeyDown(KEY_BACKSPACE)) {
            if (text != "") {
                if (fmod(dt, 0.1f) < GetFrameTime()) {
                    text.erase(text.size() - 1);
                }
            }
        } else {
            int ch = GetCharPressed();
            if (ch != 0) text += (char)ch;
        }

    }
}

void TextField::SetPromptText(const std::string& text) {
    promptText = text;
}

void TextField::ClearPromptText() {
    promptText = "";
}

std::string TextField::GetPromptText() const {
    return promptText;
}

//--- Label ---
void Label::DrawMyself(const float& dt) const {
    if (text != "") {
        float startX, startY, textOffset = 0, yTextOffSet = 0;
        float textWidth = MeasureText(text.c_str(), font.fontSize);
        switch (alignment.GetAlignment()) {
            case Alignment::Alignments::TOP_LEFT:
                startX = xAnchor; startY = yAnchor; textOffset = textMargin; yTextOffSet = textMargin; break;
            case Alignment::Alignments::TOP_CENTRE:
                startX = xAnchor + (width / 2); startY = yAnchor; textOffset = -textWidth; yTextOffSet = textMargin; break;
            case Alignment::Alignments::TOP_RIGHT:
                startX = xAnchor + width; startY = yAnchor; textOffset = -textMargin - textWidth; yTextOffSet = textMargin; break;
            case Alignment::Alignments::CENTRE_LEFT:
                startX = xAnchor; startY = yAnchor + (height / 2); textOffset = textMargin; yTextOffSet = -font.fontSize / 2; break;
            case Alignment::Alignments::CENTRE:
                startX = xAnchor + (width / 2); startY = yAnchor + (height / 2); textOffset = -(textWidth / 2); yTextOffSet = -font.fontSize / 2; break;
            case Alignment::Alignments::CENTRE_RIGHT:
                startX = xAnchor + width; startY = yAnchor + (height / 2); textOffset = -textMargin - textWidth; yTextOffSet = -font.fontSize / 2; break;
            case Alignment::Alignments::BOTTOM_LEFT:
                startX = xAnchor; startY = yAnchor + height; textOffset = textMargin; yTextOffSet = -font.fontSize; break;
            case Alignment::Alignments::BOTTOM_CENTRE:
                startX = xAnchor + (width / 2); startY = yAnchor + height; textOffset = -(textWidth / 2); yTextOffSet = -font.fontSize; break;
            case Alignment::Alignments::BOTTOM_RIGHT:
                startX = xAnchor + width; startY = yAnchor + height; textOffset = -textMargin - textWidth; yTextOffSet = -font.fontSize; break;
        }
       DrawText(text.c_str(), startX + textOffset, startY + yTextOffSet, font.fontSize, font.colour.GetColour());
    }
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void Label::DoActiveAction(const float& dt) {
    return;
}

//--- Button ---
void Button::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text != "") {
       DrawText(text.c_str(), xAnchor + (width / 2) - (MeasureText(text.c_str(), font.fontSize) / 2), yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
    }
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void Button::DoActiveAction(const float& dt) {
    if (active) {

        if (timer > 0) {
            if (dt > timer) {
                colour.SetColour(unPressedColour.GetColour());
                timer = 0;
                active = false;
            }
        } else {
            timer = dt + 0.1f;
            colour.SetColour(pressedColour.GetColour());
        }
    } else {
        timer = 0;
    }
}

void CheckBox::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text != "") {
       DrawText(text.c_str(), xAnchor + width + textMargin + labelMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
    }
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
    }
    if (pressed && drawsX) { //TODO add other shapes options
        DrawLineEx({xAnchor, yAnchor}, {xAnchor + width, yAnchor + height}, width / 10, unPressedColour.GetColour());
        DrawLineEx({xAnchor, yAnchor + height}, {xAnchor + width, yAnchor}, width / 10, unPressedColour.GetColour());
    }
}

void CheckBox::DoActiveAction(const float& dt) {
   if (active) {
        if (pressed) {
            colour.SetColour(unPressedColour.GetColour());
            pressed = false;
        } else {
            colour.SetColour(pressedColour.GetColour());
            pressed = true;
        }
        active = false;
   }
}

void CheckBox::SetLabelMargin(const float& value) {
    if (value >= 0) {
        labelMargin = value;
    } else {
        labelMargin = 0;
        throw std::invalid_argument("In " + this->ID + ": negative value of label margin.");
    }
}

float CheckBox::GetLabelMargin() const {
    return labelMargin;
}

float CheckBox::GetTotalWidth() const {
    return width + labelMargin + (2 * textMargin) + MeasureText(text.c_str(), font.fontSize);
}

//--- Container ---

void Container::AddItem(Item* item) {
    if (!IsIDTaken(item->ID)) {
        Items.insert({item->ID, item});
        ItemsInDrawingOrder.push_back(item);
    } else {
        throw std::invalid_argument("Item " + item->ID + " is already in the container " + ID);
    }
    needsSorting = true;
    needsOrdering = true;
}

void Container::RemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(),[&ID](const Item* item) { return item->ID == ID; }),ItemsInDrawingOrder.end());
        Items.erase(ID);
    } else {
        throw std::invalid_argument("Item " + ID + " is not in the container " + this->ID + " or doesn't exist");
    }
}

void Container::SafeRemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [&ID](const Item* item) { return item->ID == ID; }), ItemsInDrawingOrder.end());
        Items.erase(ID);
    }
}

bool Container::IsIDTaken(const std::string& ID) const {
    return Items.contains(ID);
}

void Container::SortOrder() {
    std::sort(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [](const Item* a, const Item* b) {
        return a->priority > b->priority;
    });
    needsSorting = false;
}

void Container::DoPassiveAction(const float& dt) {
    if (needsSorting) {
        SortOrder();
    }
    if (needsOrdering) {
        SetPositionsOfItems();
        needsOrdering = false;
    }
}

void Box::SetPadding(const float& value) {
    if (value < 0) {
        throw std::out_of_range("Error: Negative padding in " + ID);
    }
    padding = value;
    needsOrdering = true;
}

void Workspace::SetPositionsOfItems() {
    for (auto& item : ItemsInDrawingOrder) {
        if (item->xAnchor < this->xAnchor) {
            item->SetX(this->xAnchor);
        } else if (item->xAnchor > this->xAnchor + this->width) {
            item->SetX(this->xAnchor + this->width - item->width);
        }
        if (item->yAnchor < this->yAnchor) {
            item->SetY(this->yAnchor);
        } else if (item->yAnchor > this->yAnchor + this->height) {
            item->SetY(this->yAnchor + this->height - item->height);
        }
    }
}

void AnchorPane::SetPositionsOfItems() {
    for (auto& item : ItemsInDrawingOrder) {
        switch (alignment.GetAlignment()) {
            case Alignment::Alignments::TOP_LEFT:
                item->SetX(item->xAnchor - previousX + xAnchor);
                item->SetY(item->yAnchor - previousY + yAnchor);
                break;
            case Alignment::Alignments::TOP_CENTRE:
                item->SetX(item->xAnchor - (previousX + previousWidth / 2) + (xAnchor + width / 2));
                item->SetY(item->yAnchor - previousY + yAnchor);
                break;
            case Alignment::Alignments::TOP_RIGHT:
                item->SetX(item->xAnchor - (previousX + previousWidth) + (xAnchor + width));
                item->SetY(item->yAnchor - previousY + yAnchor);
                break;
            case Alignment::Alignments::CENTRE_LEFT:
                item->SetX(item->xAnchor - previousX + xAnchor);
                item->SetY(item->yAnchor - (previousY + previousHeight / 2) + (yAnchor + height / 2));
                break;
            case Alignment::Alignments::CENTRE:
                item->SetX(item->xAnchor - (previousX + previousWidth / 2) + (xAnchor + width / 2));
                item->SetY(item->yAnchor - (previousY + previousHeight / 2) + (yAnchor + height / 2));
                break;
            case Alignment::Alignments::CENTRE_RIGHT:
                item->SetX(item->xAnchor - (previousX + previousWidth) + (xAnchor + width));
                item->SetY(item->yAnchor - (previousY + previousHeight / 2) + (yAnchor + height / 2));
                break;
            case Alignment::Alignments::BOTTOM_LEFT:
                item->SetX(item->xAnchor - previousX + xAnchor);
                item->SetY(item->yAnchor - (previousY + previousHeight) + (yAnchor + height));
                break;
            case Alignment::Alignments::BOTTOM_CENTRE:
                item->SetX(item->xAnchor - (previousX + previousWidth / 2) + (xAnchor + width / 2));
                item->SetY(item->yAnchor - (previousY + previousHeight) + (yAnchor + height));
                break;
            case Alignment::Alignments::BOTTOM_RIGHT:
                item->SetX(item->xAnchor - (previousX + previousWidth) + (xAnchor + width));
                item->SetY(item->yAnchor - (previousY + previousHeight) + (yAnchor + height));
                break;
        }
    }
}

void AnchorPane::SetX(const float& x) {
    if (!Items.empty()) {
        previousX = xAnchor;
    }

    xAnchor = x;
}
void AnchorPane::SetY(const float& y) {
    if (!Items.empty()) {
        previousY = yAnchor;
    }
    yAnchor = y;
}

void AnchorPane::SetHeight(const float& value) {
    if (value < 0) {
        throw std::range_error("Range error: Negative height.");
    } else {
        if (!Items.empty()) {
            previousHeight = height;
        }
        height = value;
    }
}

void AnchorPane::SetWidth(const float& value) {
    if (value < 0) {
        throw std::range_error("Range error: Negative width.");
    } else {
        if (!Items.empty()) {
            previousWidth = width;
        }
        width = value;
    }
}

void AnchorPane::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void AnchorPane::DoActiveAction(const float& dt) {
    return;
}

void Workspace::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void Workspace::DoActiveAction(const float& dt) {
    return;
}

void Box::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void Box::DoActiveAction(const float& dt) {
    return;
}

void VBox::SetPositionsOfItems() {
    /*float currentPadding = padding;
    for (auto& item : ItemsInDrawingOrder) {
        item->SetX(xAnchor);
        item->SetY(yAnchor + currentPadding);
        currentPadding += padding + item->height;
    }*/
    float startX, startY;
    switch (alignment.GetAlignment()) {
        case Alignment::Alignments::TOP_LEFT:
            startX = xAnchor; startY = yAnchor; break;
        case Alignment::Alignments::TOP_CENTRE:
            startX = xAnchor + (width / 2); startY = yAnchor; break;
        case Alignment::Alignments::TOP_RIGHT:
            startX = xAnchor + width; startY = yAnchor; break;
        case Alignment::Alignments::CENTRE_LEFT:
            startX = xAnchor; startY = yAnchor + (height / 2); break;
        case Alignment::Alignments::CENTRE:
            startX = xAnchor + (width / 2); startY = yAnchor + (height / 2); break;
        case Alignment::Alignments::CENTRE_RIGHT:
            startX = xAnchor + width; startY = yAnchor + (height / 2); break;
        case Alignment::Alignments::BOTTOM_LEFT:
            startX = xAnchor; startY = yAnchor + height; break;
        case Alignment::Alignments::BOTTOM_CENTRE:
            startX = xAnchor + (width / 2); startY = yAnchor + height; break;
        case Alignment::Alignments::BOTTOM_RIGHT:
            startX = xAnchor + width; startY = yAnchor + height; break;
    }
    float currentY = startY;
    float itemOffsetX = 0, itemOffsetY = 0;
    for (auto& item : ItemsInDrawingOrder) {
        if (alignment.IsRightAlignment()) {
            itemOffsetX = -item->width;
            itemOffsetY = 0;
        } else if (alignment.IsCentreAlignment()) {
            itemOffsetX = -item->width / 2;
            itemOffsetY = 0;
        } else if (alignment.IsBottomAlignment()) {
            itemOffsetY = -item->height;
        } else {
            itemOffsetY = 0;
            itemOffsetX = 0;
        }
        item->SetX(startX + itemOffsetX); //TODO Look into it

        item->SetY(currentY + itemOffsetY);

        currentY += padding + item->height;
        if (currentY > yAnchor + height) {
            height = currentY - yAnchor;
        }
    }
}

void HBox::SetPositionsOfItems() {
    /*float currentPadding = padding;
    for (auto& item : ItemsInDrawingOrder) {
        item->SetY(yAnchor);
        item->SetX(xAnchor + currentPadding);
        currentPadding += padding + item->width;
    }*/
    float startX, startY;
    switch (alignment.GetAlignment()) {
        case Alignment::Alignments::TOP_LEFT:
            startX = xAnchor; startY = yAnchor; break;
        case Alignment::Alignments::TOP_CENTRE:
            startX = xAnchor + (width / 2); startY = yAnchor; break;
        case Alignment::Alignments::TOP_RIGHT:
            startX = xAnchor + width; startY = yAnchor; break;
        case Alignment::Alignments::CENTRE_LEFT:
            startX = xAnchor; startY = yAnchor + (height / 2); break;
        case Alignment::Alignments::CENTRE:
            startX = xAnchor + (width / 2); startY = yAnchor + (height / 2); break;
        case Alignment::Alignments::CENTRE_RIGHT:
            startX = xAnchor + width; startY = yAnchor + (height / 2); break;
        case Alignment::Alignments::BOTTOM_LEFT:
            startX = xAnchor; startY = yAnchor + height; break;
        case Alignment::Alignments::BOTTOM_CENTRE:
            startX = xAnchor + (width / 2); startY = yAnchor + height; break;
        case Alignment::Alignments::BOTTOM_RIGHT:
            startX = xAnchor + width; startY = yAnchor + height; break;
    }

    float currentX = startX;
    for (auto& item : ItemsInDrawingOrder) { //TODO Make it work properly
        item->SetX(currentX);
        item->SetY(startY);
        currentX += padding + item->width;
        if (currentX > xAnchor + width) {
            width = currentX - xAnchor;
        }
    }
}

//--- Spinner ---

void Spinner::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    DrawText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value), xAnchor + valueMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());

    incrementButton.DrawMyself(dt);
    decrementButton.DrawMyself(dt);

    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void Spinner::DoActiveAction(const float& dt, const Vector2& mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.active = true;
        incrementButton.onClick();
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.active = true;
        decrementButton.onClick();
    }
    decrementButton.DoActiveAction(dt);
    incrementButton.DoActiveAction(dt);
}

void Spinner::DoActiveAction(const float& dt) {
    incrementButton.DoActiveAction(dt);
    decrementButton.DoActiveAction(dt);
}

bool Spinner::WasIClicked(const Vector2& mousePosition) const {
    int xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width + incrementButton.width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

void Spinner::Increment() {
    value += stepValue;
    if (hasMax) {
        if (value > maxValue) {
            if (allowWrap && hasMin) {
                value = minValue;
            } else value = std::min(value, maxValue);
        }
    }
}

void Spinner::Decrement() {
    value -= stepValue;
    if (hasMin) {
        if (value < minValue) {
            if (allowWrap && hasMax) {
                value = maxValue;
            } else value = std::max(value, minValue);
        }
    }
}

void Spinner::SetValue(const float& value) {
    this->value = value;
}

void Spinner::SetStep(const float& step) {
    stepValue = step;
}

float Spinner::GetValue() const {
    return value;
}

float Spinner::GetStep() const {
    return stepValue;
}

void Spinner::SetX(const float& x) {
    xAnchor = x;
    incrementButton.SetX(xAnchor + width);
    decrementButton.SetX(xAnchor + width);
}

void Spinner::SetY(const float& y) {
    yAnchor = y;
    incrementButton.SetY(yAnchor);
    decrementButton.SetY(yAnchor + (height / 2));
}

void Spinner::SetWidth(const float& value) {
    width = value;
    incrementButton.SetX(xAnchor + width);
    decrementButton.SetX(xAnchor + width);
}

void Spinner::SetHeight(const float& value) {
    height = value;
    incrementButton.SetHeight(height / 2);
    decrementButton.SetHeight(height / 2);
    decrementButton.SetY(yAnchor + (height / 2));
}

void Spinner::Initialise() {
    incrementButton.SetX(xAnchor + width);
    incrementButton.SetY(yAnchor);
    incrementButton.SetHeight(height / 2);
    incrementButton.SetWidth(50);
    incrementButton.SetText("/\\");
    incrementButton.border.SetThickness(2);
    incrementButton.onClick = [this]() { Increment(); };
    decrementButton.SetX(xAnchor + width);
    decrementButton.SetY(yAnchor + (height / 2));
    decrementButton.SetHeight(height / 2);
    decrementButton.SetWidth(50);
    decrementButton.SetText("\\/");
    decrementButton.border.SetThickness(2);
    decrementButton.onClick = [this]() { Decrement(); };
    border.SetThickness(2);
}

void Spinner::SetButtonsWidth(const float& value) {
    if (value < 0) {
        throw std::range_error("In Spinner " + this->ID + ": negative buttons width.");
    }
    incrementButton.SetWidth(value);
    decrementButton.SetWidth(value);
}

void Spinner::SetMax(const float& value) {
    maxValue = value;
}

void Spinner::SetMin(const float& value) {
    minValue = value;
}

//--- EditableSpinner ---

void EditableSpinner::SetValue(const float& value) {
    this->value = value;
    editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
}

void EditableSpinner::DrawMyself(const float& dt) const {
    if (!editArea.active) {
       DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
       DrawText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value), xAnchor + valueMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
    } else {
        editArea.DrawMyself(dt);
    }
    incrementButton.DrawMyself(dt);
    decrementButton.DrawMyself(dt);

    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

void EditableSpinner::DoActiveAction(const float& dt, const Vector2& mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.active = true;
        incrementButton.onClick();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
        editArea.active = false;
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.active = true;
        editArea.active = false;
        decrementButton.onClick();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else if (editArea.WasIClicked(mousePosition)) {
        editArea.active = true;
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else {
        if (editArea.active) {
            try {
                value = std::stof(editArea.GetText());
            } catch (const std::invalid_argument&) {
                //do nothing. Doesn't change the value.
            }
            editArea.active = false;
        }
    }
    decrementButton.DoActiveAction(dt);
    incrementButton.DoActiveAction(dt);
    editArea.DoActiveAction(dt);
}

void EditableSpinner::DoActiveAction(const float& dt) {
    if (!active) editArea.active = false;
    incrementButton.DoActiveAction(dt);
    decrementButton.DoActiveAction(dt);
    editArea.DoActiveAction(dt);
}

void EditableSpinner::SetX(const float& x) {
    xAnchor = x;
    incrementButton.SetX(xAnchor + width);
    decrementButton.SetX(xAnchor + width);
    editArea.SetX(xAnchor);
}

void EditableSpinner::SetY(const float& y) {
    yAnchor = y;
    incrementButton.SetY(yAnchor);
    decrementButton.SetY(yAnchor + (height / 2));
    editArea.SetY(yAnchor);
}

void EditableSpinner::SetWidth(const float& value) {
    width = value;
    incrementButton.SetX(xAnchor + width);
    decrementButton.SetX(xAnchor + width);
    editArea.SetWidth(width);
}

void EditableSpinner::SetHeight(const float& value) {
    height = value;
    incrementButton.SetHeight(height / 2);
    decrementButton.SetHeight(height / 2);
    decrementButton.SetY(yAnchor + (height / 2));
    editArea.SetHeight(height);
}

void EditableSpinner::Deactivate() {
    editArea.active = false;
    active = false;
    try {
        value = std::stof(editArea.GetText());
    } catch (const std::invalid_argument&) {
        return; //no need to reset the value if it didn't change
    }
    editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
}

//--- PasswordField ---

void PasswordField::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text == "") {
        if (promptText != "" && !active) {
            DrawText(Truncate(promptText).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, GREY);
        }
    } else {
        std::string password(text.size(), '*');
        std::string truncated = Truncate(password);
        DrawText(Truncate(truncated).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
        if (active && fmod(dt, 1.0f) < 0.5f) {
            DrawText("|", MeasureText(truncated.c_str(), font.fontSize) + xAnchor + textMargin + 2, yAnchor + (height / 2) - (font.fontSize / 2),
                      font.fontSize, font.colour.GetColour());
        }
    }
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
}

//--- ProgressIndicator ---

void ProgressIndicator::SetProgress(const float& value) {
    if ((value > 1 || value < 0) && value != -1) {
        throw std::out_of_range("Progress in ProgressIndicator " + ID + " beyond the valid range - " + std::to_string(value));
    } else {
        this->value = value;
    }
}

void ProgressIndicator::IncreaseProgress(const float& change) {
    value += change;
    if (value > 1) {
        value = 1;
    } else if (value < 0) {
        value = 0;
    }
}

void ProgressIndicator::DecreaseProgress(const float& change) {
    value -= change;
    if (value < 0) {
        value = 0;
    }
}

float ProgressIndicator::GetProgress() const {
    return value;
}

void ProgressIndicator::MakeIndefinite() {
    value = -1;
}

bool ProgressIndicator::IsComplete() const {
    if (value >= 1) {
        return true;
    }
    return false;
}

void ProgressIndicator::DoActiveAction(const float& dt) {
    return;
}

void ProgressIndicator::SetShape(const ProgressIndicator::Shapes& shape) {
    if (shape != ProgressIndicator::Shapes::RING && shape != ProgressIndicator::Shapes::DOTS && shape != ProgressIndicator::Shapes::CIRCLE ) {
        throw std::invalid_argument("Invalid shape");
    }
    this->shape = shape;
}

ProgressIndicator::Shapes ProgressIndicator::StringToShape(const std::string& shape) const {
    static const std::unordered_map<std::string, ProgressIndicator::Shapes> shapeMap = {
        {"RING", ProgressIndicator::Shapes::RING},
        {"CIRCLE", ProgressIndicator::Shapes::CIRCLE},
        {"DOTS", ProgressIndicator::Shapes::DOTS}
    };
    auto it = shapeMap.find(shape);
    if (it != shapeMap.end()) return it->second;
    throw std::invalid_argument("Invalid shape: " + shape);
}

std::string ProgressIndicator::ShapeToString(const ProgressIndicator::Shapes& shape) const {
    switch (shape) {
        case ProgressIndicator::Shapes::RING: return "RING";
        case ProgressIndicator::Shapes::CIRCLE:  return "CIRCLE";
        case ProgressIndicator::Shapes::DOTS: return "DOTS";
        default: throw std::invalid_argument("Invalid shape");
    }
}

void ProgressIndicator::SetShape(const std::string& shape) {
    this->shape = StringToShape(shape);
}

ProgressIndicator::Shapes ProgressIndicator::GetShape() const {
    return shape;
}

std::string ProgressIndicator::GetShapeString() const {
    return ShapeToString(shape);
}

void ProgressIndicator::DrawMyself(const float& dt) const {
    if (value == -1 || shape == Shapes::DOTS) {
        int dotCount = 8;
        float radius = width / 2;
        float dotRadius = radius / 6; //TODO make stuff customisable
        float angle = fmod(dt * 180.0f, 360.0f); // rotation speed
        for (int i = 0; i < dotCount; i++) {
            float dotAngle = angle + (360.0f / dotCount) * i;
            float x = xAnchor + width / 2 + cos(dotAngle * DEG2RAD) * radius;
            float y = yAnchor + height / 2 + sin(dotAngle * DEG2RAD) * radius;
            float alpha = (float)i / dotCount; // fade trail //TODO fix proper fading, proper tail
            DrawCircle(x, y, dotRadius, ColorAlpha(colour.GetColour(), alpha));
        }
    } else if (shape == Shapes::CIRCLE) {
        DrawCircleSector(
            {xAnchor + width / 2, yAnchor + height / 2},
            width / 2,
            0, value * 360.0f,
            32,
            colour.GetColour()
        );
    } else if (shape == Shapes::RING) {
        DrawRing(
            {xAnchor + width / 2, yAnchor + height / 2},
            width / 4,
            width / 2,
            0, value * 360.0f,
            32,
            colour.GetColour()
        );
    }
    if (displayValue && value != -1) {
        std::string valueString = value * 100 == (int)(value * 100) ? TextFormat("%d", (int)(value * 100)) : TextFormat("%.1f", value * 100);
        valueString += "%";
        DrawText(valueString.c_str(), xAnchor + (width / 2) - (MeasureText(valueString.c_str(), font.fontSize) / 2), yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
    }
}

void ProgressBar::DrawMyself(const float& dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (segmented) {
       float segmentWidth = ((width - (2 * barMargin)) - (gapBetweenSegments * (numberOfSegments - 1))) / numberOfSegments;
        float step = segmentWidth + gapBetweenSegments;

        float currentX = xAnchor + barMargin;
        size_t filledSegments = std::min((size_t)(value * numberOfSegments), (size_t)numberOfSegments);
        for (size_t i = 0; i < filledSegments; i++) {
            DrawRectangle(currentX, yAnchor + barMargin, segmentWidth, height - (2 * barMargin), barColour.GetColour());
            currentX += step;
        }
    } else DrawRectangle(xAnchor + barMargin , yAnchor + barMargin, value * (width - (2 * barMargin)), height - (2 * barMargin), barColour.GetColour());

    if (displayValue && value != -1) {
        std::string valueString = value * 100 == (int)(value * 100) ? TextFormat("%d", (int)(value * 100)) : TextFormat("%.1f", value * 100);
        valueString += "%";
        DrawText(valueString.c_str(), xAnchor + (width / 2) - (MeasureText(valueString.c_str(), font.fontSize) / 2), yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
    }
}

void ProgressBar::SetBarMargin(const float& value) {
    if (value < 0) {
        barMargin = 0;
        throw std::out_of_range("Bar margin value in " + this->ID + " is lower than 0.");
    } else if (value > height / 2) {
        barMargin = height / 2;
        throw std::out_of_range("Bar margin value in " + this->ID + " is too big.");
    } else {
        barMargin = value;
    }
}

float ProgressBar::GetBarMargin() const {
    return barMargin;
}

void ProgressBar::SetNumberOfSegments(const int& value) {
    if (value < 0) {
        numberOfSegments = 0;
        throw std::out_of_range("Number of segments in " + this->ID + " is lower than 0.");
    } else {
        numberOfSegments = value;
    }
}

int ProgressBar::GetNumberOfSegments() const {
    return numberOfSegments;
}

void ProgressBar::SetGapBetweenSegments(const float& value) {
    if (value < 0) {
        gapBetweenSegments = 0;
        throw std::out_of_range("Gap between segments in " + this->ID + " is lower than 0.");
    } else if (((width - (2 * barMargin)) / numberOfSegments) + (value * numberOfSegments) > width - (2 * barMargin)) {
        gapBetweenSegments = (width - (2 * barMargin)) / numberOfSegments;
        throw std::out_of_range("Gap between segments in " + this->ID + " is too big.");
    } else {
        gapBetweenSegments = value;
    }
}

float ProgressBar::GetGapBetweenSegments() const {
    return gapBetweenSegments;
}

void ProgressBar::SetSegments(const int& number, const float& gap) {
    SetNumberOfSegments(number);
    SetGapBetweenSegments(gap);
}

void PressedButton::DrawMyself(const float& dt) const {
    std::string textToDisplay;
    if (pressed) {
        DrawRectangle(xAnchor, yAnchor, width, height, pressedColour.GetColour());
        textToDisplay = pressedText;
    } else {
        DrawRectangle(xAnchor, yAnchor, width, height, unPressedColour.GetColour());
        textToDisplay = text;
    }

    if (textToDisplay != "") {
       DrawText(textToDisplay.c_str(), xAnchor + (width / 2) - (MeasureText(textToDisplay.c_str(), font.fontSize) / 2), yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
    }
    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }
};

void PressedButton::DoActiveAction(const float& dt) {
    return;
}

void PressedButton::DoActiveAction(const float& dt, const Vector2& mousePosition) {
    pressed = !pressed;
}

void PressedButton::SetPressedText(const std::string& value) {
    pressedText = value;
}

void PressedButton::ClearPressedText() {
    pressedText = "";
}

std::string PressedButton::GetPressedText() const {
    return pressedText;
}

void Chart::AddElement(const std::string& label, const double& value) {
    labels.push_back(label);
    values.push_back(value);
}

void Chart::AddElement(const double& value) {
    values.push_back(value);
}

double Chart::GetElement(const std::string& label) const {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        return values[index];
    } else throw std::out_of_range("Chart " + this->ID + " at element getting: does not contain a label " + label);
}

double Chart::GetElement(const int& index) const {
    if (index < 0 || size_t(index) > values.size()) {
        throw std::out_of_range("Chart " + this->ID + ": invalid index at getting");
    } else return values[index];
}

void Chart::SetElement(const std::string& label, const double& newValue) {
    auto it = std::find(labels.begin(), labels.end(), label);
    if (it != labels.end()) {
        size_t index = std::distance(labels.begin(), it);
        values[index] = newValue;
    } else throw std::out_of_range("Chart " + this->ID + " in element setting: does not contain a label " + label);
}

void Chart::SetElement(const int& index, const double& newValue) {
    if (index < 0 || size_t(index) > values.size()) {
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

void Chart::RemoveElement(const int& index) {
    if (index < 0 || size_t(index) > values.size()) {
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

void PieChart::DoActiveAction(const float& dt) {
    return;
}

void PieChart::DrawMyself(const float& dt) const {
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    std::vector<Color> colours = {BLUE, RED, GREEN, PINK, BROWN, DARKGREEN, PURPLE}; //TODO Add user colours

    if (border.GetThickness() > 0) {
        border.DrawMyself(xAnchor, yAnchor, width, height);
    }

    float currentAngle = 0;
    size_t index = 0;
    for (auto& element : values) {
        double percent = element / sum;
        Color c;
        if (index >= colours.size()) {
            c = colours[index % colours.size()];
        } else c = colours[index];
        DrawCircleSector(Vector2{xAnchor, yAnchor}, width, currentAngle, currentAngle + (percent * 360), std::max(4, (int)(percent * 36)), c);

        if (showPercentage) {
            float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified
            std::string valueString = percent * 100 == (int)(percent * 100) ? TextFormat("%d", (int)(percent * 100)) : TextFormat("%.1f", percent * 100);
            valueString += "%";
            DrawText(valueString.c_str(), xAnchor + ((width / 2) * cos(textAngle * DEG2RAD)) - (MeasureText(valueString.c_str(), font.fontSize) / 2), yAnchor + ((width / 2) * sin(textAngle * DEG2RAD)), font.fontSize, font.colour.GetColour());
        }
        if (showLabels) {
            if (values.size() == labels.size()) {
                float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified
                DrawText(labels[index].c_str(), xAnchor + (width  * cos(textAngle * DEG2RAD)) - (MeasureText(labels[index].c_str(), font.fontSize) / 2), yAnchor + (width * sin(textAngle * DEG2RAD)), font.fontSize, font.colour.GetColour());
            } else {
                float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified
                std::string elementText = std::to_string(element);
                DrawText(elementText.c_str(), xAnchor + (width  * cos(textAngle * DEG2RAD)) - (MeasureText(elementText.c_str(), font.fontSize) / 2), yAnchor + (width * sin(textAngle * DEG2RAD)), font.fontSize, font.colour.GetColour());
            }
        }
        currentAngle += 360 * percent;
        index++;
    }
}
