#ifndef ITEMS_H
#define ITEMS_H

#include "raylib.h"
#include "mixins.h"
#include "bases.h"

/*********************************************************************************
 *  @file items
 *  @brief This file contains the widgets.
 ********************************************************************************/

namespace CPPFX {

// --- Text Items ---

/**
 *  @class Label
 *  @brief A simple text displayer.
 *  @details Displays the text. Position is based on alignment. Has border.
 */
class Label : public TextItem {
public:

    Alignment alignment;

    Label() : Item("Label"), TextItem("Label") {colour.SetColour(BLANK); text = "Label";}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

};

/**
 *  @class TextField
 *  @brief Allows user to enter text into the field.
 *  @details Displays the text and allows input when focused. Has border.
 */
class TextField : public TextItem {
public:


    TextField() : Item("TextField"), TextItem("TextField"), promptText("") {}
    TextField(const std::string& i) : Item("TextField"), TextItem(i), promptText("") {}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    void SetPromptText(const std::string& text);
    void ClearPromptText();
    std::string GetPromptText() const;

protected:
    std::string promptText; ///< string displayed when the text is empty
};

/**
 * @class Button
 * @brief A single click pushable item.
 * @details Unpushes after a click. Has border. Has colour for clicked and unclicked state.
 */
class Button : public TextItem {
public:

    Colour pressedColour;
    Colour unPressedColour;

    Button() : Item("Button"), TextItem("Button"), pressedColour(GREY), unPressedColour(LIGHTGREY) {}
    Button(const std::string& id) : TextItem(id), pressedColour(GREY), unPressedColour(LIGHTGREY) {}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

};

/**
 * @class CheckBox
 * @brief A turn on - turn off button.
 * @details Width is the length of the box, not the total one. Total length is width + MeasureText(text) + textMargin + labelMargin. Has border. Has colour for pressed state and unpressed.
 */
class CheckBox : public TextItem, public virtual PersistentState {
public:
     ///<automatically stretches to accommodate text.
    Border clickBorder; ///< border solely around clickable area.

    Colour pressedColour;
    Colour unPressedColour;

    bool drawsX = true; //TODO add different shapes

    CheckBox() : Item("CheckBox", 50, 50), TextItem("CheckBox", 50, 50), PersistentState(), pressedColour(GRAY), unPressedColour(LIGHTGRAY), labelMargin(10) {font.SetFontSize(height / 2);}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    float GetTotalWidth() const override;

    /**
     *  @brief Sets the distance between the text and the box.
     *  @param value new value to be set to
     *  @throws std::invalid_argument if the new value is negative.
     */
    void SetLabelMargin(float value);
    float GetLabelMargin() const;

private:
    float labelMargin; ///<distance between the box and the label text.
};

/**
 *  @class Dropdown
 *  @brief A choice menu dropping down after a click.
 *  @details Has border. Works as template <>. Unrolled height is = height * number of items inside.
 *  @note You probably want to set its priority to top, to make sure that unrolled it will remain on top.
 */
template <typename T>
class DropDown : public TextItem {
public:

    DropDown() : Item("DropDown"), TextItem("DropDown"), currentLabel("") {}

    void DrawMyself(float elapsedTime) const override {
        DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
        if (currentLabel != "") {
            DrawText(Truncate(currentLabel).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
        }
        if (border.GetThickness() > 0) {
            border.DrawMyself(xAnchor, yAnchor, width + textMargin, height);
        }
        if (focused) {
            float yCurrent = yAnchor + height;
            DrawLineEx({xAnchor, yCurrent}, {xAnchor + width, yCurrent}, 10, BLACK);
            for (auto& [label, value] : values) { //TODO make insertion and other orders available
                DrawLineEx({xAnchor, yCurrent}, {xAnchor + width, yCurrent}, 5, BLACK);
                DrawRectangle(xAnchor, yCurrent, width, height, colour.GetColour());
                DrawText(Truncate(label).c_str(), xAnchor + textMargin, yCurrent + (height / 2) - (font.GetFontSize() / 2), font.GetFontSize(), font.colour.GetColour());
                yCurrent += height;
            }
        }
    }

    bool WasIClicked(const Vector2& mousePosition) const override {
        if (focused) {
            if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width && mousePosition.y >= yAnchor && mousePosition.y <= yAnchor + ((values.size() + 1) * height)) {
                return true;
            }
            return false;
        } else return Item::WasIClicked(mousePosition);
    }

    bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const override {
        float height = this->height * camera.zoom;
        float width = this->width * camera.zoom;
        //std::cout << "xAnchor: " << xAnchor << " yAnchor: " << yAnchor << " xEnd: " << xAnchor + width << " yEnd: " << yAnchor + height << " mouseX: " << mousePosition.x << " mouseY: " << mousePosition.y << " zoom: " << camera.zoom << "\n";
        if (focused) {
            if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width && mousePosition.y >= yAnchor && mousePosition.y <= yAnchor + ((values.size() + 1) * height)) {
                return true;
            }
            return false;
        } else {
            if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width && mousePosition.y >= yAnchor && mousePosition.y <= yAnchor + height) {
                return true;
            }
            return false;
        }
    }

    void DoFocusAction(float elapsedTime) {
        return;
    }

    void DoPassiveAction(float elapsedTime) override {
        if (currentLabel == "" && !values.empty()) {
            SetCurrent(values.begin()->first);
        }
    }

    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override {
        if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width && mousePosition.y <= (yAnchor + ((values.size() + 1) * height)) && mousePosition.y >= yAnchor + height) {
            int index = (mousePosition.y - (yAnchor + height)) / height;
            auto it = std::next(values.begin(), index);
            currentLabel = it->first;
            currentValue = it->second;
            focused = false;
            return;
        }
        if (!Item::WasIClicked(mousePosition)) {
            Defocus();
        }
    }

    void DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera) override {
        float height = this->height * camera.zoom;

        if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + (width * camera.zoom) && mousePosition.y <= (yAnchor + ((values.size() + 1) * height)) && mousePosition.y >= yAnchor + height) {
            int index = (mousePosition.y - (yAnchor + height)) / height;
            auto it = std::next(values.begin(), index);
            currentLabel = it->first;
            currentValue = it->second;
            focused = false;
            std::cout << "DoFocusAction fired. mouseY: " << mousePosition.y << " yAnchor: " << yAnchor << " yAnchor+height: " << yAnchor + height << "\n";
            return;
        }
        if (!WasIClicked(mousePosition, camera)) {
            Defocus();
        }
    }

    /**
     *  @brief Inserts the value into the dropdown.
     *  @param label name under which the value will be displayed
     *  @param value the new value
     *  @throws std::invalid_argument if the label is empty.
     */
    void InsertItem(const std::string& label, const T& value) {
        if (label == "") {
            CPPFX_WARN("Use space (' ') if you want to have it actually empty.");
            throw std::invalid_argument("In dropdown " + ID + ": cannot add item with empty label.");
        }
        values.insert({label,value});
    }

    /**
     *  @brief Removes a value from the dropdown.
     *  @param label label for removal
     *  @throws std::out_of_range if there's no label in the dropdown.
     */
    void RemoveItem(const std::string& label) {
        if (IsLabelTaken(label)) {
            values.erase(label);
        } else throw std::out_of_range("In " + this->ID + ": No value with label " + label + " found.");
    }

    /**
     *  @brief Replaces the old label with the new one.
     *  @details Doesn't change the value.
     *  @param oldLabel label to be replaced
     *  @param newLabel new label on that spot
     *  @throws std::out_of_range if there's no oldLabel.
     *  @throws std::invalid_argument if newLabel already exists.
     */
    void ChangeLabel(const std::string& oldLabel, const std::string& newLabel) {
        if (!IsLabelTaken(oldLabel)) {
            throw std::out_of_range("In " + this->ID + ": No value with label " + oldLabel + " found - couldn't replace it.");
        }
        if (IsLabelTaken(newLabel)) {
            throw std::invalid_argument("In " + this->ID + ": " + newLabel + " already is in the dropdown.");
        }
        auto node = values.extract(oldLabel);
        node.key() = newLabel;
        values.insert(std::move(node));
    }

    /**
     *  @brief Changes a value on the specified key
     *  @param label the key
     *  @param newValue value that'll replace the current one
     *  @throws std::out_of_range if the label is not in the dropdown.
     */
    void ChangeValue(const std::string& label, const T& newValue) {
        if (IsLabelTaken(label)) {
            values.find(label)->second = newValue;
        } else throw std::out_of_range("No key " + label + " found in dropdown " + ID);
    }

    /**
     *  @brief Gets a value from the label.
     *  @param label the key
     *  @return the value from the specified key
     *  @throws std::out_of_range if there's no such label in dropdown
     */
    T GetValue(const std::string& label) const {
        if (IsLabelTaken(label)) {
            return values.at(label);
        } else throw std::out_of_range("No key " + label + " found in dropdown " + ID);
    }

    T GetCurrentValue() const {
        return currentValue;
    }

    std::string GetCurrentLabel() const {
        return currentLabel;
    }

    /**
     *  @brief Sets current label and value to another item based on label.
     *  @param label name of the item to be set to.
     *  @throws std::out_of_range if no label found
     */
    void SetCurrent(const std::string& label) {
        if (IsLabelTaken(label)) {
            currentLabel = label;
            currentValue = values.at(currentLabel);
        } else throw std::out_of_range("No key " + label + " found in dropdown " + ID);
    }

    /**
     *  @see TextItem::ExpandToText.
     *  @details Works based on currentLabel.
     */
    void ExpandToText() override {
        int textWidth = MeasureText(currentLabel.c_str(), font.GetFontSize());
        if (textWidth > width) {
            width = textWidth + textMargin;
        }

        if (font.GetFontSize() > height) {
            height += font.GetFontSize() - height;
        }
    }

    /**
     *  @see TextItem::FitToText.
     *  @details Works based on currentLabel.
     */
    void FitToText() {
        int textWidth = MeasureText(currentLabel.c_str(), font.GetFontSize());
        if (textWidth != width) {
            width = textWidth + textMargin;
        }

        if (font.GetFontSize() != height) {
            height = font.GetFontSize() + textMargin;
        }
    }

    /**
     *  @brief Checks whether the label already exists.
     *  @param label label which existence will be checked
     *  @returns true if label is already in the dropdown
     */
    bool IsLabelTaken(const std::string& label) const {
        return values.contains(label);
    }

    /**
     *  @brief Height of all the items combined.
     *  @details height times number of items + 1 (for the default).
     *  @returns Height when rolled out.
     */
    float GetTotalHeight() const override {
        return height * (values.size() + 1);
    }


