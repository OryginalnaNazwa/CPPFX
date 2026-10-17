#ifndef MIXINS_H
#define MIXINS_H

#include <functional>
#include <raylib.h>

#include "base_item.hpp"
#include "properties.hpp"


/**
 *  @file
 *  @brief Contains mixins that give widgets extra functionality.
 *  @details Circular, PersistentState, Padded and Embedded.
 */

namespace CPPFX {

/**
 *  @class Circular
 *  @brief Used for items in shape of a circle.
 *  @details Anchored at the centre or at the top-left corner. Width is the diameter.
 */
class Circular : public virtual Item {
public:

    /**
     *  @brief Checks whether the item was clicked.
     *  @details Uses circle collision check.
     *  @param mousePosition position of mouse during the click
     *  @returns true if the click was inside the circle.
     */
    virtual bool WasIClicked(const Vector2& mousePosition) const override;

    /**
     *  @brief Sets x,y as the centre of the circle.
     */
    void MakeRadiusBased();
    /**
     *  @brief Sets x,y as the top left corner of the circle.
     *  @details Circle inside a square of width by width. Default.
     */
    void MakeTopLeftBased();
    /**
     *  @brief Sets whether the x,y coordinates are of the circle's centre or top left corner.
     *  @param flag true - centre, false - top left
     */
    void SetCoordinatesBase(bool flag);
    /**
     *  @brief Checks whether x,y is centre of the circle.
     *  @returns true if it is
     */
    bool IsAnchoredAtCentre() const;

    /**
     *  @brief Gets x of the circle's centre.
     *  @details x unchanged if anchored at the centre, x + radius otherwise.
     *  @returns x coordinate of the circle's centre.
     */
    virtual float TranslateXToCentre() const;
    /**
     *  @brief Gets y of the circle's centre.
     *  @details y unchanged if anchored at the centre, y + radius otherwise.
     *  @returns y coordinate of the circle's centre.
     */
    virtual float TranslateYToCentre() const;
    /**
     *  @brief Gets x,y of the circle's centre.
     *  @details x,y unchanged if anchored at the centre, moved by radius otherwise.
     *  @returns x,y coordinates of the circle's centre.
     */
    virtual Vector2 TranslateXYToCentre() const;
    /**
     *  @brief Gets x of the circle's top-left corner.
     *  @details x unchanged if anchored at the corner, x - radius otherwise.
     *  @returns x coordinate of the circle's corner.
     */
    virtual float TranslateXToCorner() const;
    /**
     *  @brief Gets y of the circle's top-left corner.
     *  @details y unchanged if anchored at the corner, y - radius otherwise.
     *  @returns y coordinate of the circle's corner.
     */
    virtual float TranslateYToCorner() const;
    /**
     *  @brief Gets x,y of the circle's top-left corner.
     *  @details x,y unchanged if anchored at the corner, moved by radius otherwise.
     *  @returns x,y coordinates of the circle's corner.
     */
    virtual Vector2 TranslateXYToCorner() const;
    /**
     *  @brief Gets radius
     *  @details Radius is half of width.
     *  @returns radius
     */
    virtual float GetRadius() const;
    /**
     *  @brief Sets radius of the circle.
     *  @details Sets width and height to twice the radius.
     *  @param radius new radius
     *  @throws std::invalid_argument if radius is negative
     *  @note Prints a warning if radius is 0.
     */
    virtual void SetRadius(float radius);
    /**
     *  @brief Returns x of the opposite side.
     *  @returns x of the corner if in centre, x of the centre if in corner.
     *  @see TranslateXToCentre
     *  @see TranslateXToCorner
     */
    virtual float GetXTransformed() const;
    /**
     *  @brief Returns y of the opposite side.
     *  @returns y of the corner if in centre, y of the centre if in corner.
     *  @see TranslateYToCentre
     *  @see TranslateYToCorner
     */
    virtual float GetYTransformed() const;
    /**
     *  @brief Returns x,y of the opposite side.
     *  @returns x,y of the corner if in centre, x,y of the centre if in corner.
     *  @see TranslateXYToCentre
     *  @see TranslateXYToCorner
     */
    virtual Vector2 GetXYTransformed() const;

protected:
    /**
     *  @brief Constructor for circular items.
     *  @details Sets a custom border drawing method.
     *  @note Remember to draw the border first! It's drawn as a filled circle of
     *        radius + thickness, so draw it before the item itself.
     *  @see Border
     */
    Circular() : Item(), xyIsCentre(false) {
        border.SetDrawingMethod([this](float, float, float, float){
            DrawCircle(TranslateXToCentre(), TranslateYToCentre(), GetRadius() + border.GetThickness(), border.colour.GetColour());
        });
    }

