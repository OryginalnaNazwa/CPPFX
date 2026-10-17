#ifndef ITEMS_H
#define ITEMS_H

#include <cstddef>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <concepts>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <raylib.h>

#include "base_item.hpp"
#include "bases.hpp"
#include "mixins.hpp"
#include "properties.hpp"

/**
 *  @file
 *  @brief Contains the widgets.
 */

namespace CPPFX {

// --- Text Items ---

/**
 *  @class Label
 *  @brief A simple text displayer.
 *  @details Displays the text. Position is based on alignment. Default alignment is CENTRE. Has border.
 */
class Label : public TextItem {
public:

    Alignment alignment;

    Label() : Item("Label"), TextItem("Label"), alignment(Alignment::CENTRE) {colour.SetColour(BLANK); text = "Label";}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

};

/**
 *  @class TextField
 *  @brief Allows user to enter text into the field.
 *  @details Displays the text and allows input when focused. Has border. Has different font object for prompt.
 */
class TextField : public TextItem {
public:
    using TextItem::Truncate;
    using TextItem::DrawAlignedText;

    Font promptFont; ///< font of the prompt text, mostly here for its colour

    TextField() : Item("TextField"), TextItem("TextField"), promptFont(height / 4.0f, GRAY), promptText("Text goes here...") {ClearText();}
    explicit TextField(const std::string& i) : Item("TextField"), TextItem(i), promptFont(height / 4.0f, GRAY), promptText("Text goes here...") {ClearText();}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    /**
     *  @brief Sets the text shown while the field is empty and unfocused.
     *  @param text new prompt text
     */
    void SetPromptText(const std::string& text);
    void ClearPromptText();
    std::string GetPromptText() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

    /**
     *  @see TextItem::Truncate
     *  @details Leaves room for the caret.
     */
    virtual std::string Truncate(const std::string& text, const CPPFX::Font& font, float maxWidth) const override;

protected:
    std::string promptText; ///< string displayed when the text is empty
};

/**
 *  @class EmbeddedTextField
 *  @brief TextField used as a part of composite widgets.
 *  @see Embedded
 */
class EmbeddedTextField : public TextField, public virtual Embedded {
public:

    EmbeddedTextField() : Item("EmbeddedTextField"), Embedded(), TextField() {}
};

/**
 * @class Button
 * @brief A single click pushable item.
 * @details Shows pressedColour for a moment after a click. Has border.
 */
class Button : public TextItem {
public:
    Colour pressedColour; ///< colour shown right after a click

    Button() : Item("Button"), TextItem("Button"), pressedColour(GRAY) {}
    explicit Button(const std::string& id) : Item("Button"), TextItem(id), pressedColour(GRAY) {}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class EmbeddedButton
 *  @brief Button used as a part of composite widgets.
 *  @see Embedded
 */
class EmbeddedButton : public Button, public virtual Embedded {
public:

    EmbeddedButton() : Item("EmbeddedButton"), Embedded(), Button() {}
};

/**
 *  @class CheckBox
 *  @brief A turn on - turn off button.
 *  @details Width and height are the box's. Alignment sets which side of the box the label is on.
 *  With the label left or right, total width is width + label width + labelMargin + 2 * textMargin, plus borders.
 *  With the label above or below, total height is height + label height + labelMargin + 2 * textMargin, plus borders.
 *  border goes around the box and the label, clickBorder around the box only.
 *  The mark is drawn in colour, and pressedColour fills the box while pressed.
 *  RADIO is the exception: colour fills the circle and pressedColour is the dot.
 *  @note Probably works best for equal width and height, but X and TICK should work correctly nonetheless at least.
 */
class CheckBox : public TextItem, public virtual PersistentState {
public:
    Border clickBorder; ///< border solely around clickable area.
    enum Shapes {
        X,
        TICK,
        O,
        RADIO,
        // aliases
        CROSS = X,
        CHECK = TICK,
        CIRCLE = O,
        MARU = O,
        GONGPYO = O
    }; ///< mark shown while pressed
    Alignment alignment; ///< which side of the box the label is on

    CheckBox() : Item("CheckBox", 50.0f, 50.0f), PersistentState(), TextItem("CheckBox", 50.0f, 50.0f), alignment(Alignment::RIGHT), labelMargin(10.0f), shape(CheckBox::X) {
                    font.SetFontSize(height / 2.0f);
                    DoNotTruncate(); // perhaps a max width to which might truncate label?
                }

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    /**
     *  @brief Gets total width.
     *  @details Counts the label.
     *  @see Item::GetTotalWidth
     */
    float GetTotalWidth() const override;
    /**
     *  @brief Gets total height.
     *  @details Counts the label.
     *  @see Item::GetTotalHeight
     */
    float GetTotalHeight() const override;

    /**
     *  @brief Sets the distance between the text and the box.
     *  @param value new value to be set to
     *  @throws std::invalid_argument if the new value is negative.
     */
    void SetLabelMargin(float value);
    float GetLabelMargin() const;

    /**
     *  @brief Sets new shape
     *  @param shape a shape from the Shapes enum in CheckBox
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    virtual void SetShape(const CheckBox::Shapes& shape);
    /**
     *  @brief Sets new shape
     *  @param shape a string containing the name of the shape
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    virtual void SetShape(const std::string& shape);
    Shapes GetShape() const;
    std::string GetShapeString() const;


    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    float labelMargin; ///< distance between the box and the label
    Shapes shape;

    Shapes StringToShape(const std::string& shape) const;
    std::string ShapeToString(const CheckBox::Shapes& shape) const;
};

/**
 *  @class RadioButton
 *  @brief Checkbox with default RADIO shape.
 *  @details Small convenience wrapper. For mutual exclusiveness, see RadioGroup.
 *  @see CheckBox
 */
class RadioButton : public CheckBox  {
public:

    RadioButton() : Item("CheckBox", 50.0f, 50.0f) {shape = RADIO;}
};

/**
 *  @class RadioGroup
 *  @brief Mutually exclusive group of RadioButtons
 */
class RadioGroup : public virtual Item, public virtual Padded {
public:

    RadioGroup() : Item("RadioGroup"), Padded(), vertical(true), buttonMargin(5.0f), groups(1), needsOrdering(false) {colour.SetColour(BLANK);}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;
    void DoPassiveAction(float elapsedTime) override;

    /**
     *  @brief Adds a button to the group.
     *  @details Presses it if it's the first one.
     *  @param label new button's label
     *  @throws std::invalid_argument if label is empty or if label already exists
     */
    void AddButton(const std::string& label);
    /**
     *  @brief Removes button with specified label.
     *  @param label label of the button to be deleted
     *  @throws std::runtime_error if there are no buttons
     *  @throws std::out_of_range if the button wasn't found
     */
    void RemoveButton(const std::string& label);

    /**
     *  @brief Presses the given button
     *  @details Unpresses other buttons
     *  @param label label of the button to be pressed
     *  @throws std::runtime_error if there are no buttons
     *  @throws std::out_of_range if the button wasn't found
     */
    void PressButton(const std::string& label);
    /**
     *  @brief Checks whether the given button is pressed.
     *  @param label label of the button to be checked
     *  @returns State of the given button
     *  @throws std::runtime_error if there are no buttons
     *  @throws std::out_of_range if the button wasn't found
     */
    bool IsButtonPressed(const std::string& label) const;
    /**
     *  @brief Checks which button is currently pressed
     *  @returns Label of the currently pressed button.
     *  @throws std::runtime_error if none was found
     */
    std::string WhichButtonIsCurrentlyPressed() const;