private:
    std::map<std::string, T> values;
    std::string currentLabel;
    T currentValue;

    std::string Truncate(const std::string& text) const override {
        const float BLINKER_PAD = 2; //makes sure that the blinker won't get out
        if (MeasureText((text + "|").c_str(), font.GetFontSize()) + textMargin + BLINKER_PAD > width) {
            std::string truncated = "";
            for (const auto& c : text) {
                const std::string characterString(1,c);
                if (MeasureText(truncated.c_str(), font.GetFontSize()) + MeasureText("...|", font.GetFontSize()) + MeasureText(characterString.c_str(), font.GetFontSize()) + BLINKER_PAD > width) {
                    return truncated + "...";
                }
                truncated += c;
            }
            return truncated;
        } else return text;
    }
};

// --- Containers ---

/**
 *  @class Workspace
 *  @brief A simple container that keeps items in-bound.
 *  @details If item is outside the workspace, it gets pushed back to the nearest side inside the area.
 */
class Workspace : public Container {
public:

    Workspace() : Item("Workspace"), Container("Workspace", 500, 500) {}

    void SetPositionsOfItems() override;
};

/**
 *  @class AnchorPane
 *  @brief A container keeping its children in the same relative positions to it after moving and changing dimensions.
 */
class AnchorPane : public Container {
public:

    AnchorPane() : Item("AnchorPane"), Container("AnchorPane", 500, 500), previousX(500), previousY(500), previousWidth(200), previousHeight(200) {}

    void SetPositionsOfItems() override;

    /**
     *  @see Item::SetX
     *  @details Saves previous x
     */
    void SetX(float x) override;
    /**
     *  @see Item::SetY
     *  @details Saves previous y
     */
    void SetY(float y) override;
    /**
     *  @see Item::SetWidth
     *  @details Saves previous width
     */
    void SetWidth(float value) override;
    /**
     *  @see Item::SetHeight
     *  @details Saves previous height
     */
    void SetHeight(float value) override;

private:
    // used for recalculating positions of children when pane moves or reshapes.
    float previousX;
    float previousY;
    float previousWidth;
    float previousHeight;
};

/**
 *  @class VBox
 *  @brief A container for displaying the items in a vertical column.
 *  @details Height automatically increases with items.
 */
class VBox : public Box {
public:

    VBox() : Item("VBox"), Box("VBox", 100, 200) {}

    void SetPositionsOfItems() override;
    /**
     *  @see Item::GetTotalHeight
     *  @details (heights + padding) * number of items, roughly
     */
    float GetTotalHeight() const override;
};

/**
 *  @class HBox
 *  @brief A container for displaying the items in a horizontal line.
 *  @details Width automatically increases with items.
 */
class HBox : public Box {
public:

    HBox() : Item("HBox"), Box("HBox", 200, 100) {}

