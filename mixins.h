#ifndef MIXINS_H
#define MIXINS_H

#include "base_item.h"

/**************************************************************************************
 *  @file mixins.h
 *  @brief This file contains mixins used to give widgets appropriate functionalities.
 **************************************************************************************/

namespace CPPFX {

/**
 *  @brief Used for items in shape of a circle.
 *  @details Radius-based or top left based. Width is radius.
 */
class Circular : public virtual Item {
public:

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

    /**
     *  @brief Checks whether the item was clicked.
     *  @details Uses circle collision check.
     */
    virtual bool WasIClicked(const Vector2& mousePosition) const override;
    /**
     *  @brief Checks whether the item was clicked in screen space.
     *  @details Uses circle collision check.
     */
    virtual bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const override;

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
    bool xyIsCentre; ///< whether the x,y coordinates represent the centre of the circle or the top-left corner
};

/**
 *  @brief Used for widgets with persistent state, ones that remember whether they have been clicked, for example.
 */
class PersistentState : public virtual Item {
public:
    Colour pressedColour; ///< colour after press

    /**
     *  @brief Sets pressed colour to GREY (as darker default colour)
     */
    PersistentState() : Item(), pressed(false) {pressedColour.SetColour(GREY);}

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
    bool pressed; ///< whether the item is clicked.
};

//class EmbeddedTextField;

/**
 *  @brief Gives the padding functionality, used for spacing children inside an item.
 */
class Padded : public virtual Item {
public:

    /**
     *  @brief Default padding is 10.
     */
    Padded() : Item(), padding(10) {}

    /**
     *  @brief Sets padding.
     *  @param value new padding value
     */
    virtual void SetPadding(float value);
    float GetPadding() const;

protected:
    float padding; ///< distance between children
};

/*class DoesPromptText : public virtual Item {
public:

    DoesPromptText() : promptText("") {}
};*/

}

#endif // MIXINS_H