    /**
     *  @brief Sets primary colour of the buttons.
     *  @details Only affects buttons already in the group.
     *  @param colour new colour
     *  @see CheckBox
     */
    void SetButtonsBackgroundColour(const Color& colour);
    /**
     *  @brief Sets pressed colour of the buttons.
     *  @details Only affects buttons already in the group.
     *  @param colour new colour
     *  @see CheckBox::pressedColour
     */
    void SetButtonsPressedColour(const Color& colour);
    /**
     *  @brief Sets both colours of the buttons.
     *  @details Convenience wrapper, just calls the two previous methods.
     *  @see SetButtonsBackgroundColour
     *  @see SetButtonsPressedColour
     */
    void SetButtonColours(const Color& backgroundColour, const Color& pressedColour);
    /**
     *  @brief Sets label margins of the buttons.
     *  @param margin new value
     *  @throws std::invalid_argument if margin is negative
     *  @throws std::runtime_error if there are no buttons
     *  @see CheckBox::labelMargin
     */
    void SetLabelMargins(float margin);
    /**
     *  @brief Sets text margins of the buttons.
     *  @param margin new value
     *  @throws std::invalid_argument if margin is negative
     *  @throws std::runtime_error if there are no buttons
     *  @see TextItem::textMargin
     */
    void SetTextMargins(float margin);
    /**
     *  @brief Sets buttons' alignment.
     *  @details Only affects buttons already in the group.
     *  @param alignment new alignment.
     *  @see CheckBox::alignment
     */
    void SetButtonAlignment(Alignment::Alignments alignment);

    void SetToVertical();
    void SetToHorizontal();
    void SetVerticality(bool flag);
    bool IsVertical() const;

    /**
     *  @brief Checks whether a button with such label already exists.
     *  @param label label to be checked
     *  @returns true if it exists
     */
    bool IsLabelTaken(const std::string& label) const;

    /**
     *  @brief Arranges the buttons.
     *  @details Gets called automatically on the first frame after getting dirty.
     */
    void SetButtonsPositions();
    /**
     *  @brief Expands width and height to fit the buttons.
     *  @details Called automatically in SetButtonsPositions.
     *  @throws std::runtime_error if there are no buttons.
     *  @see SetButtonsPositions
     */
    void ExpandToButtons();
    /**
     *  @brief Expands or shrinks width and height to fit the buttons.
     *  @throws std::runtime_error if there are no buttons.
     */
    void FitToButtons();

    /**
     *  @brief Sets the distance between button and the limits of the widget.
     *  @param distance the new value
     *  @throws std::invalid_argument if distance is negative
     */
    void SetButtonMargin(float distance);
    float GetButtonMargin() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

    void SetX(float x) override; ///< flags for ordering
    void SetY(float y) override; ///< flags for ordering

    /**
     *  @brief Sets the number of groups the buttons will be divided into.
     *  @param number the new number of columns or rows
     *  @throws std::invalid_argument if the number is less than 1
     */
    void SetGroupsNumber(int number);
    /**
     *  @brief Sets the number of columns the buttons will be divided into.
     *  @details Wrapper over groups, solely for convenience. Does the same.
     *  @see SetGroupsNumber
     */
    void SetColumnsNumber(int number);
    /**
     *  @brief Sets the number of rows the buttons will be divided into.
     *  @details Wrapper over groups, solely for convenience. Does the same.
     *  @see SetGroupsNumber
     */
    void SetRowsNumber(int number);
    /**
     *  @returns The number of groups the buttons will be divided into.
     */
    size_t GetGroupsNumber() const;
    /**
     *  @details Same as GetGroupsNumber.
     *  @returns The number of columns the buttons will be divided into.
     */
    size_t GetColumnsNumber() const;
    /**
     *  @details Same as GetGroupsNumber.
     *  @returns The number of rows the buttons will be divided into.
     */
    size_t GetRowsNumber() const;

private:
    std::unordered_map<std::string, std::unique_ptr<RadioButton>> buttons; ///< radio buttons
    std::vector<RadioButton*> buttonsInDrawingOrder;

    bool vertical; ///< whether the buttons will be arranged vertically or horizontally
    float buttonMargin; ///< distance from border to buttons (both x and y)
    size_t groups; ///< into how many groups will the buttons be divided (columns/rows)

    bool needsOrdering; ///< lazy ordering

    void UnpressButtons(); ///< unpresses all buttons in the group
};

/**
 *  @class DropDown
 *  @brief A choice menu dropping down after a click.
 *  @details Template over the type of the stored values. Has border.
 *  @note You probably want to give it top priority, so the unrolled list draws over other items.
 */
template <typename T>
class DropDown : public TextItem {
public:
    /**
     *  @brief Possible sorting/ordering methods
     */
    enum ORDER {
        insertion,
        reverseInsertion,
        alphabetic,
        alphabeticCaseInsensitive,
        reverseAlphabetic,
        reverseAlphabeticCaseInsensitive,
        natural, ///< item9 before item10
        reverseNatural,
        naturalCaseInsensitive,
        reverseNaturalCaseInsensitive,
        custom
    };

    struct Header : public NestedProperty {
        friend DropDown;

        OverrideColour colour;
        Colour dividerColour;
        Border border;
        OverrideFont font;

        /**
         *  @brief Sets the thickness of the divider between the header and the list.
         *  @details Drawn in dividerColour, full width. 0 turns it off.
         *  @throws std::invalid_argument if thickness is negative
         */
        void SetDividerThickness(float thickness) {
            if (thickness < 0.0f) CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Cannot set header divider thickness: cannot be negative.");
            headerDividerThickness = thickness;
        }
        float GetDividerThickness() const {
            return headerDividerThickness;
        }

        private:
            explicit Header(Item* o) : NestedProperty(o), headerDividerThickness(10.0f) { dividerColour.SetColour(BLACK); }
            float headerDividerThickness = 0.0f;
    } header{this};

    struct MenuList : public NestedProperty {
        friend DropDown;

        OverrideColour colour;
        Border border;
        OverrideFont font;
        Colour dividerColour;

        /**
         *  @brief Sets the thickness of the dividers between list items.
         *  @details Only between items - none after the last one.
         *  @throws std::invalid_argument if thickness is negative
         */
        void SetDividerThickness(float thickness) {
            if (thickness < 0.0f) CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Cannot set list divider thickness: cannot be negative.");
            dividerThickness = thickness;
        }
        float GetDividerThickness() const {
            return dividerThickness;
        }
        /**
         *  @brief Sets the height of a row in the unrolled list.
         *  @details Independent of the header's height.
         *  @throws std::invalid_argument if height is negative
         *  @note Prints a warning if height is below the list font's size (only when list.font is overridden).
         */
        void SetItemInListHeight(float height) {
            if (height < 0.0f) CPPFX_THROW_NO_ITEM(std::invalid_argument, owner->GetFxID(), owner->GetID(), "Cannot set the height of the item in the list");
            if (font.IsOverridden() && height < font.GetFontSize()) CPPFX_WARN("Height of the item in the " + owner->GetFxID() + "'s " + owner->GetID() + " is smaller than the font's size.");
            listItemHeight = height;
        }
        float GetItemInListHeight() const {
            return listItemHeight;
        }

        private:
            explicit MenuList(Item* o) : NestedProperty(o), dividerThickness(5.0f) { dividerColour.SetColour(BLACK); }
            float dividerThickness; ///< seam between rows; none after the last one
            float listItemHeight = 50.0f;   ///< row height, independent of the header's
    } list{this};

    DropDown() : Item("DropDown"), TextItem("DropDown"), order(insertion), currentLabel("") {
        list.SetItemInListHeight(height);
    }

    void DrawMyself(float elapsedTime) const override {
        DrawHeader(elapsedTime);
        if (focused) DrawList(elapsedTime);
        DrawFrame();
    }

    void DoFocusAction(float /*elapsedTime*/) override {
        return;
    }

    void DoPassiveAction(float /*elapsedTime*/) override {
        if (dirty) {
            Sort();
            dirty = false;
        }
        if (currentLabel == "" && !values.empty()) {
            SetCurrent(valuesInOrder[0]);
            if (expandsToTextAutomatically) ExpandToText();
        }
    }

    bool WasIClicked(const Vector2& mousePosition) const override {
        if (!focused) return Item::WasIClicked(mousePosition);
        return mousePosition.x >= xAnchor && mousePosition.x <= xAnchor + width
            && mousePosition.y >= yAnchor
            && mousePosition.y <= GetListTop() + GetListContentHeight();
    }

    void Focus() override {
        TextItem::Focus();
        justOpened = true;
    }

    void DoFocusAction(float /*elapsedTime*/, const Vector2& mousePosition) override {
        if (justOpened) { justOpened = false; return; }
        if (!WasIClicked(mousePosition)) { Defocus(); return; }
        if (mousePosition.y < GetListTop()) { Defocus(); return; }
        const int index = GetIndexAt(mousePosition.y);
        if (index >= 0) {
            const std::string label = valuesInOrder[index];
            SetCurrent(label);
            Defocus();
        } else if (values.empty()) {
            Defocus();
        }
    }