    bool xyIsCentre; ///< whether the x,y coordinates represent the centre of the circle or the top-left corner
};

/**
 *  @class PersistentState
 *  @brief Used for widgets with persistent state, ones that remember whether they have been clicked, for example.
 */
class PersistentState : public virtual Item {
public:
    Colour pressedColour; ///< colour while pressed

    /**
     *  @brief Sets whether the item is pressed.
     *  @param flag true - pressed
     */
    void SetPressed(bool flag);
    /**
     *  @brief Sets pressed to true.
     */
    void Press();
    /**
     *  @brief Sets pressed to false.
     */
    void Unpress();
    /**
     *  @brief Toggles pressed.
     */
    void SwitchPress();
    /**
     *  @brief Checks whether the item is pressed.
     *  @returns true if yes.
     */
    bool IsPressed() const;

protected:
    /**
     *  @brief Sets pressed colour to GRAY (as darker default colour)
     */
    PersistentState() : Item(), pressedColour(GRAY), pressed(false) {}

    bool pressed; ///< whether the item is pressed.
};

/**
 *  @class Padded
 *  @brief Gives the padding functionality, used for spacing children inside an item.
 */
class Padded : public virtual Item { // this doesn't need to be derived from Item - should be for simplicity, or decide per mixin?
public:

    /**
     *  @brief Sets padding.
     *  @param value new padding value
     *  @throws std::invalid_argument if the padding is negative
     */
    virtual void SetPadding(float value);
    /**
     *  @brief Gets padding.
     *  @returns padding
     */
    float GetPadding() const;

protected:
    /**
     *  @brief Default padding is 10.
     */
    Padded() : Item(), padding(10.0f) {}

    float padding; ///< distance between children
};

/**
 *  @class Embedded
 *  @brief For embedded widgets forming composite widgets.
 *  @details Hides unsafe Item methods as protected. The overrides only forward to Item -
 *           they exist to restrict access. Uses friends to limit boilerplate in the
 *           composites using the restricted methods.
 *  @note Doesn't hide them fully, a C++ limitation: they can still be called with the
 *        Item:: prefix, or through an Item pointer or reference, since access is checked
 *        against the static type. It stops accidental misuse and that's important.
 */
class Embedded : public virtual Item {
public:
    friend class Spinner;
    friend class EditableSpinner;
    friend class Button;
    friend class ComboBox;
    friend class SearchableDropDown;

protected:

    Embedded() : Item() {}

    void SetX(float x) override { Item::SetX(x); }
    void SetY(float y) override { Item::SetY(y); }
    void SetXY(float x, float y) override { Item::SetXY(x, y); }
    void SetXY(const Vector2& xy) override { Item::SetXY(xy); }
    void SetXY(float xy) override { Item::SetXY(xy); }
    void SetHeight(float value) override { Item::SetHeight(value); }
    void SetWidth(float value) override { Item::SetWidth(value); }
    void SetDimensions(float width, float height) override { Item::SetDimensions(width, height); }
    void SetDimensions(const Vector2& dimensions) override { Item::SetDimensions(dimensions); }
    void SetDimensions(float value) override { Item::SetDimensions(value); }
    void SetToScreen() override { Item::SetToScreen(); }
    void SetToWorld() override { Item::SetToWorld(); }
    void ConsumeClicks() override { Item::ConsumeClicks(); }
    void LetClicksThrough() override { Item::LetClicksThrough(); }
    bool DoesEatClicks() const override { return Item::DoesEatClicks(); }
    void SetID(const std::string& id) override { Item::SetID(id); }
    std::string GetID() const override { return Item::GetID(); }
    void SetPriority(int value) override { Item::SetPriority(value); }
    void MoveUpPriority() override { Item::MoveUpPriority(); }
    void MoveDownPriority() override { Item::MoveDownPriority(); }
    size_t GetPriority() const override { return Item::GetPriority(); }
};

}

#endif // MIXINS_H
