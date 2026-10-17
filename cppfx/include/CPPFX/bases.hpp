#ifndef BASES_H
#define BASES_H

#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <raylib.h>

#include "base_item.hpp"
#include "mixins.hpp"
#include "properties.hpp"


/**
 *  @file
 *  @brief Contains base classes for other widgets.
 *  @details TextItem, Container, Box, Chart and Shape.
 */

namespace CPPFX {

/**
 *  @class TextItem
 *  @brief Base for widgets that display text.
 */
class TextItem : public virtual Item {
public:
    Font font;

    /**
     *  @brief Expands the width and height to accommodate text.
     *  @details Only grows. Includes the text margin on every side.
     */
    virtual void ExpandToText();

    /**
     *  @brief Expands or shrinks width and height to the text dimensions.
     *  @details Includes the text margin on every side.
     */
    virtual void FitToText();

    /**
     *  @brief Sets text
     *  @details Expands the item if it expands to text automatically.
     *  @param text new text to be set
     */
    virtual void SetText(const std::string& text);
    /**
     *  @brief Clears text.
     *  @details Sets text to an empty std::string.
     */
    virtual void ClearText();
    /**
     *  @brief Returns text.
     *  @returns text
     */
    virtual std::string GetText() const;
    /**
     *  @brief Checks whether the text is empty.
     *  @returns true if text is an empty string
     */
    virtual bool IsEmpty() const;

    /**
     *  @brief Sets the distance between border and text in x axis.
     *  @details Expands the item if it expands to text automatically.
     *  @param margin new value
     *  @throws std::invalid_argument if margin is negative.
     */
    virtual void SetTextMargin(float margin);
    /**
     *  @brief Returns text margin
     *  @returns text margin
     */
    virtual float GetTextMargin() const;

    /**
     *  @brief Makes the item grow to fit its text whenever the text or text margin changes.
     *  @details Only ever grows - the item never shrinks back once the text gets shorter. Default.
     */
    void ExpandToTextAutomatically();
    /**
     *  @brief Stops the automatic expansion.
     *  @details Text too long for the current dimensions gets truncated instead.
     */
    void DoNotExpandToTextAutomatically();
    /**
     *  @brief Sets whether the item expands to its text on its own.
     *  @param should true - expands, false - truncates
     */
    void ShouldExpandToTextAutomatically(bool should);
    /**
     *  @brief Checks whether the item expands to its text on its own.
     *  @returns true if expanding automatically
     */
    bool IsExpandingToTextAutomatically() const;

    /**
     *  @brief Draws text aligned within an arbitrary box.
     *  @details The raw version: everything is a parameter. Truncates first if
     *           the item truncates, so what gets measured for placement is what
     *           actually gets drawn. Use it for text outside the item's own
     *           bounds, such as CheckBox labels.
     *
     *           Alignment is a parameter rather than a member because it means
     *           different things in different widgets - within the box for a
     *           Label, but which side of the box for a CheckBox.
     *  @param alignment where to place the text within the box
     *  @param text text to draw
     *  @param font font to draw it with
     *  @param x left edge of the box
     *  @param y top edge of the box
     *  @param width width of the box
     *  @param height height of the box
     *  @returns The string actually drawn, truncated or not.
     *  @see Alignment::GetAlignedXY
     *  @see Font::DrawTextAt
     */
    virtual std::string DrawAlignedText(const Alignment& alignment,
                                        const std::string& text, const CPPFX::Font& font,
                                        float x, float y, float width, float height) const;
    /**
     *  @brief Draws given text within this item's box.
     *  @details For a second text and font, such as TextField's prompt.
     *  @param alignment where to place the text within the box
     *  @param text text to draw
     *  @param font font to draw it with
     *  @returns The string actually drawn, truncated or not.
     */
    virtual std::string DrawAlignedText(const Alignment& alignment,
                                        const std::string& text, const CPPFX::Font& font) const;
    /**
     *  @overload
     *  @brief Draws this item's text within an arbitrary box.
     */
    virtual std::string DrawAlignedText(const Alignment& alignment,
                                        float x, float y, float width, float height) const;
    /**
     *  @brief Draws this item's text within its own box.
     *  @details The usual one. Box is the item's anchors and dimensions, minus
     *           text margin on every side.
     *  @param alignment where to place the text within the box
     *  @returns The string actually drawn, truncated or not.
     */
    virtual std::string DrawAlignedText(const Alignment& alignment) const;

    /**
     *  @brief Makes the item cut text too long for its box.
     *  @details Default. Cut text ends in an ellipsis.
     */
    void DoTruncate();
    /**
     *  @brief Stops the item cutting text too long for its box.
     *  @details Text overflows the box instead.
     */
    void DoNotTruncate();
    /**
     *  @brief Sets whether the item cuts text too long for its box.
     *  @param doTruncation true - truncates, false - overflows
     */
    void DoTruncation(bool doTruncation);
    /**
     *  @brief Checks whether the item cuts text too long for its box.
     *  @returns true if truncating
     */
    bool DoesTruncate() const;

