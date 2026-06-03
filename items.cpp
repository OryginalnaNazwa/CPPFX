#include "items.h"

using namespace CPPFX;

//--- Text Field ---

void TextField::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text == "") {
        if (promptText != "" && !focused) {
            DrawText(Truncate(promptText).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), GREY);
        }
    } else {
        std::string truncated = Truncate(text);
        DrawText(truncated.c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
        if (focused && fmod(elapsedTime, 1.0f) < 0.5f) { //checks whether half a second passed
            DrawText("|", MeasureText(truncated.c_str(), font.GetFontSize()) + xAnchor + textMargin + 2, yAnchor + (height / 2) - (font.GetFontSize() / 2),
                      font.GetFontSize(), font.colour.GetColour());
        }
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void TextField::DoFocusAction(float elapsedTime) {
    if (focused) {
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (text != "") {
                text.erase(text.size() - 1);
            }
        } else if (IsKeyDown(KEY_BACKSPACE)) {
            if (text != "") {
                if (fmod(elapsedTime, 0.1f) < GetFrameTime()) {
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

void Label::DrawMyself(float elapsedTime) const {
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

void Label::DoFocusAction(float elapsedTime) {
    Defocus();
    return;
}

//--- Button ---

void Button::DrawMyself(float elapsedTime) const {
    if (timer > 0) {
        DrawRectangle(xAnchor, yAnchor, width, height, pressedColour.GetColour());
    } else DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());

    if (text != "") {
       DrawText(text.c_str(), xAnchor + (width / 2) - (MeasureText(text.c_str(), font.GetFontSize()) / 2), yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Button::DoFocusAction(float elapsedTime) {
    if (focused) {
        if (timer > 0) {
            if (elapsedTime > timer) {
                timer = 0;
                focused = false;
            }
        } else {
            timer = elapsedTime + 0.1f;
        }
    } else {
        timer = 0;
    }
}

// --- CheckBox ---

void CheckBox::DrawMyself(float elapsedTime) const {
    if (!pressed) {
        DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    } else DrawRectangle(xAnchor, yAnchor, width, height, pressedColour.GetColour());

    if (text != "") {
       DrawText(text.c_str(), xAnchor + width + textMargin + labelMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
    clickBorder.DrawMyself(xAnchor, yAnchor, width, height);
    if (pressed && drawsX) { //TODO add other shapes options
        DrawLineEx({xAnchor, yAnchor}, {xAnchor + width, yAnchor + height}, width / 10, colour.GetColour());
        DrawLineEx({xAnchor, yAnchor + height}, {xAnchor + width, yAnchor}, width / 10, colour.GetColour());
    }
}

void CheckBox::DoFocusAction(float elapsedTime) {
   if (focused) {
        pressed = !pressed;
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

// --- Containers ---

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

void Spinner::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    DrawText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value), xAnchor + valueMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());

    incrementButton.DrawMyself(elapsedTime);
    decrementButton.DrawMyself(elapsedTime);

    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Spinner::DrawMyself(float elapsedTime, const Camera2D& camera) const {
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
    incrementButton.Item::DrawMyself(elapsedTime, camera);
    decrementButton.Item::DrawMyself(elapsedTime, camera);
    incrementButton.SetX(savedButtonX);
    decrementButton.SetX(savedButtonX);
    incrementButton.SetY(savedIncButtonY);
    decrementButton.SetY(savedDecButtonY);

    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Spinner::DoFocusAction(float elapsedTime, const Vector2& mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.Focus();
        if (incrementButton.onClick) incrementButton.onClick();
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.Focus();
        if (decrementButton.onClick) decrementButton.onClick();
    }
    decrementButton.DoFocusAction(elapsedTime);
    incrementButton.DoFocusAction(elapsedTime);
}

void Spinner::DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera) {
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

    decrementButton.Item::DoFocusAction(elapsedTime, camera);
    incrementButton.Item::DoFocusAction(elapsedTime, camera);
}

void Spinner::DoFocusAction(float elapsedTime) {
    incrementButton.DoFocusAction(elapsedTime);
    decrementButton.DoFocusAction(elapsedTime);
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

// --- EditableSpinner ---

void EditableSpinner::SetValue(float value) {
    this->value = value;
    editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
}

void EditableSpinner::DrawMyself(float elapsedTime) const {
    editArea.DrawMyself(elapsedTime);
    incrementButton.DrawMyself(elapsedTime);
    decrementButton.DrawMyself(elapsedTime);

    border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
}

void EditableSpinner::DrawMyself(float elapsedTime, const Camera2D& camera) const {
    float xAnchor = camera.target.x + this->xAnchor / camera.zoom;
    float yAnchor = camera.target.y + this->yAnchor / camera.zoom;

    editArea.Item::DrawMyself(elapsedTime, camera);
    //shift the buttons to where they should be
    const float savedButtonX = incrementButton.GetX();
    const float savedIncButtonY = incrementButton.GetY(), savedDecButtonY = decrementButton.GetY();
    incrementButton.SetX(this->xAnchor + width * camera.zoom);
    decrementButton.SetX(this->xAnchor + width * camera.zoom);
    incrementButton.SetY(this->yAnchor);
    decrementButton.SetY(this->yAnchor + (height / 2) * camera.zoom);

    incrementButton.Item::DrawMyself(elapsedTime, camera);
    decrementButton.Item::DrawMyself(elapsedTime, camera);

    incrementButton.SetX(savedButtonX);
    decrementButton.SetX(savedButtonX);
    incrementButton.SetY(savedIncButtonY);
    decrementButton.SetY(savedDecButtonY);

    border.DrawMyself(xAnchor, yAnchor, GetTotalWidth(), height);
}

void EditableSpinner::DoFocusAction(float elapsedTime, const Vector2& mousePosition) {
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
    decrementButton.DoFocusAction(elapsedTime);
    incrementButton.DoFocusAction(elapsedTime);
    editArea.DoFocusAction(elapsedTime);
}

void EditableSpinner::DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera) {
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

    decrementButton.Item::DoFocusAction(elapsedTime, camera);
    incrementButton.Item::DoFocusAction(elapsedTime, camera);
    editArea.Item::DoFocusAction(elapsedTime, camera);
}

void EditableSpinner::DoFocusAction(float elapsedTime) {
    if (!IsFocused()) editArea.Defocus();
    incrementButton.DoFocusAction(elapsedTime);
    decrementButton.DoFocusAction(elapsedTime);
    editArea.DoFocusAction(elapsedTime);
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

// --- PasswordField ---

void PasswordField::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    if (text == "") {
        if (promptText != "" && !focused) {
            DrawText(Truncate(promptText).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), GREY);
        }
    } else {
        std::string password(text.size(), mask);
        std::string truncated = Truncate(password);
        DrawText(Truncate(truncated).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
        if (focused && fmod(elapsedTime, 1.0f) < 0.5f) {
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

// --- ProgressIndicator ---

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

void ProgressIndicator::DoFocusAction(float elapsedTime) {
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

void ProgressIndicator::DrawMyself(float elapsedTime) const {
    if (value == -1 || shape == Shapes::DOTS) {
        int dotCount = 8;
        float radius = width / 2;
        float dotRadius = radius / 6; //TODO make stuff customisable
        float angle = fmod(elapsedTime * 180.0f, 360.0f); // rotation speed
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

// --- ProgressBar ---

void ProgressBar::DrawMyself(float elapsedTime) const {
    if (shape != Shapes::BAR) {
        ProgressIndicator::DrawMyself(elapsedTime);
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

// --- Pressed button ---

void PressedButton::DrawMyself(float elapsedTime) const {
    std::string textToDisplay;
    if (pressed) {
        DrawRectangle(xAnchor, yAnchor, width, height, PersistentState::pressedColour.GetColour());
        textToDisplay = pressedText;
    } else {
        DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
        textToDisplay = text;
    }

    if (textToDisplay != "") {
       DrawText(textToDisplay.c_str(), xAnchor + (width / 2) - (MeasureText(textToDisplay.c_str(), font.GetFontSize()) / 2), yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
};

void PressedButton::DoFocusAction(float elapsedTime) {
    return;
}

void PressedButton::DoFocusAction(float elapsedTime, const Vector2& mousePosition) {
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

// ---------- Pie chart -----------

void PieChart::DoFocusAction(float elapsedTime) {
    return;
}

void PieChart::DrawMyself(float elapsedTime) const {
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    std::vector<Color> colours = {BLUE, RED, GREEN, PINK, BROWN, DARKGREEN, PURPLE}; //TODO Add user colours

    border.DrawMyself(TranslateXToCentre(), TranslateYToCentre(), GetRadius(), height);

    float currentAngle = 0;
    size_t index = 0;
    for (auto& element : values) {
        double percent = element / sum;
        Color c;
        if (index >= colours.size()) {
            c = colours[index % colours.size()];
        } else c = colours[index];

        DrawCircleSector(Vector2{TranslateXToCentre(), TranslateYToCentre()}, GetRadius(), currentAngle, currentAngle + (percent * 360), std::max(4, (int)(percent * 36)), c);

        if (showPercentage) {
            float xShift = 0;
            float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified

            std::string valueString = percent * 100 == (int)(percent * 100) ? TextFormat("%d", (int)(percent * 100)) : TextFormat("%.1f", percent * 100);
            valueString += "%";

            if (textAngle < 90) {
                xShift = MeasureText(valueString.c_str(), font.GetFontSize());
            }

            DrawText(valueString.c_str(), TranslateXToCentre() + (GetRadius() / 2 * cos(textAngle * DEG2RAD)) - (MeasureText(valueString.c_str(), font.GetFontSize()) / 2) + xShift,
                    TranslateYToCentre() + (GetRadius() / 2 * sin(textAngle * DEG2RAD)), font.GetFontSize(), font.colour.GetColour());
        }
        if (showLabels) {
            float xShift = 0, yShift = 0;
            float textAngle = currentAngle + (180 * percent); //average of start and end angle, simplified

            if (textAngle > 180) {
                yShift += -font.GetFontSize() - border.GetThickness();
            }

            if (values.size() == labels.size()) {
                std::string label = labels[index];
                if (textAngle > 90 && textAngle < 270) {
                    xShift = -MeasureText(label.c_str(), font.GetFontSize()) - border.GetThickness();
                } else if (textAngle < 90) {
                    xShift = MeasureText(label.c_str(), font.GetFontSize()) + border.GetThickness();
                }

                DrawText(label.c_str(), TranslateXToCentre() + (GetRadius()  * cos(textAngle * DEG2RAD)) - (MeasureText(label.c_str(), font.GetFontSize()) / 2) + xShift,
                        TranslateYToCentre() + (GetRadius() * sin(textAngle * DEG2RAD)) + yShift, font.GetFontSize(), font.colour.GetColour());
            } else {
                std::string elementText = std::to_string(element);
                DrawText(elementText.c_str(), TranslateXToCentre() + (GetRadius()  * cos(textAngle * DEG2RAD)) - (MeasureText(elementText.c_str(), font.GetFontSize()) / 2) + xShift,
                        TranslateYToCentre() + (GetRadius() * sin(textAngle * DEG2RAD)) + yShift, font.GetFontSize(), font.colour.GetColour());
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

// --- Line ---
float Line::CalculateMyArea() const {
    return sqrt(pow((xEnd - xAnchor), 2) + pow((yEnd - yAnchor), 2));
}

void Line::DrawMyself(float elapsedTime) const {
    if (pointToPoint) {
        DrawLineEx({xAnchor, yAnchor}, {xEnd, yEnd}, thickness, colour.GetColour());
    } else {
        DrawLineEx({xAnchor, yAnchor}, {xAnchor + (float)(sin(angle * DEG2RAD) * width), yAnchor - ((float)(cos(angle * DEG2RAD)) * width)}, thickness, colour.GetColour());
    }
}

void Line::SetLength(float length) {
    if (length < 0) {
        throw std::invalid_argument("In line " + ID + ": negative length");
    }
    width = length;
}

float Line::GetLength() const {
    return width;
}

void Line::SetWidth(float value) {
    SetLength(value);
}

void Line::SetHeight(float value) {
    SetLength(value);
}

float Line::GetWidth() const {
    return GetLength();
}

float Line::GetHeight() const {
    return GetLength();
}

float Line::GetTotalHeight() const {
    return GetLength();
}

float Line::GetTotalWidth() const {
    return GetLength();
}

void Line::SetAngle(float angle) {
    if (angle < 0 || angle > 360) {
        throw std::out_of_range("In line " + ID + ": angle beyond the [0;360] range");
    }
    this->angle = angle;
}

void Line::IncreaseAngle(float change) {
    angle += change;
    if (angle < 0) {
        angle = 360 + angle;
    } else if (angle > 360) {
        angle -= 360;
    }
}

float Line::GetAngle() const {
    return angle;
}

void Line::SetEndPoint(float x, float y) {
    if (x == xAnchor && y == yAnchor) {
        CPPFX_WARN("In line " + ID + ": new end coordinates are the same as the start coordinates.");
    }
    xEnd = x;
    yEnd = y;
    CalculateLength();
}

void Line::SetEndPoint(const Vector2& coordinates) {
    if (coordinates.x == xAnchor && coordinates.y == yAnchor) {
        CPPFX_WARN("In line " + ID + ": new end coordinates are the same as the start coordinates.");
    }
    xEnd = coordinates.x;
    yEnd = coordinates.y;
    CalculateLength();
}

void Line::CalculateLength() {
    width = CalculateMyArea();
}

Vector2 Line::GetEndPoint() const {
    return Vector2{xEnd, yEnd};
}

void Line::SetThickness(float thickness) {
    if (thickness < 0) {
        throw std::invalid_argument("In line " + ID + ": negative thickness.");
    }
    this->thickness = thickness;
}

float Line::GetThickness() const {
    return thickness;
}

void Line::DrawPointToPoint() {
    pointToPoint = true;
}

void Line::DrawLengthAndAngle() {
    pointToPoint = false;
}

void Line::SetDrawingMethod(bool flag) {
    pointToPoint = flag;
}

bool Line::GetDrawingMethod() const {
    return pointToPoint;
}

// --- Square ---

void CPPFX::Square::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, width, colour.GetColour());
    border.DrawMyself(xAnchor, yAnchor, width, width);
}

float CPPFX::Square::CalculateMyArea() const {
    return width * width;
}

void CPPFX::Square::SetWidth(float value) {
    Item::SetWidth(value);
    height = width;
}

void CPPFX::Square::SetHeight(float value) {
    SetWidth(value);
}

float CPPFX::Square::GetHeight() const {
    return GetWidth();
}

float CPPFX::Square::GetTotalHeight() const {
    return GetWidth();
}

// --- Rectangle ---

void CPPFX::Rectangle::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

float CPPFX::Rectangle::CalculateMyArea() const {
    return width * height;
}

void CPPFX::Circle::DrawMyself(float elapsedTime) const {
    border.DrawMyself(TranslateXToCentre(), TranslateYToCentre(), GetRadius(), GetRadius());
    DrawCircle(TranslateXToCentre(), TranslateYToCentre(), GetRadius(), colour.GetColour());
}

float CPPFX::Circle::CalculateMyArea() const {
    return PI * GetRadius() * GetRadius();
}