    void SetPositionsOfItems() override;
    /**
     *  @see Item::GetTotalHeight
     *  @details (widths + padding) * number of items, roughly
     */
    float GetTotalWidth() const override;
};

/**
 *  @class Spinner
 *  @brief A value picker. Use buttons to increase or decrease value.
 *  @details Can wrap around. Composed of buttons. Has border. Has font. Min and max values can be set. Bounds are inclusive.
 */
class Spinner : public Item {
public:
    mutable Button incrementButton;
    mutable Button decrementButton;

    Font font;

    Spinner()
        : Item("Spinner", 200, 100), font(height / 2), valueMargin(10), value(0), stepValue(1), hasMin(false), hasMax(false), minValue(0), maxValue(100), allowWrap(false)  {
        Initialise();
    }
    Spinner(const std::string& i)
        : Item(i, 200, 100), font(height / 2), valueMargin(10), value(0), stepValue(1), hasMin(false), hasMax(false), minValue(0), maxValue(100), allowWrap(false) {
        Initialise();
    }

    void DrawMyself(float elapsedTime) const override;
    void DrawMyself(float elapsedTime, const Camera2D& camera) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera) override;
    bool WasIClicked(const Vector2& mousePosition) const override;
    bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const override;

    void SetToScreen() override;
    void SetToWorld() override;

    virtual void SetValue(float value);
    float GetValue() const;
    void SetStep(float step);
    float GetStep() const;
    void AllowWrap();
    void StopWrapping();
    void SetWrap(const bool flag);
    bool IsWrapAllowed() const;

    /**
     *  @brief Sets max value.
     *  @param value new value to be set
     *  @details Does NOT set hasMax. maxValue lesser than minValue is undefined behaviour.
     */
    void SetMaxValue(float value);
    /**
     *  @brief Gives max value.
     *  @details Does NOT set hasMax.
     *  @returns maxValue
     */
    float GetMaxValue() const;
    /**
     *  @brief Sets min value.
     *  @param value new value to be set
     *  @details Does NOT set hasMin. minValue greater than maxValue is undefined behaviour.
     */
    void SetMinValue(float value);
    /**
     *  @brief Gives min value.
     *  @details Does NOT set hasMin.
     *  @returns minValue
     */
    float GetMinValue() const;
    /**
     *  @brief Sets a flag to enable/disable upper limit
     *  @param flag whether to enable or disable (true or false)
     */
    void SetMaxLimit(const bool& flag);
    /**
     *  @brief Enables upper bound.
     */
    void SetMax();
    bool HasMax() const;
    /**
     *  @brief Sets a flag to enable/disable lower limit
     *  @param flag whether to enable or disable (true or false)
     */
    void SetMinLimit(const bool& flag);
    /**
     *  @brief Enables lower bound.
     */
    void SetMin();
    bool HasMin() const;

    /**
     *  @brief Checks whether the current value is at max.
     *  @returns False if max limit is not enabled or if value is lower than max
     */
    bool IsAtMax() const;
    /**
     *  @brief Checks whether the current value is at min.
     *  @returns False if min limit is not enabled or if value is greater than min
     */
    bool IsAtMin() const;

    /**
     *  @brief Sets the distance between limits and the value display.
     *  @param margin the new value
     *  @throws std::invalid_argument if the margin is negative
     */
    virtual void SetValueMargin(float margin);
    virtual float GetValueMargin() const;

    /**
     *  @see Item::SetX
     *  @details Updates buttons
     */
    void SetX(float x) override;
    /**
     *  @see Item::SetY
     *  @details Updates buttons
     */
    void SetY(float y) override;
    /**
     *  @see Item::SetWidth
     *  @details Moves buttons away.
     */
    void SetWidth(float value) override;
    /**
     *  @see Item::SetHeight
     *  @details Updates buttons height - each gets half.
     */
    void SetHeight(float value) override;

    /**
     *  @brief Sets the width of the buttons.
     *  @param value new width of the buttons
     *  @throws std::invalid_argument if value is negative.
     */
    void SetButtonsWidth(float value);

    /**
     *  @see Item::GetTotalWidth
     *  @details width + buttons width
     */
    float GetTotalWidth() const override;

protected:
    float valueMargin; ///< distance between value and border

    float value; ///< current value
    float stepValue; ///< single press increase/decrease
    bool hasMin; ///< does have a lower bound
    bool hasMax; ///< does have an upper bound
    float minValue; ///< value of the lower bound
    float maxValue; ///< value of the upper bound
    bool allowWrap; ///< causes overflow and underflow of values: max + step = min, min - step = max.

    void Initialise(); ///< Sets up buttons
    void Increment(); ///< Increases value by stepValue
    void Decrement(); ///< Decreases value by stepValue

