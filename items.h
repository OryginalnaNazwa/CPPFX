#ifndef ITEMS_H
#define ITEMS_H

#include "raylib.h"
#include "properties.h"
#include <string>
#include <unordered_set>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <memory>
#include <cmath>
#include <functional>
#include <iostream>
#include <vector>
#include <numeric>

namespace CPPFX {

/**
 *  @class Item
 *  @brief Virtual widget class.
 */
class Item {
public:
    Colour colour;

    Item(const std::string& i)
        : ID(""), xAnchor(0), yAnchor(0), height(100), width(200), focused(false),visible(true), inactive(false), eatsClick(true), priority(3), screenBased(false), fxID(i) {}
    Item(const std::string& i, const float& w, const float& h)
        : ID(""), xAnchor(0), yAnchor(0), height(h), width(w), focused(false),visible(true), inactive(false), eatsClick(true), priority(3), screenBased(false), fxID(i) {}


    virtual void DrawMyself(const float& dt) const;
    virtual void DrawMyself(const float& dt, const Camera2D& camera) const;
    virtual bool WasIClicked(const Vector2& mousePosition) const; //screen
    virtual bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const; //world
    /**
     *  @brief Action done every frame.
     *  @param dt frame time
     */
    virtual void DoPassiveAction(const float& dt);
    virtual void DoPassiveAction(const float& dt, const Camera2D& camera);
    /**
     *  @brief Action done while the item is focused.
     *  @param dt Frame time
     */
    virtual void DoFocusAction(const float& dt) = 0;
    virtual void DoFocusAction(const float& dt, const Camera2D& camera);
    /**
     *  @brief Action done while the item is focused and it depends on mouse click.
     *  @details Defaults to the previous DoActiveAction().
     *  @param dt Frame time
     *  @param mousePosition vector2 of mouse's x and y world coordinates during the recent click.
     */
    virtual void DoFocusAction(const float& dt, const Vector2& mousePosition);
    virtual void DoFocusAction(const float& dt, const Vector2& mousePosition, const Camera2D& camera);
    /**
     *  @brief Sets focused to false;
     */
    virtual void Defocus();
    virtual void Focus();
    bool IsFocused() const;

    void MakeInactive();
    void MakeActive();
    void SetInactive(const bool& flag);
    bool IsInactive() const;

    virtual void SetToWorld();
    virtual void SetToScreen();
    bool IsScreenBased() const;

    void MakeInvisible();
    void MakeVisible();
    void SetVisible(const bool& flag);
    bool IsVisible() const;

    /**
     *  @brief Sets both inactive and invisible to true.
     */
    void Hide();
    void Show();

    void ConsumeClicks();
    void LetClicksThrough();
    bool DoesEatClicks() const;

    /**
     *  @brief Launches on click.
     *  @details Lambda, put in your own.
     */
    std::function<void()> onClick;
    std::function<void()> onHover;

    /**
     *  @brief Sets the x coordinate of the item.
     *  @param x new value to be set
     */
    virtual void SetX(const float& x);
    /**
     *  @brief Sets the y coordinate of the item.
     *  @param y new value to be set
     */
    virtual void SetY(const float& y);
    /**
     *  @brief Sets the height of the item.
     *  @param value The new height.
     *  @throws std::range_error If the value is negative.
     */
    virtual void SetHeight(const float& value);
    /**
     *  @brief Sets the width of the item.
     *  @param value The new width.
     *  @throws std::range_error If the value is negative.
     */
    virtual void SetWidth(const float& value);
    void SetID(const std::string& id);
    /**
     *  @brief Sets the priority in the order of drawing of the item.
     *  @details Sets 0 on negative value.
     *  @param value The new priority.
     *  @throws std::range_error If the value is negative.
     */
    void SetPriority(const int& value);
    /**
     *  @brief Increases order by one, decreasing the priority by one.
     *  @details If priority is 0, does nothing.
     */
    void MoveUpPriority();
    /**
     *  @brief Decreases order by one, increasing the priority by one.
     */
    void MoveDownPriority();