    /**
     *  @brief Cuts text to fit a width, ending it with an ellipsis.
     *  @details Walks whole codepoints, so multi-byte characters are never
     *           split in half. Returns the text unchanged if it already fits.
     *  @param text text to cut
     *  @param font font it will be drawn with; different fonts fit differently
     *  @param maxWidth width to fit into
     *  @returns The text, cut and suffixed with an ellipsis if it did not fit.
     *  @see Font::MeasureTextWidth
     *  @note This one should be overridden, others route to it.
     */
    virtual std::string Truncate(const std::string& text, const CPPFX::Font& font, float maxWidth) const;
    /**
     *  @brief Cuts text to fit this item's box.
     *  @param font font it will be drawn with
     */
    virtual std::string Truncate(const std::string& text, const CPPFX::Font& font) const;
    /**
     *  @brief Cuts text to fit a width, using this item's font.
     *  @param maxWidth width to fit into
     */
    virtual std::string Truncate(const std::string& text, float maxWidth) const;
    /**
     *  @brief Cuts text to fit this item's box, using this item's font.
     */
    virtual std::string Truncate(const std::string& text) const;
    /**
     *  @brief Cuts this item's own text to fit its own box.
     */
    virtual std::string Truncate() const; // optional overflow message?

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    explicit TextItem(const std::string& i) : Item(i), font(height / 4.0f), text(i), textMargin(10.0f), expandsToTextAutomatically(true) {}
    TextItem(const std::string& i, float w, float h) : Item(i, w, h), font(h / 4.0f), text(i), textMargin(10.0f), expandsToTextAutomatically(true) {}

    std::string text;                ///< displayed text
    float textMargin;                ///< distance from the item's edge to the text, on every side
    bool expandsToTextAutomatically; ///< whether it expands its dimensions to the text's
    bool truncates = true;           ///< whether text too long for the box is cut
};

/**
 * @class Container
 * @brief Base for layout containers.
 * @details Has border. Has alignment. Has colour - blank by default. Containers have default priority of 10 (makes sure they're under most items so they don't interfere).
 * @note Containers mostly just set X and Ys of their children.
 */
class Container : public virtual Item {
public:
    Alignment alignment;

    /**
     *  @brief Adds item to the container.
     *  @details Doesn't take ownership - the item stays owned by GUI.
     *  @param item pointer to the item to be added
     *  @throws std::invalid_argument if the pointer is null or an item with the same ID is already in the container
     */
    void AddItem(Item* item);

    /**
     *  @brief Removes an item from the container.
     *  @details Doesn't destroy the item.
     *  @param ID ID of the item to be removed
     *  @throws std::out_of_range if the ID is not in the container
     */
    void RemoveItem(const std::string& ID);
    /**
     *  @brief Removes an item from the container.
     *  @details Doesn't destroy the item.
     *  @param item pointer to the item to be removed
     *  @throws std::invalid_argument if the pointer is null
     *  @throws std::out_of_range if the item is not in the container.
     */
    void RemoveItem(const Item* item);

    /**
     *  @brief Removes an item from the container.
     *  @details Does nothing if the item isn't in the container.
     *  @param ID ID of the item to be removed
     */
    void SafeRemoveItem(const std::string& ID);
    /**
     *  @brief Removes an item from the container.
     *  @details Does nothing if the item isn't in the container or the pointer is null.
     *  @param item pointer to the item to be removed
     */
    void SafeRemoveItem(const Item* item);

    /// @name Geometry
    /// As in Item, and flags the container for rearrangement.
    ///@{
    virtual void SetX(float x) override;
    virtual void SetY(float y) override;
    virtual void SetWidth(float value) override;
    virtual void SetHeight(float value) override;
    ///@}

    /// @name State
    /// As in Item, and applies to every child as well.
    ///@{
    virtual void MakeInvisible() override;
    virtual void MakeVisible() override;
    virtual void SetVisible(bool flag) override;
    virtual void MakeActive() override;
    virtual void MakeInactive() override;
    virtual void SetInactive(bool flag) override;
    virtual void Show() override;
    virtual void Hide() override;
    virtual void ConsumeClicks() override;
    virtual void LetClicksThrough() override;
    virtual void SetToWorld() override;
    virtual void SetToScreen() override;
    ///@}

    /**
     *  @brief Arranges the items.
     *  @details Called automatically on the first frame after getting dirty.
     */
    virtual void SetPositionsOfItems() = 0;

    /**
     *  @brief Clears the rearrangement flag.
     */
    virtual void DoNotOrder();
    /**
     *  @brief Flags the container for rearrangement on the next frame.
     */
    virtual void Order();

    virtual void DoPassiveAction(float elapsedTime) override; ///< rearranges children if flagged
    virtual void DrawMyself(float elapsedTime) const override; ///< draws background and border
    virtual void DoFocusAction(float elapsedTime) override; ///< defocuses itself