private:

};

/**
 *  @class EditableSpinner
 *  @brief Spinner in which you can enter a value.
 *  @details Composed of a Spinner and a TextField.
 */
class EditableSpinner : public Spinner {
public:
    mutable TextField editArea;

    EditableSpinner() : Spinner("EditableSpinner") {
        editArea.SetX(xAnchor);
        editArea.SetY(yAnchor);
        editArea.SetWidth(width);
        editArea.SetHeight(height);
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
        editArea.font.SetFontSize(font.GetFontSize());
    }

    void DrawMyself(float elapsedTime) const override;
    void DrawMyself(float elapsedTime, const Camera2D& camera) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera) override;
    void Defocus() override;

    void SetToScreen() override;
    void SetToWorld() override;

    /**
     *  @see Spinner::SetX
     *  @details Moves editArea.
     */
    void SetX(float x) override;
    /**
     *  @see Spinner::SetY
     *  @details Moves editArea.
     */
    void SetY(float y) override;
    /**
     *  @see Spinner::SetWidth
     *  @details Changes the width of the text field.
     */
    void SetWidth(float value) override;
    /**
     *  @see Spinner::SetHeight
     *  @details Changes the height of the text field.
     */
    void SetHeight(float value) override;
    /**
     *  @see Spinner::SetValue
     *  @details Puts the value into the text field
     */
    void SetValue(float value) override;

    /**
     *  @see Spinner::SetValueMargin
     *  @details Interface for text field.
     */
    void SetValueMargin(float margin) override;
    /**
     *  @see Spinner::SetValueMargin
     *  @details Interface for text field.
     */
    float GetValueMargin() const override;
};

/**
 *  @class PasswordField
 *  @brief TextField with masked input.
 *  @details Can change with what the input is masked. By default with an asterisk.
 */
class PasswordField : public TextField {
public:

    PasswordField() : Item("PasswordField"), TextField("PasswordField"), mask('*') {promptText = "Password"; ClearText();}

    void DrawMyself(float elapsedTime) const override;

    void SetMask(const char character);
    char GetMask() const;

private:
    char mask;
};

/**
 *  @class ProgressIndicator
 *  @brief Shows some progress.
 *  @details Has font. Dots are circling dots - only shape for indefinite. Circle is a pie filling. Ring is a cirlce with empty inside.
 */
class ProgressIndicator : public Item {
public:
    enum Shapes {DOTS, CIRCLE, RING, BAR};

    Font font;

    ProgressIndicator() : Item("ProgressIndicator"), font(height / 4), shape(ProgressIndicator::Shapes::DOTS), displayValue(true), value(-1)  {}
    ProgressIndicator(const std::string& i) : Item(i), font(height / 4), shape(ProgressIndicator::Shapes::BAR), displayValue(true), value(-1) {}

    /**
     *  @brief Sets progress value;
     *  @param value 0 - 1, -1 is indefinite progress
     *  @throws std::out_of_range if parameter is beyond the range.
     */
    void SetProgress(float value);
    /**
     *  @brief Increases the progress value;
     *  @details Accounts for overflow.
     *  @param value how much it will increase
     */
    void IncreaseProgress(float change);
    /**
     *  @brief Decreases the progress value;
     *  @details Accounts for underflow.
     *  @param value how much it will decrease
     */
    void DecreaseProgress(float change);
    /**
     *  @brief Stops showing the percentage completed.
     *  @details Sets the value to -1.
     */
    void MakeIndefinite();
    /**
     *  @brief Gets progress value.
     *  @returns A value between 0 and 1 inclusive. -1 if indefinite.
     */
    float GetProgress() const;
    /**
     *  @brief Gets progress value in percents.
     *  @returns A natural value between 0 and 100 inclusive. -1 if indefinite.
     */
    int GetProgressPercentage() const;
    /**
     * @brief Checks whether the value is 100%
     * @return true if value is 1, false otherwise.
     */
    bool IsComplete() const;
    /**
     *  @brief Sets new shape
     *  @param shape a ProgressIndicator::SHAPE enum - {DOTS, RING, CIRCLE}
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    void SetShape(const ProgressIndicator::Shapes& shape);
    /**
     *  @brief Sets new shape
     *  @param shape a string containing the name of the shape - {"DOTS", "RING", "CIRCLE"}
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    void SetShape(const std::string& shape);
    Shapes GetShape() const;
    std::string GetShapeString() const;

    /**
     *  @brief Displays value in percents.
     *  @details Does not change the state from indefinite.
     */
    void DoDisplayValue();
    /**
     *  @brief Doesn't display value in percents.
     *  @details Does not change the state from indefinite.
     */
    void DoNotDisplayValue();
    /**
     *  @brief Sets whether the indicatior should display the numerical value (percentage) of progress.
     *  @param flag true - enable, false - disable.
     */
    void SetDisplayValue(const bool& flag);
    bool IsDisplayingValue() const;

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

protected:
    Shapes shape;
    bool displayValue; ///< whether displays the value in percents. Indefinite doesn't display value.
    float value; ///< Raw value of the progress - between 0 and 1, or -1;