    /**
     *  @brief Returns x coordinate of the item.
     *  @returns x coordinate
     */
    float GetX() const;
    /**
     *  @brief Returns y coordinate of the item.
     *  @returns y coordinate
     */
    float GetY() const;
    /**
     *  @brief Returns height of the item.
     *  @returns height of the item
     */
    virtual float GetHeight() const;
    /**
     *  @brief Returns width of the item.
     *  @returns width of the item
     */
    virtual float GetWidth() const;
    /**
     *  @brief Returns priority of the item.
     *  @details Remember that larger number is lesser priority.
     *  @returns number representing priority
     */
    size_t GetPriority() const;
    std::string GetID() const;

    /**
     *  @brief Returns the internal library identificator of the item.
     *  @details Roughly the name of the class.
     *  @returns Internal ID
     */
    std::string GetFxID() const;

    float currentFrameTime;

protected:
    std::string ID; ///<ID used by the user, variable.
    float xAnchor; ///<top left x coordinate
    float yAnchor; ///<top left y coordinate
    float height;
    float width;

    bool focused;
    bool visible; ///<invisible items are still interactable
    bool inactive; ///<inactive items are not interactable
    bool eatsClick; ///<if false, allows the click to continue under it
    size_t priority; ///<order of drawing. Set to 3 by default for convenience of moving priority around. The higher the priority, the quicker it gets done - but gets drawn under.

    bool screenBased;
    const std::string fxID; ///ID for the library.
    float timer = 0;

private:
};

/**
 *  @class TextItem
 *  @brief A virtual Item subclass focused on a variable text.
 */
class TextItem : public Item {
public:
    Font font;

    TextItem(const std::string& i) : Item(i), font(height / 4), text(i), textMargin(10) {}
    TextItem(const std::string& i, const float& w, const float& h) : Item(i, w, h), font(h / 4), text(i), textMargin(10) {}

    /**
     *  @brief Expands the width and height to accommodate text.
     */
    virtual void ExpandToText();

    /**
     *  @brief Expands or shrinks width and height to the text dimensions.
     */
    virtual void FitToText();

    /**
     *  @brief Sets text
     *  @param text new text to be set
     */
    void SetText(const std::string& text);
    /**
     *  @brief Clears text.
     *  @details Sets text to an empty std::string.
     */
    void ClearText();
    /**
     *  @brief Returns text.
     *  @returns text
     */
    std::string GetText() const;

    /**
     *  @brief Sets the distance between border and text in x axis.
     *  @param margin new value
     *  @details Sets 0 at throw.
     *  @throws std::invalid_argument if margin is negative.
     */
    void SetTextMargin(const float& margin);
    float GetTextMargin() const;

protected:
    std::string text;
    float textMargin;

    /**
     *  @brief Truncates text if needed.
     *  @return truncated string
     */
    virtual std::string Truncate(const std::string& text) const;
};

/**
 *  @class Label
 *  @brief A simple text displayer.
 *  @details Displays the text. Position is based on alignment. Has border.
 */
class Label : public TextItem {
public:
    Border border;
    Alignment alignment;

    Label() : TextItem("Label") {text = "Label";}

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

};

/**
 *  @class TextField
 *  @brief Allows user to enter text into the field.
 *  @details Displays the text and allows input when focused. Has border.
 */
class TextField : public TextItem {
public:
    Border border;

    TextField() : TextItem("TextField"), promptText("") {}
    TextField(const std::string& i) : TextItem(i), promptText("") {}

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

    void SetPromptText(const std::string& text);
    void ClearPromptText();
    std::string GetPromptText() const;

protected:
    std::string promptText; /// string displayed when the text is empty
};

/**
 * @class Button
 * @brief A single click pushable item.
 * @details Unpushes after a click. Has border. Has colour for clicked and unclicked state.
 */
class Button : public TextItem {
public:
    Border border;
    Colour pressedColour;
    Colour unPressedColour;

