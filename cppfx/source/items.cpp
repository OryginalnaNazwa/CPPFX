#include "../include/CPPFX/items.hpp"
#include <cmath>          // for cos, sin, fmod, pow, sqrt
#include <numeric>        // for accumulate
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <limits>         // for numeric_limits
#include <algorithm>

using namespace CPPFX;

//--- Label ---

void Label::DrawMyself(float /*elapsedTime*/) const {
    border.DrawMyself(xAnchor, yAnchor, width, height);
    DrawRectangleRec(::Rectangle{xAnchor, yAnchor, width, height}, colour.GetColour());
    if (!IsTextEmpty()) {
        DrawAlignedText(alignment);
    }
}

void Label::DoFocusAction(float /*elapsedTime*/) {
    Defocus();
}

std::string Label::GetClassID() {
    return "Label";
}

//--- Text Field ---

void TextField::DrawCaret(const std::string& beforeCaret) const {
    font.DrawText("|",
                xAnchor + textMargin + font.MeasureTextWidth(beforeCaret) + 2.0f,
                yAnchor + font.GetVerticalCentreOffset(height));
}

void TextField::DrawMyself(float elapsedTime) const {
    border.DrawMyself(xAnchor, yAnchor, width, height);
    DrawRectangleRec(::Rectangle{xAnchor, yAnchor, width, height}, colour.GetColour());

    if (IsTextEmpty()) {
        if (!IsPromptTextEmpty() && !IsFocused()) {
            DrawAlignedText(Alignment::CENTRE_LEFT, promptText, promptFont);
        } else if (IsFocused()) {
            DrawCaret("");
        }
    } else if (!IsFocused()) {
        DrawAlignedText(Alignment::CENTRE_LEFT, ShownFromTo(0, Length()), font);
    } else {
        const size_t lastVisible = VisibleEnd(firstVisible);
        const std::string left = (firstVisible > 0) ? "..." : "";
        const std::string right = (lastVisible < Length()) ? "..." : "";
        DrawAlignedText(Alignment::CENTRE_LEFT,
                        left + ShownFromTo(firstVisible, lastVisible) + right, font);
        if (fmod(elapsedTime, 1.0f) < 0.5f) {
            DrawCaret(left + ShownFromTo(firstVisible, caretPosition));
        }
    }
}

void TextField::DoFocusAction(float /*elapsedTime*/) {
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (caretPosition > 0) RemoveAt(--caretPosition);
    } else if (IsKeyPressed(key_submit)) {
        onSubmit.Arm();
        Defocus();
    } else if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        if (caretPosition < Length()) RemoveAt(caretPosition);
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        if (caretPosition > 0) --caretPosition;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        if (caretPosition < Length()) ++caretPosition;
    } else {
        int ch = GetCharPressed();
        while (ch > 0) {
            InsertAt(caretPosition++, ch);
            ch = GetCharPressed();
        }
    }
    UpdateScroll();
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

bool TextField::IsPromptTextEmpty() const {
    return promptText.empty();
}

void TextField::SetSubmitKey(int key)      { key_submit = key; }
void TextField::RestoreSubmitKey()         { key_submit = KEY_ENTER; }

void TextField::GetCallbacks(std::vector<Callback*>& out) {
    TextItem::GetCallbacks(out);
    out.push_back(&onSubmit);
}

std::string TextField::Truncate(const std::string& text, const CPPFX::Font& font, float maxWidth) const {
    return TextItem::Truncate(text, font, maxWidth - font.MeasureTextWidth("|"));
}

float TextField::GetEditWidth() const {
    return width - (2.0f * textMargin) - font.MeasureTextWidth("|");
}

std::string TextField::ShownFromTo(size_t from, size_t to) const {
    return StringFromTo(from, to);
}

size_t TextField::VisibleEnd(size_t first) const {
    const float available = GetEditWidth();
    const std::string left = (first > 0) ? "..." : "";
    if (font.MeasureTextWidth(left + ShownFromTo(first, Length())) <= available) return Length();
    size_t last = first;
    while (last < Length() &&
           font.MeasureTextWidth(left + ShownFromTo(first, last + 1) + "...") <= available) {
        ++last;
    }
    return last;
}

void TextField::UpdateScroll() {
    // caret moved left out of view
    if (firstVisible > caretPosition) firstVisible = caretPosition;
    // caret moved right out of view
    while (firstVisible < caretPosition && caretPosition > VisibleEnd(firstVisible)) {
        ++firstVisible;
    }
    // text got shorter - slide back while everything to the end still fits
    while (firstVisible > 0 && VisibleEnd(firstVisible - 1) == Length()) {
        --firstVisible;
    }
}

void TextField::SetText(const std::string& text) {
    TextItem::SetText(text);
    caretPosition = Length();
    UpdateScroll();
}

void TextField::ClearText() {
    TextItem::ClearText();
    caretPosition = 0;
    firstVisible = 0;
}

std::string TextField::GetClassID() {
    return "TextField";
}

void EmbeddedTextField::DoFocusAction(float elapsedTime) {
    if (IsFocused()) TextField::DoFocusAction(elapsedTime);
}

//--- Button ---