    Shapes StringToShape(const std::string& shape) const;
    std::string ShapeToString(const ProgressIndicator::Shapes& shape) const;
};


class ProgressBar : public ProgressIndicator {
public:
    Colour barColour;

    ProgressBar() : ProgressIndicator("ProgressBar"),
                    barColour(GREEN), barMargin(5), segmented(false), numberOfSegments(10), gapBetweenSegments(1) {shape = ProgressIndicator::Shapes::BAR;}

    void DrawMyself(float elapsedTime) const override;

    /**
     *  @brief Sets bar margin.
     *  @details In case of over- or underflow sets the value to maximum or minimum respectively.
     *  @param value new bar margin length
     *  @throws std::out_of_range if value is greater than half of the height
     *  @throws std::invalid_argument if value is lower than 0
     */
    void SetBarMargin(float value);
    float GetBarMargin() const;
    /**
     *  @brief Sets number of segments.
     *  @details In case of underflow sets the value to minimum.
     *  @param value new number of segments
     *  @throws std::invalid_argument if value is lower than 0
     */
    void SetNumberOfSegments(int value);
    int GetNumberOfSegments() const;
    /**
     *  @brief Sets length of the gap between segments.
     *  @details In case of over- or underflow sets the value to maximum or minimum respectively.
     *  @param value new gap length
     *  @throws std::out_of_range if value is greater than the available maximum gap based on width and the number of segments
     *  @throws std::invalid_argument if value is lower than 0
     */
    void SetGapBetweenSegments(float value);
    float GetGapBetweenSegments() const;
    /**
     *  @brief Sets the number of segments and the gap between them.
     *  @details Calls setters for the fields.
     *  @param number number of segments
     *  @param gap length of the gap between segments
     *  @throws std::invalid_argument if number or gap is negative
     *  @throws std::out_of_range if number or gap is too big.
     *  @see ProgressBar::SetGapBetweenSegments
     *  @see ProgressBar::SetNumberOfSegments
     */
    void SetSegments(int number, float gap);

    /**
     *  @brief Segments the bar for display.
     */
    void Segmented();
    /**
     *  @brief Turns off segmentation.
     */
    void Solid();
    /**
     *  @brief Sets whether the bar should be segmented or not.
     *  @param flag true - yes, false - no.
     */
    void SetSegemented(const bool& flag);
    bool IsSegmented() const;

private:
    float barMargin; ///< the distance between the top/side of the main bar and the progress bar.
    bool segmented;
    int numberOfSegments;
    float gapBetweenSegments;
};

class PressedButton : public Button, public virtual PersistentState {
public:

    PressedButton() : Item("PressedButton"), Button("PressedButton"), pressedText("Pressed Button") {}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;

    void SetPressedText(const std::string& value);
    void ClearPressedText();
    std::string GetPressedText() const;

private:
    std::string pressedText;
};

template<typename T>
class List : public virtual Item, public virtual Padded {
public:
    Font font;


    List() : Item("List"), Padded(), font(height / 4), vertical(true) {}

    void AddItem(const T& item) {
        items.push_back(item);
    }
    void RemoveItem(int index) {
        if (index < 0 || index >= items.size()) {
            throw std::out_of_range("In List " + this->ID + ": index beyond range at removal.");
        }
        items.erase(items.begin() + index);
    }
    void ReplaceItem(int index, const T& item) {
        if (index < 0 || index >= items.size()) {
            throw std::out_of_range("In List " + this->ID + ": index beyond range at replacing.");
        }
        items[index] = item;
    }

    const T& GetItem(int index) const {
        if (index < 0 || index >= items.size()) {
            throw std::out_of_range("In List " + this->ID + ": index beyond range at getting.");
        }
        return items[index];
    }
    std::vector<T> GetItems() const {
        return items;
    }

