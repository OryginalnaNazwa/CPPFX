#include "items.h"

using namespace CPPFX;

// --- Items ---
// --- Item ---

void Item::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
}

void Item::DrawMyself(float dt, const Camera2D& camera) const {
    float savedX = xAnchor, savedY = yAnchor;
    xAnchor = camera.target.x + (xAnchor - camera.offset.x) / camera.zoom;
    yAnchor = camera.target.y + (yAnchor - camera.offset.y) / camera.zoom;
    DrawMyself(dt);
    xAnchor = savedX;
    yAnchor = savedY;
}

bool Item::WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const {
    float height = this->height * camera.zoom;
    float width = this->width * camera.zoom;
    int xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

bool Item::WasIClicked(const Vector2& mousePosition) const {
    int xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

void Item::DoFocusAction(float dt, const Vector2& mousePosition) {
    return DoFocusAction(dt);
}

void Item::DoFocusAction(float dt, const Vector2& mousePosition, const Camera2D& camera) {
    return DoFocusAction(dt, mousePosition);
}

void Item::DoFocusAction(float dt, const Camera2D& camera) {
    return DoFocusAction(dt);
}

void Item::DoPassiveAction(float dt) {
    return;
}

void Item::DoPassiveAction(float dt, const Camera2D& camera) {
    return DoPassiveAction(dt);
}

// --- Helpers ---


// --- Setters ---
void Item::SetX(float x) {
    xAnchor = x;
}
void Item::SetY(float y) {
    yAnchor = y;
}

void Item::SetHeight(float value) {
    if (value < 0) {
        throw std::invalid_argument("In item " + ID + ": Negative height.");
    } else {
        height = value;
    }
}

void Item::SetWidth(float value) {
    if (value < 0) {
        throw std::invalid_argument("In item " + ID + ": Negative width.");
    } else {
        width = value;
    }
}

void Item::SetPriority(int value) {
    if (value < 0) {
        throw std::invalid_argument("In item " + ID + ": Negative priority.");
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

void Item::SetID(const std::string& id) {
    if (id.empty()) {
        throw std::invalid_argument("In item: " + ID + ": New ID would be empty.");
    }
    ID = id;
}

void Item::Focus() {
    focused = true;
}

void Item::Defocus() {
    focused = false;
}

void Item::MakeInactive() {
    inactive = true;
}

void Item::MakeActive() {
    inactive = false;
}

void Item::SetInactive(const bool& flag) {
    inactive = flag;
}

void Item::MakeInvisible() {
    visible = false;
}

void Item::MakeVisible() {
    visible = true;
}

void Item::SetVisible(const bool& flag) {
    visible = flag;
}

void Item::Hide() {
    inactive = true;
    visible = false;
}

void Item::Show() {
    inactive = false;
    visible = true;
}

void Item::SetToWorld() {
    screenBased = false;
}

void Item::SetToScreen() {
    screenBased = true;
}

void Item::ConsumeClicks() {
    eatsClick = true;
}

void Item::LetClicksThrough() {
    eatsClick = false;
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

float Item::GetTotalHeight() const {
    return height;
}

float Item::GetWidth() const {
    return width;
}

float Item::GetTotalWidth() const {
    return width;
}

std::string Item::GetFxID() const {
    return fxID;
}

size_t Item::GetPriority() const {
    return priority;
}

std::string Item::GetID() const {
    return ID;
}

bool Item::IsFocused() const {
    return focused;
}

bool Item::IsInactive() const {
    return inactive;
}

bool Item::IsScreenBased() const {
    return screenBased;
}

bool Item::DoesEatClicks() const {
    return eatsClick;
}

bool Item::IsVisible() const {
    return visible;
}

//end Item

std::string TextItem::Truncate(const std::string& text) const {
    const static float BLINKER_PAD = 2; //makes sure that the blinker won't get out
    if (MeasureText((text + "|").c_str(), font.GetFontSize()) + textMargin + BLINKER_PAD > width) {
        std::string truncated = "";
        for (const auto& c : text) {
            const std::string characterString(1,c);
            if (MeasureText((truncated + characterString + "_...|").c_str(), font.GetFontSize()) + BLINKER_PAD > width) {//may have to switch to MeasureTextEx with fonts //'_' - little hack to make sure the blinker will stay inbound
                return truncated + "...";
            }
            truncated += c;
        }
        return truncated;
    } else return text;
}

void TextItem::ExpandToText() {
    int textWidth = MeasureText(text.c_str(), font.GetFontSize());
    if (textWidth > width) {
        width = textWidth + textMargin;
    }

    if (font.GetFontSize() > height) {
        height += font.GetFontSize() - height + textMargin;
    }
}

void TextItem::FitToText() {
    int textWidth = MeasureText(text.c_str(), font.GetFontSize());
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
    if (margin < 0) {
        textMargin = 0;
        throw std::invalid_argument("Negative text margin value in " + ID + " type " + fxID);
    } else textMargin = margin;
}

float TextItem::GetTextMargin() const {
    return textMargin;
}

//--- Text Field ---

void TextField::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text == "") {
        if (promptText != "" && !focused) {
            DrawText(Truncate(promptText).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), GREY);
        }
    } else {
        std::string truncated = Truncate(text);
        DrawText(truncated.c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
        if (focused && fmod(dt, 1.0f) < 0.5f) { //checks whether half a second passed
            DrawText("|", MeasureText(truncated.c_str(), font.GetFontSize()) + xAnchor + textMargin + 2, yAnchor + (height / 2) - (font.GetFontSize() / 2),
                      font.GetFontSize(), font.colour.GetColour());
        }
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void TextField::DoFocusAction(float dt) {
    if (focused) {
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
void Label::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text != "") {
        float startX = xAnchor, startY = yAnchor, textOffset = 0, yTextOffSet = 0;
        float textWidth = MeasureText(text.c_str(), font.GetFontSize());
        switch (alignment.GetAlignment()) {
            case Alignment::Alignments::TOP_LEFT:
                startX = xAnchor; startY = yAnchor; textOffset = textMargin; yTextOffSet = textMargin; break;
            case Alignment::Alignments::TOP_CENTRE:
                startX = xAnchor + (width / 2); startY = yAnchor; textOffset = -textWidth; yTextOffSet = textMargin; break;
            case Alignment::Alignments::TOP_RIGHT:
                startX = xAnchor + width; startY = yAnchor; textOffset = -textMargin - textWidth; yTextOffSet = textMargin; break;
            case Alignment::Alignments::CENTRE_LEFT:
                startX = xAnchor; startY = yAnchor + (height / 2); textOffset = textMargin; yTextOffSet = -font.GetFontSize() / 2; break;
            case Alignment::Alignments::CENTRE:
                startX = xAnchor + (width / 2); startY = yAnchor + (height / 2); textOffset = -(textWidth / 2); yTextOffSet = -font.GetFontSize() / 2; break;
            case Alignment::Alignments::CENTRE_RIGHT:
                startX = xAnchor + width; startY = yAnchor + (height / 2); textOffset = -textMargin - textWidth; yTextOffSet = -font.GetFontSize() / 2; break;
            case Alignment::Alignments::BOTTOM_LEFT:
                startX = xAnchor; startY = yAnchor + height; textOffset = textMargin; yTextOffSet = -font.GetFontSize(); break;
            case Alignment::Alignments::BOTTOM_CENTRE:
                startX = xAnchor + (width / 2); startY = yAnchor + height; textOffset = -(textWidth / 2); yTextOffSet = -font.GetFontSize(); break;
            case Alignment::Alignments::BOTTOM_RIGHT:
                startX = xAnchor + width; startY = yAnchor + height; textOffset = -textMargin - textWidth; yTextOffSet = -font.GetFontSize(); break;
        }
       DrawText(text.c_str(), startX + textOffset, startY + yTextOffSet, font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Label::DoFocusAction(float dt) {
    Defocus();
    return;
}

//--- Button ---
void Button::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text != "") {
       DrawText(text.c_str(), xAnchor + (width / 2) - (MeasureText(text.c_str(), font.GetFontSize()) / 2), yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Button::DoFocusAction(float dt) {
    if (focused) {
        if (timer > 0) {
            if (dt > timer) {
                colour.SetColour(unPressedColour.GetColour());
                timer = 0;
                focused = false;
            }
        } else {
            timer = dt + 0.1f;
            colour.SetColour(pressedColour.GetColour());
        }
    } else {
        timer = 0;
    }
}

void CheckBox::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text != "") {
       DrawText(text.c_str(), xAnchor + width + textMargin + labelMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
    clickBorder.DrawMyself(xAnchor, yAnchor, width, height);
    if (pressed && drawsX) { //TODO add other shapes options
        DrawLineEx({xAnchor, yAnchor}, {xAnchor + width, yAnchor + height}, width / 10, unPressedColour.GetColour());
        DrawLineEx({xAnchor, yAnchor + height}, {xAnchor + width, yAnchor}, width / 10, unPressedColour.GetColour());
    }
}

void CheckBox::DoFocusAction(float dt) {
   if (focused) {
        if (pressed) {
            colour.SetColour(unPressedColour.GetColour());
            pressed = false;
        } else {
            colour.SetColour(pressedColour.GetColour());
            pressed = true;
        }
        focused = false;
   }
}

void CheckBox::SetLabelMargin(float value) {
    if (value >= 0) {
        labelMargin = value;
    } else {
        throw std::invalid_argument("In " + this->ID + ": negative value of label margin.");
    }
}

float CheckBox::GetLabelMargin() const {
    return labelMargin;
}

float CheckBox::GetTotalWidth() const {
    return width + labelMargin + (2 * textMargin) + MeasureText(text.c_str(), font.GetFontSize());
}

void CheckBox::SetPressed(const bool& flag) {
    pressed = flag;
}

void CheckBox::Press() {
    pressed = true;
}

void CheckBox::Unpress() {
    pressed = false;
}

void CheckBox::SwitchPress() {
    pressed = !pressed;
}

bool CheckBox::IsPressed() const {
    return pressed;
}

//--- Container ---

void Container::AddItem(Item* item) {
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
}

void Container::SafeRemoveItem(const std::string& ID) {
    if (IsIDTaken(ID)) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [&ID](const Item* item) { return item->GetID() == ID; }), ItemsInDrawingOrder.end());
        Items.erase(ID);
    }
}

void Container::SafeRemoveItem(const Item* item) {
    if (item && IsIDTaken(item->GetID())) {
        ItemsInDrawingOrder.erase(std::remove_if(ItemsInDrawingOrder.begin(), ItemsInDrawingOrder.end(), [&item](const Item* i) { return i->GetID() == item->GetID(); }), ItemsInDrawingOrder.end());
        Items.erase(item->GetID());
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

void Container::DoPassiveAction(float dt) {
    if (needsSorting) {
        SortOrder();
    }
    if (needsOrdering) {
        SetPositionsOfItems();
        needsOrdering = false; // needs to be here, SetPositionsOfItems is fully virtual.
    }
}

void Container::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Box::SetPadding(float value) {
    if (value < 0) {
        throw std::invalid_argument("Error: Negative padding in " + ID);
    }
    padding = value;
    needsOrdering = true;
}

float Box::GetPadding() const {
    return padding;
}

void Workspace::SetPositionsOfItems() {
    for (auto& item : ItemsInDrawingOrder) {
        if (item->GetX() < this->xAnchor) {
            item->SetX(this->xAnchor);
        } else if (item->GetX() > this->xAnchor + this->width) {
            item->SetX(this->xAnchor + this->width - item->GetWidth());
        }
        if (item->GetY() < this->yAnchor) {
            item->SetY(this->yAnchor);
        } else if (item->GetY() > this->yAnchor + this->height) {
            item->SetY(this->yAnchor + this->height - item->GetHeight());
        }
    }
}

void AnchorPane::SetPositionsOfItems() {
    for (auto& item : ItemsInDrawingOrder) {
        switch (alignment.GetAlignment()) {
            case Alignment::Alignments::TOP_LEFT:
                item->SetX(item->GetX() - previousX + xAnchor);
                item->SetY(item->GetY() - previousY + yAnchor);
                break;
            case Alignment::Alignments::TOP_CENTRE:
                item->SetX(item->GetX() - (previousX + previousWidth / 2) + (xAnchor + width / 2));
                item->SetY(item->GetY() - previousY + yAnchor);
                break;
            case Alignment::Alignments::TOP_RIGHT:
                item->SetX(item->GetX() - (previousX + previousWidth) + (xAnchor + width));
                item->SetY(item->GetY() - previousY + yAnchor);
                break;
            case Alignment::Alignments::CENTRE_LEFT:
                item->SetX(item->GetX() - previousX + xAnchor);
                item->SetY(item->GetY() - (previousY + previousHeight / 2) + (yAnchor + height / 2));
                break;
            case Alignment::Alignments::CENTRE:
                item->SetX(item->GetX() - (previousX + previousWidth / 2) + (xAnchor + width / 2));
                item->SetY(item->GetY() - (previousY + previousHeight / 2) + (yAnchor + height / 2));
                break;
            case Alignment::Alignments::CENTRE_RIGHT:
                item->SetX(item->GetX() - (previousX + previousWidth) + (xAnchor + width));
                item->SetY(item->GetY() - (previousY + previousHeight / 2) + (yAnchor + height / 2));
                break;
            case Alignment::Alignments::BOTTOM_LEFT:
                item->SetX(item->GetX() - previousX + xAnchor);
                item->SetY(item->GetY() - (previousY + previousHeight) + (yAnchor + height));
                break;
            case Alignment::Alignments::BOTTOM_CENTRE:
                item->SetX(item->GetX() - (previousX + previousWidth / 2) + (xAnchor + width / 2));
                item->SetY(item->GetY() - (previousY + previousHeight) + (yAnchor + height));
                break;
            case Alignment::Alignments::BOTTOM_RIGHT:
                item->SetX(item->GetX() - (previousX + previousWidth) + (xAnchor + width));
                item->SetY(item->GetY() - (previousY + previousHeight) + (yAnchor + height));
                break;
        }
    }
}

void AnchorPane::SetX(float x) {
    if (!Items.empty()) {
        needsOrdering = true;
    }
    previousX = xAnchor;
    xAnchor = x;
}
void AnchorPane::SetY(float y) {
    if (!Items.empty()) {
        needsOrdering = true;
    }
    previousY = yAnchor;
    yAnchor = y;
}

void AnchorPane::SetHeight(float value) {
    if (value < 0) {
        throw std::invalid_argument("In item " + ID + ": Negative height.");
    } else {
        if (!Items.empty()) {
            needsOrdering = true;
        }
        previousHeight = height;
        height = value;
    }
}

void AnchorPane::SetWidth(float value) {
    if (value < 0) {
        throw std::invalid_argument("In item " + ID + ": Negative height.");
    } else {
        if (!Items.empty()) {
           needsOrdering = true;
        }
        previousWidth = width;
        width = value;
    }
}

void Container::DoFocusAction(float dt) {
    Defocus();
    return;
}

void VBox::SetPositionsOfItems() {
    /*float currentPadding = padding;
    for (auto& item : ItemsInDrawingOrder) {
        item->SetX(xAnchor);
        item->SetY(yAnchor + currentPadding);
        currentPadding += padding + item->height;
    }*/
    float startX = xAnchor, startY = yAnchor;
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
            itemOffsetX = -item->GetWidth();
            itemOffsetY = 0;
        } else if (alignment.IsCentreAlignment()) {
            itemOffsetX = -item->GetWidth() / 2;
            itemOffsetY = 0;
        } else if (alignment.IsBottomAlignment()) {
            itemOffsetY = -item->GetHeight();
        } else {
            itemOffsetY = 0;
            itemOffsetX = 0;
        }
        item->SetX(startX + itemOffsetX); //TODO Look into it

        item->SetY(currentY + itemOffsetY);

        currentY += padding + item->GetHeight();
        if (currentY > yAnchor + height) {
            height = currentY - yAnchor;
        }
    }
}

float VBox::GetTotalHeight() const {
    float heightSum = 0;
    for (auto& child : Items) {
        heightSum += child.second->GetTotalHeight();
        heightSum += padding;
    }
    return heightSum;
}

void HBox::SetPositionsOfItems() {
    /*float currentPadding = padding;
    for (auto& item : ItemsInDrawingOrder) {
        item->SetY(yAnchor);
        item->SetX(xAnchor + currentPadding);
        currentPadding += padding + item->width;
    }*/
    float startX = xAnchor, startY = yAnchor;
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
        currentX += padding + item->GetWidth();
        if (currentX > xAnchor + width) {
            width = currentX - xAnchor;
        }
    }
}

float HBox::GetTotalWidth() const {
    float widthSum = 0;
    for (auto& child : Items) {
        widthSum += child.second->GetTotalWidth();
        widthSum += padding;
    }
    return widthSum;
}

//--- Spinner ---

void Spinner::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    DrawText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value), xAnchor + valueMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());

    incrementButton.DrawMyself(dt);
    decrementButton.DrawMyself(dt);

    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Spinner::DrawMyself(float dt, const Camera2D& camera) const {
    float xAnchor = camera.target.x + this->xAnchor / camera.zoom;
    float yAnchor = camera.target.y + this->yAnchor / camera.zoom;

    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    DrawText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value), xAnchor + valueMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());

    //shift the buttons to where they should be
    const float savedButtonX = incrementButton.GetX();
    const float savedIncButtonY = incrementButton.GetY(), savedDecButtonY = decrementButton.GetY();
    incrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetX(this->xAnchor + width * camera.zoom);
    incrementButton.SetY(this->yAnchor);
    decrementButton.SetY(this->yAnchor + (height / 2) * camera.zoom);
    incrementButton.Item::DrawMyself(dt, camera);
    decrementButton.Item::DrawMyself(dt, camera);
    incrementButton.SetX(savedButtonX);
    decrementButton.SetX(savedButtonX);
    incrementButton.SetY(savedIncButtonY);
    decrementButton.SetY(savedDecButtonY);

    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Spinner::DoFocusAction(float dt, const Vector2& mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.Focus();
        if (incrementButton.onClick) incrementButton.onClick();
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.Focus();
        if (decrementButton.onClick) decrementButton.onClick();
    }
    decrementButton.DoFocusAction(dt);
    incrementButton.DoFocusAction(dt);
}

void Spinner::DoFocusAction(float dt, const Vector2& mousePosition, const Camera2D& camera) {
    //shift the buttons to where they should be, translation happens in WasIClicked
    const float savedButtonX = incrementButton.GetX(), savedButtonHeight = incrementButton.GetHeight();
    const float savedDecButtonY = decrementButton.GetY();
    incrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetY(this->yAnchor + savedButtonHeight * camera.zoom);
    incrementButton.SetHeight(savedButtonHeight * camera.zoom);
    decrementButton.SetHeight(savedButtonHeight * camera.zoom);

    if (incrementButton.WasIClicked(mousePosition, camera)) {
        incrementButton.Focus();
        if (incrementButton.onClick) incrementButton.onClick();
    } else if (decrementButton.WasIClicked(mousePosition, camera)) {
        decrementButton.Focus();
        if (decrementButton.onClick) decrementButton.onClick();
    }
    incrementButton.SetX(savedButtonX);
    decrementButton.SetX(savedButtonX);
    decrementButton.SetY(savedDecButtonY);
    incrementButton.SetHeight(savedButtonHeight);
    decrementButton.SetHeight(savedButtonHeight);

    decrementButton.Item::DoFocusAction(dt, camera);
    incrementButton.Item::DoFocusAction(dt, camera);
}

void Spinner::DoFocusAction(float dt) {
    incrementButton.DoFocusAction(dt);
    decrementButton.DoFocusAction(dt);
}

bool Spinner::WasIClicked(const Vector2& mousePosition) const {
    int xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width + incrementButton.GetWidth()) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

bool Spinner::WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const {
    float scaledHeight = height * camera.zoom;
    float scaledWidth = (width + incrementButton.GetWidth()) * camera.zoom;
    int xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + scaledWidth) && yClick >= yAnchor && yClick <= (yAnchor + scaledHeight)) {
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

void Spinner::SetValue(float value) {
    this->value = value;
    if ((hasMin && value < minValue) || (hasMax && value > maxValue)) {
        CPPFX_WARN("In Spinner " + ID + ": value set outside range.");
    }
}

void Spinner::SetStep(float step) {
    stepValue = step;
    if (hasMin && hasMax && step > (maxValue - minValue)) {
        CPPFX_WARN("In Spinner " + ID + ": step is more than the range from min to max.");
    }
}

float Spinner::GetValue() const {
    return value;
}

float Spinner::GetStep() const {
    return stepValue;
}

void Spinner::SetX(float x) {
    xAnchor = x;
    incrementButton.SetX(xAnchor + width);
    decrementButton.SetX(xAnchor + width);
}

void Spinner::SetY(float y) {
    yAnchor = y;
    incrementButton.SetY(yAnchor);
    decrementButton.SetY(yAnchor + (height / 2));
}

void Spinner::SetWidth(float value) {
    if (value < 0) {
        throw std::invalid_argument("In Spinner " + ID + ": Negative width.");
    }
    width = value;
    incrementButton.SetX(xAnchor + width);
    decrementButton.SetX(xAnchor + width);
}

void Spinner::SetHeight(float value) {
    if (value < 0) {
        throw std::invalid_argument("In Spinner " + ID + ": Negative width.");
    }
    height = value;
    incrementButton.SetHeight(height / 2);
    decrementButton.SetHeight(height / 2);
    decrementButton.SetY(yAnchor + (height / 2));
}

void Spinner::SetValueMargin(float value) {
    if (value < 0) {
        throw std::invalid_argument("In Spinner " + ID + ": Negative value margin.");
    }
    valueMargin = value;
}

float Spinner::GetValueMargin() const {
    return valueMargin;
}

void Spinner::AllowWrap() {
    allowWrap = true;
}

void Spinner::StopWrapping() {
    allowWrap = false;
}

void Spinner::SetWrap(const bool flag) {
    allowWrap = flag;
}

bool Spinner::IsWrapAllowed() const {
    return allowWrap;
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

void Spinner::SetButtonsWidth(float value) {
    if (value < 0) {
        throw std::invalid_argument("In spinner " + ID + ": Negative width of buttons.");
    }
    incrementButton.SetWidth(value);
    decrementButton.SetWidth(value);
}

void Spinner::SetMaxValue(float value) {
    maxValue = value;
    if (minValue > maxValue) {
        CPPFX_WARN("minValue greater than maxValue in Spinner " + ID);
    }
}

void Spinner::SetMinValue(float value) {
    minValue = value;
    if (minValue > maxValue) {
        CPPFX_WARN("minValue greater than maxValue in Spinner " + ID);
    }
}

void Spinner::SetToWorld() {
    screenBased = false;
    incrementButton.SetToWorld();
    decrementButton.SetToWorld();
}

void Spinner::SetToScreen() {
    screenBased = true;
    incrementButton.SetToScreen();
    decrementButton.SetToScreen();
}

float Spinner::GetMaxValue() const {
    return maxValue;
}

float Spinner::GetMinValue() const {
    return minValue;
}

void Spinner::SetMaxLimit(const bool& flag) {
    hasMax = flag;
}

void Spinner::SetMax() {
    hasMax = true;
}

bool Spinner::HasMax() const {
    return hasMax;
}

void Spinner::SetMinLimit(const bool& flag) {
    hasMin = flag;
}

void Spinner::SetMin() {
    hasMin = true;
}

bool Spinner::HasMin() const {
    return hasMin;
}

bool Spinner::IsAtMax() const {
    if (!hasMax) return false;
    return maxValue == value;
}

bool Spinner::IsAtMin() const {
    if (!hasMin) return false;
    return minValue == value;
}

float Spinner::GetTotalWidth() const {
    return width + incrementButton.GetTotalWidth();
}

//--- EditableSpinner ---

void EditableSpinner::SetValue(float value) {
    this->value = value;
    editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
}

void EditableSpinner::DrawMyself(float dt) const {
    editArea.DrawMyself(dt);
    incrementButton.DrawMyself(dt);
    decrementButton.DrawMyself(dt);

    border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
}

void EditableSpinner::DrawMyself(float dt, const Camera2D& camera) const {
    float xAnchor = camera.target.x + this->xAnchor / camera.zoom;
    float yAnchor = camera.target.y + this->yAnchor / camera.zoom;

    editArea.Item::DrawMyself(dt, camera);
    //shift the buttons to where they should be
    const float savedButtonX = incrementButton.GetX();
    const float savedIncButtonY = incrementButton.GetY(), savedDecButtonY = decrementButton.GetY();
    incrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetX(this->xAnchor + width * camera.zoom);
    incrementButton.SetY(this->yAnchor);
    decrementButton.SetY(this->yAnchor + (height / 2) * camera.zoom);

    incrementButton.Item::DrawMyself(dt, camera);
    decrementButton.Item::DrawMyself(dt, camera);

    incrementButton.SetX(savedButtonX);
    decrementButton.SetX(savedButtonX);
    incrementButton.SetY(savedIncButtonY);
    decrementButton.SetY(savedDecButtonY);

    border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
}

void EditableSpinner::DoFocusAction(float dt, const Vector2& mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.Focus();
        if (incrementButton.onClick) incrementButton.onClick();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
        editArea.Defocus();
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.Focus();
        editArea.Defocus();
        if (decrementButton.onClick) decrementButton.onClick();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else if (editArea.WasIClicked(mousePosition)) {
        editArea.Focus();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else {
        if (editArea.IsFocused()) {
            try {
                value = std::stof(editArea.GetText());
            } catch (const std::invalid_argument&) {
                //do nothing. Doesn't change the value.
            }
            editArea.Defocus();
        }
    }
    decrementButton.DoFocusAction(dt);
    incrementButton.DoFocusAction(dt);
    editArea.DoFocusAction(dt);
}

void EditableSpinner::DoFocusAction(float dt, const Vector2& mousePosition, const Camera2D& camera) {
    //shift the buttons to where they should be, translation happens in WasIClicked
    const float savedButtonX = incrementButton.GetX(), savedButtonHeight = incrementButton.GetHeight();
    const float savedDecButtonY = decrementButton.GetY();
    incrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetY(this->yAnchor + savedButtonHeight * camera.zoom);
    incrementButton.SetHeight(savedButtonHeight * camera.zoom);
    decrementButton.SetHeight(savedButtonHeight * camera.zoom);

    if (incrementButton.WasIClicked(mousePosition, camera)) {
        incrementButton.Focus();
        if (incrementButton.onClick) incrementButton.onClick();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
        editArea.Defocus();
    } else if (decrementButton.WasIClicked(mousePosition, camera)) {
        decrementButton.Focus();
        editArea.Defocus();
        if (decrementButton.onClick) decrementButton.onClick();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else if (editArea.WasIClicked(mousePosition, camera)) {
        editArea.Focus();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else {
        if (editArea.IsFocused()) {
            try {
                value = std::stof(editArea.GetText());
            } catch (const std::invalid_argument&) {
                //do nothing. Doesn't change the value.
            }
            editArea.Defocus();
        }
    }

    incrementButton.SetX(savedButtonX);
    decrementButton.SetX(savedButtonX);
    decrementButton.SetY(savedDecButtonY);
    incrementButton.SetHeight(savedButtonHeight);
    decrementButton.SetHeight(savedButtonHeight);

    decrementButton.Item::DoFocusAction(dt, camera);
    incrementButton.Item::DoFocusAction(dt, camera);
    editArea.Item::DoFocusAction(dt, camera);
}

void EditableSpinner::DoFocusAction(float dt) {
    if (!IsFocused()) editArea.Defocus();
    incrementButton.DoFocusAction(dt);
    decrementButton.DoFocusAction(dt);
    editArea.DoFocusAction(dt);
}

void EditableSpinner::SetX(float x) {
    Spinner::SetX(x);
    editArea.SetX(xAnchor);
}

void EditableSpinner::SetY(float y) {
    Spinner::SetY(y);
    editArea.SetY(yAnchor);
}

void EditableSpinner::SetWidth(float value) {
    Spinner::SetWidth(value);
    editArea.SetWidth(width);
}

void EditableSpinner::SetHeight(float value) {
    Spinner::SetHeight(value);
    editArea.SetHeight(height);
}

void EditableSpinner::SetValueMargin(float value) {
    if (value < 0) {
        throw std::invalid_argument("In Spinner " + ID + ": Negative value margin.");
    }
    editArea.SetTextMargin(value);
}

float EditableSpinner::GetValueMargin() const {
    return editArea.GetTextMargin();
}

void EditableSpinner::Defocus() {
    editArea.Defocus();
    focused = false;
    try {
        value = std::stof(editArea.GetText());
    } catch (const std::invalid_argument&) {
        return; //no need to reset the value if it didn't change
    }
    editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
}

void EditableSpinner::SetToWorld() {
    editArea.SetToWorld();
    Spinner::SetToWorld();
}

void EditableSpinner::SetToScreen() {
    editArea.SetToScreen();
    Spinner::SetToScreen();
}

//--- PasswordField ---

void PasswordField::DrawMyself(float dt) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text == "") {
        if (promptText != "" && !focused) {
            DrawText(Truncate(promptText).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), GREY);
        }
    } else {
        std::string password(text.size(), mask);
        std::string truncated = Truncate(password);
        DrawText(Truncate(truncated).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
        if (focused && fmod(dt, 1.0f) < 0.5f) {
            DrawText("|", MeasureText(truncated.c_str(), font.GetFontSize()) + xAnchor + textMargin + 2, yAnchor + (height / 2) - (font.GetFontSize() / 2),
                      font.GetFontSize(), font.colour.GetColour());
        }
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void PasswordField::SetMask(const char character) {
    mask = character;
}

char PasswordField::GetMask() const {
    return mask;
}

//--- ProgressIndicator ---

void ProgressIndicator::SetProgress(float value) {
    if ((value > 1 || value < 0) && value != -1) {
        throw std::out_of_range("Progress in ProgressIndicator " + ID + " beyond the valid range - " + std::to_string(value));
    } else {
        this->value = value;
    }
}

void ProgressIndicator::IncreaseProgress(float change) {
    value += change;
    if (value > 1) {
        value = 1;
    } else if (value < 0) {
        value = 0;
    }
}

void ProgressIndicator::DecreaseProgress(float change) {
    value -= change;
    if (value < 0) {
        value = 0;
    }
}

float ProgressIndicator::GetProgress() const {
    return value;
}

int ProgressIndicator::GetProgressPercentage() const {
    if (value == -1) return value;
    return (int)(value * 100);
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

void ProgressIndicator::DoFocusAction(float dt) {
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

void ProgressIndicator::DrawMyself(float dt) const {
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
        DrawText(valueString.c_str(), xAnchor + (width / 2) - (MeasureText(valueString.c_str(), font.GetFontSize()) / 2), yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
}

void ProgressIndicator::DoDisplayValue() {
    displayValue = true;
}

void ProgressIndicator::DoNotDisplayValue() {
    displayValue = false;
}

void ProgressIndicator::SetDisplayValue(const bool& flag) {
    displayValue = flag;
}

bool ProgressIndicator::IsDisplayingValue() const {
    return displayValue;
}

// ProgressBar

void ProgressBar::DrawMyself(float dt) const {
    if (shape != Shapes::BAR) {
        ProgressIndicator::DrawMyself(dt);
        return;
    }
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
        DrawText(valueString.c_str(), xAnchor + (width / 2) - (MeasureText(valueString.c_str(), font.GetFontSize()) / 2), yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
}

void ProgressBar::SetBarMargin(float value) {
    if (value < 0) {
        barMargin = 0;
        throw std::invalid_argument("Bar margin value in " + this->ID + " is lower than 0.");
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

void ProgressBar::SetNumberOfSegments(int value) {
    if (value < 0) {
        numberOfSegments = 0;
        throw std::invalid_argument("Number of segments in " + this->ID + " is lower than 0.");
    } else {
        numberOfSegments = value;
    }
}

int ProgressBar::GetNumberOfSegments() const {
    return numberOfSegments;
}

void ProgressBar::SetGapBetweenSegments(float value) {
    if (value < 0) {
        gapBetweenSegments = 0;
        throw std::invalid_argument("Gap between segments in " + this->ID + " is lower than 0.");
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

void ProgressBar::SetSegments(int number, float gap) {
    SetNumberOfSegments(number);
    SetGapBetweenSegments(gap);
}

void ProgressBar::Segmented() {
    segmented = true;
}

void ProgressBar::Solid() {
    segmented = false;
}

void ProgressBar::SetSegemented(const bool& flag) {
    segmented = flag;
}

bool ProgressBar::IsSegmented() const {
    return segmented;
}

// Pressed button

void PressedButton::DrawMyself(float dt) const {
    std::string textToDisplay;
    if (pressed) {
        DrawRectangle(xAnchor, yAnchor, width, height, pressedColour.GetColour());
        textToDisplay = pressedText;
    } else {
        DrawRectangle(xAnchor, yAnchor, width, height, unPressedColour.GetColour());
        textToDisplay = text;
    }

    if (textToDisplay != "") {
       DrawText(textToDisplay.c_str(), xAnchor + (width / 2) - (MeasureText(textToDisplay.c_str(), font.GetFontSize()) / 2), yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
};

void PressedButton::DoFocusAction(float dt) {
    return;
}

void PressedButton::DoFocusAction(float dt, const Vector2& mousePosition) {
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

void PressedButton::SetPressed(const bool& flag) {
    pressed = flag;
}

void PressedButton::Press() {
    pressed = true;
}

void PressedButton::Unpress() {
    pressed = false;
}

void PressedButton::SwitchPress() {
    pressed = !pressed;
}

bool PressedButton::IsPressed() const {
    return pressed;
}

// ------- Chart ---------

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
    if (index < 0 || size_t(index) > values.size()) {
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

void Chart::RemoveElement(int index) {
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

// ---------- Pie chart -----------

void PieChart::DoFocusAction(float dt) {
    return;
}

void PieChart::DrawMyself(float dt) const {
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    std::vector<Color> colours = {BLUE, RED, GREEN, PINK, BROWN, DARKGREEN, PURPLE}; //TODO Add user colours

    border.DrawMyself(xAnchor, yAnchor, width, height);

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
            float xShift = 0;
            float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified

            std::string valueString = percent * 100 == (int)(percent * 100) ? TextFormat("%d", (int)(percent * 100)) : TextFormat("%.1f", percent * 100);
            valueString += "%";

            if (textAngle < 90) {
                xShift = MeasureText(valueString.c_str(), font.GetFontSize());
            }

            DrawText(valueString.c_str(), xAnchor + ((width / 2) * cos(textAngle * DEG2RAD)) - (MeasureText(valueString.c_str(), font.GetFontSize()) / 2) + xShift, yAnchor + ((width / 2) * sin(textAngle * DEG2RAD)), font.GetFontSize(), font.colour.GetColour());
        }
        if (showLabels) {
            float xShift = 0, yShift = 0;
            float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified

            if (textAngle > 180) {
                yShift = -font.GetFontSize();
            }

            if (values.size() == labels.size()) {
                std::string label = labels[index];
                if (textAngle > 90 && textAngle < 270) {
                    xShift = -MeasureText(label.c_str(), font.GetFontSize());
                } else if (textAngle < 90) {
                    xShift = MeasureText(label.c_str(), font.GetFontSize());
                }

                DrawText(label.c_str(), xAnchor + (width  * cos(textAngle * DEG2RAD)) - (MeasureText(label.c_str(), font.GetFontSize()) / 2) + xShift, yAnchor + (width * sin(textAngle * DEG2RAD)) + yShift, font.GetFontSize(), font.colour.GetColour());
            } else {
                std::string elementText = std::to_string(element);
                DrawText(elementText.c_str(), xAnchor + (width  * cos(textAngle * DEG2RAD)) - (MeasureText(elementText.c_str(), font.GetFontSize()) / 2) + xShift, yAnchor + (width * sin(textAngle * DEG2RAD)) + yShift, font.GetFontSize(), font.colour.GetColour());
            }
        }
        currentAngle += 360 * percent;
        index++;
    }
}

void PieChart::DoShowLabels() {
    showLabels = true;
}

void PieChart::DoNotShowLabels() {
    showLabels = false;
}

void PieChart::SetShowingLabels(const bool& flag) {
    showLabels = flag;
}

bool PieChart::IsShowingLabels() const {
    return showLabels;
}

void PieChart::DoShowPercentage() {
    showPercentage =  true;
}

void PieChart::DoNotShowPercentage() {
    showPercentage = false;
}

void PieChart::SetShowingPercentage(const bool& flag) {
    showPercentage = flag;
}

bool PieChart::IsShowingPercentage() const {
    return showPercentage;
}