void Button::DrawMyself(float /*elapsedTime*/) const {
    if (timer > 0.0f) {
        DrawRectangle(xAnchor, yAnchor, width, height, pressedColour.GetColour()); //button during click
    } else DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());

    if (!IsTextEmpty()) {
       DrawAlignedText(Alignment::CENTRE);
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Button::DoFocusAction(float elapsedTime) {
    if (timer > 0.0f) {
        if (elapsedTime > timer) {
            Defocus();
        }
    } else {
        timer = elapsedTime + 0.1f;
    }
}

void Button::Defocus() {
    Item::Defocus();
    timer = 0.0f;
}

std::string Button::GetClassID() {
    return "Button";
}

void EmbeddedButton::DoFocusAction(float elapsedTime) {
    if (IsFocused()) Button::DoFocusAction(elapsedTime);
}

// --- CheckBox ---

void CheckBox::DrawMyself(float /*elapsedTime*/) const {
    if (shape != CheckBox::RADIO) {
        if (!pressed) {
            DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
        } else DrawRectangle(xAnchor, yAnchor, width, height, pressedColour.GetColour());
    } else {
        DrawCircle(xAnchor + (width / 2.0f), yAnchor + (height / 2.0f), width / 2.0f, colour.GetColour());
    }

    const Layout layout = CalculateLayout();
    if (!IsTextEmpty()) {
        DrawAlignedText(layout.position, GetText(), font,
                        layout.label.x, layout.label.y, layout.label.width, layout.label.height);
    }
    border.DrawMyself(layout.frame.x, layout.frame.y, layout.frame.width, layout.frame.height);
    clickBorder.DrawMyself(xAnchor, yAnchor, width, height);

    if (pressed) { // draws shapes. Eyeballed numeric values, but looks good. Can stay.
        if (shape == CheckBox::X) {
            DrawLineEx({xAnchor + (width * 0.1f), yAnchor + (height * 0.1f)}, {xAnchor + (0.9f * width), yAnchor + (0.9f * height)}, width / 10.0f, colour.GetColour());
            DrawLineEx({xAnchor + (width * 0.1f), yAnchor + (0.9f * height)}, {xAnchor + (0.9f * width), yAnchor + (0.1f * height)}, width / 10.0f, colour.GetColour());
        } else if (shape == CheckBox::TICK) {
            DrawLineEx({xAnchor + (width * 0.1f), yAnchor + (height * 0.5f)}, {xAnchor + (width * 0.4f), yAnchor + (height * 0.85f)}, width / 10.0f, colour.GetColour());
            DrawLineEx({xAnchor + (width * 0.4f), yAnchor + (height * 0.85f)}, {xAnchor + (width * 0.9f), yAnchor + (height * 0.15f)}, width / 10.0f, colour.GetColour());
        } else if (shape == CheckBox::O) {
            DrawRing({xAnchor + (width / 2.0f), yAnchor + (height / 2.0f)}, 0.8f * (width / 2.0f), 0.9f * width / 2.0f, 0.0f, 360.0f, 32, colour.GetColour());
        } else if (shape == CheckBox::RADIO) {
            DrawCircle(xAnchor + (width / 2.0f), yAnchor + (height / 2.0f), width / 4.0f, pressedColour.GetColour());
        }
    }
}

CheckBox::Layout CheckBox::CalculateLayout() const {
    const float t = clickBorder.GetThickness();
    Layout l{};

    if (IsTextEmpty()) {
        // no label - the frame wraps only the box and its click border
        l.position = Alignment::Alignments::RIGHT;
        l.frame    = { xAnchor - t, yAnchor - t, width + (2.0f * t), height + (2.0f * t) };
        return l;
    }

    const Vector2 ink = font.GetInkSize(GetText());
    const float gap   = textMargin + labelMargin;

    // beside is the most common case; corners go beside, degenerate case of CENTRE falls back to right
    const bool beside = alignment.IsLeftAlignment() || alignment.IsRightAlignment()
                     || alignment.GetAlignment() == Alignment::Alignments::CENTRE;
    if (beside) {
        // label sits beside the box, so the taller of the two sets the height
        const bool left = alignment.IsLeftAlignment();
        l.position     = left ? Alignment::Alignments::LEFT : Alignment::Alignments::RIGHT;
        l.label        = { left ? xAnchor - gap - ink.x : xAnchor + width + gap, yAnchor, ink.x, height };
        l.frame.width  = width + (2.0f * textMargin) + labelMargin + ink.x + t;
        l.frame.height = std::max(height + (2.0f * t), ink.y + (2.0f * textMargin));
        l.frame.x      = left ? xAnchor - (2.0f * textMargin) - labelMargin - ink.x : xAnchor - t;
        l.frame.y      = yAnchor - ((l.frame.height - height) / 2.0f);
    } else {
        // top and bottom differ only in whether the label sits above the box
        const bool top = alignment.IsTopAlignment();
        l.position     = top ? Alignment::Alignments::ABOVE : Alignment::Alignments::BOTTOM;
        l.label        = { xAnchor, top ? yAnchor - gap - ink.y : yAnchor + height + gap, width, ink.y };
        l.frame.width  = std::max(width + (2.0f * t), ink.x + (2.0f * textMargin));
        l.frame.height = height + (2.0f * textMargin) + labelMargin + ink.y + t;
        l.frame.x      = xAnchor - ((l.frame.width - width) / 2.0f);
        l.frame.y      = top ? yAnchor - (2.0f * textMargin) - labelMargin - ink.y : yAnchor - t;
    }
    return l;
}

void CheckBox::DoFocusAction(float /*elapsedTime*/) {
   if (IsFocused()) {
        SwitchPress();
        Defocus();
   }
}

void CheckBox::SetLabelMargin(float value) {
    if (value >= 0.0f) {
        labelMargin = value;
    } else {
        CPPFX_THROW(std::invalid_argument, "Negative label margin.");
    }
}

float CheckBox::GetLabelMargin() const {
    return labelMargin;
}

float CheckBox::GetTotalWidth() const {
    return CalculateLayout().frame.width + (2.0f * border.GetThickness());
}

float CheckBox::GetTotalHeight() const {
    return CalculateLayout().frame.height + (2.0f * border.GetThickness());
}

std::string CheckBox::GetClassID() {
    return "CheckBox";
}

void CheckBox::SetShape(const CheckBox::Shapes& shape) {
    try {
        ShapeToString(shape);
    } catch (const std::invalid_argument&) {
        CPPFX_THROW(std::invalid_argument, "Invalid shape - cannot set shape.");
    }
    this->shape = shape;
}

CheckBox::Shapes CheckBox::StringToShape(const std::string& shape) const {
    static const std::unordered_map<std::string, CheckBox::Shapes> stringMap = {
        {"X", CheckBox::Shapes::X}, {"CROSS", CheckBox::Shapes::X},
        {"TICK", CheckBox::Shapes::TICK}, {"CHECK", CheckBox::Shapes::TICK},
        {"O", CheckBox::Shapes::O}, {"CIRCLE", CheckBox::Shapes::O}, {"MARU", CheckBox::Shapes::O}, {"GONGPYO", CheckBox::Shapes::O},
        {"RADIO", CheckBox::Shapes::RADIO}
    };

    std::string normal = shape;
    std::transform(normal.begin(), normal.end(), normal.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    auto it = stringMap.find(normal);
    if (it != stringMap.end()) return it->second;
    CPPFX_THROW(std::invalid_argument, "Invalid shape (" + shape + ")");
}

std::string CheckBox::ShapeToString(const CheckBox::Shapes& shape) const {
    static const std::unordered_map<CheckBox::Shapes, std::string> shapeMap = {
        {CheckBox::Shapes::X,    "X"},
        {CheckBox::Shapes::TICK, "TICK"},
        {CheckBox::Shapes::O,    "O"},
        {CheckBox::Shapes::RADIO,    "RADIO"}
    };

    auto it = shapeMap.find(shape);
    if (it != shapeMap.end()) return it->second;
    CPPFX_THROW(std::invalid_argument, "Invalid shape");
}

void CheckBox::SetShape(const std::string& shape) {
    this->shape = StringToShape(shape);
}

CheckBox::Shapes CheckBox::GetShape() const {
    return shape;
}

std::string CheckBox::GetShapeString() const {
    return ShapeToString(shape);
}

// --- RadioGroup ---

void RadioGroup::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    border.DrawMyself(xAnchor, yAnchor, width, height);
    for (const auto& button : buttonsInDrawingOrder) {
        button->DrawMyself(elapsedTime);
    }
}

void RadioGroup::DoFocusAction(float /*elapsedTime*/) {
    return;
}

void RadioGroup::DoFocusAction(float /*elapsedTime*/, Vector2 mousePosition) {
    for (auto& [key, button] : buttons) {
        if (button->WasIClicked(mousePosition)) {
            if (!button->IsPressed()) onChange.Arm();
            UnpressButtons();
            button->Press();
            return;
        }
    }
}

void RadioGroup::DoPassiveAction(float /*elapsedTime*/) {
    if (needsOrdering) {
        SetButtonsPositions();
    }
}

void RadioGroup::GetCallbacks(std::vector<Callback*>& out) {
    // buttons first: the group's own callbacks may remove buttons, so they must run last
    for (auto& button : buttonsInDrawingOrder) {
        out.insert(out.end(), {&button->onHover, &button->onDehover});
    }
    Item::GetCallbacks(out);
}

void RadioGroup::DoHoverAction(float /*elapsedTime*/, const Vector2 mousePosition) {
    for (auto& button : buttonsInDrawingOrder) {
        if (button->WasIClicked(mousePosition)) button->Hover();
        else button->Dehover();
    }
}

void RadioGroup::Dehover() {
    Item::Dehover();
    for (auto& button : buttonsInDrawingOrder) button->Dehover();
}

void RadioGroup::AddButton(const std::string& label) {
    if (label == "") {
        CPPFX_WARN("If for some reason you want no label, use ' '/ spaces.");
        CPPFX_THROW(std::invalid_argument, "Cannot add button - empty label.");
    }
    if (IsLabelTaken(label)) {
        CPPFX_THROW(std::invalid_argument, "Cannot add button - label " + label + " already taken.");
    }

    auto button = std::make_unique<RadioButton>();
    button->SetID(label);
    button->SetText(label);
    button->hoveredColour.BlendTo(GREY, 0.25f);
    button->SetHoverCallbacksToSwap();
    if (buttons.empty()) button->Press();
    buttonsInDrawingOrder.push_back(button.get());
    buttons.insert({label, std::move(button)});
    needsOrdering = true;
}

void RadioGroup::RemoveButton(const std::string& label) {
    if (buttons.empty()) {
        CPPFX_THROW(std::runtime_error, "Cannot remove button because there's none.");
    }
    if (!IsLabelTaken(label)) {
        CPPFX_THROW(std::out_of_range, "No button with label " + label + " found - cannot remove it.");
    }

    if (buttons.size() > 1 && buttons.at(label)->IsPressed()) {
        buttonsInDrawingOrder[0]->Press();
    }
    buttonsInDrawingOrder.erase(std::remove_if(buttonsInDrawingOrder.begin(), buttonsInDrawingOrder.end(),
                                               [&label](const RadioButton* button) { return button->GetID() == label; }), buttonsInDrawingOrder.end());
    buttons.erase(label);
    needsOrdering = true;
}

void RadioGroup::ChangeButtonLabel(const std::string& oldLabel, const std::string& newLabel) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change labels on no buttons.");
    if (!IsLabelTaken(oldLabel)) CPPFX_THROW(std::out_of_range, "No button with label " + oldLabel + " found - cannot change it.");
    if (oldLabel == newLabel) return;
    if (newLabel == "") {
        CPPFX_WARN("If for some reason you want no label, use ' '/ spaces.");
        CPPFX_THROW(std::invalid_argument, "Cannot change label to an empty one.");
    }
    if (IsLabelTaken(newLabel)) CPPFX_THROW(std::invalid_argument, "Label " + newLabel + " already exists - cannot change another button to it.");

    auto node = buttons.extract(oldLabel);
    node.key() = newLabel;
    node.mapped()->SetID(newLabel);
    node.mapped()->SetText(newLabel);
    buttons.insert(std::move(node));
    needsOrdering = true;
}

bool RadioGroup::IsButtonPressed(const std::string& label) const {
    if (buttons.empty()) {
        CPPFX_THROW(std::runtime_error, "Cannot check whether a button is pressed because there's none.");
    }
    if (!IsLabelTaken(label)) {
        CPPFX_THROW(std::out_of_range, "No button with label " + label + " found - cannot check whether it's pressed.");
    }

    return buttons.at(label)->IsPressed();
}

std::string RadioGroup::WhichButtonIsCurrentlyPressed() const {
    for (const auto& [key, button] : buttons) {
        if (button->IsPressed()) return key;
    }
    CPPFX_THROW(std::runtime_error, "No pressed buttons found. Either there are no buttons or something went deeply wrong.");
}

void RadioGroup::PressButton(const std::string& label) {
    if (buttons.empty()) {
        CPPFX_THROW(std::runtime_error, "Cannot press a button because there's none.");
    }
    if (!IsLabelTaken(label)) {
        CPPFX_THROW(std::out_of_range, "No button with label " + label + " found - cannot press.");
    }

    UnpressButtons();
    buttons.at(label)->Press();
}

void RadioGroup::SetButtonsBackgroundColour(const Color& colour) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change background colour on no buttons.");
    for (auto& [key, button] : buttons) {
        button->colour.SetColour(colour);
    }
}

void RadioGroup::SetButtonsPressedColour(const Color& colour) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change pressed colour on no buttons.");
    for (auto& [key, button] : buttons) {
        button->pressedColour.SetColour(colour);
    }
}

void RadioGroup::SetButtonColours(const Color& backgroundColour, const Color& pressedColour) {
    SetButtonsBackgroundColour(backgroundColour);
    SetButtonsPressedColour(pressedColour);
}

void RadioGroup::SetButtonsHoverColour(const Color& colour) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change hover colour on no buttons.");
    for (auto& [key, button] : buttons) {
        // while hovered with the swap on, the hover colour currently sits in colour
        if (button->IsHovered() && button->onHover.IsSet()) button->colour.SetColour(colour);
        else button->hoveredColour.SetColour(colour);
    }
}

void RadioGroup::EnableButtonsHover() {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot enable hover on no buttons.");
    for (auto& [key, button] : buttons) {
        if (!button->onHover.IsSet()) button->SetHoverCallbacksToSwap();
    }
}

void RadioGroup::DisableButtonsHover() {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot disable hover on no buttons.");
    for (auto& [key, button] : buttons) {
        if (!button->onHover.IsSet()) continue;
        if (button->IsHovered()) button->SwapHoverColour();
        button->onHover.Clear();
        button->onDehover.Clear();
    }
}

std::string RadioGroup::GetHoveredButton() const {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "No button can be hovered as there's none.");
    for (const auto& button : buttonsInDrawingOrder) {
        if (button->IsHovered()) return button->GetID();
    }
    return "";
}

void RadioGroup::SetLabelMargins(float margin) {
    if (margin  < 0.0f) CPPFX_THROW(std::invalid_argument, "Cannot set buttons label margin to a negative value.");
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot set buttons label margin because there's no buttons");

    for (auto& [key, button] : buttons) {
        button->SetLabelMargin(margin);
    }
    needsOrdering = true;
}

void RadioGroup::SetTextMargins(float margin) {
    if (margin  < 0.0f) CPPFX_THROW(std::invalid_argument, "Cannot set buttons text margin to a negative value.");
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot set buttons text margin because there's no buttons");

    for (auto& [key, button] : buttons) {
        button->SetTextMargin(margin);
    }
    needsOrdering = true;
}

void RadioGroup::SetButtonAlignment(Alignment::Alignments alignment) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change alignment on no buttons.");
    for (auto& [key, button] : buttons) {
        button->alignment.SetAlignment(alignment);
    }
    needsOrdering = true;
}

void RadioGroup::SetButtonsFont(::Font font) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change font on no buttons.");
    for (auto& [key, button] : buttons) {
        button->font.SetFont(font);   // keeps each button's size/colour, swaps the atlas
    }
    needsOrdering = true;             // total width/height depend on the font
}