    /**
     *  @brief Inserts the value into the dropdown.
     *  @param label name under which the value will be displayed
     *  @param value the new value
     *  @throws std::invalid_argument if the label is empty
     *  @throws std::out_of_range if the label is taken.
     */
    void AddItem(const std::string& label, const T& value) {
        if (label == "") {
            CPPFX_WARN("Use space (' ') if you want to have it actually empty.");
            CPPFX_THROW(std::invalid_argument, "Cannot add item with empty label.");
        }
        if (IsLabelTaken(label)) {
            CPPFX_THROW(std::out_of_range, "Label already taken - cannot add a new item with the label " + label);
        }
        values.insert({label,value});
        valuesInOrder.push_back(label);
        insertionOrder.push_back(label);
        if (order != insertion) dirty = true;
    }

   /**
    *  @brief Inserts a value that is its own label.
    *  @note Only available for DropDown<std::string>.
    *  @see AddItem(const std::string&, const T&)
    */
    void AddItem(const std::string& label) requires std::same_as<T, std::string> {
        AddItem(label, label);
    }

    /**
     *  @brief Removes a value from the dropdown.
     *  @param label label for removal
     *  @throws std::out_of_range if there's no label in the dropdown.
     */
    void RemoveItem(const std::string& label) {
        if (IsLabelTaken(label)) {
            values.erase(label);
            valuesInOrder.erase(std::remove_if(valuesInOrder.begin(), valuesInOrder.end(),[&label](const std::string& lab) { return label == lab; }),valuesInOrder.end());
            insertionOrder.erase(std::remove_if(insertionOrder.begin(), insertionOrder.end(),[&label](const std::string& lab) { return label == lab; }),insertionOrder.end());
            labelToColour.erase(label);
            if (currentLabel == label) {
                currentLabel = "";
                currentValue = T{};
            }
        } else CPPFX_THROW(std::out_of_range, "No value with label " + label + " found - couldn't remove.");
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
            CPPFX_THROW(std::out_of_range, "No value with label " + oldLabel + " found - couldn't replace.");
        }
        if (IsLabelTaken(newLabel)) {
            CPPFX_THROW(std::invalid_argument, newLabel + " already is in the dropdown.");
        }
        auto node = values.extract(oldLabel);
        node.key() = newLabel;
        values.insert(std::move(node));
        if (labelToColour.contains(oldLabel)) {
            auto colourNode = labelToColour.extract(oldLabel);
            colourNode.key() = newLabel;
            labelToColour.insert(std::move(colourNode));
        }
        for (auto& label : valuesInOrder) {
            if (label == oldLabel) {
                label = newLabel;
                break;
            }
        }
        for (auto& label : insertionOrder) {
            if (label == oldLabel) {
                label = newLabel;
                break;
            }
        }
        if (currentLabel == oldLabel) currentLabel = newLabel;
        if (expandsToTextAutomatically) ExpandToText();
        dirty = true;
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
        } else CPPFX_THROW(std::out_of_range, "No key " + label + " found.");
    }

    /**
    *  @brief Changes a value that is its own label.
    *  @note Only available for DropDown<std::string>.
    *  @see ChangeValue(std::string, T)
    */
    void ChangeValue(const std::string& label) requires std::same_as<T, std::string> {
        ChangeValue(label, label);
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
        } else CPPFX_THROW(std::out_of_range, "No key " + label + " found.");
    }

    /**
     *  @brief Returns the currently picked value.
     *  @throws std::runtime_error if there is no current value.
     */
    T GetCurrentValue() const {
        if (currentLabel == "") {
            CPPFX_THROW(std::runtime_error, "No current value");
        }
        return currentValue;
    }

    /**
     *  @brief Returns the currently picked label.
     *  @throws std::runtime_error if nothing is picked.
     */
    std::string GetCurrentLabel() const {
        if (currentLabel == "") {
            CPPFX_THROW(std::runtime_error, "No current label");
        }
        return currentLabel;
    }

    /**
     *  @brief Returns current label and value.
     *  @throws std::runtime_error if nothing is picked.
     */
    std::pair<std::string, T> GetCurrentChoice() const {
        if (currentLabel == "") {
            CPPFX_THROW(std::runtime_error, "No current pick");
        }
        return {currentLabel, currentValue};
    }

    /**
     *   @brief Returns the current pick.
     *   @details Same as GetCurrentLabel(). Label and value are the same when
     *            added with AddItem(label).
     *   @note Only available for DropDown<std::string>.
     *   @throws std::runtime_error if there is no current pick.
     *   @see GetCurrentChoice
     */
    std::string GetCurrent() const requires std::same_as<T, std::string> {
        return GetCurrentLabel();
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
        } else CPPFX_THROW(std::out_of_range, "No key " + label + " found.");
        if (expandsToTextAutomatically) ExpandToText();
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
     *  @brief Header height, plus the list while open.
     *  @see Item::GetTotalHeight
     */
    float GetTotalHeight() const override {
        if (!IsFocused()) return Item::GetTotalHeight();
        return Item::GetTotalHeight() + GetTotalListHeight();
    }
    /**
     *  @brief Width plus the thicker of the header and list borders, on both sides.
     *  @see Item::GetTotalWidth
     */
    float GetTotalWidth() const override {
        return Item::GetTotalWidth() + (2.0f * (header.border.GetThickness() > list.border.GetThickness() ? header.border.GetThickness() : list.border.GetThickness()));
    }

    /** @brief Labels in the order they are drawn. */
    const std::vector<std::string>& GetLabelsInOrder() const {
        return valuesInOrder;
    }

    /**
     *  @brief Sorts the labels by the current order.
     *  @details Called automatically on the next frame after a change.
     *  @throws std::runtime_error if the order is custom and no comparator is set.
     */
    void Sort() {
        if (order == ORDER::insertion || order == ORDER::reverseInsertion) {
            valuesInOrder = insertionOrder;
            if (order == ORDER::reverseInsertion)
                std::reverse(valuesInOrder.begin(), valuesInOrder.end());
            return;
        }

        std::function<bool(const std::string&, const std::string&)> cmp;
        bool rev = false;

        switch (order) {
            case ORDER::alphabetic:                       cmp = std::less<std::string>{}; break;
            case ORDER::reverseAlphabetic:                cmp = std::less<std::string>{}; rev = true; break;
            case ORDER::alphabeticCaseInsensitive:        cmp = CaseInsensitiveLess; break;
            case ORDER::reverseAlphabeticCaseInsensitive: cmp = CaseInsensitiveLess; rev = true; break;
            case ORDER::natural:                          cmp = NaturalLess; break;
            case ORDER::reverseNatural:                   cmp = NaturalLess; rev = true; break;
            case ORDER::naturalCaseInsensitive:           cmp = NaturalLessCaseInsensitive; break;
            case ORDER::reverseNaturalCaseInsensitive:    cmp = NaturalLessCaseInsensitive; rev = true; break;
            case ORDER::custom:                           if (!customSort) { CPPFX_THROW(std::runtime_error, "No custom sorting method set"); } cmp = customSort; break;
            default: return;
        }

        std::stable_sort(valuesInOrder.begin(), valuesInOrder.end(), cmp);
        if (rev) std::reverse(valuesInOrder.begin(), valuesInOrder.end());
    }

    /** @brief Sets the sorting order. Applied on the next frame. */
    void SetOrder(DropDown::ORDER orderType) {
        order = orderType;
        dirty = true;
    }

    DropDown::ORDER GetOrder() const {
        return order;
    }

    /** @brief Sets the comparator used by ORDER::custom. */
    void SetCustomSort(const std::function<bool(const std::string&, const std::string&)>& custom) {
        customSort = custom;
    }