    Button() : TextItem("Button"), pressedColour(GRAY), unPressedColour(LIGHTGRAY) {}
    Button(const std::string& id) : TextItem(id), pressedColour(GREY), unPressedColour(LIGHTGRAY) {}

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

};

/**
 * @class CheckBox
 * @brief A turn on - turn off button.
 * @details Width is the length of the box, not the total one. Total length is width + MeasureText(text) + textMargin + labelMargin. Has border. Has colour for pressed state and unpressed.
 */
class CheckBox : public TextItem {
public:
    Border border; ///<automatically stretches to accommodate text.

    Colour pressedColour;
    Colour unPressedColour;


    bool drawsX = true; //TODO add different shapes

    CheckBox() : TextItem("CheckBox", 50, 50), pressedColour(GRAY), unPressedColour(LIGHTGRAY), pressed(false), labelMargin(10) {font.fontSize = height / 2;}

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

    float GetTotalWidth() const;

    /**
     *  @brief Sets the distance between the text and the box.
     *  @details Sets the value to 0 on error.
     *  @param value new value to be set to
     *  @throws std::invalid_argument if the new value is negative.
     */
    void SetLabelMargin(const float& value);
    float GetLabelMargin() const;

    void SetPressed(const bool& flag);
    void Press();
    void Unpress();
    void SwitchPress();
    bool IsPressed() const;

private:
    bool pressed;
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
    Border border;

    DropDown() : TextItem("DropDown"), currentLabel("") {}

