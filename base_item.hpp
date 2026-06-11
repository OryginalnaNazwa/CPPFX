#ifndef ITEM_H
#define ITEM_H

#include "properties.hpp"
#include <stddef.h>      // for size_t
#include <functional>    // for function
#include <string>        // for allocator, string
#include "raylib.h"      // for Vector2, Camera2D

namespace CPPFX {

/**
 *  @class Item
 *  @brief Virtual widget class.
 */
class Item {
public:
    Colour colour;
    Border border; //all but a few items have it.

    Item() {}
    /**
     *  @brief Default constructor.
     *  @param i fxID of the new item.
     */
    Item(const std::string& i)
        : ID(""), xAnchor(0.0f), yAnchor(0.0f), height(100.0f), width(200.0f), focused(false), visible(true), inactive(false), eatsClick(true), priority(3), screenBased(false), fxID(i) {}
    /**
     *  @brief Constructor for items with different dimensions than the default ones.
     *  @param i fxID of the new item.
     *  @param w width
     *  @param h height
     */
    Item(const std::string& i, float w, float h)
        : ID(""), xAnchor(0.0f), yAnchor(0.0f), height(h), width(w), focused(false), visible(true), inactive(false), eatsClick(true), priority(3), screenBased(false), fxID(i) {}

    virtual ~Item() = default;

    /**
     *  @brief Draws the item in world coordinates.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DrawMyself(float elapsedTime) const = 0;
    /**
     *  @brief Draws the item in screen coordinates.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DrawMyself(float elapsedTime, const Camera2D& camera) const;
    /**
     *  @brief Checks whether the item was clicked in screen coordinates.
     *  @details Checks whether the mouse position is within the area of the item.
     *  @param mousePosition position of mouse during the click
     *  @returns true if was clicked.
     */
    virtual bool WasIClicked(const Vector2& mousePosition) const; //screen
    /**
     *  @brief Checks whether the item was clicked in world coordinates.
     *  @details Checks whether the mouse position is within the area of the item.
     *  @param mousePosition position of mouse during the click
     *  @returns true if was clicked.
     */
    virtual bool WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const; //world
    /**
     *  @brief Action done every frame in world coordinates.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DoPassiveAction(float elapsedTime);
    /**
     *  @brief Action done every frame in screen coordinates.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DoPassiveAction(float elapsedTime, const Camera2D& camera);
    /**
     *  @brief Action done while the item is focused in world coordinates.
     *  @param elapsedTime Frame time
     */
    virtual void DoFocusAction(float elapsedTime) = 0;
    /**
     *  @brief Action done while the item is focused in screen coordinates.
     *  @param elapsedTime Frame time
     */
    virtual void DoFocusAction(float elapsedTime, const Camera2D& camera);
    /**
     *  @brief Action done while the item is focused and it depends on mouse click in world coordinates.
     *  @details Defaults to the previous DoFocusAction().
     *  @param elapsedTime Frame time
     *  @param mousePosition vector2 of mouse's x and y world coordinates during the recent click.
     */
    virtual void DoFocusAction(float elapsedTime, const Vector2& mousePosition);
    /**
     *  @brief Action done while the item is focused and it depends on mouse click in screen coordinates.
     *  @details Defaults to the previous DoFocusAction().
     *  @param elapsedTime Frame time
     *  @param mousePosition vector2 of mouse's x and y world coordinates during the recent click.
     */
    virtual void DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera);
    /**
     *  @brief Sets focused to false.
     */
    virtual void Defocus();
    /**
     *  @brief Sets focused to true.
     */
    virtual void Focus();
    /**
     *  @brief Checks whether the item is in focus.
     *  @returns Focus state - true if focused
     */
    bool IsFocused() const;

    /**
     *  @brief Makes the item inactive.
     */
    void MakeInactive();
    /**
     *  @brief Makes the item active.
     */
    void MakeActive();
    /**
     *  @brief Sets the item's activity state.
     *  @param flag true - inactive, false - active
     */
    void SetInactive(bool flag);
    /**
     *  @brief Checks whether the item is inactive.
     *  @returns true if inactive
     */
    bool IsInactive() const;

    /**
     *  @brief Sets the coordinates' system of the item to World.
     */
    virtual void SetToWorld();
    /**
     *  @brief Sets the coordinates' system of the item to Screen.
     */
    virtual void SetToScreen();
    /**
     *  @brief Checks whether the current coordinates' system of the item is Screen based.
     *  @returns true if screen based.
     */
    bool IsScreenBased() const;

    /**
     *  @brief Makes the item invisible.
     */
    void MakeInvisible();
    /**
     *  @brief Makes the item visible.
     */
    void MakeVisible();
    /**
     *  @brief Sets the item's visibility.
     *  @param flag true - visible.
     */
    void SetVisible(bool flag);
    /**
     *  @brief Checks whether the item is visible.
     *  @returns true if visible.
     */
    bool IsVisible() const;

    /**
     *  @brief Makes the item invisible and not interactable.
     *  @details Sets both inactive and invisible to false.
     */
    void Hide();
    /**
     *  @brief Makes the item visible and interactable.
     *  @details Sets both inactive and invisible to true.
     */
    void Show();

    /**
     *  @brief Makes the item consume clicks.
     */
    virtual void ConsumeClicks();
    /**
     *  @brief Makes the item let clicks through it and propagate.
     */
    virtual void LetClicksThrough();
    /**
     *  @brief Checks whether the item eats clicks.
     *  @returns true if eats clicks.
     */
    virtual bool DoesEatClicks() const;

    /**
     *  @brief Launches on click.
     *  @details Lambda, put in your own.
     */
    std::function<void()> onClick;
    /**
     *  @brief Launches when cursor is over the item.
     *  @details Lambda, put in your own.
     */
    std::function<void()> onHover;