void RadioGroup::SetButtonsFont(const CPPFX::Font& font) {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot change font on no buttons.");
    for (auto& [key, button] : buttons) {
        button->font = font;          // shares the atlas, also copies size, spacing and colour
    }
    needsOrdering = true;
}

void RadioGroup::SetToVertical() {
    vertical = true;
    needsOrdering = true;
}

void RadioGroup::SetToHorizontal() {
    vertical = false;
    needsOrdering = true;
}

void RadioGroup::SetVerticality(bool flag) {
    vertical = flag;
    needsOrdering = true;
}

bool RadioGroup::IsVertical() const {
    return vertical;
}

std::string RadioGroup::GetClassID() {
    return "RadioGroup";
}

bool RadioGroup::IsLabelTaken(const std::string& label) const {
    return buttons.contains(label);
}

void RadioGroup::UnpressButtons() {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "Cannot unpress nonexistent buttons.");
    for (auto& [key, button] : buttons) {
        button->Unpress();
    }
}

void RadioGroup::SetX(float x) {
    Item::SetX(x);
    needsOrdering = true;
}

void RadioGroup::SetY(float y) {
    Item::SetY(y);
    needsOrdering = true;
}

void RadioGroup::SetButtonsPositions() {
    if (buttonsInDrawingOrder.empty()) return;

    size_t perGroup = buttons.size() / groups;
    size_t remainder = buttons.size() % groups;

    if (vertical) {
        std::vector<float> columnWidths(groups, 0.0f);
        size_t currentGroup = 0, currentButton = 1;
        for (const auto& button : buttonsInDrawingOrder) {
            columnWidths[currentGroup] = std::max(columnWidths[currentGroup], button->GetTotalWidth());
            size_t thisColumnSize = (currentGroup < remainder) ? perGroup + 1 : perGroup;
            if (currentButton == thisColumnSize) {
                currentButton = 1;
                currentGroup++;
            } else currentButton++;
        }

        std::vector<float> columnX(groups, 0.0f);
        columnX[0] = xAnchor + buttonMargin;
        for (size_t c = 1; c < groups; c++) {
            columnX[c] = columnX[c-1] + columnWidths[c-1] + padding + buttonMargin;
        }

        currentGroup = 0; currentButton = 1;
        float currentY = yAnchor + buttonMargin;
        for (const auto& button : buttonsInDrawingOrder) {
            button->SetX(columnX[currentGroup]);
            button->SetY(currentY);
            currentY += button->GetTotalHeight() + padding;
            size_t thisColumnSize = (currentGroup < remainder) ? perGroup + 1 : perGroup;
            if (currentButton == thisColumnSize) {
                currentButton = 1;
                currentGroup++;
                currentY = yAnchor + buttonMargin;
            } else currentButton++;
        }

    } else {
        std::vector<float> rowHeights(groups, 0.0f);
        size_t currentGroup = 0, currentButton = 1;
        for (const auto& button : buttonsInDrawingOrder) {
            rowHeights[currentGroup] = std::max(rowHeights[currentGroup], button->GetTotalHeight());
            size_t thisRowSize = (currentGroup < remainder) ? perGroup + 1 : perGroup;
            if (currentButton == thisRowSize) {
                currentButton = 1;
                currentGroup++;
            } else currentButton++;
        }

        std::vector<float> rowY(groups, 0.0f);
        rowY[0] = yAnchor + buttonMargin;
        for (size_t r = 1; r < groups; r++) {
            rowY[r] = rowY[r-1] + rowHeights[r-1] + padding + buttonMargin;
        }

        currentGroup = 0; currentButton = 1;
        float currentX = xAnchor + buttonMargin;
        for (const auto& button : buttonsInDrawingOrder) {
            button->SetX(currentX);
            button->SetY(rowY[currentGroup]);
            currentX += button->GetTotalWidth() + padding;
            size_t thisRowSize = (currentGroup < remainder) ? perGroup + 1 : perGroup;
            if (currentButton == thisRowSize) {
                currentButton = 1;
                currentGroup++;
                currentX = xAnchor + buttonMargin;
            } else currentButton++;
        }
    }

    ExpandToButtons();
    needsOrdering = false;
}

void RadioGroup::ExpandToButtons() {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "No buttons to expand to.");

    float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();
    for (const auto& [key, button] : buttons) {
        if (button->GetX() + button->GetTotalWidth() > maxX) maxX = button->GetX() + button->GetTotalWidth();
        if (button->GetY() + button->GetTotalHeight() > maxY) maxY = button->GetY() + button->GetTotalHeight();
    }
    if (maxX > xAnchor + width) SetWidth(maxX - xAnchor + buttonMargin);
    if (maxY > yAnchor + height) SetHeight(maxY - yAnchor + buttonMargin);
}

void RadioGroup::FitToButtons() {
    if (buttons.empty()) CPPFX_THROW(std::runtime_error, "No buttons to fit to.");

    float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();
    for (const auto& [key, button] : buttons) {
        if (button->GetX() + button->GetTotalWidth() > maxX) maxX = button->GetX() + button->GetTotalWidth();
        if (button->GetY() + button->GetTotalHeight() > maxY) maxY = button->GetY() + button->GetTotalHeight();
    }
    SetWidth(maxX - xAnchor + buttonMargin);
    SetHeight(maxY - yAnchor + buttonMargin);
}

void RadioGroup::SetButtonMargin(float distance) {
    if (distance < 0.0f) CPPFX_THROW(std::invalid_argument, "Cannot set button margin to a negative value.");
    buttonMargin = distance;
}

float RadioGroup::GetButtonMargin() const {
    return buttonMargin;
}

void RadioGroup::SetGroupsNumber(int number) {
    if (number < 1) CPPFX_THROW(std::invalid_argument, "There's no way to have less than 1 group.");
    groups = (size_t)(number);
    needsOrdering = true;
}

void RadioGroup::SetColumnsNumber(int number) {
    try {
        SetGroupsNumber(number);
    } catch (const std::invalid_argument&) {
        CPPFX_THROW(std::invalid_argument, "There's no way to have less than 1 column.");
    }
}

void RadioGroup::SetRowsNumber(int number) {
    try {
        SetGroupsNumber(number);
    } catch (const std::invalid_argument&) {
        CPPFX_THROW(std::invalid_argument, "There's no way to have less than 1 row.");
    }
}

size_t RadioGroup::GetGroupsNumber() const {
    return groups;
}

size_t RadioGroup::GetColumnsNumber() const {
    return GetGroupsNumber();
}

size_t RadioGroup::GetRowsNumber() const {
    return GetGroupsNumber();
}

// --- ComboBox ---

void ComboBox::DrawMyself(float elapsedTime) const {
    if (focused) {
        addArea.DrawMyself(elapsedTime);
        DrawList(elapsedTime);
    } else {
        DrawHeader(elapsedTime);
    }
    DrawFrame();
}

void ComboBox::CommitAddArea() {
    std::string label = addArea.GetText();
    if (label.empty()) return;
    if (!IsLabelTaken(label)) {
        try {
            AddItem(label);
        } catch (const std::invalid_argument&) {
            addArea.ClearText();
            return;
        }
    }
    SetCurrent(label);
    onCommit.Arm();
    if (expandsToTextAutomatically) ExpandToText();
    Defocus();
}

void ComboBox::DoFocusAction(float elapsedTime) {
    justOpened = false;
    if (!addArea.IsFocused()) return;
    if (IsKeyPressed(KEY_ENTER)) CommitAddArea();
    else addArea.DoFocusAction(elapsedTime);
}

void ComboBox::DoFocusAction(float elapsedTime, Vector2 mousePosition) {
    if (justOpened) {
        justOpened = false;
        return;
    }
    if (!WasIClicked(mousePosition)) {
        Defocus();
        return;
    }
    if (mousePosition.y < GetListTop()) {   // the header slot - addArea lives there while open
        if (addArea.WasIClicked(mousePosition)) addArea.Focus();
        DoFocusAction(elapsedTime);
        return;
    }
    const int index = GetIndexAt(mousePosition.y);
    if (index >= 0) {
        const std::string label = valuesInOrder[index];
        SetCurrent(label);
        Defocus();
    } else if (values.empty()) {
        Defocus();
    }
}

void ComboBox::GetCallbacks(std::vector<Callback*>& out) {
    DropDown<std::string>::GetCallbacks(out);
    out.push_back(&onCommit);
}

void ComboBox::SetX(float x) {
    DropDown<std::string>::SetX(x);
    addArea.SetX(xAnchor);
}

void ComboBox::SetY(float y) {
    DropDown<std::string>::SetY(y);
    addArea.SetY(yAnchor);
}

void ComboBox::SetWidth(float value) {
    DropDown<std::string>::SetWidth(value);
    addArea.SetWidth(width);
}

void ComboBox::SetHeight(float value) {
    DropDown<std::string>::SetHeight(value);
    addArea.SetHeight(height);
}

void ComboBox::SetToWorld() {
    DropDown<std::string>::SetToWorld();
    addArea.SetToWorld();
}

void ComboBox::SetToScreen() {
    DropDown<std::string>::SetToScreen();
    addArea.SetToScreen();
}

void ComboBox::MakeVisible() {
    DropDown<std::string>::MakeVisible();
    addArea.MakeVisible();
}

void ComboBox::MakeInvisible() {
    DropDown<std::string>::MakeInvisible();
    addArea.MakeInvisible();
}

void ComboBox::SetVisible(bool flag) {
    DropDown<std::string>::SetVisible(flag);
    addArea.SetVisible(flag);
}

void ComboBox::MakeActive() {
    DropDown<std::string>::MakeActive();
    addArea.MakeActive();
}

void ComboBox::MakeInactive() {
    DropDown<std::string>::MakeInactive();
    addArea.MakeInactive();
}

void ComboBox::SetInactive(bool flag) {
    DropDown<std::string>::SetInactive(flag);
    addArea.SetInactive(flag);
}

void ComboBox::Focus() {
    if (opensOnSecondClick) justOpened = true;
    DropDown<std::string>::Focus();
    addArea.Show();
}

void ComboBox::Defocus() {
    addArea.ClearText();
    addArea.Defocus();
    addArea.Hide();
    DropDown<std::string>::Defocus();
}

void ComboBox::SetTextMargin(float margin) {
    TextItem::SetTextMargin(margin);
    addArea.SetTextMargin(margin);
}

void ComboBox::ImmediatelyFocusAddArea() {
    opensOnSecondClick = false;
}

void ComboBox::AddAreaFocusesOnSecondClick() {
    opensOnSecondClick = true;
}