    void DrawMyself(const float& dt) const override {
        DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
        if (currentLabel != "") {
            DrawText(Truncate(currentLabel).c_str(), xAnchor + textMargin, yAnchor + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
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
                DrawText(Truncate(label).c_str(), xAnchor + textMargin, yCurrent + (height / 2) - (font.fontSize / 2), font.fontSize, font.colour.GetColour());
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
        if (focused) {
            if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width && mousePosition.y >= yAnchor && mousePosition.y <= yAnchor + ((values.size() + 1) * height)) {
                return true;
            }
            return false;
        } else return Item::WasIClicked(mousePosition, camera);
    }

    void DoFocusAction(const float& dt) {
        return;
    }

    void DoPassiveAction(const float& dt) override {
        if (currentLabel == "" && !values.empty()) {
            SetCurrent(values.begin()->first);
        }
    }

    void DoFocusAction(const float& dt, const Vector2& mousePosition) override {
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

    void DoFocusAction(const float& dt, const Vector2& mousePosition, const Camera2D& camera) override {
        if (mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width && mousePosition.y <= (yAnchor + ((values.size() + 1) * height)) && mousePosition.y >= yAnchor + height) {
            int index = (mousePosition.y - (yAnchor + height)) / height;
            auto it = std::next(values.begin(), index);
            currentLabel = it->first;
            currentValue = it->second;
            focused = false;
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
     */
    void InsertItem(const std::string& label, const T& value) {
        values.insert({label,value});
    }

    /**
     *  @brief Removes a value from the dropdown.
     *  @param label label for removal
     *  @throws std::invalid_argument if there's no label in the dropdown.
     */
    void RemoveItem(const std::string& label) {
        if (IsLabelTaken(label)) {
            values.erase(label);
        } else throw std::invalid_argument("In " + this->ID + ": No value with label " + label + " found.");
    }

    void ChangeLabel(const std::string& oldLabel, const std::string& newLabel) {
        auto node = values.extract(oldLabel);
        node.key() = newLabel;
        values.insert(std::move(node));
    }

    /**
     *  @brief Changes a value on the specified key
     *  @param label the key
     *  @param newValue value that'll replace the current one
     *  @throws std::out_of_range
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
     *  @throws std::out_of_range
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
     *  @brief Sets current label and value.
     *  @param label name of the label to set value to
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
        int textWidth = MeasureText(currentLabel.c_str(), font.fontSize);
        if (textWidth > width) {
            width = textWidth + textMargin;
        }

        if (font.fontSize > height) {
            height += font.fontSize - height;
        }
    }

    /**
     *  @see TextItem::FitToText.
     *  @details Works based on currentLabel.
     */
    void FitToText() {
        int textWidth = MeasureText(currentLabel.c_str(), font.fontSize);
        if (textWidth != width) {
            width = textWidth + textMargin;
        }

        if (font.fontSize != height) {
            height = font.fontSize + textMargin;
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


private:
    std::map<std::string, T> values;
    std::string currentLabel;
    T currentValue;

    std::string Truncate(const std::string& text) const override {
        if (MeasureText(text.c_str(), font.fontSize) + MeasureText("|", font.fontSize) + textMargin + 2 > width) {//makes sure that the blinker won't get out
            std::string truncated = "";
            for (const auto& c : text) {
                const std::string characterString(1,c);
                if (MeasureText(truncated.c_str(), font.fontSize) + MeasureText("...|", font.fontSize) + MeasureText(characterString.c_str(), font.fontSize) + 2 > width) {
                    return truncated + "...";
                }
                truncated += c;
            }
            return truncated;
        } else return text;
    }
};

/**
 * @class Container
 * @brief A virtual layout organiser base.
 * @details Has border. Has alignement. Has colour - blank by default. Containers have default priority of 10 (makes sure they're under most items so they don't interfere).
 * @note Containers mostly just set X and Ys of their children.
 */
class Container : public Item {
public:
    Alignment alignment;
    Border border;

    Container(const std::string& i) : Item(i) {colour.SetColour(BLANK); priority = 10;}
    Container(const std::string& i, const float& w, const float& h) : Item(i, w, h) {colour.SetColour(BLANK); priority = 10;}

    void AddItem(Item* item);

    /**
     *  @brief Removes an item from the container.
     *  @param ID ID of the item to be removed
     *  @throws std::invalid_argument if the ID is not in the container
     */
    void RemoveItem(const std::string& ID);

    /**
     *  @brief Removes an item from the container without throwing an error.
     *  @param ID ID of the item to be removed
     */
    void SafeRemoveItem(const std::string& ID);

    virtual void SetPositionsOfItems() = 0;

    void DoPassiveAction(const float& dt) override;

    /**
     *  @brief Sorts the drawing order by priority.
     *  @note Remember to call after manual priority change.
     */
    void SortOrder();

    bool IsIDTaken(const std::string& ID) const;

protected:
    std::map<std::string, Item*> Items;
    std::vector<Item*> ItemsInDrawingOrder;

    bool needsSorting = false;
    bool needsOrdering = false;
};

class Workspace : public Container {
public:

    Workspace() : Container("Workspace", 500, 500) {}

    void SetPositionsOfItems() override;

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;
};

class AnchorPane : public Container {
public:

    AnchorPane() : Container("AnchorPane", 500, 500), previousX(500), previousY(500), previousWidth(200), previousHeight(200) {}

    void SetPositionsOfItems() override;

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

    void SetX(const float& x) override;
    void SetY(const float& y) override;
    void SetWidth(const float& value) override;
    void SetHeight(const float& value) override;

private:
    float previousX;
    float previousY;
    float previousWidth;
    float previousHeight;
};

class Box : public Container {
public:
    float padding = 10;

    Box(const std::string& i) : Container(i) {}
    Box(const std::string& i, const float& w, const float& h) : Container(i, w, h) {}

    void SetPadding(const float& value);

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

};

class VBox : public Box {
public:

    VBox() : Box("VBox", 100, 200) {}

    void SetPositionsOfItems() override;
};

class HBox : public Box {
public:

    HBox() : Box("HBox", 200, 100) {}

    void SetPositionsOfItems() override;
};

class Spinner : public Item {
public:
    Button incrementButton;
    Button decrementButton;
    Border border;
    Font font;

    Spinner()
        : Item("Spinner", 200, 100), font(height / 2), valueMargin(10), value(0), stepValue(1), hasMin(false), hasMax(false), minValue(0), maxValue(100), allowWrap(false)  {
        Initialise();
    }
    Spinner(const std::string& i)
        : Item(i, 200, 100), font(height / 2), valueMargin(10), value(0), stepValue(1), hasMin(false), hasMax(false), minValue(0), maxValue(100), allowWrap(false) {
        Initialise();
    }

    void DrawMyself(const float& dt) const override;
    void DrawMyself(const float& dt, const Camera2D& camera) const override;
    void DoFocusAction(const float& dt) override;
    void DoFocusAction(const float& dt, const Vector2& mousePosition) override;
    void DoFocusAction(const float& dt, const Vector2& mousePosition, const Camera2D& camera) override;
    bool WasIClicked(const Vector2& mousePosition) const override;
    bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const override;

    void SetToScreen() override;
    void SetToWorld() override;

    virtual void SetValue(const float& value);
    float GetValue() const;
    void SetStep(const float& step);
    float GetStep() const;

    /**
     *  @brief Sets max value.
     *  @param value new value to be set
     *  @details Does NOT set hasMax. maxValue lesser than minValue is undefined behaviour.
     */
    void SetMaxValue(const float& value);
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
    void SetMinValue(const float& value);
    /**
     *  @brief Gives max value.
     *  @details Does NOT set hasMax.
     *  @returns minValue
     */
    float GetMinValue() const;
    /**
     *  @brief Sets a flag to enable/disable upper limit
     *  @param flag whether to enable or disable (true or false)
     */
    void SetMaxLimit(const bool& flag);
    bool HasMax() const;
    /**
     *  @brief Sets a flag to enable/disable lower limit
     *  @param flag whether to enable or disable (true or false)
     */
    void SetMinLimit(const bool& flag);
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

    void SetX(const float& x) override;
    void SetY(const float& y) override;
    void SetWidth(const float& value) override;
    void SetHeight(const float& value) override;
    void SetButtonsWidth(const float& value);

protected:
    float valueMargin; ///<distance between value and border

    float value; ///<current value
    float stepValue; ///<single press increase/decrease
    bool hasMin;
    bool hasMax;
    float minValue;
    float maxValue;
    bool allowWrap; ///< causes overflow and underflow of values: max + step = min, min - step = max.

    void Initialise();
    void Increment();
    void Decrement();

private:

};

class EditableSpinner : public Spinner {
public:
    TextField editArea;

    EditableSpinner() : Spinner("EditableSpinner") {
        editArea.SetX(xAnchor);
        editArea.SetY(yAnchor);
        editArea.SetWidth(width);
        editArea.SetHeight(height);
        editArea.SetText(std::to_string(value));
        editArea.font.fontSize = font.fontSize;
    }

    void DrawMyself(const float& dt) const override;
    void DrawMyself(const float& dt, const Camera2D& camera) const override;
    void DoFocusAction(const float& dt) override;
    void DoFocusAction(const float& dt, const Vector2& mousePosition) override;
    void DoFocusAction(const float& dt, const Vector2& mousePosition, const Camera2D& camera) override;
    void Defocus() override;

    void SetToScreen() override;
    void SetToWorld() override;

    void SetX(const float& x) override;
    void SetY(const float& y) override;
    void SetWidth(const float& value) override;
    void SetHeight(const float& value) override;
    void SetValue(const float& value) override;
};

class PasswordField : public TextField {
public:

    PasswordField() : TextField("PasswordField") {promptText = "Password"; ClearText();}

    void DrawMyself(const float& dt) const override;
};

class ProgressIndicator : public Item {
public:
    enum Shapes {DOTS, CIRCLE, RING};

    Shapes shape;
    Font font;

    ProgressIndicator() : Item("ProgressIndicator"), shape(ProgressIndicator::Shapes::DOTS), font(height / 4), displayValue(true), value(-1)  {}
    ProgressIndicator(const std::string& i) : Item(i), font(height / 4), displayValue(true), value(-1) {}

    /**
     *  @brief Sets progress value;
     *  @param value 0 - 1, -1 is indefinite progress
     *  @throws std::out_of_range if parameter is beyond the range.
     */
    void SetProgress(const float& value);
    /**
     *  @brief Increases the progress value;
     *  @details Accounts for overflow.
     *  @param value how much it will increase
     */
    void IncreaseProgress(const float& change);
    /**
     *  @brief Decreases the progress value;
     *  @details Accounts for underflow.
     *  @param value how much it will decrease
     */
    void DecreaseProgress(const float& change);
    /**
     *  @brief Stops showing the percentage completed.
     *  @details Sets the value to -1.
     */
    void MakeIndefinite();
    float GetProgress() const;
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

    void DoDisplayValue();
    void DoNotDisplayValue();
    /**
     *  @brief Sets whether the indicatior should display the numerical value (percentage) of progress.
     *  @param flag true - enable, false - disable.
     */
    void SetDisplayValue(const bool& flag);
    bool IsDisplayingValue() const;

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;

protected:
    bool displayValue;
    float value; ///< Raw value of the progress - between 0 and 1, or -1;

    Shapes StringToShape(const std::string& shape) const;
    std::string ShapeToString(const ProgressIndicator::Shapes& shape) const;
};

class ProgressBar : public ProgressIndicator {
public:
    Colour barColour;

    ProgressBar() : ProgressIndicator("ProgressBar"), barColour(GREEN), barMargin(5), segmented(false), numberOfSegments(10), gapBetweenSegments(1) {}

    void DrawMyself(const float& dt) const override;

    /**
     *  @brief Sets bar margin.
     *  @details In case of over- or underflow sets the value to maximum or minimum respectively.
     *  @param value new bar margin length
     *  @throws std::out_of_range if value is lower than 0 or greater than half of the height
     */
    void SetBarMargin(const float& value);
    float GetBarMargin() const;
    /**
     *  @brief Sets number of segments.
     *  @details In case of underflow sets the value to minimum.
     *  @param value new number of segments
     *  @throws std::invalid_argument if value is lower than 0
     */
    void SetNumberOfSegments(const int& value);
    int GetNumberOfSegments() const;
    /**
     *  @brief Sets length of the gap between segments.
     *  @details In case of over- or underflow sets the value to maximum or minimum respectively.
     *  @param value new gap length
     *  @throws std::out_of_range if value is lower than 0 or greater than the available maximum gap based on width and the number of segments
     */
    void SetGapBetweenSegments(const float& value);
    float GetGapBetweenSegments() const;
    /**
     *  @brief Sets the number of segments and the gap between them.
     *  @details Calls setters for the fields.
     *  @param number number of segments
     *  @param gap length of the gap between segments
     *  @throws The same errors as the setter methods to the fields.
     *  @see SetGapBetweenSegments
     *  @see SetNumberOfSegments
     */
    void SetSegments(const int& number, const float& gap);

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

class PressedButton : public Button {
public:

    PressedButton() : Button("PressedButton"), pressed(false), pressedText("Pressed Button") {}

    void DrawMyself(const float& dt) const override;
    void DoFocusAction(const float& dt) override;
    void DoFocusAction(const float& dt, const Vector2& mousePosition) override;

    void SetPressedText(const std::string& value);
    void ClearPressedText();
    std::string GetPressedText() const;

    void SetPressed(const bool& flag);
    void Press();
    void Unpress();
    void SwitchPress();
    bool IsPressed() const;

private:
    bool pressed;
    std::string pressedText;
};

template<typename T>
class List : public Item {
public:
    Font font;
    Border border;

    List() : Item("List"), font(height / 4), vertical(true), padding(10) {}

    void AddItem(const T& item) {
        items.push_back(item);
    }
    void RemoveItem(const int& index) {
        if (index < 0 || index >= items.size()) {
            throw std::out_of_range("In List " + this->ID + ": index beyond range at removal.");
        }
        items.erase(items.begin() + index);
    }
    void ReplaceItem(const int& index, const T& item) {
        if (index < 0 || index >= items.size()) {
            throw std::out_of_range("In List " + this->ID + ": index beyond range at replacing.");
        }
        items[index] = item;
    }

    const T& GetItem(const int& index) const {
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

    void DrawMyself(const float& dt) const override {
        if (displayMethod) {
            if (vertical) {
                float nextY = 0;
                for (const auto& item : items) {
                    DrawText(displayMethod(item).c_str(), xAnchor, yAnchor + nextY, font.fontSize, font.colour.GetColour());
                    nextY += font.fontSize + padding;
                }
            } else {
                float nextX = 0;
                std::string text = "";
                for (const auto& item : items) {
                    text = displayMethod(item);
                    DrawText(text.c_str(), xAnchor + nextX, yAnchor, font.fontSize, font.colour.GetColour());
                    nextX += MeasureText(text.c_str(), font.fontSize) + padding;
                }
            }
        } else {
            if constexpr (std::is_same_v<T, std::string>) {
                if (vertical) {
                    float nextY = 0;
                    for (const auto& item : items) {
                        DrawText(item.c_str(), xAnchor, yAnchor + nextY, font.fontSize, font.colour.GetColour());
                        nextY += font.fontSize + padding;
                    }
                } else {
                    float nextX = 0;
                    for (const auto& item : items) {
                        DrawText(item.c_str(), xAnchor + nextX, yAnchor, font.fontSize, font.colour.GetColour());
                        nextX += MeasureText(item.c_str(), font.fontSize) + padding;
                    }
                }
            } else if constexpr (requires { std::to_string(T{}); }) {
                if (vertical) {
                    float nextY = 0;
                    for (const auto& item : items) {
                        DrawText(std::to_string(item).c_str(), xAnchor, yAnchor + nextY, font.fontSize, font.colour.GetColour());
                        nextY += font.fontSize + padding;
                    }
                } else {
                    float nextX = 0;
                    std::string text = "";
                    for (const auto& item : items) {
                        text = std::to_string(item);
                        DrawText(text.c_str(), xAnchor + nextX, yAnchor, font.fontSize, font.colour.GetColour());
                        nextX += MeasureText(text.c_str(), font.fontSize) + padding;
                    }
                }
            } else {
                static_assert(!sizeof(T), "List: no display method set for a non-standard type.");
            }
        }
    }
    void DoFocusAction(const float& dt) override {
        return;
    }

    void SetVertical() {
        vertical = true;
    }
    void SetHorizontal() {
        vertical = false;
    }
    bool IsVertical() const {
        return vertical;
    }

    void SetPadding(const float& padding) {
        if (padding < 0) {
            throw std::invalid_argument("In List " + this->ID + ": negative padding");
        }
        this->padding = padding;
    }
    float GetPadding() const {
        return padding;
    }

private:
    std::vector<T> items;
    std::function<std::string(const T&)> displayMethod;

    bool vertical;
    float padding;
};

class Chart : public Item {
public:
    Font font;
    Border border;

    Chart (const std::string& i) : Item(i), font(height / 4) {}

    void AddElement(const std::string& label, const double& value);
    void AddElement(const double& value);

    double GetElement(const std::string& label) const;
    double GetElement(const int& index) const;

    void SetElement(const std::string& label, const double& newValue);
    void SetElement(const int& index, const double& newValue);

    void RemoveElement(const std::string& label);
    void RemoveElement(const int& index);

    size_t GetLabelsSize() const;
    size_t GetValuesSize() const;

protected:
    std::vector<std::string> labels;
    std::vector<double> values;
};

class PieChart : public Chart {
public:

    PieChart() : Chart("PieChart"), showLabels(false), showPercentage(false) {
        border.SetDrawingMethod([this](const float& x, const float& y, const float& w, const float& h){
            DrawCircle(x, y, w + border.GetThickness(), border.colour.GetColour());
        });
    }

    void DoFocusAction(const float& dt) override;
    void DrawMyself(const float& dt) const override;

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

}

#endif // ITEMS_H