    /**
     *  @brief Sets the x coordinate of the item.
     *  @param x new value to be set
     */
    virtual void SetX(float x);
    /**
     *  @brief Sets the y coordinate of the item.
     *  @param y new value to be set
     */
    virtual void SetY(float y);
    /**
     *  @brief Sets both x and y coordinates to the same value
     *  @details Convenience wrapper over individual setters.
     *  @param xy coordinates
     *  @see SetX
     *  @see SetY
     */
    virtual void SetXY(float xy);
    /**
     *  @brief Sets both x and y coordinate of the item.
     *  @details Convenience wrapper over individual setters.
     *  @param x new value to be set of x coordinate
     *  @param y new value to be set of y coordinate
     *  @see SetX
     *  @see SetY
     */
    virtual void SetXY(float x, float y);
    /**
     *  @brief Sets both x and y coordinate of the item.
     *  @details Convenience wrapper over individual setters.
     *  @param xy pair of the new coordinates
     *  @see SetX
     *  @see SetY
     */
    virtual void SetXY(const Vector2& xy);
    /**
     *  @brief Sets the height of the item.
     *  @param value The new height.
     *  @throws std::invalid_argument If the value is negative.
     */
    virtual void SetHeight(float value);
    /**
     *  @brief Sets the width of the item.
     *  @param value The new width.
     *  @throws std::invalid_argument If the value is negative.
     */
    virtual void SetWidth(float value);
    /**
     *  @brief Sets both width and height of the item.
     *  @details Just calls the appropriate methods - convenience wrapper.
     *  @param width new width
     *  @param height new height
     *  @see SetWidth
     *  @see SetHeight
     */
    virtual void SetDimensions(float width, float height);
    /**
     *  @brief Sets both width and height of the item.
     *  @details Just calls the appropriate methods - convenience wrapper.
     *  @param dimensions new width and height (in this order)
     *  @see SetWidth
     *  @see SetHeight
     */
    virtual void SetDimensions(const Vector2& dimensions);
    /**
     *  @brief Sets both width and height of the item to the same value.
     *  @details Just calls the appropriate methods - convenience wrapper.
     *  @param value new value of both width and height
     *  @see SetWidth
     *  @see SetHeight
     */
    virtual void SetDimensions(float value);
    /**
     *  @brief Sets item's ID.
     *  @param id new ID to be set.
     *  @throws std::invalid_argument if the id is empty.
     */
    virtual void SetID(const std::string& id);
    /**
     *  @brief Sets the priority in the order of drawing of the item.
     *  @param value The new priority.
     *  @throws std::invalid_argument If the value is negative.
     */
    virtual void SetPriority(int value);
    /**
     *  @brief Increases order by one, decreasing the priority by one.
     *  @details If priority is 0, does nothing.
     */
    virtual void MoveUpPriority();
    /**
     *  @brief Decreases order by one, increasing the priority by one.
     */
    virtual void MoveDownPriority();

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
     *  @brief Gets the x,y coordinates of the item.
     *  @returns pair of coordinates
     */
    Vector2 GetXY() const;
    /**
     *  @brief Returns height of the item.
     *  @returns height of the item
     */
    virtual float GetHeight() const;
    /**
     *  @brief Returns total height of the item, if it's different from the standard. Takes border into account.
     *  @details Sometimes items have variable total height at runtime, or the standard height is a dimension of the "main body." For interaction, use GetHeight, for drawing - GetTotalHeight.
     *  @returns runtime height
     */
    virtual float GetTotalHeight() const;
    /**
     *  @brief Returns width of the item.
     *  @returns width of the item
     */
    virtual float GetWidth() const;
    /**
     *  @brief Returns total width of the item, if it's different from the standard. Takes border into account.
     *  @details Sometimes items have variable total width at runtime, or the standard width is a dimension of the "main body." For interaction, use GetWidth, for drawing - GetTotalWidth.
     *  @returns runtime width
     */
    virtual float GetTotalWidth() const;
    /**
     *  @brief Returns priority of the item.
     *  @details Remember that larger number is lesser priority.
     *  @returns number representing priority
     */
    virtual size_t GetPriority() const;
    /**
     *  @brief Returns the item's ID.
     *  @returns ID
     */
    virtual std::string GetID() const;

    /**
     *  @brief Returns the internal library identificator of the item.
     *  @details Roughly the name of the class.
     *  @returns Internal ID
     */
    std::string GetFxID() const;
    /**
     *  @brief Returns the internal library identificator of the item.
     *  @details The same as GetFxID, but static.
     *  @returns Internal ID
     *  @see Item::GetFxID
     *  @note It's pointless here, but it cannot be inherited. It's just a reminder here - add this to other items.
     */
    static const std::string GetClassID();

protected:
    std::string ID; ///< ID used by the user, variable.
    mutable float xAnchor; ///< top left x coordinate. Mutable for screen coordinate translation
    mutable float yAnchor; ///< top left y coordinate. Mutable for screen coordinate translation
    float height; ///< vertical length of the item
    float width; ///< horizontal length of the item

    bool focused; ///< whether the item is currently doing something; was it clicked.
    bool visible; ///< invisible items are still interactable
    bool inactive; ///< inactive items are not interactable
    bool eatsClick; ///< if false, allows the click to continue under it
    size_t priority; ///< order of drawing. Set to 3 by default for convenience of moving priority around. The higher the priority, the quicker it gets done - but gets drawn under.

    bool screenBased; ///< if true, item is drawn based on screen, so follows camera.
    const std::string fxID; ///< ID for the library.
    float timer = 0.0f; ///< in case a widget needs to measure out time. Fully internal.
};

}

#endif // ITEM_H