void ComboBox::AddAreaShouldFocusOnSecondClick(bool should) {
    opensOnSecondClick = should;
}

bool ComboBox::DoesAddAreaFocusOnSecondClick() const {
    return opensOnSecondClick;
}

std::string ComboBox::GetClassID() {
    return "ComboBox";
}

// --- SearchableDropDown --

float SearchableDropDown::GetListContentHeight(size_t number) const {
    if (number == 0) return header.GetDividerThickness();
    return header.GetDividerThickness() + ((float)(number) * list.GetItemInListHeight()) + ((float)(number - 1) * list.GetDividerThickness());
}

float SearchableDropDown::GetListContentHeight() const {
    return GetListContentHeight(filtered.size());
}

bool SearchableDropDown::DefaultSearch(const std::string& label, const std::string& query) const {
    return label.starts_with(query);
}

void SearchableDropDown::FilterList() {
    const std::string& query = searchArea.GetText();
    if (!filterDirty && query == lastQuery) return;
    lastQuery = query;
    filterDirty = false;

    filtered.clear();
    for (size_t i = 0; i < valuesInOrder.size(); ++i) {
        const std::string& value = valuesInOrder[i];
        if (query.empty() || (customSearch ? customSearch(value, query) : DefaultSearch(value, query)))
            filtered.push_back(i);
    }
}

void SearchableDropDown::DrawList(float /*elapsedTime*/) const {
    float yCurrent = GetListTop();
    Color drawColour = colour.GetColour();
    if (list.colour.IsOverridden()) drawColour = list.colour.GetColour();
    CPPFX::Font drawFont = font;
    if (list.font.IsOverridden()) drawFont = list.font.GetOverride();

    DrawRectangle(xAnchor, yCurrent, width, header.GetDividerThickness(), header.dividerColour.GetColour());
    yCurrent += header.GetDividerThickness();

    for (size_t i = 0; i < filtered.size(); ++i) {
        const std::string& label = valuesInOrder[filtered[i]];

        Color rowColour = labelToColour.contains(label)
                                ? labelToColour.at(label).GetColour()
                                : drawColour;
        if ((int)i == hoveredRow) {
            rowColour = Colour::Blend(rowColour, list.hoveredColour.GetColour(), list.GetHoverBlendFactor());
        }

        DrawRectangle(xAnchor, yCurrent, width, list.GetItemInListHeight(), rowColour);
        DrawAlignedText(Alignment::CENTRE, label, drawFont,
                        xAnchor + textMargin, yCurrent + textMargin,
                        width - (2.0f * textMargin),
                        list.GetItemInListHeight() - (2.0f * textMargin));

        if (i + 1 < filtered.size()) {
            DrawRectangle(xAnchor, yCurrent + list.GetItemInListHeight(), width,
                          list.GetDividerThickness(), list.dividerColour.GetColour());
        }
        yCurrent += GetRowPitch();
    }

    list.border.DrawMyself(xAnchor, GetListTop(), width, GetListContentHeight(filtered.size()));
}

void SearchableDropDown::DrawFrame() const {
    const float outer = std::max(header.border.GetThickness(), list.border.GetThickness());
    const float listSpan = focused
                         ? GetListContentHeight(filtered.size()) + (2.0f * list.border.GetThickness())
                         : 0.0f;
    border.DrawMyself(xAnchor - outer, yAnchor - header.border.GetThickness(),
                      width + (2.0f * outer),
                      height + (2.0f * header.border.GetThickness()) + listSpan);
}

void SearchableDropDown::DrawMyself(float elapsedTime) const {
    if (focused) {
        searchArea.DrawMyself(elapsedTime);
        DrawList(elapsedTime);
    } else {
        DrawHeader(elapsedTime);
    }
    DrawFrame();
}

void SearchableDropDown::DoFocusAction(float elapsedTime) {
    FilterList();
    justOpened = false;
    if (!searchArea.IsFocused()) return;
    else searchArea.DoFocusAction(elapsedTime);
}

void SearchableDropDown::DoFocusAction(float elapsedTime, Vector2 mousePosition) {
    if (justOpened) {
        justOpened = false;
        return;
    }
    if (!WasIClicked(mousePosition)) {
        Defocus();
        return;
    }
    if (mousePosition.y < GetListTop()) {
        if (searchArea.WasIClicked(mousePosition)) searchArea.Focus();
        DoFocusAction(elapsedTime);
        return;
    }
    const int index = GetIndexAt(mousePosition.y);
    if (index >= 0 && (size_t)index < filtered.size()) {
        const std::string label = valuesInOrder[filtered[index]];
        SetCurrent(label);
        Defocus();
    } else if (values.empty()) {
        Defocus();
    }
}

void SearchableDropDown::SetX(float x) {
    DropDown<std::string>::SetX(x);
    searchArea.SetX(xAnchor);
}

void SearchableDropDown::SetY(float y) {
    DropDown<std::string>::SetY(y);
    searchArea.SetY(yAnchor);
}

void SearchableDropDown::SetWidth(float value) {
    DropDown<std::string>::SetWidth(value);
    searchArea.SetWidth(width);
}

void SearchableDropDown::SetHeight(float value) {
    DropDown<std::string>::SetHeight(value);
    searchArea.SetHeight(height);
}

void SearchableDropDown::SetToWorld() {
    DropDown<std::string>::SetToWorld();
    searchArea.SetToWorld();
}

void SearchableDropDown::SetToScreen() {
    DropDown<std::string>::SetToScreen();
    searchArea.SetToScreen();
}

void SearchableDropDown::MakeVisible() {
    DropDown<std::string>::MakeVisible();
    searchArea.MakeVisible();
}

void SearchableDropDown::MakeInvisible() {
    DropDown<std::string>::MakeInvisible();
    searchArea.MakeInvisible();
}

void SearchableDropDown::SetVisible(bool flag) {
    DropDown<std::string>::SetVisible(flag);
    searchArea.SetVisible(flag);
}

void SearchableDropDown::MakeActive() {
    DropDown<std::string>::MakeActive();
    searchArea.MakeActive();
}

void SearchableDropDown::MakeInactive() {
    DropDown<std::string>::MakeInactive();
    searchArea.MakeInactive();
}

void SearchableDropDown::SetInactive(bool flag) {
    DropDown<std::string>::SetInactive(flag);
    searchArea.SetInactive(flag);
}

void SearchableDropDown::SetTextMargin(float margin) {
    TextItem::SetTextMargin(margin);
    searchArea.SetTextMargin(margin);
}

void SearchableDropDown::Focus() {
    if (opensOnSecondClick) justOpened = true;
    DropDown<std::string>::Focus();
    searchArea.Show();
}

void SearchableDropDown::Defocus() {
    searchArea.ClearText();
    searchArea.Defocus();
    searchArea.Hide();
    DropDown<std::string>::Defocus();
}

std::string SearchableDropDown::GetClassID() {
    return "SearchableDropDown";
}

void SearchableDropDown::ImmediatelyFocusSearchArea() {
    opensOnSecondClick = false;
}

void SearchableDropDown::SearchAreaFocusesOnSecondClick() {
    opensOnSecondClick = true;
}

void SearchableDropDown::SearchAreaShouldFocusOnSecondClick(bool should) {
    opensOnSecondClick = should;
}

bool SearchableDropDown::DoesSearchAreaFocusOnSecondClick() const {
    return opensOnSecondClick;
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
        if (IsContainer(item->GetFxID())) {
            dynamic_cast<Container*>(item)->Order();
        }
    }
}

std::string Workspace::GetClassID() {
    return "Workspace";
}

void AnchorPane::SetPositionsOfItems() {
    float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();
    for (auto& item : ItemsInDrawingOrder) {
        item->SetX(item->GetX() + xAnchor - previousX);
        item->SetY(item->GetY() + yAnchor - previousY);
        //item->SetWidth(item->GetWidth() + previousWidth - width); //TODO resizing items (toggable by a flag)
        //item->SetHeight(item->GetHeight() + previousHeight - height);
        if (item->GetY() + item->GetHeight() > maxY) maxY = item->GetY() + item->GetHeight();
        if (item->GetX() + item->GetWidth() > maxX) maxX = item->GetX() + item->GetWidth();
        if (IsContainer(item->GetFxID())) {
            dynamic_cast<Container*>(item)->Order();
        }
    }
    if (maxX > xAnchor + width) {
        width = maxX - xAnchor;
        if (width != previousWidth) onResize.Arm();
        previousWidth = width;
    }
    if (maxY > yAnchor + height) {
        height = maxY - yAnchor;
        if (height != previousHeight) onResize.Arm();
        previousHeight = height;
    }
    previousX = xAnchor;
    previousY = yAnchor;
    needsOrdering = false;
}

void AnchorPane::SetX(float x) {
    previousX = xAnchor;
    Container::SetX(x);
}
void AnchorPane::SetY(float y) {
    previousY = yAnchor;
    Container::SetY(y);
}

void AnchorPane::SetHeight(float value) {
    previousHeight = height;
    Container::SetHeight(value);
}

void AnchorPane::SetWidth(float value) {
    previousWidth = width;
    Container::SetWidth(value);
}

std::string AnchorPane::GetClassID() {
    return "AnchorPane";
}

void VBox::SetPositionsOfItems() {
    if (ItemsInDrawingOrder.empty()) return;

    // nested containers arrange first, so they're measured at their real size
    for (auto* item : ItemsInDrawingOrder) {
        if (IsContainer(item->GetFxID())) {
            dynamic_cast<Container*>(item)->SetPositionsOfItems();
        }
    }

    const float stackHeight = GetChildrenTotalLength();
    float widest = 0.0f;
    for (const auto* item : ItemsInDrawingOrder) {
        widest = std::max(widest, item->GetTotalWidth());
    }

    // content bigger than the box aligns as if the box already fit it - ExpandToChildren grows it after
    const float boxWidth  = std::max(width, widest);
    const float boxHeight = std::max(height, stackHeight);

    float currentY = alignment.GetAlignedY(yAnchor, stackHeight, boxHeight);
    for (auto* item : ItemsInDrawingOrder) {
        item->SetX(alignment.GetAlignedX(xAnchor, item->GetTotalWidth(), boxWidth));
        item->SetY(currentY);
        currentY += item->GetTotalHeight() + padding;
    }
    ExpandToChildren();
}

float VBox::GetTotalHeight() const {
    float heightSum = GetChildrenTotalLength();
    if (height > heightSum) {
        return height;
    }
    return heightSum;
}

