#ifndef BASES_H
#define BASES_H

#include "mixins.h"

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

    TextItem(const std::string& i) : Item(i), font(height / 4), text(i), textMargin(10) {}
    TextItem(const std::string& i, float w, float h) : Item(i, w, h), font(h / 4), text(i), textMargin(10) {}

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
    void SetTextMargin(float margin);
    /**
     *  @brief Returns text margin
     *  @returns text margin
     */
    float GetTextMargin() const;

protected:
    std::string text;
    float textMargin; ///<distance from limit to text.

    /**
     *  @brief Truncates text if needed.
     *  @return truncated string
     */
    virtual std::string Truncate(const std::string& text) const;
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

    Container(const std::string& i) : Item(i) {colour.SetColour(BLANK); priority = 10;}
    Container(const std::string& i, float w, float h) : Item(i, w, h) {colour.SetColour(BLANK); priority = 10;}

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

    /**
     *  @brief Arranges the items.
     *  @details Called automatically on the first frame after getting dirty.
     */
    virtual void SetPositionsOfItems() = 0;

    virtual void DoPassiveAction(float elapsedTime) override; ///< dirty sort and arrangement
    virtual void DrawMyself(float elapsedTime) const override; ///< just the border drawing and background drawing.
    virtual void DoFocusAction(float elapsedTime) override; ///< nothing, defocuses itself.

    /**
     *  @brief Sorts the drawing order by priority.
     *  @details Called automatically on the first frame after getting dirty.
     */
    void SortOrder();

    /**
     *  @brief Checks whether the item of this ID is already a child of the container.
     *  @param ID ID of the item to be checked.
     *  @returns true if the item already is in the container.
     */
    bool IsIDTaken(const std::string& ID) const;

protected:
    std::map<std::string, Item*> Items;
    std::vector<Item*> ItemsInDrawingOrder;

    bool needsSorting = false; ///< dirty sord of z-order
    bool needsOrdering = false; ///< dirty arrangement of children
};

/**
 *  @class Box
 *  @brief A virtual class for later boxes.
 *  @details Adds padding and sets display.
 */
class Box : public Container, public virtual Padded {
public:

    Box(const std::string& i) : Container(i), Padded() {}
    Box(const std::string& i, float w, float h) : Container(i, w, h), Padded() {}

    /**
     *  @brief Sets padding.
     *  @details Flags for rearrangement.
     *  @param value new padding value
     *  @throws std::invalid_argument if the padding is negative
     */
    void SetPadding(float value) override;

};

class Chart : public virtual Item {
public:
    Font font;


    Chart (const std::string& i) : Item(i), font(height / 4) {}

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

protected:
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


    Shape(const std::string& i) : Item(i) {}

    /**
     *  @brief Calculates area.
     *  @returns Area of the shape.
     */
    virtual float CalculateMyArea() const = 0;
    void DoFocusAction(float elapsedTime) override; ///< does nothing
};

}

#endif // BASES_H