    void Sort() {
        std::sort(items.begin(), items.end());
    }
    void ReverseSort() {
        std::sort(items.begin(), items.end());
        std::reverse(items.begin(), items.end());

    }
    template<typename Compare>
    void SortBy(Compare sortingMethod) {
        std::sort(items.begin(), items.end(), sortingMethod);
    }

    template<typename Compare>
    void ReverseSortBy(Compare sortingMethod) {
        std::sort(items.begin(), items.end(), sortingMethod);
        std::reverse(items.begin(), items.end());
    }

    void SetDisplayMethod(const std::function<std::string(const T&)>& displayMethod) {
        this->displayMethod = displayMethod;
    }

    size_t GetSize() const {
        return items.size();
    }


    /**
     * @throws std::runtime_error if display method not set for a type that is not a string nor is able to use to_string.
     * @see Item::DrawMyself
     */
    void DrawMyself(float elapsedTime) const override {
        if (displayMethod) {
            if (vertical) {
                float nextY = 0;
                for (const auto& item : items) {
                    DrawText(displayMethod(item).c_str(), xAnchor, yAnchor + nextY, font.GetFontSize(), font.colour.GetColour());
                    nextY += font.GetFontSize() + padding;
                }
            } else {
                float nextX = 0;
                std::string text = "";
                for (const auto& item : items) {
                    text = displayMethod(item);
                    DrawText(text.c_str(), xAnchor + nextX, yAnchor, font.GetFontSize(), font.colour.GetColour());
                    nextX += MeasureText(text.c_str(), font.GetFontSize()) + padding;
                }
            }
        } else {
            if constexpr (std::is_same_v<T, std::string>) {
                if (vertical) {
                    float nextY = 0;
                    for (const auto& item : items) {
                        DrawText(item.c_str(), xAnchor, yAnchor + nextY, font.GetFontSize(), font.colour.GetColour());
                        nextY += font.GetFontSize() + padding;
                    }
                } else {
                    float nextX = 0;
                    for (const auto& item : items) {
                        DrawText(item.c_str(), xAnchor + nextX, yAnchor, font.GetFontSize(), font.colour.GetColour());
                        nextX += MeasureText(item.c_str(), font.GetFontSize()) + padding;
                    }
                }
            } else if constexpr (requires { std::to_string(T{}); }) {
                if (vertical) {
                    float nextY = 0;
                    for (const auto& item : items) {
                        DrawText(std::to_string(item).c_str(), xAnchor, yAnchor + nextY, font.GetFontSize(), font.colour.GetColour());
                        nextY += font.GetFontSize() + padding;
                    }
                } else {
                    float nextX = 0;
                    std::string text = "";
                    for (const auto& item : items) {
                        text = std::to_string(item);
                        DrawText(text.c_str(), xAnchor + nextX, yAnchor, font.GetFontSize(), font.colour.GetColour());
                        nextX += MeasureText(text.c_str(), font.GetFontSize()) + padding;
                    }
                }
            } else {
               throw std::runtime_error("List " + this->ID + ": no display method set for a non-standard type.");
            }
        }
    }
    void DoFocusAction(float elapsedTime) override {
        return;
    }

    void SetVertical() {
        vertical = true;
    }
    void SetHorizontal() {
        vertical = false;
    }
    /**
     *  @brief Sets whether the list should be vertical or horizontal.
     *  @param flag true - vertical, false - horizontal.
     */
    void SetVerticality(const bool& flag) {
        vertical = flag;
    }
    bool IsVertical() const {
        return vertical;
    }

    /**
     *  @brief Returns total width.
     *  @see Item::GetTotalWidth
     *  @returns width if vertical, sum of padding and items (in string form) length otherwise.
     *  @throws std::runtime_error if display method not set for a type that is not a string nor is able to use to_string.
     */
    float GetTotalWidth() const override {
        if (vertical || items.empty()) {
            return width;
        }
        if (displayMethod) {
            float sum = 0;
            for (const auto& item : items) {
                sum += MeasureText(displayMethod(item).c_str(), font.GetFontSize()) + padding;
            }
            return sum;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            float sum = 0;
            for (const auto& item : items) {
                sum += MeasureText(item.c_str(), font.GetFontSize()) + padding;
            }
            return sum;
        }
        if constexpr (requires { std::to_string(T{}); }) {
            float sum = 0;
            for (const auto& item : items) {
                sum += MeasureText(std::to_string(item).c_str(), font.GetFontSize()) + padding;
            }
            return sum;
        }
        throw std::runtime_error("List " + this->ID + ": no display method set for a non-standard type - couldn't calculate width");
    }
    /**
     *  @brief Returns total height.
     *  @see Item::GetTotalHeight
     *  @returns height if not vertical, sum of padding and font size times number of items otherwise.
     */
    float GetTotalHeight() const override {
        if (!vertical || items.empty()) {
            return height;
        }
        return (items.size() - 1) * (padding + font.GetFontSize()) + font.GetFontSize();
    }

private:
    std::vector<T> items;
    std::function<std::string(const T&)> displayMethod;