float VBox::GetChildrenTotalLength() const {
    if (ItemsInDrawingOrder.empty()) return 0.0f;
    float heightSum = 0.0f;
    for (const auto& child : ItemsInDrawingOrder) {
        heightSum += child->GetTotalHeight();
    }
    return heightSum + (padding * (float)(ItemsInDrawingOrder.size() - 1));
}

float VBox::GetItemsTotalHeight() const {
    return GetChildrenTotalLength();
}

std::string VBox::GetClassID() {
    return "VBox";
}

void HBox::SetPositionsOfItems() {
    if (ItemsInDrawingOrder.empty()) return;

    // nested containers arrange first, so they're measured at their real size
    for (auto* item : ItemsInDrawingOrder) {
        if (IsContainer(item->GetFxID())) {
            dynamic_cast<Container*>(item)->SetPositionsOfItems();
        }
    }

    const float rowWidth = GetChildrenTotalLength();
    float tallest = 0.0f;
    for (const auto* item : ItemsInDrawingOrder) {
        tallest = std::max(tallest, item->GetTotalHeight());
    }

    // content bigger than the box aligns as if the box already fit it - ExpandToChildren grows it after
    const float boxWidth  = std::max(width, rowWidth);
    const float boxHeight = std::max(height, tallest);

    float currentX = alignment.GetAlignedX(xAnchor, rowWidth, boxWidth);
    for (auto* item : ItemsInDrawingOrder) {
        item->SetX(currentX);
        item->SetY(alignment.GetAlignedY(yAnchor, item->GetTotalHeight(), boxHeight));
        currentX += item->GetTotalWidth() + padding;
    }
    ExpandToChildren();
}

float HBox::GetTotalWidth() const {
    float widthSum = GetChildrenTotalLength();
    if (width > widthSum) {
        return width;
    }
    return widthSum;
}

float HBox::GetChildrenTotalLength() const {
    if (ItemsInDrawingOrder.empty()) return 0.0f;
    float widthSum = 0.0f;
    for (const auto& child : ItemsInDrawingOrder) {
        widthSum += child->GetTotalWidth();
    }
    return widthSum + (padding * (float)(ItemsInDrawingOrder.size() - 1));
}

float HBox::GetItemsTotalWidth() const {
    return GetChildrenTotalLength();
}

std::string HBox::GetClassID() {
    return "HBox";
}

//--- Spinner ---

void Spinner::DrawMyself(float elapsedTime) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    const std::string shown = (value == (int)value)
        ? std::to_string((int)value)
        : TextFormat("%.2f", value);
    font.DrawAligned(shown, Alignment::CENTRE_LEFT,
                     xAnchor + valueMargin, yAnchor, width - valueMargin, height);

    incrementButton.DrawMyself(elapsedTime);
    decrementButton.DrawMyself(elapsedTime);

    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void Spinner::DoFocusAction(float elapsedTime, Vector2 mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.Focus();
        const float old = value;
        incrementButton.onClick.ForceDo();
        if (value != old) onChange.Arm();
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.Focus();
        const float old = value;
        decrementButton.onClick.ForceDo();
        if (value != old) onChange.Arm();
    }
    decrementButton.DoFocusAction(elapsedTime);
    incrementButton.DoFocusAction(elapsedTime);
}

void Spinner::DoFocusAction(float elapsedTime) {
    incrementButton.DoFocusAction(elapsedTime);
    decrementButton.DoFocusAction(elapsedTime);
}

bool Spinner::WasIClicked(Vector2 mousePosition) const {
    float xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width + incrementButton.GetWidth()) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
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
    Item::SetX(x);
    incrementButton.SetX(xAnchor + width + border.GetThickness());
    decrementButton.SetX(xAnchor + width + border.GetThickness());
}

void Spinner::SetY(float y) {
    Item::SetY(y);
    incrementButton.SetY(yAnchor);
    decrementButton.SetY(yAnchor + (height / 2.0f));
}

void Spinner::SetWidth(float value) {
    Item::SetWidth(value);
    incrementButton.SetX(xAnchor + width + border.GetThickness());
    decrementButton.SetX(xAnchor + width + border.GetThickness());
}

void Spinner::SetHeight(float value) {
    Item::SetHeight(value);
    incrementButton.SetHeight(height / 2.0f);
    decrementButton.SetHeight(height / 2.0f);
    decrementButton.SetY(yAnchor + (height / 2.0f));
}

void Spinner::SetValueMargin(float value) {
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative value margin.");
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
    border.SetThickness(2.0f);
    incrementButton.SetX(xAnchor + width + border.GetThickness());
    incrementButton.SetY(yAnchor);
    incrementButton.SetHeight(height / 2.0f);
    incrementButton.SetWidth(75.0f);
    incrementButton.SetText("/\\");
    incrementButton.border.SetThickness(2.0f);
    incrementButton.onClick.Set([this]() { Increment(); });
    decrementButton.SetX(xAnchor + width + border.GetThickness());
    decrementButton.SetY(yAnchor + (height / 2.0f));
    decrementButton.SetHeight(height / 2.0f);
    decrementButton.SetWidth(75.0f);
    decrementButton.SetText("\\/");
    decrementButton.border.SetThickness(2.0f);
    decrementButton.onClick.Set([this]() { Decrement(); });

    decrementButton.hoveredColour.BlendTo(GREY, 0.25f);
    incrementButton.hoveredColour.BlendTo(GREY, 0.25f);
    decrementButton.SetHoverCallbacksToSwap();
    incrementButton.SetHoverCallbacksToSwap();
}

void Spinner::DoHoverAction(float /*elapsedTime*/, const Vector2 mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) incrementButton.Hover(); else incrementButton.Dehover();
    if (decrementButton.WasIClicked(mousePosition)) decrementButton.Hover(); else decrementButton.Dehover();
}

void Spinner::Dehover() {
    Item::Dehover();
    incrementButton.Dehover();
    decrementButton.Dehover();
}

void Spinner::GetCallbacks(std::vector<Callback*>& out) {
    Item::GetCallbacks(out);
    out.insert(out.end(), {&incrementButton.onHover, &incrementButton.onDehover,
                           &decrementButton.onHover, &decrementButton.onDehover});
}

void Spinner::SetButtonsWidth(float value) {
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative buttons' width.");
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
    return Item::GetTotalWidth() + incrementButton.GetWidth();
}

std::string Spinner::GetClassID() {
    return "Spinner";
}

// --- EditableSpinner ---

void EditableSpinner::SetValue(float value) {
    Spinner::SetValue(value);
    editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
}

void EditableSpinner::DrawMyself(float elapsedTime) const {
    editArea.DrawMyself(elapsedTime);
    editArea.border.DrawMyself(xAnchor, yAnchor, width, height);
    incrementButton.DrawMyself(elapsedTime);
    decrementButton.DrawMyself(elapsedTime);

    border.DrawMyself(xAnchor, yAnchor, width + incrementButton.GetWidth() + editArea.border.GetThickness(), height);
}

void EditableSpinner::DoFocusAction(float elapsedTime, Vector2 mousePosition) {
    if (incrementButton.WasIClicked(mousePosition)) {
        incrementButton.Focus();
        const float old = value;
        incrementButton.onClick.ForceDo();
        if (value != old) onChange.Arm();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
        editArea.Defocus();
    } else if (decrementButton.WasIClicked(mousePosition)) {
        decrementButton.Focus();
        editArea.Defocus();
        const float old = value;
        decrementButton.onClick.ForceDo();
        if (value != old) onChange.Arm();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else if (editArea.WasIClicked(mousePosition)) {
        editArea.Focus();
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
    } else {
        if (editArea.IsFocused()) {
            float old = value;
            try {
                value = std::stof(editArea.GetText());
            } catch (const std::invalid_argument&) {
                //do nothing. Doesn't change the value.
            } catch (const std::out_of_range&) {
                value = editArea.GetText().starts_with('-') ? std::numeric_limits<float>::lowest()
                                                            : std::numeric_limits<float>::max();
            }
            if (hasMax && value >= maxValue) {
                value = maxValue;
            } else if (hasMin && value <= minValue) {
                value = minValue;
            }
            if (old != value) onChange.Arm();
            editArea.Defocus();
        }
    }
    decrementButton.DoFocusAction(elapsedTime);
    incrementButton.DoFocusAction(elapsedTime);
    editArea.DoFocusAction(elapsedTime);
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
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative value margin");
    }
    editArea.SetTextMargin(value);
}

float EditableSpinner::GetValueMargin() const {
    return editArea.GetTextMargin();
}

