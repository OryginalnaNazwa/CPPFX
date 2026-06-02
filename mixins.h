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

    Circular() : Item(), xyIsCentre(false) {
        border.SetDrawingMethod([this](float x, float y, float w, float h){
            DrawCircle(x, y, w + border.GetThickness(), border.colour.GetColour());
        });
    }

    virtual bool WasIClicked(const Vector2& mousePosition) const override;
    virtual bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const override;

    void MakeRadiusBased();
    void MakeTopLeftBased();
    void SetCoordinatesBase(bool flag);
    bool IsAnchoredAtCentre() const;

    virtual float TranslateXToCentre() const;
    virtual float TranslateYToCentre() const;
    virtual Vector2 TranslateXYToCentre() const;
    virtual float TranslateXToCorner() const;
    virtual float TranslateYToCorner() const;
    virtual Vector2 TranslateXYToCorner() const;
    virtual float GetRadius() const;
    virtual float GetXTransformed() const;
    virtual float GetYTransformed() const;
    virtual Vector2 GetXYTransformed() const;


protected:
    bool xyIsCentre;
};

class PersistentState : public virtual Item {
public:

    PersistentState() : Item(), pressed(false) {}

    void SetPressed(const bool& flag);
    void Press();
    void Unpress();
    void SwitchPress();
    bool IsPressed() const;

protected:
    bool pressed;
};

class EmbeddedTextField;
class Padded : public virtual Item {
public:

    Padded() : Item(), padding(10) {}

    /**
     *  @brief Sets padding.
     *  @details Flags for rearrangement.
     *  @param value new padding value
     */
    virtual void SetPadding(float value);
    float GetPadding() const;

protected:
    float padding;
};

/*class DoesPromptText : public virtual Item {
public:

    DoesPromptText() : promptText("") {}
};*/

}

#endif // MIXINS_H