    bool vertical;
};

class PieChart : public Chart, virtual public Circular {
public:

    PieChart() : Item("PieChart"), Chart("PieChart"), Circular(), showLabels(false), showPercentage(false) {}

    void DoFocusAction(float elapsedTime) override;
    void DrawMyself(float elapsedTime) const override;

    void DoShowLabels();
    void DoNotShowLabels();
    void SetShowingLabels(const bool& flag);
    bool IsShowingLabels() const;

    void DoShowPercentage();
    void DoNotShowPercentage();
    void SetShowingPercentage(const bool& flag);
    bool IsShowingPercentage() const;

private:
    bool showLabels;
    bool showPercentage;
};

/**
 *  @class Line
 *  @brief Straight line.
 *  @details Has two modes of drawing: by end point or by angle and length. Doesn't use border.
 */
class Line : public Shape {
public:

    Line() : Item("Line"), Shape("Line"), pointToPoint(true), thickness(10), angle(0), xEnd(xAnchor + width), yEnd(yAnchor) {}

    /**
     *  @brief Returns length of the line.
     */
    float CalculateMyArea() const override;

    void DrawMyself(float elapsedTime) const override;

    /**
     *  @brief Sets length of the line.
     *  @details Used only when drawing by length and angle.
     *  @param length new value of length
     *  @throws std::invalid_argument if the length is negative
     *  @note Uses width internally.
     */
    void SetLength(float length);
    float GetLength() const;
    void SetWidth(float value) override; /// overrides into Length.
    void SetHeight(float value) override; /// overrides into Length.
    float GetWidth() const override; /// overrides into Length.
    float GetHeight() const override; /// overrides into Length.
    float GetTotalHeight() const override;
    float GetTotalWidth() const override;

    /**
     *  @brief Sets the angle at which the line is drawn.
     *  @details Used only when drawing by length and angle.
     *  @param angle in degrees
     *  @throws std::out_of_range if the angle is beyond the 0 - 360 range.
     */
    void SetAngle(float angle);
    /**
     *  @brief Changes angle by a value.
     *  @details Change can be negative. Handles overflow and underflow.
     *  @param change how much the angle will move.
     */
    void IncreaseAngle(float change);
    float GetAngle() const;

    void SetEndPoint(float x, float y);
    void SetEndPoint(const Vector2& coordinates);
    Vector2 GetEndPoint() const;

    /**
     *  @brief Sets the thickness of the line.
     *  @param thickness new value of thickness.
     *  @throws std::invalid_argument if the thickness is negative.
     */
    void SetThickness(float thickness);
    float GetThickness() const;

    /**
     *  @brief Sets the drawing mode to from point to point.
     *  @details Use EndPoint to draw it.
     */
    void DrawPointToPoint();
    /**
     *  @brief Sets the drawing mode to using length and angle.
     *  @details Set length and angle to draw it.
     */
    void DrawLengthAndAngle();
    /**
     *  @brief Sets drawing method.
     *  @param flag true - point to point, false - length and angle.
     */
    void SetDrawingMethod(bool flag);
    /**
     *  @brief Returns drawing method.
     *  @returns true - point to point, false - length and angle.
     */
    bool GetDrawingMethod() const;

private:
    bool pointToPoint;
    float thickness;
    float angle;
    float xEnd;
    float yEnd;

    void CalculateLength(); // sets length (width)
};

class Square : public Shape {
public:

    Square() : Item("Square"), Shape("Square") {height = width;}

    void DrawMyself(float elapsedTime) const override;

    float CalculateMyArea() const override;

    void SetWidth(float value) override; /// also sets height.
    void SetHeight(float value) override; /// overrides into width.
    float GetHeight() const override; /// overrides into width.
    float GetTotalHeight() const override;
};

class Rectangle : public Shape {
public:

    Rectangle() : Item("Rectangle"), Shape("Rectangle") {height = width;}

    void DrawMyself(float elapsedTime) const override;

    float CalculateMyArea() const override;
};

}

#endif // ITEMS_H