    /**
     *  @see TextItem::ExpandToText
     *  @details Measures currentLabel. Also grows the list's row height to fit it.
     */
    virtual void ExpandToText() override {
        text = currentLabel;
        TextItem::ExpandToText();
        const CPPFX::Font& rowFont = list.font.IsOverridden() ? list.font.GetOverride() : font;
        const float rowNeeds = rowFont.GetInkSize(currentLabel).y + (2.0f * textMargin);
        if (rowNeeds > list.GetItemInListHeight()) list.SetItemInListHeight(rowNeeds);
    }
    /**
     *  @see TextItem::FitToText
     *  @details Measures currentLabel. Also sets the list's row height to fit it.
     */
    void FitToText() override {
        text = currentLabel;
        TextItem::FitToText();
        const CPPFX::Font& rowFont = list.font.IsOverridden() ? list.font.GetOverride() : font;
        const float rowNeeds = rowFont.GetInkSize(currentLabel).y + (2.0f * textMargin);
        list.SetItemInListHeight(rowNeeds);
    }

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID() {
        return "DropDown";
    }

    /**
     *  @brief Gives one label its own background colour, overriding list.colour.
     *  @note Prints a warning if the label isn't in the dropdown. The colour is
     *        still stored and applies if the label is added later.
     */
    void SetColourToLabel(const std::string& label, const Colour& colour) {
        if (!IsLabelTaken(label)) CPPFX_WARN("Label " + label + " is not in the " + GetFxID() + " " + GetID());
        if (labelToColour.contains(label)) {
            labelToColour.at(label) = colour;
        } else {
            labelToColour.insert({label, colour});
        }
    }
    /**
     *  @brief Drops a label's colour, returning that row to list.colour.
     *  @note Prints a warning if the label had no colour set.
     */
    void RemoveColourFromLabel(const std::string& label) {
        if (labelToColour.contains(label)) {
            labelToColour.erase(label);
        } else {
            CPPFX_WARN("No colour was set to this label: " + label + " in the " + GetFxID() + " " + GetID());
        }
    }
    /** @brief Drops every per-label colour. Labels themselves are untouched. */
    void ClearColoursFromLabels() {
        labelToColour.clear();
    }
    /** @details Labels with no colour of their own are absent from the map. */
    const std::unordered_map<std::string, Colour>& GetColoursToLabels() const {
        return labelToColour;
    }

    /** @brief Combined height of the rows alone - no dividers, no borders. */
    float GetRowsHeight() const {
        return (float)(GetNumberOfItems()) * list.GetItemInListHeight();
    }
    /** @brief Number of items. The header shows one of them rather than being one. */
    size_t GetNumberOfItems() const {
        return values.size();
    }
    /** @brief Content height plus the list border on both sides. */
    float GetTotalListHeight() const {
        return GetListContentHeight() + (2.0f * list.border.GetThickness());
    }
    /**
     *  @brief Height of everything inside the list border.
     *  @details Header divider, rows, and the dividers between them. An empty
     *           dropdown still measures the header divider, so opening one
     *           shows a stub.
     */
    virtual float GetListContentHeight() const {
        if (values.empty()) return header.GetDividerThickness();
        return header.GetDividerThickness() + (float)valuesInOrder.size() * list.GetItemInListHeight() + (float)(values.size() - 1) * list.GetDividerThickness();
    }

    /** @brief Top of the list, where the header divider starts. */
    float GetListTop() const {
        return yAnchor + height + header.border.GetThickness() + list.border.GetThickness();
    }
    /** @brief Distance from one row's top to the next. */
    float GetRowPitch() const {
        return list.GetItemInListHeight() + list.GetDividerThickness();
    }
    /**
     *  @brief Index of the row at a given y.
     *  @returns index into GetLabelsInOrder, or -1 if y is on the header
     *           divider, on a divider between rows, or past the list.
     */
    int GetIndexAt(float y) const {
        if (values.empty()) return -1;
        const float rowsTop = GetListTop() + header.GetDividerThickness();
        if (y < rowsTop) return -1;
        const float offset = y - rowsTop;
        const int index = (int)(offset / GetRowPitch());
        if (index < 0 || index >= (int)values.size()) return -1;
        if (fmod(offset, GetRowPitch()) >= list.GetItemInListHeight()) return -1;
        return index;
    }

protected:
    std::unordered_map<std::string, T> values;      ///< source of truth: every label and the value it stands for
    std::vector<std::string> valuesInOrder;         ///< labels as drawn, top to bottom; rebuilt by Sort
    std::vector<std::string> insertionOrder;        ///< labels in the order they arrived, so insertion order survives sorting
    ORDER order;                                    ///< how valuesInOrder is arranged
    std::string currentLabel;                       ///< the pick shown in the header; empty means nothing picked yet
    T currentValue;                                 ///< value behind currentLabel, kept alongside it to avoid a lookup per frame
    bool dirty = false;                             ///< lazy sorting - set on insert or reorder, consumed by DoPassiveAction
    std::function<bool(const std::string&, const std::string&)> customSort; ///< comparator for ORDER::custom
    bool justOpened = false;                        ///< swallows the click that opened the list
    std::unordered_map<std::string, Colour> labelToColour; ///< per-row background overrides; labels absent from here use list.colour

    /** @brief Draws the header - background, current pick, and its border. */
    virtual void DrawHeader(float /*elapsedTime*/) const {
        if (header.colour.IsOverridden()) DrawRectangle(xAnchor, yAnchor, width, height, header.colour.GetColour());
        else DrawRectangle(xAnchor, yAnchor, width, height, colour.GetColour());
        if (!currentLabel.empty()) {
            if (header.font.IsOverridden()) DrawAlignedText(Alignment::CENTRE, currentLabel, header.font.GetOverride());
            else DrawAlignedText(Alignment::CENTRE, currentLabel, font);
        }
        header.border.DrawMyself(xAnchor, yAnchor, width, height);
    }

    /**
     *  @brief Draws the unrolled list - divider, rows, and the list border.
     *  @details Advances by GetRowPitch, the same figure GetIndexAt divides by,
     *           so what is drawn and what is clickable cannot drift apart.
     */
    virtual void DrawList(float /*elapsedTime*/) const {
        float yCurrent = GetListTop();
        Color drawColour = colour.GetColour();
        if (list.colour.IsOverridden()) drawColour = list.colour.GetColour();
        CPPFX::Font drawFont = font;
        if (list.font.IsOverridden()) drawFont = list.font.GetOverride();

        DrawRectangle(xAnchor, yCurrent, width, header.GetDividerThickness(), drawColour);

        yCurrent += header.GetDividerThickness();
        for (size_t i = 0; i < valuesInOrder.size(); ++i) {
            const std::string& label = valuesInOrder[i];
            const Color rowColour = labelToColour.contains(label)
                                  ? labelToColour.at(label).GetColour()
                                  : drawColour;

            DrawRectangle(xAnchor, yCurrent, width, list.GetItemInListHeight(), rowColour);
            DrawAlignedText(Alignment::CENTRE, label, drawFont,
                            xAnchor + textMargin, yCurrent + textMargin,
                            width - (2.0f * textMargin),
                            list.GetItemInListHeight() - (2.0f * textMargin));

            if (i + 1 < valuesInOrder.size()) {
                DrawRectangle(xAnchor, yCurrent + list.GetItemInListHeight(), width,
                              list.GetDividerThickness(), list.dividerColour.GetColour());
            }
            yCurrent += GetRowPitch();
        }

        list.border.DrawMyself(xAnchor, GetListTop(), width, GetListContentHeight());
    }

    /** @brief Draws the outer border around the header and, when open, the list. */
    virtual void DrawFrame() const {
        const float outer = std::max(header.border.GetThickness(), list.border.GetThickness());
        const float listSpan = focused
                             ? GetListContentHeight() + (2.0f * list.border.GetThickness())
                             : 0.0f;
        border.DrawMyself(xAnchor - outer, yAnchor - header.border.GetThickness(),
                          width + (2.0f * outer),
                          height + (2.0f * header.border.GetThickness()) + listSpan);
    }

