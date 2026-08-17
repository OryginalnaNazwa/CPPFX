#ifndef BASES_H
#define BASES_H

#include <stddef.h>      // for size_t
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <string>        // for string
#include <vector>        // for vector
#include "base_item.hpp"   // for Item
#include "mixins.hpp"      // for Padded
#include "properties.hpp"  // for Colour, Font, Alignment
#include "raylib.h"      // for BLANK

/**********************************************************************
 *  @file base.h
 *  @brief This file contains base virtual classes for other widgets.
 **********************************************************************/

namespace CPPFX {

/**
 *  @class TextItem
 *  @brief A virtual Item subclass focused on a variable text.
 */
class TextItem : public virtual Item {
public:
    Font font;

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
     *  @brief Sets the distance between border and text in x axis.
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
     *  @brief Makes the item widen itself to fit its text every frame.
     *  @details Only ever grows - the item never shrinks back once the text gets shorter.
     */
    void ExpandToTextAutomatically();
    /**
     *  @brief Stops the automatic expansion.
     *  @details Text too long for the current dimensions gets truncated instead. Default.
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
     *  @param text text to draw
     *  @param font font to draw it with
     *  @returns The string actually drawn, truncated or not.
     */
    virtual std::string DrawAlignedText(const Alignment& alignment,
                                        const std::string& text, const CPPFX::Font& font) const;
    /**
     *  @brief Draws this item's text within an arbitrary box.
     *  @returns The string actually drawn, truncated or not.
     */
    virtual std::string DrawAlignedText(const Alignment& alignment,
                                        float x, float y, float width, float height) const;
    /**
     *  @brief Draws this item's text within its own box.
     *  @details The usual one. Box is the item's anchors and dimensions, minus
     *           text margin on every side.
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
     *  @note This one should be overridden, others rout to it.
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
     *  @note Pointless here, just a reminder to put it in inherited classes.
     */
    static const std::string GetClassID();

protected:
    TextItem(const std::string& i) : Item(i), font(height / 4.0f), text(i), textMargin(10.0f), expandsToTextAutomatically(true) {}
    TextItem(const std::string& i, float w, float h) : Item(i, w, h), font(h / 4.0f), text(i), textMargin(10.0f), expandsToTextAutomatically(true) {}

    std::string text;
    float textMargin; ///< distance from limit to text.
    bool expandsToTextAutomatically; ///< whether it expands its dimensions to the text's.
    bool truncates = true;
};

/**
 * @class Container
 * @brief A virtual layout organiser base.
 * @details Has border. Has alignement. Has colour - blank by default. Containers have default priority of 10 (makes sure they're under most items so they don't interfere).
 * @note Containers mostly just set X and Ys of their children.
 */
class Container : public virtual Item {
public:
    Alignment alignment;

    /**
     *  @brief Adds item to the container.
     *  @param item pointer to the item to be added
     *  @throw std::invalid_argument if the pointer is null or if the item already is in the container
     */
    void AddItem(Item* item);

    /**
     *  @brief Removes an item from the container.
     *  @param ID ID of the item to be removed
     *  @throws std::invalid_argument if the ID is not in the container
     */
    void RemoveItem(const std::string& ID);
    /**
     *  @brief Removes an item from the container.
     *  @param item pointer to the item to be removed
     *  @throws std::invalid_argument if the item doesn't exist
     *  @throws std::out_of_range if the item is not in the container.
     */
    void RemoveItem(const Item* item);

    /**
     *  @brief Removes an item from the container without throwing an error.
     *  @param ID ID of the item to be removed
     */
    void SafeRemoveItem(const std::string& ID);
    /**
     *  @brief Removes an item from the container without throwing an error.
     *  @param item pointer to the item to be removed
     */
    void SafeRemoveItem(const Item* item);

    virtual void SetX(float x) override;
    virtual void SetY(float y) override;
    virtual void SetWidth(float value) override;
    virtual void SetHeight(float value) override;

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

    /**
     *  @brief Arranges the items.
     *  @details Called automatically on the first frame after getting dirty.
     */
    virtual void SetPositionsOfItems() = 0;

    /**
     *  @brief Manually stop sorting order.
     */
    virtual void DoNotOrder();
    /**
     *  @brief Manually flag for ordering.
     */
    virtual void Order();

    virtual void DoPassiveAction(float elapsedTime) override; ///< dirty sort and arrangement
    virtual void DrawMyself(float elapsedTime) const override; ///< just the border drawing and background drawing.
    virtual void DoFocusAction(float elapsedTime) override; ///< nothing, defocuses itself.

    /**
     *  @brief Checks whether the item of this ID is already a child of the container.
     *  @param ID ID of the item to be checked.
     *  @returns true if the item already is in the container.
     */
    bool IsIDTaken(const std::string& ID) const;

    /**
     *  @brief Grows width and height to fit children.
     *  @details Automatically called while setting positions of items. Does nothing if the container is empty.
     */
    void ExpandToChildren();
    /**
     *  @brief Grows or shrinks width and height to fit children. Does nothing if the container is empty.
     */
    void FitToChildren();

    /**
     *  @see Item::GetClassID()
     *  @note Pointless here, just a reminder to put it in inherited classes.
     */
    static const std::string GetClassID();

protected:
    Container(const std::string& i) : Item(i) {colour.SetColour(BLANK); priority = 10;}
    Container(const std::string& i, float w, float h) : Item(i, w, h) {colour.SetColour(BLANK); priority = 10;}

    std::unordered_map<std::string, Item*> Items;
    std::vector<Item*> ItemsInDrawingOrder;

    bool needsOrdering = false; ///< dirty arrangement of children


    bool IsContainer(const std::string& fxID) const;
    static const std::unordered_set<std::string> CONTAINERS;
};

/**
 *  @class Box
 *  @brief A virtual class for later boxes.
 *  @details Adds padding and sets display.
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
     *  @brief Returns the sum of appropriate dimensions of the contents.
     */
    virtual float GetChildrenTotalLength() const = 0;

    /**
     *  @see Item::GetClassID()
     *  @note Pointless here, just a reminder to put it in inherited classes.
     */
    static const std::string GetClassID();

protected:
    Box(const std::string& i) : Padded(), Container(i)  {}
    Box(const std::string& i, float w, float h) : Padded(), Container(i, w, h)  {}
};

/**
 *  @class Chart
 *  @brief A virtual class for charts and data analysis tools.
 *  @details Does the basic element and label work.
 */
class Chart : public virtual Item {
public:
    Font font;

    void AddElement(const std::string& label, double value);
    void AddElement(double value);

    double GetElement(const std::string& label) const;
    double GetElement(int index) const;

    void SetElement(const std::string& label, double newValue);
    void SetElement(int index, double newValue);

    void RemoveElement(const std::string& label);
    void RemoveElement(int index);

    size_t GetLabelsSize() const;
    size_t GetValuesSize() const;

    /**
     *  @see Item::GetClassID()
     *  @note Pointless here, just a reminder to put it in inherited classes.
     */
    static const std::string GetClassID();

protected:
    Chart (const std::string& i) : Item(i), font(height / 4.0f) {}

    std::vector<std::string> labels;
    std::vector<double> values;
};

/**
 *  @class Shape
 *  @brief Virtual class for simple shapes objects.
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
     *  @note Pointless here, just a reminder to put it in inherited classes.
     */
    static const std::string GetClassID();

protected:
    Shape(const std::string& i) : Item(i) {}
};

}

#endif // BASES_H