    /**
     *  @brief Checks whether the item of this ID is already a child of the container.
     *  @param ID ID of the item to be checked.
     *  @returns true if the item already is in the container.
     */
    bool IsIDTaken(const std::string& ID) const;

    /**
     *  @brief Grows width and height to fit children.
     *  @details Grows from the top-left anchor only. VBox and HBox call it after
     *           arranging their children. Does nothing if the container is empty.
     */
    void ExpandToChildren();
    /**
     *  @brief Grows or shrinks width and height to fit children.
     *  @details Resizes from the top-left anchor only. Does nothing if the container is empty.
     */
    void FitToChildren();

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    explicit Container(const std::string& i) : Item(i) {colour.SetColour(BLANK); priority = 10;}
    Container(const std::string& i, float w, float h) : Item(i, w, h) {colour.SetColour(BLANK); priority = 10;}

    std::unordered_map<std::string, Item*> Items; ///< children by ID
    std::vector<Item*> ItemsInDrawingOrder;        ///< children in the order they are arranged

    bool needsOrdering = false; ///< dirty arrangement of children

    /// Checks whether the fxID belongs to a container type.
    static bool IsContainer(const std::string& fxID);
    static const std::unordered_set<std::string> CONTAINERS; ///< fxIDs of container types
};

/**
 *  @class Box
 *  @brief Base for VBox and HBox.
 *  @details Adds padding; changing it flags rearrangement.
 */
class Box : public Container, public virtual Padded {
public:

    /**
     *  @brief Sets padding.
     *  @details Flags for rearrangement.
     *  @param value new padding value
     *  @throws std::invalid_argument if the padding is negative
     */
    void SetPadding(float value) override;

    /**
     *  @brief Sums the children's total lengths along the box's axis.
     *  @returns The sum, plus padding after each child.
     */
    virtual float GetChildrenTotalLength() const = 0;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    explicit Box(const std::string& i) : Padded(), Container(i)  {}
    Box(const std::string& i, float w, float h) : Padded(), Container(i, w, h)  {}
};

/**
 *  @class Chart
 *  @brief Base for charts and data analysis tools.
 *  @details Holds values, each with an optional label.
 */
class Chart : public virtual Item {
public:
    Font font;

    /**
     *  @brief Adds a labelled value.
     *  @param label label of the value
     *  @param value value to be added
     */
    void AddElement(const std::string& label, double value);
    /**
     *  @brief Adds a value without a label.
     *  @details Stored with an empty label.
     *  @param value value to be added
     */
    void AddElement(double value);

    /**
     *  @brief Gets the value under a label.
     *  @param label label of the value
     *  @returns The value.
     *  @throws std::out_of_range if there's no such label
     */
    double GetElement(const std::string& label) const;
    /**
     *  @brief Gets the value at an index.
     *  @param index index of the value
     *  @returns The value.
     *  @throws std::out_of_range if the index is invalid
     */
    double GetElement(int index) const;

    /**
     *  @brief Replaces the value under a label.
     *  @param label label of the value
     *  @param newValue value to be set
     *  @throws std::out_of_range if there's no such label
     */
    void SetElement(const std::string& label, double newValue);
    /**
     *  @brief Replaces the value at an index.
     *  @param index index of the value
     *  @param newValue value to be set
     *  @throws std::out_of_range if the index is invalid
     */
    void SetElement(int index, double newValue);

    /**
     *  @brief Removes the value under a label, together with the label.
     *  @param label label of the value
     *  @throws std::out_of_range if there's no such label
     */
    void RemoveElement(const std::string& label);
    /**
     *  @brief Removes the value at an index, together with its label.
     *  @param index index of the value
     *  @throws std::out_of_range if the index is invalid
     */
    void RemoveElement(int index);

    /**
     *  @brief Gets the number of labels, empty ones included.
     *  @returns Number of labels.
     */
    size_t GetLabelsSize() const;
    /**
     *  @brief Gets the number of values.
     *  @returns Number of values.
     */
    size_t GetValuesSize() const;

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    explicit Chart(const std::string& i) : Item(i), font(height / 4.0f) {}

    std::vector<std::string> labels; ///< labels, index-matched with values
    std::vector<double> values;      ///< values, index-matched with labels
};

/**
 *  @class Shape
 *  @brief Base for simple shapes.
 *  @details Made to wrap over raylib's Draw{Shape} functions. Has border.
 */
class Shape : public virtual Item {
public:

    /**
     *  @brief Calculates area.
     *  @returns Area of the shape.
     */
    virtual float CalculateMyArea() const = 0;
    void DoFocusAction(float elapsedTime) override; ///< does nothing

    /**
     *  @see Item::GetClassID()
     */
    static const std::string GetClassID();

protected:
    explicit Shape(const std::string& i) : Item(i) {}
};

}

#endif // BASES_H