    /// Natural comparison; key maps each non-digit character before comparing.
    template <typename Key>
    static bool NaturalLessBy(const std::string& a, const std::string& b, Key key) {
        size_t i = 0, j = 0;
        while (i < a.size() && j < b.size()) {
            if (std::isdigit((unsigned char)a[i]) && std::isdigit((unsigned char)b[j])) {
                // skip leading zeros
                size_t iStart = i, jStart = j;
                while (i < a.size() && a[i] == '0') ++i;
                while (j < b.size() && b[j] == '0') ++j;

                size_t iNum = i, jNum = j;
                while (i < a.size() && std::isdigit((unsigned char)a[i])) ++i;
                while (j < b.size() && std::isdigit((unsigned char)b[j])) ++j;

                size_t iLen = i - iNum, jLen = j - jNum;
                if (iLen != jLen) return iLen < jLen;          // fewer digits = smaller
                int cmp = a.compare(iNum, iLen, b, jNum, jLen); // same length: lexical works
                if (cmp != 0) return cmp < 0;

                // equal numerically: fewer leading zeros sorts first, as a tiebreak
                if ((iNum - iStart) != (jNum - jStart)) {
                    return (iNum - iStart) < (jNum - jStart);
                }
            } else {
                const auto ka = key((unsigned char)a[i]), kb = key((unsigned char)b[j]);
                if (ka != kb) return ka < kb;
                ++i; ++j;
            }
        }
        return (a.size() - i) < (b.size() - j);
    }

    static bool NaturalLess(const std::string& a, const std::string& b) {
        return NaturalLessBy(a, b, [](unsigned char c) { return c; });
    }

    static bool NaturalLessCaseInsensitive(const std::string& a, const std::string& b) {
        return NaturalLessBy(a, b, [](unsigned char c) { return std::tolower(c); });
    }

    static bool CaseInsensitiveLess(const std::string& a, const std::string& b) {
        return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(),
            [](unsigned char c1, unsigned char c2) {
                return std::tolower(c1) < std::tolower(c2);
            });
    }

private:
    /**
     *  @details Text is internal here - it mirrors currentLabel for measuring only.
     *  Hides TextItem's text interface into private, the same way Embedded does.
     *  @see DropDown::GetCurrentLabel
     *  @see DropDown::SetCurrent
     */
    void SetText(const std::string& text) override { TextItem::SetText(text); }
    void ClearText() override { TextItem::ClearText(); }
    std::string GetText() const override { return TextItem::GetText(); }
};

/**
 *  @class ComboBox
 *  @brief DropDown<std::string> you can add items to.
 *  @details Type into addArea and press Enter to add a label and pick it. An added label is also its value.
 */
class ComboBox : public DropDown<std::string> {
public:
    EmbeddedTextField addArea;

    ComboBox() : Item("ComboBox"), DropDown<std::string>() {
        addArea.ClearText();
        addArea.SetPromptText("Add new item");
        addArea.Hide();
    }

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;

    /** @see DropDown::SetX @details Moves addArea. */
    void SetX(float x) override;
    /** @see DropDown::SetY @details Moves addArea. */
    void SetY(float y) override;
    /** @see DropDown::SetWidth @details Resizes addArea. */
    void SetWidth(float value) override;
    /** @see DropDown::SetHeight @details Resizes addArea. */
    void SetHeight(float value) override;

    void SetToWorld() override;
    void SetToScreen() override;

    void MakeVisible() override;
    void MakeInvisible() override;
    void SetVisible(bool flag) override;

    void MakeActive() override;
    void MakeInactive() override;
    void SetInactive(bool flag) override;

    /**
     *  @see Item::Focus
     *  @details Unrolls the dropdown and reveals addArea. Arms the click-swallowing flag,
     *  unless the ComboBox is set to focus addArea immediately.
     */
    void Focus() override;
    /** @see Item::Defocus @details Clears and defocuses addArea. */
    void Defocus() override;

    /** @brief Sets the text margin, addArea's included. */
    void SetTextMargin(float margin);

    /**
     *  @brief Makes addArea take focus the moment the dropdown opens.
     *  @details The opening click falls through to addArea, so the caret appears
     *  and the prompt text gives way at once - one click to start typing.
     */
    void ImmediatelyFocusAddArea();
    /**
     *  @brief Makes the opening click only unroll the dropdown.
     *  @details The click that focuses the ComboBox is swallowed, so addArea stays
     *  unfocused and keeps showing its prompt text. A second click on the header
     *  focuses it for input. Default.
     */
    void AddAreaFocusesOnSecondClick();
    /**
     *  @brief Sets whether the opening click is swallowed.
     *  @param should true - second click needed to type, false - addArea focuses on open
     *  @see ComboBox::ImmediatelyFocusAddArea
     *  @see ComboBox::AddAreaFocusesOnSecondClick
     */
    void AddAreaShouldFocusOnSecondClick(bool should);
    /**
     *  @brief Checks whether addArea takes focus as the dropdown unrolls.
     *  @returns true if addArea focuses on open
     */
    bool DoesAddAreaFocusOnSecondClick() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
    void CommitAddArea();

    bool opensOnSecondClick = true; ///< if true, the click that unrolls the dropdown doesn't reach addArea
};

/**
 *  @class SearchableDropDown
 *  @brief DropDown<std::string> with a search field.
 *  @details Typing into searchArea filters the list. By default a label stays if it starts with the typed text.
 */
class SearchableDropDown : public DropDown<std::string> {
public:
    EmbeddedTextField searchArea;

    SearchableDropDown() : Item("SearchableDropDown"), DropDown<std::string>() {
        searchArea.ClearText();
        searchArea.SetPromptText("Search");
        searchArea.Hide();
    }

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;

    /** @see DropDown::SetX @details Moves searchArea. */
    void SetX(float x) override;
    /** @see DropDown::SetY @details Moves searchArea. */
    void SetY(float y) override;
    /** @see DropDown::SetWidth @details Resizes searchArea. */
    void SetWidth(float value) override;
    /** @see DropDown::SetHeight @details Resizes searchArea. */
    void SetHeight(float value) override;

    void SetToWorld() override;
    void SetToScreen() override;

    void MakeVisible() override;
    void MakeInvisible() override;
    void SetVisible(bool flag) override;

    void MakeActive() override;
    void MakeInactive() override;
    void SetInactive(bool flag) override;

    /**
     *  @see Item::Focus
     *  @details Unrolls the dropdown and reveals searchArea. Arms the click-swallowing flag,
     *  unless the SearchableDropDown is set to focus searchArea immediately.
     */
    void Focus() override;
    /** @see Item::Defocus @details Clears and defocuses searchArea. */
    void Defocus() override;

    /** @brief Sets the text margin, searchArea's included. */
    void SetTextMargin(float margin);

    /**
     *  @brief Makes searchArea take focus the moment the dropdown opens.
     *  @details The opening click falls through to searchArea, so the caret appears
     *  and the prompt text gives way at once - one click to start typing.
     */
    void ImmediatelyFocusSearchArea();
    /**
     *  @brief Makes the opening click only unroll the dropdown.
     *  @details The click that focuses the SearchableDropDown is swallowed, so searchArea stays
     *  unfocused and keeps showing its prompt text. A second click on the header
     *  focuses it for input. Default.
     */
    void SearchAreaFocusesOnSecondClick();
    /**
     *  @brief Sets whether the opening click is swallowed.
     *  @param should true - second click needed to type, false - searchArea focuses on open
     */
    void SearchAreaShouldFocusOnSecondClick(bool should);
    /**
     *  @brief Checks whether searchArea takes focus as the dropdown unrolls.
     *  @returns true if searchArea focuses on open
     */
    bool DoesSearchAreaFocusOnSecondClick() const;

    float GetListContentHeight() const override; ///< height of the filtered list.
    /**
     *  @brief Height of N items inside the list border.
     *  @details Header divider, rows, and the dividers between them. An empty
     *           dropdown still measures the header divider, so opening one
     *           shows a stub.
     *  @param number number of items in the list
     */
    float GetListContentHeight(size_t number) const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
     bool opensOnSecondClick = true; ///< if true, the click that unrolls the dropdown doesn't reach searchArea
     /**
      * @brief Decides whether the label (first argument) belongs in the list based on the value of TextField (second argument).
      * @see DefaultSearch
      */
     std::function<bool(const std::string&, const std::string&)> customSearch;
     std::vector<size_t> filtered;   ///< indices into valuesInOrder
     std::string lastQuery;
     bool filterDirty = true;        ///< set this wherever valuesInOrder changes

     void DrawList(float elapsedTime) const override;
     void DrawFrame() const override;
     void FilterList();
     bool DefaultSearch(const std::string& label, const std::string& query) const; ///< simple filter, checks whether the label begins with the text in searchArea
};

