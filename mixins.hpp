#ifndef MIXINS_H
#define MIXINS_H

#include <functional>    // for function
#include "base_item.hpp"   // for Item
#include "properties.hpp"  // for Border, Colour, GREY
#include "raylib.h"      // for Vector2, DrawCircle, Camera2D

/**************************************************************************************
 *  @file mixins.h
 *  @brief This file contains mixins used to give widgets appropriate functionalities.
 **************************************************************************************/

namespace CPPFX {
class EditableSpinner; //forward declare for Embedded

/**
 *  @brief Used for items in shape of a circle.
 *  @details Radius-based or top left based. Width is radius.
 */
class Circular : public virtual Item {
public:

    /**
     *  @brief Checks whether the item was clicked.
     *  @details Uses circle collision check.
     */
    virtual bool WasIClicked(const Vector2& mousePosition) const override;

    /**
     *  @brief Sets x,y as the centre of the circle.
     */
    void MakeRadiusBased();
    /**
     *  @brief Sets x,y as the top left corner of the circle.
     *  @details Circle inside a square of width by width.
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
     *  @brief Gives x if it was moved to the centre.
     *  @details returns just x if it is centre, moves by radius otherwise.
     *  @returns x coordinate of the circle's centre.
     */
    virtual float TranslateXToCentre() const;
    /**
     *  @brief Gives y if it was moved to the centre.
     *  @details returns just y if it is centre, moves by radius otherwise.
     *  @returns y coordinate of the circle's centre.
     */
    virtual float TranslateYToCentre() const;
    /**
     *  @brief Gives x,y if it was moved to the centre.
     *  @details returns just x,y if it is centre, moves by radius otherwise.
     *  @returns x,y coordinate of the circle's centre.
     */
    virtual Vector2 TranslateXYToCentre() const;
    /**
     *  @brief Gives x if it was moved to the corner.
     *  @details returns just x if it is corner, moves by radius otherwise.
     *  @returns x coordinate of the circle's corner.
     */
    virtual float TranslateXToCorner() const;
    /**
     *  @brief Gives y if it was moved to the corner.
     *  @details returns just y if it is corner, moves by radius otherwise.
     *  @returns y coordinate of the circle's corner.
     */
    virtual float TranslateYToCorner() const;
    /**
     *  @brief Gives x,y if it was moved to the corner.
     *  @details returns just x,y if it is corner, moves by radius otherwise.
     *  @returns x,y coordinate of the circle's corner.
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
     *  @details Sets width to twice the radius.
     *  @param radius new radius
     *  @throws std::invalid_argument if radius is negative
     *  @warning if radius is 0.
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
     *  @note Rembember to put the border drawing first! It's just a solid circle.
     *  @see CPPFX::Border in properties.h
     */
    Circular() : Item(), xyIsCentre(false) {
        border.SetDrawingMethod([this](float x, float y, float w, float h){
            DrawCircle(TranslateXToCentre(), TranslateYToCentre(), GetRadius() + border.GetThickness(), border.colour.GetColour());
        });
    }

    bool xyIsCentre; ///< whether the x,y coordinates represent the centre of the circle or the top-left corner
};

/**
 *  @brief Used for widgets with persistent state, ones that remember whether they have been clicked, for example.
 */
class PersistentState : public virtual Item {
public:
    Colour pressedColour; ///< colour after press

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
     *  @brief Sets pressed to the opposite logical state to the current one.
     */
    void SwitchPress();
    /**
     *  @brief Checks whether the item is pressed.
     *  @returns true if yes.
     */
    bool IsPressed() const;

protected:
    /**
     *  @brief Sets pressed colour to GREY (as darker default colour)
     */
    PersistentState() : Item(), pressedColour(GREY), pressed(false) {}

    bool pressed; ///< whether the item is clicked.
};

/**
 *  @brief Gives the padding functionality, used for spacing children inside an item.
 */
class Padded : public virtual Item {
public:

    /**
     *  @brief Sets padding.
     *  @param value new padding value
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
 *  @details Hides unsafe Item methods into private. These aren't actually overrides, they just call the original method - it's done for explicit privateness only. Uses friends to limit boilerplate in the constructors using the methods made private.
 *  @note Doesn't hide them fully actually, a bit of a C++ limitation (out of the solutions I found), these methods can still be called using Item:: prefix. But at that point you're actively trying to break things. It stops accidental misuse and that's important.
 */
class Embedded : public virtual Item {
public:
    friend class Spinner;
    friend class EditableSpinner;
    friend class Button;

protected:

    Embedded() : Item() {}

private:
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

/*class DoesPromptText : public virtual Item {
public:

    DoesPromptText() : promptText("") {}
};*/ //just one item does it (for now), no need to abstract it

}

#endif // MIXINS_H