void EditableSpinner::Defocus() {
    editArea.Defocus();
    Spinner::Defocus();
    float old = value;
    try {
        value = std::stof(editArea.GetText());
    } catch (const std::invalid_argument&) {
        return; //no need to reset the value if it didn't change
    } catch (const std::out_of_range&) {
        value = editArea.GetText().starts_with('-') ? std::numeric_limits<float>::lowest()
                                                    : std::numeric_limits<float>::max();
    }
    if (hasMax && value >= maxValue) {
        value = maxValue;
    } else if (hasMin && value <= minValue) {
        value = minValue;
    }
    if (old != value) onChange.Arm();
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

std::string EditableSpinner::GetClassID() {
    return "EditableSpinner";
}

// --- PasswordField ---

std::string PasswordField::ShownFromTo(size_t from, size_t to) const {
    if (IsPasswordVisible()) return StringFromTo(from, to);
    to = std::min(to, Length());
    std::string out;
    for (size_t i = from; i < to; ++i) Font::AppendCodepoint(out, (int)mask);
    return out;
}

void PasswordField::SetMask(char32_t character) {
    mask = character;
    if (!font.HasGlyph((int)mask)) {
        std::string glyph;
        Font::AppendCodepoint(glyph, (int)mask);
        font.AddCharset(glyph);
    }
}

void PasswordField::SetMask(const std::string& character) {
    int size = 0;
    const int cp = GetCodepointNext(character.c_str(), &size);
    if (character.empty() || (size_t)size != character.size()) {
        CPPFX_THROW(std::invalid_argument, "Mask must be exactly one character.");
    }
    SetMask((char32_t)cp);
}

char32_t PasswordField::GetMask() const {
    return mask;
}

void PasswordField::MakePasswordVisible() {
    isPasswordVisible = true;
}

void PasswordField::MakePasswordHidden() {
    isPasswordVisible = false;
}

void PasswordField::SetPasswordVisibility(bool visible) {
    isPasswordVisible = visible;
}

void PasswordField::SwitchPasswordVisibility() {
    isPasswordVisible = !isPasswordVisible;
}

bool PasswordField::IsPasswordVisible() const {
    return isPasswordVisible;
}

std::string PasswordField::GetClassID() {
    return "PasswordField";
}

// --- ProgressIndicator ---

float ProgressIndicator::GetSpinAngle(float elapsedTime, float revolutionDuration) const {
    const float angle = (fmod(elapsedTime, revolutionDuration) / revolutionDuration) * 360.0f;
    return clockwise ? angle : -angle;
}

void ProgressIndicator::DrawDots(float elapsedTime) const {
    const Vector2 centre = TranslateXYToCentre();
    const int count = dots.GetCount();
    const float dotSize = dots.GetSize() * GetRadius();
    const float orbit = GetRadius() - dotSize;
    const float step = (clockwise ? 360.0f : -360.0f) / (float)count;
    const float headAngle = startAngle + GetSpinAngle(elapsedTime, dots.GetRevolutionDuration());
    const int trail = std::min(dots.GetTrailLength(), count - 1);
    const float minimumAlpha = dots.GetMinimumAlpha();
    const Color base = colour.GetColour();

    for (int k = count - 1; k >= 0; --k) { // faintest first, so the head draws on top
        const float angle = (headAngle - (step * (float)k)) * DEG2RAD;
        const float alpha = (k <= trail)
                          ? 1.0f - ((1.0f - minimumAlpha) * (float)k / (float)(trail + 1))
                          : minimumAlpha;
        Color dot = base;
        dot.a = (unsigned char)((float)base.a * alpha);
        DrawCircleV({centre.x + (std::cos(angle) * orbit), centre.y + (std::sin(angle) * orbit)}, dotSize, dot);
    }
}

void ProgressIndicator::DrawArc(float elapsedTime) const {
    const Vector2 centre = TranslateXYToCentre();
    const float outer = GetRadius();
    const float inner = outer * (1.0f - arc.GetThickness());
    const float head = startAngle + GetSpinAngle(elapsedTime, arc.GetRevolutionDuration());
    const float tail = clockwise ? head - arc.GetLength() : head + arc.GetLength();

    if (trackColour.GetColour().a > 0) DrawRing(centre, inner, outer, 0.0f, 360.0f, 0, trackColour.GetColour());
    DrawRing(centre, inner, outer, std::min(head, tail), std::max(head, tail), 0, colour.GetColour());
}

void ProgressIndicator::DrawCircleFill() const {
    DrawFill(0.0f);
}

void ProgressIndicator::DrawRingFill() const {
    DrawFill(GetRadius() * (1.0f - ring.GetThickness()));
}

void ProgressIndicator::DrawFill(float innerRadius) const {
    const Vector2 centre = TranslateXYToCentre();
    const float outer = GetRadius();
    const float fullTurn = clockwise ? 360.0f : -360.0f;
    const float end = startAngle + (fullTurn * value);

    auto drawSector = [&](float from, float to, const Color& tint) {
        if (from == to || tint.a == 0) return;
        const float low = std::min(from, to), high = std::max(from, to);
        if (innerRadius <= 0.0f) DrawCircleSector(centre, outer, low, high, 0, tint);
        else DrawRing(centre, innerRadius, outer, low, high, 0, tint);
    };

    drawSector(end, startAngle + fullTurn, trackColour.GetColour()); // unfilled part only
    drawSector(startAngle, end, colour.GetColour());
}

void ProgressIndicator::SetShape(Shapes shape) {
    try {
        ShapeToString(shape);
    } catch (const std::invalid_argument&) {
        CPPFX_THROW(std::invalid_argument, "Invalid shape - cannot set shape.");
    }
    this->shape = shape;
}

void ProgressIndicator::SetShape(const std::string& shape) {
    SetShape(StringToShape(shape));
}

ProgressIndicator::Shapes ProgressIndicator::GetShape() const {
    return shape;
}

std::string ProgressIndicator::GetShapeString() const {
    return ShapeToString(shape);
}

void ProgressIndicator::SetIndefiniteShape(Shapes shape) {
    if (!IsIndefiniteCapable(shape)) {
        CPPFX_THROW(std::invalid_argument, "Shape can't be drawn without a value - cannot set indefinite shape.");
    }
    indefiniteShape = shape;
}

void ProgressIndicator::SetIndefiniteShape(const std::string& shape) {
    SetIndefiniteShape(StringToShape(shape));
}

ProgressIndicator::Shapes ProgressIndicator::GetIndefiniteShape() const {
    return indefiniteShape;
}

std::string ProgressIndicator::GetIndefiniteShapeString() const {
    return ShapeToString(indefiniteShape);
}

void ProgressIndicator::MakeClockwise() {
    clockwise = true;
}

void ProgressIndicator::MakeCounterclockwise() {
    clockwise = false;
}

void ProgressIndicator::SetClockwise(bool flag) {
    clockwise = flag;
}

bool ProgressIndicator::IsClockwise() const {
    return clockwise;
}

void ProgressIndicator::SetStartAngle(float degrees) {
    startAngle = degrees;
}

float ProgressIndicator::GetStartAngle() const {
    return startAngle;
}

std::string ProgressIndicator::GetClassID() {
    return "ProgressIndicator";
}

bool ProgressIndicator::IsIndefiniteCapable(Shapes shape) {
    switch (shape) {
        case DOTS:
        case ARC:
            return true;
        case CIRCLE:
        case RING:
            return false;
    }
    return false;
}

ProgressIndicator::Shapes ProgressIndicator::StringToShape(const std::string& shape) const {
    static const std::unordered_map<std::string, Shapes> shapeMap = {
        {"DOTS", DOTS},
        {"ARC", ARC},
        {"CIRCLE", CIRCLE},
        {"RING", RING}
    };

    std::string normal = shape;
    std::transform(normal.begin(), normal.end(), normal.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    auto it = shapeMap.find(normal);
    if (it != shapeMap.end()) return it->second;

    CPPFX_THROW(std::invalid_argument, "Invalid shape (" + shape + ")");
}

std::string ProgressIndicator::ShapeToString(Shapes shape) const {
    switch (shape) {
        case DOTS:   return "DOTS";
        case ARC:    return "ARC";
        case CIRCLE: return "CIRCLE";
        case RING:   return "RING";
    }
    CPPFX_THROW(std::invalid_argument, "Invalid shape");
}

void ProgressIndicator::DrawProgress(float elapsedTime) const {
    const Shapes drawn = (IsIndefinite() && !IsIndefiniteCapable(shape)) ? indefiniteShape : shape;
    switch (drawn) {
        case DOTS:   DrawDots(elapsedTime); break;
        case ARC:    DrawArc(elapsedTime);  break;
        case CIRCLE: DrawCircleFill();      break;
        case RING:   DrawRingFill();        break;
    }
}

void ProgressIndicator::DrawValue() const {
    const Vector2 corner = TranslateXYToCorner();
    font.DrawAligned(FormatPercentage((float)GetFilledCount(1000) / 1000.0f), Alignment::CENTRE, corner.x, corner.y, width, width);
}

// --- ProgressIndicator::Dots ---

void ProgressIndicator::Dots::SetCount(int count) {
    if (count < 1) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Dot count less than 1 - cannot set it.");
    }
    this->count = count;
}

int ProgressIndicator::Dots::GetCount() const {
    return count;
}

void ProgressIndicator::Dots::SetSize(float fraction) {
    if (!(fraction > 0.0f && fraction < 1.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Dot size outside (0, 1) - cannot set it.");
    }
    size = fraction;
}

float ProgressIndicator::Dots::GetSize() const {
    return size;
}

void ProgressIndicator::Dots::SetTrailLength(int length) {
    if (length < 0) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Negative trail length - cannot set it.");
    }
    trailLength = length;
}

int ProgressIndicator::Dots::GetTrailLength() const {
    return trailLength;
}

void ProgressIndicator::Dots::SetMinimumAlpha(float alpha) {
    if (!(alpha >= 0.0f && alpha <= 1.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Minimum alpha outside 0 - 1 - cannot set it.");
    }
    minimumAlpha = alpha;
}

float ProgressIndicator::Dots::GetMinimumAlpha() const {
    return minimumAlpha;
}

void ProgressIndicator::Dots::SetRevolutionDuration(float seconds) {
    if (!(seconds > 0.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Revolution duration must be positive - cannot set it.");
    }
    revolutionDuration = seconds;
}

float ProgressIndicator::Dots::GetRevolutionDuration() const {
    return revolutionDuration;
}

// --- ProgressIndicator::Arc ---

void ProgressIndicator::Arc::SetLength(float degrees) {
    if (!(degrees > 0.0f && degrees < 360.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Arc length outside (0, 360) - cannot set it.");
    }
    length = degrees;
}

float ProgressIndicator::Arc::GetLength() const {
    return length;
}

void ProgressIndicator::Arc::SetThickness(float fraction) {
    if (!(fraction > 0.0f && fraction <= 1.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Arc thickness outside (0, 1] - cannot set it.");
    }
    thickness = fraction;
}

float ProgressIndicator::Arc::GetThickness() const {
    return thickness;
}

void ProgressIndicator::Arc::SetRevolutionDuration(float seconds) {
    if (!(seconds > 0.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Revolution duration must be positive - cannot set it.");
    }
    revolutionDuration = seconds;
}

float ProgressIndicator::Arc::GetRevolutionDuration() const {
    return revolutionDuration;
}

// --- ProgressIndicator::Ring ---

void ProgressIndicator::Ring::SetThickness(float fraction) {
    if (!(fraction > 0.0f && fraction <= 1.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Ring thickness outside (0, 1] - cannot set it.");
    }
    thickness = fraction;
}

float ProgressIndicator::Ring::GetThickness() const {
    return thickness;
}

// --- ProgressBar::Sweep ---

void ProgressBar::Sweep::SetLength(float fraction) {
    if (!(fraction > 0.0f && fraction <= 1.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Sweep length outside (0, 1] - cannot set it.");
    }
    length = fraction;
}

float ProgressBar::Sweep::GetLength() const {
    return length;
}

void ProgressBar::Sweep::SetPassDuration(float seconds) {
    if (!(seconds > 0.0f)) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Pass duration must be positive - cannot set it.");
    }
    passDuration = seconds;
}

float ProgressBar::Sweep::GetPassDuration() const {
    return passDuration;
}

void ProgressBar::Sweep::SetMotion(Motions motion) {
    try {
        MotionToString(motion);
    } catch (const std::invalid_argument&) {
        CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Invalid motion - cannot set motion.");
    }
    this->motion = motion;
}

void ProgressBar::Sweep::SetMotion(const std::string& motion) {
    SetMotion(StringToMotion(motion));
}

ProgressBar::Sweep::Motions ProgressBar::Sweep::GetMotion() const {
    return motion;
}

std::string ProgressBar::Sweep::GetMotionString() const {
    return MotionToString(motion);
}

ProgressBar::Sweep::Motions ProgressBar::Sweep::StringToMotion(const std::string& motion) const {
    static const std::unordered_map<std::string, Motions> motionMap = {
        {"WRAP", WRAP},
        {"BOUNCE", BOUNCE}
    };

    std::string normal = motion;
    std::transform(normal.begin(), normal.end(), normal.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    auto it = motionMap.find(normal);
    if (it != motionMap.end()) return it->second;

    CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Invalid motion (" + motion + ")");
}

std::string ProgressBar::Sweep::MotionToString(Motions motion) const {
    switch (motion) {
        case WRAP:   return "WRAP";
        case BOUNCE: return "BOUNCE";
    }
    CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Invalid motion");
}

// --- ProgressBar ---

void ProgressBar::SetShape(Shapes shape) {
    try {
        ShapeToString(shape);
    } catch (const std::invalid_argument&) {
        CPPFX_THROW(std::invalid_argument, "Invalid shape - cannot set shape.");
    }
    this->shape = shape;
}

void ProgressBar::SetShape(const std::string& shape) {
    SetShape(StringToShape(shape));
}

ProgressBar::Shapes ProgressBar::GetShape() const {
    return shape;
}

std::string ProgressBar::GetShapeString() const {
    return ShapeToString(shape);
}

ProgressBar::Shapes ProgressBar::StringToShape(const std::string& shape) const {
    static const std::unordered_map<std::string, Shapes> shapeMap = {
        {"FILL", FILL},
        {"SWEEP", SWEEP}
    };

    std::string normal = shape;
    std::transform(normal.begin(), normal.end(), normal.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    auto it = shapeMap.find(normal);
    if (it != shapeMap.end()) return it->second;

    CPPFX_THROW(std::invalid_argument, "Invalid shape (" + shape + ")");
}

std::string ProgressBar::ShapeToString(Shapes shape) const {
    switch (shape) {
        case FILL:  return "FILL";
        case SWEEP: return "SWEEP";
    }
    CPPFX_THROW(std::invalid_argument, "Invalid shape");
}

void ProgressBar::SetBarMargin(float value) {
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative bar margin.");
    }
    barMargin = value;
    if (barMargin > GetMaxBarMargin()) {
        CPPFX_WARN("In " + fxID + " " + ID + ": bar margin " + std::to_string(value) + " doesn't fit - drawn as " + std::to_string(GetMaxBarMargin()) + ".");
    }
}

float ProgressBar::GetBarMargin() const {
    return barMargin;
}

void ProgressBar::SetNumberOfSegments(int value) {
    if (value < 1) {
        CPPFX_THROW(std::invalid_argument, "Number of segments less than 1.");
    }
    numberOfSegments = value;
    if (gapBetweenSegments > GetMaxGap()) {
        CPPFX_WARN("In " + fxID + " " + ID + ": gap between segments doesn't fit " + std::to_string(value) + " segments - drawn as " + std::to_string(GetMaxGap()) + ".");
    }
}

int ProgressBar::GetNumberOfSegments() const {
    return numberOfSegments;
}

void ProgressBar::SetGapBetweenSegments(float value) {
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative width of gap between segments.");
    }
    gapBetweenSegments = value;
    if (gapBetweenSegments > GetMaxGap()) {
        CPPFX_WARN("In " + fxID + " " + ID + ": gap between segments " + std::to_string(value) + " doesn't fit - drawn as " + std::to_string(GetMaxGap()) + ".");
    }
}

float ProgressBar::GetGapBetweenSegments() const {
    return gapBetweenSegments;
}

void ProgressBar::SetSegments(int number, float gap) {
    if (number < 1) {
        CPPFX_THROW(std::invalid_argument, "Number of segments less than 1.");
    }
    if (gap < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative width of gap between segments.");
    }
    numberOfSegments = number;
    gapBetweenSegments = gap;
    if (gapBetweenSegments > GetMaxGap()) {
        CPPFX_WARN("In " + fxID + " " + ID + ": gap between segments " + std::to_string(gap) + " doesn't fit " + std::to_string(number) + " segments - drawn as " + std::to_string(GetMaxGap()) + ".");
    }
}

void ProgressBar::Segmented() {
    segmented = true;
}

void ProgressBar::Solid() {
    segmented = false;
}

void ProgressBar::SetSegmented(bool flag) {
    segmented = flag;
}

bool ProgressBar::IsSegmented() const {
    return segmented;
}

std::string ProgressBar::GetClassID() {
    return "ProgressBar";
}

float ProgressBar::GetMaxBarMargin() const {
    return std::min(width, height) / 2.0f;
}

float ProgressBar::GetEffectiveBarMargin() const {
    return std::min(barMargin, GetMaxBarMargin());
}

float ProgressBar::GetMaxGap() const {
    if (numberOfSegments < 2) return std::numeric_limits<float>::max(); // no gaps to fit
    const float innerWidth = width - (2.0f * GetEffectiveBarMargin());
    return std::max(0.0f, (innerWidth - ((float)numberOfSegments * minimumSegmentWidth)) / (float)(numberOfSegments - 1));
}

float ProgressBar::GetEffectiveGap() const {
    return std::min(gapBetweenSegments, GetMaxGap());
}

::Rectangle ProgressBar::GetInnerRectangle() const {
    const float margin = GetEffectiveBarMargin();
    return ::Rectangle{xAnchor + margin, yAnchor + margin, width - (2.0f * margin), height - (2.0f * margin)};
}

float ProgressBar::GetSegmentWidth() const {
    return (GetInnerRectangle().width - (GetEffectiveGap() * (float)(numberOfSegments - 1))) / (float)numberOfSegments;
}

int ProgressBar::GetSweepSegments() const {
    return std::clamp((int)std::lround(sweep.GetLength() * (float)numberOfSegments), 1, numberOfSegments);
}

float ProgressBar::GetSweepPhase(float elapsedTime) const {
    const float duration = sweep.GetPassDuration();
    if (sweep.GetMotion() == Sweep::BOUNCE) {
        const float t = fmod(elapsedTime, 2.0f * duration) / duration; // 0 - 2: there and back
        return (t <= 1.0f) ? t : 2.0f - t;
    }
    return fmod(elapsedTime, duration) / duration;
}

void ProgressBar::DrawProgress(float elapsedTime) const {
    DrawRectangleRec(::Rectangle{xAnchor, yAnchor, width, height}, colour.GetColour());
    if (IsIndefinite() || shape == SWEEP) {
        if (segmented) DrawSegmentedSweep(elapsedTime);
        else DrawSweep(elapsedTime);
    } else {
        if (segmented) DrawSegments();
        else DrawSolid();
    }
}

void ProgressBar::DrawSolid() const {
    const ::Rectangle inner = GetInnerRectangle();
    DrawRectangleRec(::Rectangle{inner.x, inner.y, value * inner.width, inner.height}, barColour.GetColour());
}

void ProgressBar::DrawSegment(int index) const {
    const ::Rectangle inner = GetInnerRectangle();
    const float segmentWidth = GetSegmentWidth();
    DrawRectangleRec(::Rectangle{inner.x + ((float)index * (segmentWidth + GetEffectiveGap())), inner.y, segmentWidth, inner.height}, barColour.GetColour());
}

void ProgressBar::DrawSegments() const {
    const int filled = GetFilledCount(numberOfSegments);
    for (int i = 0; i < filled; ++i) DrawSegment(i);
}

void ProgressBar::DrawSweep(float elapsedTime) const {
    const ::Rectangle inner = GetInnerRectangle();
    const float chunk = sweep.GetLength();
    const float phase = GetSweepPhase(elapsedTime);
    const float start = (sweep.GetMotion() == Sweep::WRAP)
                      ? (phase * (1.0f + chunk)) - chunk // slides in from the left edge and out past the right
                      : phase * (1.0f - chunk);          // stays inside the bar
    const float left = std::max(start, 0.0f), right = std::min(start + chunk, 1.0f);
    if (right <= left) return;
    DrawRectangleRec(::Rectangle{inner.x + (left * inner.width), inner.y, (right - left) * inner.width, inner.height}, barColour.GetColour());
}

void ProgressBar::DrawSegmentedSweep(float elapsedTime) const {
    const int lit = GetSweepSegments();
    const float phase = GetSweepPhase(elapsedTime);
    int first;
    if (sweep.GetMotion() == Sweep::WRAP) {
        const int positions = numberOfSegments + lit - 1; // enters and leaves one segment at a time
        first = std::min((int)(phase * (float)positions), positions - 1) - (lit - 1);
    } else {
        const int positions = numberOfSegments - lit + 1; // stays inside the bar
        first = std::min((int)(phase * (float)positions), positions - 1);
    }
    for (int i = std::max(first, 0); i < std::min(first + lit, numberOfSegments); ++i) DrawSegment(i);
}

// --- Pressed button ---

void PressedButton::DrawMyself(float /*elapsedTime*/) const {
    std::string textToDisplay;
    if (IsPressed()) {
        DrawRectangle(xAnchor, yAnchor, width, height, PersistentState::pressedColour.GetColour());
        textToDisplay = pressedText;
    } else {
        DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
        textToDisplay = GetText();
    }

    if (textToDisplay != "") {
       DrawAlignedText(Alignment::CENTRE, textToDisplay, font);
    }
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

void PressedButton::DoFocusAction(float /*elapsedTime*/) {
    return;
}

void PressedButton::DoFocusAction(float /*elapsedTime*/, Vector2 /*mousePosition*/) {
    SwitchPress();
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

std::string PressedButton::GetClassID() {
    return "PressedButton";
}

// ---------- Pie chart -----------

void PieChart::DoFocusAction(float /*elapsedTime*/) {
    return;
}

void PieChart::DrawMyself(float /*elapsedTime*/) const {
    if (values.empty()) {
        return;
    }
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    std::vector<Color> colours = {BLUE, RED, GREEN, PINK, BROWN, DARKGREEN, PURPLE}; //TODO Add user colours

    border.DrawMyself(TranslateXToCentre(), TranslateYToCentre(), GetRadius(), GetRadius());

    float currentAngle = 0.0f;
    size_t index = 0;
    for (auto& element : values) {
        double percent = element / sum;
        Color c;
        if (index >= colours.size()) {
            c = colours[index % colours.size()];
        } else c = colours[index];

        DrawCircleSector(Vector2{TranslateXToCentre(), TranslateYToCentre()}, GetRadius(), currentAngle, currentAngle + (percent * 360), std::max(4, (int)(percent * 36)), c);

        if (showPercentage) {
            const float textAngle = currentAngle + (180.0f * percent);
            const std::string valueString = FormatPercentage((float)percent);

            const float px = TranslateXToCentre() + (GetRadius() / 2.0f * (float)cos(textAngle * DEG2RAD));
            const float py = TranslateYToCentre() + (GetRadius() / 2.0f * (float)sin(textAngle * DEG2RAD));
            font.DrawAligned(valueString, Alignment::CENTRE, px, py, 0.0f, 0.0f);
        }

        if (showLabels) {
            const float textAngle = currentAngle + (180.0f * percent);
            const std::string shown = (values.size() == labels.size())
                ? labels[index]
                : ((element == (int)element) ? TextFormat("%d", (int)element)
                                             : TextFormat("%.2f", element));

            Alignment::Alignments a = Alignment::CENTRE;
            if (textAngle > 90.0f && textAngle < 270.0f)      a = Alignment::CENTRE_RIGHT;
            else if (textAngle < 90.0f || textAngle > 270.0f) a = Alignment::CENTRE_LEFT;

            const float pad = (a == Alignment::CENTRE_LEFT) ? border.GetThickness()
                            : (a == Alignment::CENTRE_RIGHT) ? -border.GetThickness()
                            : 0.0f;
            const float lx = TranslateXToCentre() + (GetRadius() * (float)cos(textAngle * DEG2RAD)) + pad;
            const float ly = TranslateYToCentre() + (GetRadius() * (float)sin(textAngle * DEG2RAD));
            font.DrawAligned(shown, a, lx, ly, 0.0f, 0.0f);
        }
        currentAngle += 360.0f * percent;
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

std::string PieChart::GetClassID() {
    return "PieChart";
}

// --- Line ---
float Line::CalculateMyArea() const {
    if (pointToPoint) {
        return sqrt(pow((xEnd - xAnchor), 2) + pow((yEnd - yAnchor), 2));
    }
    return GetLength();
}

void Line::DrawMyself(float /*elapsedTime*/) const {
    if (pointToPoint) {
        DrawLineEx({xAnchor, yAnchor}, {xEnd, yEnd}, thickness, colour.GetColour());
    } else {
        DrawLineEx({xAnchor, yAnchor},
                   {xAnchor + std::cos(angle * DEG2RAD) * width,
                    yAnchor + std::sin(angle * DEG2RAD) * width},
                    thickness, colour.GetColour());
    }
}

void Line::SetLength(float length) {
    if (length < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative length.");
    }
    width = length;
}

float Line::GetLength() const {
    return width;
}

void Line::SetWidth(float value) {
    if (pointToPoint) {
        xEnd = xAnchor + value; // set horizontal cathetus
    } else {
        SetLength(value); // in a&l mode, width means length
    }
}

void Line::SetHeight(float value) {
    if (pointToPoint) {
        yEnd = yAnchor + value; // set vertical cathetus
    } else {
        SetLength(value);
    }
}

float Line::GetWidth() const {
    if (pointToPoint) {
        return std::abs(xEnd - xAnchor);
    } else {
        return std::abs(std::cos(angle * DEG2RAD) * width);
    }
}

float Line::GetHeight() const {
    if (pointToPoint) {
        return std::abs(yEnd - yAnchor);
    } else {
        return std::abs(std::sin(angle * DEG2RAD) * width);
    }
}

float Line::GetTotalHeight() const {
    if (pointToPoint) {
        return std::abs(yEnd - yAnchor) + thickness;
    } else {
        return std::abs(std::sin(angle * DEG2RAD) * width) + thickness;
    }
}

float Line::GetTotalWidth() const {
    if (pointToPoint) {
        return std::abs(xEnd - xAnchor) + thickness;
    } else {
        return std::abs(std::cos(angle * DEG2RAD) * width) + thickness;
    }
}

void Line::SetAngle(float angle) {
    if (angle < 0.0f || angle > 360.0f) {
        CPPFX_THROW(std::out_of_range, "Angle beyond the valid range [0 - 360].");
    }
    this->angle = angle;
}

void Line::IncreaseAngle(float change) {
    angle += change;
    if (angle < 0.0f) {
        angle = 360.0f + angle;
    } else if (angle > 360.0f) {
        angle -= 360.0f;
    }
}

float Line::GetAngle() const {
    return angle;
}

void Line::SetEndPoint(float x, float y) {
    if (x == xAnchor && y == yAnchor) {
        CPPFX_WARN("In Line " + ID + ": new end coordinates are the same as the start coordinates. The line will not be visible.");
    }
    xEnd = x;
    yEnd = y;
    CalculateLength();
}

void Line::SetEndPoint(Vector2 coordinates) {
    SetEndPoint(coordinates.x, coordinates.y);
}

void Line::CalculateLength() {
    width = CalculateMyArea();
}

float Line::GetEndX() const {
    return xEnd;
}

float Line::GetEndY() const {
    return yEnd;
}

Vector2 Line::GetEndPoint() const {
    return Vector2{xEnd, yEnd};
}

void Line::SetThickness(float thickness) {
    if (thickness < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative thickness of the line.");
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

std::string Line::GetClassID() {
    return "Line";
}

// --- Square ---

void CPPFX::Square::DrawMyself(float /*elapsedTime*/) const {
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
    return Item::GetWidth();
}

float CPPFX::Square::GetTotalHeight() const {
    return Item::GetTotalWidth();
}

std::string CPPFX::Square::GetClassID() {
    return "Square";
}

// --- Rectangle ---

void CPPFX::Rectangle::DrawMyself(float /*elapsedTime*/) const {
    DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
    border.DrawMyself(xAnchor, yAnchor, width, height);
}

float CPPFX::Rectangle::CalculateMyArea() const {
    return width * height;
}

std::string CPPFX::Rectangle::GetClassID() {
    return "Rectangle";
}

// --- Circle ---

void CPPFX::Circle::DrawMyself(float /*elapsedTime*/) const {
    border.DrawMyself(TranslateXToCentre(), TranslateYToCentre(), GetRadius(), GetRadius());
    DrawCircle(TranslateXToCentre(), TranslateYToCentre(), GetRadius(), colour.GetColour());
}

float CPPFX::Circle::CalculateMyArea() const {
    return PI * GetRadius() * GetRadius();
}

std::string Circle::GetClassID() {
    return "Circle";
}

// --- Sprite ---

void Sprite::DrawMyself(float /*elapsedTime*/) const {
    if (!textureHasBeenLoaded) {
        return;
    }
    DrawTextureEx(texture, Vector2{xAnchor + xShift, yAnchor + yShift}, rotation, scale, colour.GetColour());
}

void Sprite::DoFocusAction(float /*elapsedTime*/) {
    return;
}

void Sprite::SetFilePath(const std::string& path) {
    if (path.empty()) {
        CPPFX_THROW(std::invalid_argument, "The file name is empty.");
    }
    filePath = path;
}

std::string Sprite::GetFilePath() const {
    return filePath;
}

void Sprite::ClearFilePath() {
    filePath = "";
}

void Sprite::LoadTexture() {
    if (filePath.empty()) {
        CPPFX_THROW(std::runtime_error, "No file path set.");
    }

    if (textureHasBeenLoaded && ownsTexture) {
        UnloadTexture();
    }

    texture = ::LoadTexture(filePath.c_str());

    if (!::IsTextureValid(texture)) {
        CPPFX_THROW(std::runtime_error, "Texture at " + filePath + " did not load correctly.");
    }
    textureHasBeenLoaded = true;
    ownsTexture = true;
    if (autoFits) FitToTexture();
}

void Sprite::LoadTexture(const std::string& fileName) {
    if (fileName.empty()) {
        CPPFX_THROW(std::invalid_argument, "The file name is empty.");
    }

    if (textureHasBeenLoaded && ownsTexture) {
        UnloadTexture();
    }

    texture = ::LoadTexture(fileName.c_str());

    if (!::IsTextureValid(texture)) {
        CPPFX_THROW(std::runtime_error, "Texture at " + fileName + " did not load correctly.");
    }
    textureHasBeenLoaded = true;
    ownsTexture = true;
    if (autoFits) FitToTexture();
}

void Sprite::UnloadTexture() {
    if (!ownsTexture) {
        CPPFX_THROW(std::runtime_error, "This sprite is not the owner of this texture!");
    }
    ::UnloadTexture(texture);
    textureHasBeenLoaded = false;
    ownsTexture = false;
}

bool Sprite::IsTextureValid() const {
    return ::IsTextureValid(texture);
}

void Sprite::SetRotation(float rotation) {
    this->rotation = rotation;
}

float Sprite::GetRotation() const {
    return rotation;
}

void Sprite::SetScale(float scale) {
    if (scale == 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Scale cannot be 0.");
    }

    this->scale = scale;

    if (autoFits && textureHasBeenLoaded) FitToTexture();
}

float Sprite::GetScale() const {
    return scale;
}

void Sprite::SetTexture(const Texture2D& texture) {
    if (!::IsTextureValid(texture)) {
        CPPFX_THROW(std::invalid_argument, "New texture was not loaded correctly.");
    }
    if (textureHasBeenLoaded && ownsTexture) { UnloadTexture(); }
    this->texture = texture;
    textureHasBeenLoaded = true;
    ownsTexture = false;
    if (autoFits) FitToTexture();
}

Texture2D Sprite::GetTexture() const {
    return texture;
}

void Sprite::ClearTexture() {
    if (textureHasBeenLoaded && ownsTexture) { UnloadTexture(); }
    texture = {};
    textureHasBeenLoaded = false;
    ownsTexture = false;
}

void Sprite::FitToTexture() {
    if (!::IsTextureValid(texture)) {
        CPPFX_THROW(std::runtime_error, "No valid texture to fit to.");
    }

    SetWidth(texture.width * scale);
    SetHeight(texture.height * scale);
}

void Sprite::EnableAutoFitting() {
    autoFits = true;
}

void Sprite::DisableAutoFitting() {
    autoFits = false;
}

void Sprite::SetAutoFitting(bool value) {
    autoFits = value;
}

bool Sprite::IsAutoFitting() const {
    return autoFits;
}

void Sprite::SetXShift(float shift) {
    xShift = shift;
}

float Sprite::GetXShift() const {
    return xShift;
}

void Sprite::SetYShift(float shift) {
    yShift = shift;
}

float Sprite::GetYShift() const {
    return yShift;
}

void Sprite::SetXYShift(float shift) {
    SetXShift(shift);
    SetYShift(shift);
}

void Sprite::SetXYShift(Vector2 xyShift) {
    SetXShift(xyShift.x);
    SetYShift(xyShift.y);
}

Vector2 Sprite::GetXYShift() const {
    return Vector2{xShift, yShift};
}

std::string Sprite::GetClassID() {
    return "Sprite";
}