// --- Containers ---

/**
 *  @class Workspace
 *  @brief A simple container that keeps items in bounds.
 *  @details If item is outside the workspace, it gets pushed back to the nearest side inside the area.
 */
class Workspace : public Container {
public:

    Workspace() : Item("Workspace"), Container("Workspace", 500.0f, 500.0f) {}

    void SetPositionsOfItems() override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class AnchorPane
 *  @brief A container keeping its children in the same relative positions to it after moving and changing dimensions.
 */
class AnchorPane : public Container {
public:

    AnchorPane() : Item("AnchorPane", 500.0f, 500.0f), Container("AnchorPane", 500.0f, 500.0f), previousX(0.0f), previousY(0.0f), previousWidth(500.0f), previousHeight(500.0f) {}

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

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

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

    VBox() : Item("VBox"), Box("VBox", 100.0f, 200.0f) {}

    void SetPositionsOfItems() override;
    /**
     *  @see Item::GetTotalHeight
     *  @details Height, or the children's total height if that's greater.
     */
    float GetTotalHeight() const override;

    float GetChildrenTotalLength() const override;
    float GetItemsTotalHeight() const; ///< same as GetChildrenTotalLength

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class HBox
 *  @brief A container for displaying the items in a horizontal line.
 *  @details Width automatically increases with items.
 */
class HBox : public Box {
public:

    HBox() : Item("HBox"), Box("HBox", 200.0f, 100.0f) {}

    void SetPositionsOfItems() override;
    /**
     *  @see Item::GetTotalWidth
     *  @details Width, or the children's total width if that's greater.
     */
    float GetTotalWidth() const override;

    float GetChildrenTotalLength() const override;
    float GetItemsTotalWidth() const; ///< same as GetChildrenTotalLength

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class Spinner
 *  @brief A value picker. Use buttons to increase or decrease value.
 *  @details Can wrap around. Composed of buttons. Has border. Has font. Min and max values can be set. Bounds are inclusive.
 */
class Spinner : virtual public Item {
public:
    EmbeddedButton incrementButton;
    EmbeddedButton decrementButton;

    Font font;

    Spinner()
        : Item("Spinner", 200.0f, 100.0f), font(height / 2.0f), valueMargin(10.0f), value(0.0f), stepValue(1.0f),
            hasMin(false), hasMax(false), minValue(0.0f), maxValue(100.0f), allowWrap(false)  {
                Initialise();
    }
    explicit Spinner(const std::string& /*fxID*/)
        : Item("Spinner", 200.0f, 100.0f), font(height / 2.0f), valueMargin(10.0f), value(0.0f), stepValue(1.0f),
            hasMin(false), hasMax(false), minValue(0.0f), maxValue(100.0f), allowWrap(false) {
                Initialise();
    }

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;
    bool WasIClicked(const Vector2& mousePosition) const override;

    void SetToScreen() override;
    void SetToWorld() override;

    /**
     *  @brief Sets the current value.
     *  @note Prints a warning if the value is outside the enabled bounds. It's set anyway.
     */
    virtual void SetValue(float value);
    float GetValue() const;
    /**
     *  @brief Sets by how much will the value be incremented/decremented after a button press.
     *  @note Prints a warning if the step is bigger than max - min, with both bounds enabled.
     */
    void SetStep(float step);
    float GetStep() const;
    void AllowWrap();
    void StopWrapping();
    void SetWrap(const bool flag);
    bool IsWrapAllowed() const;

    /**
     *  @brief Sets max value.
     *  @param value new value to be set
     *  @details Doesn't enable the bound - see SetMax. Max below min isn't supported.
     *  @note Prints a warning if max is below min.
     */
    void SetMaxValue(float value);
    /**
     *  @brief Gets max value.
     *  @returns maxValue
     */
    float GetMaxValue() const;
    /**
     *  @brief Sets min value.
     *  @param value new value to be set
     *  @details Doesn't enable the bound - see SetMin. Min above max isn't supported.
     *  @note Prints a warning if min is above max.
     */
    void SetMinValue(float value);
    /**
     *  @brief Gets min value.
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
     *  @details Moves the buttons too.
     */
    void SetX(float x) override;
    /**
     *  @see Item::SetY
     *  @details Moves the buttons too.
     */
    void SetY(float y) override;
    /**
     *  @see Item::SetWidth
     *  @details Moves the buttons to the new right edge.
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

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

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

};

/**
 *  @class EditableSpinner
 *  @brief Spinner in which you can enter a value.
 *  @details Composed of a Spinner and a TextField.
 */
class EditableSpinner : virtual public Item, public Spinner {
public:
    EmbeddedTextField editArea;

    EditableSpinner() : Item("EditableSpinner"), Spinner("EditableSpinner") {
        editArea.SetX(xAnchor);
        editArea.SetY(yAnchor);
        editArea.SetWidth(width);
        editArea.SetHeight(height);
        editArea.border.SetThickness(2.0f);
        editArea.SetText(value == (int)value ? TextFormat("%d", (int)value) : TextFormat("%.2f", value));
        editArea.font.SetFontSize(font.GetFontSize());
        incrementButton.SetX(xAnchor + width + editArea.border.GetThickness());
        decrementButton.SetX(xAnchor + width + editArea.border.GetThickness());
    }

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;
    /**
     *  @brief Defocuses and loads value.
     *  @details Keeps the old value on invalid input. A number too big for a float becomes the float max. Then clamps to the enabled bounds.
     */
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
     *  @details Sets editArea's text margin.
     */
    void SetValueMargin(float margin) override;
    /**
     *  @see Spinner::GetValueMargin
     *  @details Returns editArea's text margin.
     */
    float GetValueMargin() const override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class PasswordField
 *  @brief TextField with masked input.
 *  @details The mask character can be changed. Default is '*'.
 */
class PasswordField : public TextField {
public:

    PasswordField() : Item("PasswordField"), TextField("PasswordField"), mask('*') {promptText = "Password"; ClearText();}

    void DrawMyself(float elapsedTime) const override;

    void SetMask(const char character);
    char GetMask() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
    char mask;
};

/**
 *  @class ProgressIndicator
 *  @brief Shows progress, or that something is ongoing.
 *  @details Has font. DOTS are circling dots, also drawn for indefinite progress whatever the shape.
 *  CIRCLE fills like a pie, RING like a ring. For a bar, use ProgressBar.
 */
class ProgressIndicator : public Item {
public:
    enum Shapes {DOTS, CIRCLE, RING, BAR};

    Font font;

    ProgressIndicator() : Item("ProgressIndicator"), font(height / 4.0f), shape(ProgressIndicator::Shapes::DOTS), displayValue(true), value(-1.0f)  {}
    explicit ProgressIndicator(const std::string& i) : Item(i), font(height / 4.0f), shape(ProgressIndicator::Shapes::BAR), displayValue(true), value(-1.0f) {}

    /**
     *  @brief Sets progress value.
     *  @param value 0 - 1, -1 is indefinite progress
     *  @throws std::out_of_range if parameter is beyond the range.
     */
    void SetProgress(float value);
    /**
     *  @brief Increases the progress value.
     *  @details Clamps to 0 - 1.
     *  @param change how much it will increase
     */
    void IncreaseProgress(float change);
    /**
     *  @brief Decreases the progress value.
     *  @details Clamps to 0 - 1.
     *  @param change how much it will decrease
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
     *  @returns A whole number between 0 and 100 inclusive. -1 if indefinite.
     */
    int GetProgressPercentage() const;
    /**
     * @brief Checks whether the value is 100%
     * @return true if value is 1, false otherwise.
     */
    bool IsComplete() const;
    /**
     *  @brief Sets new shape
     *  @param shape DOTS, RING or CIRCLE
     *  @throws std::invalid_argument for BAR or anything else
     *  @note Prints a warning for BAR - use ProgressBar.
     */
    virtual void SetShape(const ProgressIndicator::Shapes& shape);
    /**
     *  @brief Sets new shape
     *  @param shape a string containing the name of the shape - {"DOTS", "RING", "CIRCLE", "BAR"}
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    virtual void SetShape(const std::string& shape);
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
     *  @brief Sets whether the indicator should display the numerical value (percentage) of progress.
     *  @param flag true - enable, false - disable.
     */
    void SetDisplayValue(const bool& flag);
    bool IsDisplayingValue() const;

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    Shapes shape;
    bool displayValue; ///< whether displays the value in percents. Indefinite doesn't display value.
    float value; ///< raw value of the progress - between 0 and 1, or -1 for indefinite

    Shapes StringToShape(const std::string& shape) const;
    virtual std::string ShapeToString(const ProgressIndicator::Shapes& shape) const;
};


/**
 *  @class ProgressBar
 *  @brief Horizontal progress bar.
 *  @details Can be solid or segmented. Can also use the ProgressIndicator shapes.
 */
class ProgressBar : public ProgressIndicator {
public:
    Colour barColour; ///< colour of the filled part

    ProgressBar() : ProgressIndicator("ProgressBar"),
                    barColour(GREEN), barMargin(5.0f), segmented(false), numberOfSegments(10), gapBetweenSegments(1.0f) {shape = ProgressIndicator::Shapes::BAR;}

    void DrawMyself(float elapsedTime) const override;

    /**
     *  @brief Sets bar margin.
     *  @param value new bar margin length
     *  @throws std::out_of_range if value is greater than half of the height
     *  @throws std::invalid_argument if value is lower than 0
     */
    void SetBarMargin(float value);
    float GetBarMargin() const;
    /**
     *  @brief Sets number of segments.
     *  @param value new number of segments
     *  @throws std::invalid_argument if value is less than 1
     */
    void SetNumberOfSegments(int value);
    int GetNumberOfSegments() const;
    /**
     *  @brief Sets length of the gap between segments.
     *  @param value new gap length
     *  @throws std::invalid_argument if value is negative or too big for the width and number of segments
     */
    void SetGapBetweenSegments(float value);
    float GetGapBetweenSegments() const;
    /**
     *  @brief Sets the number of segments and the gap between them.
     *  @details Calls setters for the fields.
     *  @param number number of segments
     *  @param gap length of the gap between segments
     *  @throws std::invalid_argument if number or gap is negative, or gap is too big
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
    void SetSegmented(const bool& flag);
    bool IsSegmented() const;

    /**
     *  @brief Sets new shape
     *  @param shape DOTS, RING, CIRCLE or BAR
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    void SetShape(const ProgressIndicator::Shapes& shape) override;
    /**
     *  @brief Sets new shape
     *  @param shape a string containing the name of the shape - {"DOTS", "RING", "CIRCLE", "BAR"}
     *  @throws std::invalid_argument if the parameter is outside the available list
     */
    void SetShape(const std::string& shape) override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
    float barMargin; ///< the distance between the top/side of the main bar and the progress bar.
    bool segmented;
    int numberOfSegments;
    float gapBetweenSegments;

    std::string ShapeToString(const ProgressIndicator::Shapes& shape) const override;
};

/**
 *  @class PressedButton
 *  @brief Button that stays pressed until clicked again.
 *  @details Shows pressedText and pressedColour while pressed. Has border.
 */
class PressedButton : public TextItem, public virtual PersistentState {
public:

    PressedButton() : Item("PressedButton"), TextItem("PressedButton"), pressedText("Pressed Button") {}

    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;
    void DoFocusAction(float elapsedTime, const Vector2& mousePosition) override;

    void SetPressedText(const std::string& value);
    void ClearPressedText();
    std::string GetPressedText() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
    std::string pressedText; ///< text shown while pressed
};

/**
 *  @class List
 *  @brief Shows values as text, in a column or a row.
 *  @details Strings are shown as they are, numbers through std::to_string. Anything else needs SetDisplayMethod.
 */
template<typename T>
class List : public virtual Item, public virtual Padded {
public:
    Font font;


    List() : Item("List"), Padded(), font(height / 4), vertical(true) {}

    void AddItem(const T& item) {
        items.push_back(item);
    }
    /** @brief Removes the item at index. @throws std::out_of_range if index is invalid */
    void RemoveItem(int index) {
        if (index < 0 || (size_t)(index) >= items.size()) {
            CPPFX_THROW(std::out_of_range,"Index beyond range at removal.");
        }
        items.erase(items.begin() + index);
    }
    /** @brief Replaces the item at index. @throws std::out_of_range if index is invalid */
    void ReplaceItem(int index, const T& item) {
        if (index < 0 || (size_t)(index) >= items.size()) {
            CPPFX_THROW(std::out_of_range,"Index beyond range at replacing.");
        }
        items[index] = item;
    }

    /** @brief Gets the item at index. @throws std::out_of_range if index is invalid */
    const T& GetItem(int index) const {
        if (index < 0 || (size_t)(index) >= items.size()) {
            CPPFX_THROW(std::out_of_range,"Index beyond range at getting.");
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

    /** @brief Sets how an item is turned into text. */
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
    void DrawMyself(float /*elapsedTime*/) const override {
        float next = 0.0f;
        for (const auto& item : items) {
            std::string text;
            if (displayMethod) {
                text = displayMethod(item);
            } else if constexpr (std::is_same_v<T, std::string>) {
                text = item;
            } else if constexpr (requires { std::to_string(T{}); }) {
                text = std::to_string(item);
            } else {
                CPPFX_THROW(std::runtime_error, "No display method set for a non-standard type.");
            }

            if (vertical) {
                font.DrawText(text, xAnchor, yAnchor + next);
                next += font.GetFontSize() + padding;
            } else {
                font.DrawText(text, xAnchor + next, yAnchor);
                next += font.MeasureTextWidth(text) + padding;
            }
        }
    }

    void DoFocusAction(float /*elapsedTime*/) override {
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
        float sum = 0.0f;
        for (const auto& item : items) {
            std::string text;
            if (displayMethod) {
                text = displayMethod(item);
            } else if constexpr (std::is_same_v<T, std::string>) {
                text = item;
            } else if constexpr (requires { std::to_string(T{}); }) {
                text = std::to_string(item);
            } else {
                CPPFX_THROW(std::runtime_error, "No display method set for a non-standard type - couldn't calculate width.");
            }
            sum += font.MeasureTextWidth(text) + padding;
        }
        return sum;
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
        return (float)(items.size() - 1) * (padding + font.GetFontSize()) + font.GetFontSize();
    }

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID() {
        return "List";
    }

private:
    std::vector<T> items; ///< objects in the list
    std::function<std::string(const T&)> displayMethod; ///< how an item is turned into text; if unset, see the class description

    bool vertical; ///< whether the next item in the list will be next to or under the previous one
};

/**
 *  @class PieChart
 *  @brief Pie chart of the Chart's values.
 *  @details Can show labels and percentages. Labels are shown only if every value has one, otherwise the values are shown instead.
 */
class PieChart : public Chart, virtual public Circular {
public:

    PieChart() : Item("PieChart"), Circular(), Chart("PieChart"), showLabels(false), showPercentage(false) {}

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

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

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

    Line() : Item("Line"), Shape("Line"), pointToPoint(true), thickness(10.0f), angle(0.0f), xEnd(xAnchor + width), yEnd(yAnchor) {}

    /**
     *  @brief Returns length of the line.
     *  @details Use it to smoothly go between methods of drawing, at least length wise.
     *  @returns Line's length.
     */
    float CalculateMyArea() const override;

    void DrawMyself(float elapsedTime) const override;

    //bool WasIClicked(const Vector2& mousePosition) const override; // TODO FIX IT IT HAS A RECTANGULAR HIT DETECTION

    /**
     *  @brief Sets length of the line.
     *  @details Used only when drawing by length and angle.
     *  @param length new value of length
     *  @throws std::invalid_argument if the length is negative
     *  @note Uses width internally.
     */
    void SetLength(float length);
    float GetLength() const;
    /**
     *  @brief Sets the width of the line.
     *  @details In angle and length mode, sets the length directly.
     *  In point to point mode, moves the end point horizontally,
     *  setting the horizontal leg of the right triangle formed by the line.
     *  @param value new value
     *  @throws std::invalid_argument if value is negative
     *  @see SetLength
     *  @see SetEndPoint
     */
    void SetWidth(float value) override;

    /**
     *  @brief Sets the height of the line.
     *  @details In angle and length mode, sets the length directly.
     *  In point to point mode, moves the end point vertically,
     *  setting the vertical leg of the right triangle formed by the line.
     *  @param value new value
     *  @throws std::invalid_argument if value is negative
     *  @see SetLength
     *  @see SetEndPoint
     */
    void SetHeight(float value) override;

    /**
     *  @brief Returns the horizontal extent of the line.
     *  @details In point to point mode, returns the absolute horizontal distance between start and end points.
     *  In angle and length mode, returns the horizontal component of the length vector.
     *  @returns Horizontal leg of the bounding triangle.
     */
    float GetWidth() const override;

    /**
     *  @brief Returns the vertical extent of the line.
     *  @details In point to point mode, returns the absolute vertical distance between start and end points.
     *  In angle and length mode, returns the vertical component of the length vector.
     *  @returns Vertical leg of the bounding triangle.
     */
    float GetHeight() const override;

    /**
     *  @brief Returns the total height of the line's bounding box.
     *  @details GetHeight plus thickness, accounting for the physical space the line occupies.
     *  @see GetHeight
     */
    float GetTotalHeight() const override;

    /**
     *  @brief Returns the total width of the line's bounding box.
     *  @details GetWidth plus thickness, accounting for the physical space the line occupies.
     *  @see GetWidth
     */
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

    /**
     *  @brief Sets the end of the line.
     *  @details Used only when the drawing method is set to pointToPoint.
     *  @param x x coordinate of the end
     *  @param y y coordinate of the end
     *  @note Prints a warning if the end point is the same as the starting point.
     */
    void SetEndPoint(float x, float y);
    /**
     *  @brief Sets the end of the line.
     *  @details Used only when the drawing method is set to pointToPoint.
     *  @param coordinates the end point
     *  @note Prints a warning if the end point is the same as the starting point.
     */
    void SetEndPoint(const Vector2& coordinates);
    float GetEndX() const;
    float GetEndY() const;
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
     *  @details Set the end with SetEndPoint.
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

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
    bool pointToPoint; ///< method of drawing. False is based on angle and length (width)
    float thickness; ///< thickness of the line
    float angle; ///< angle at which the line will be drawn (from X axis counterclockwise).
    float xEnd; ///< in point to point method: point at which the line should end.
    float yEnd; ///< in point to point method: point at which the line should end.

    void CalculateLength(); // sets length (width)
};

/**
 *  @class Square
 *  @brief Equal sides.
 *  @details Uses only width, height follows it.
 */
class Square : public Shape {
public:

    Square() : Item("Square"), Shape("Square") {height = width;}

    void DrawMyself(float elapsedTime) const override;

    float CalculateMyArea() const override; ///< width * width

    void SetWidth(float value) override; ///< also sets height
    void SetHeight(float value) override; ///< sets width, and height with it
    float GetHeight() const override; ///< returns width
    float GetTotalHeight() const override;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class Rectangle
 *  @brief Rectangle shape.
 *  @details Clashes with raylib's Rectangle under using namespace CPPFX - write CPPFX::Rectangle.
 */
class Rectangle : public Shape {
public:

    Rectangle() : Item("Rectangle"), Shape("Rectangle") {}

    void DrawMyself(float elapsedTime) const override;

    float CalculateMyArea() const override; ///< width * height

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

/**
 *  @class Circle
 *  @brief Is a perfect circle.
 *  @details Composes Circular for x,y translation and radius.
 */
class Circle : public Shape, public virtual Circular {
public:

    Circle() : Item("Circle"), Shape("Circle") {}

    void DrawMyself(float elapsedTime) const override;

    float CalculateMyArea() const override; ///< uses raylib's PI definition. PI * (width/2)^2

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();
};

// --- Sprite ---

/**
 *  @class Sprite
 *  @brief A 2D texture
 *  @details Wrapper over raylib's Texture2D. Colour is the tint over texture - defaults to WHITE for that reason.
 *  @see Texture2D
 */
class Sprite : public Item {
public:

    Sprite() : Item("Sprite"), texture{}, filePath(""),
                rotation(0.0f), scale(1.0f), xShift(0.0f), yShift(0.0f),
                textureHasBeenLoaded(false), ownsTexture(false), autoFits(true) {colour.SetColour(WHITE);}

    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;

    ~Sprite() {
        if (textureHasBeenLoaded && ownsTexture) {::UnloadTexture(texture);}
    }

    /**
     *  @brief Draws the texture.
     *  @details Wrapper over DrawTextureEx
     *  @see ::DrawTextureEx
     */
    void DrawMyself(float elapsedTime) const override;
    void DoFocusAction(float elapsedTime) override;

    /**
     *  @brief Sets the file path for later loading.
     *  @param path path to the file; file name
     *  @throws std::invalid_argument if path is empty
     */
    void SetFilePath(const std::string& path);
    std::string GetFilePath() const;
    void ClearFilePath();

    /**
     *  @brief Loads the texture from the stored path.
     *  @details Wrapper over raylib's LoadTexture. Assumes it's the primary source of the texture, responsible for its unloading.
     *  @see ::LoadTexture
     *  @throws std::runtime_error if the path is empty or if the texture did not load properly.
     */
    void LoadTexture();
    /**
     *  @brief Loads the texture from the parameter
     *  @details Wrapper over raylib's LoadTexture. Doesn't update the stored path. Assumes it's the primary source of the texture, responsible for its unloading.
     *  @param fileName path to the file
     *  @see ::LoadTexture
     *  @throws std::invalid_argument if fileName is empty
     *  @throws std::runtime_error if the texture did not load properly.
     */
    void LoadTexture(const std::string& fileName);
    /**
     *  @brief Unloads the texture
     *  @details Wrapper over raylib's UnloadTexture.
     *  @throws std::runtime_error if this Sprite doesn't own a texture - nothing loaded, or set with SetTexture.
     *  @see ::UnloadTexture()
     */
    void UnloadTexture();
    /**
     *  @brief Checks if the texture has loaded correctly.
     *  @details Wrapper over raylib's IsTextureValid.
     *  @returns true if is valid
     *  @see ::IsTextureValid()
     */
    bool IsTextureValid() const;

    void SetRotation(float rotation); ///< in degrees
    float GetRotation() const;

    /**
     *  @brief Sets the scale of the texture (shrinks or enlarges it).
     *  @param scale new scale
     *  @throws std::invalid_argument if scale is 0
     */
    void SetScale(float scale);
    float GetScale() const;

    /**
     *  @brief Sets the texture by another texture.
     *  @details Assumes the texture is stored elsewhere. Unload it there.
     *  @param texture new texture to be copied to this one.
     *  @throws std::invalid_argument if the new texture has not loaded properly.
     */
    void SetTexture(const Texture2D& texture);
    Texture2D GetTexture() const;
    void ClearTexture();

    /**
     *  @brief Changes dimensions to fit with those of the texture.
     *  @details Called automatically on loading or setting a texture, and on SetScale, while auto fitting is on.
     *  @throws std::runtime_error if there's no valid texture
     */
    void FitToTexture();
    /**
     *  @brief Turns automatic fitting to the texture's size on.
     *  @details Turned on by default.
     */
    void EnableAutoFitting();
    /**
     *  @brief Turns automatic fitting to the texture's size off.
     */
    void DisableAutoFitting();
    /**
     *  @brief Turns automatic fitting to the texture's size on or off.
     *  @details Turned on by default.
     *  @param value true for enabled auto fitting
     */
    void SetAutoFitting(bool value);
    /**
     *  @brief Checks whether auto fitting is on.
     *  @returns true if yes
     */
    bool IsAutoFitting() const;

    void    SetXShift(float shift);
    float   GetXShift() const;
    void    SetYShift(float shift);
    float   GetYShift() const;
    void    SetXYShift(float shift);
    void    SetXYShift(const Vector2& xyShift);
    Vector2 GetXYShift() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

private:
    Texture2D texture;
    std::string filePath;
    float rotation;
    float scale;
    float xShift; ///< Texture is drawn at Sprite's (x,y) (top-left corner) - use this to shift the drawing point from there
    float yShift;

    bool textureHasBeenLoaded;
    bool ownsTexture;
    bool autoFits;
};

}

#endif // ITEMS_H
