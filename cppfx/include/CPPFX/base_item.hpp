#ifndef ITEM_H
#define ITEM_H

/**
 *  @file
 *  @brief Item, the abstract base of every widget.
 *  @details Holds what all widgets share: position, size, colour, border,
 *           visibility, activity, focus, priority and the click/hover callbacks.
 *           Included through CPPFX.hpp.
 */

#include "properties.hpp"
#include <cstddef>       // for size_t
#include <functional>    // for function
#include <string>        // for string
#include <raylib.h>      // for Vector2

namespace CPPFX {

/**
 *  @class Item
 *  @brief Abstract base of every widget.
 */
class Item {
public:
    Colour colour; ///< primary colour
    Colour hoverColour; ///< you can also use it for other things, it's legal!
    Border border; ///< all but a few items have it.

    Item() : ID(""), xAnchor(0.0f), yAnchor(0.0f), height(0.0f), width(0.0f), focused(false), visible(true), inactive(false), eatsClick(true), priority(3), screenBased(false), fxID("") {} ///< for vectors and stuff, don't use normally
    /**
     *  @brief Constructor with the default size (200 x 100).
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
     *  @brief Draws the item.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DrawMyself(float elapsedTime) const = 0;
    /**
     *  @brief Checks whether the item was clicked.
     *  @details Checks whether the mouse position is within the area of the item.
     *  @param mousePosition position of mouse during the click
     *  @returns true if was clicked.
     */
    virtual bool WasIClicked(const Vector2& mousePosition) const;
    /**
     *  @brief Action done every frame.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DoPassiveAction(float elapsedTime);
    /**
     *  @brief Action done while the item is focused.
     *  @param elapsedTime total elapsed time of all frames
     */
    virtual void DoFocusAction(float elapsedTime) = 0;
    /**
     *  @brief Action done while the item is focused and it depends on mouse click.
     *  @details Defaults to the previous DoFocusAction().
     *  @param elapsedTime total elapsed time of all frames
     *  @param mousePosition mouse's x and y during the recent click, in the item's
     *         own coordinate system (screen or world).
     */
    virtual void DoFocusAction(float elapsedTime, const Vector2& mousePosition);
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
    virtual void MakeInactive();
    /**
     *  @brief Makes the item active.
     */
    virtual void MakeActive();
    /**
     *  @brief Sets the item's activity state.
     *  @param flag true - inactive, false - active
     */
    virtual void SetInactive(bool flag);
    /**
     *  @brief Checks whether the item is inactive.
     *  @returns true if inactive
     */
    virtual bool IsInactive() const;

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
    virtual bool IsScreenBased() const;

    /**
     *  @brief Makes the item invisible.
     */
    virtual void MakeInvisible();
    /**
     *  @brief Makes the item visible.
     */
    virtual void MakeVisible();
    /**
     *  @brief Sets the item's visibility.
     *  @param flag true - visible.
     */
    virtual void SetVisible(bool flag);
    /**
     *  @brief Checks whether the item is visible.
     *  @returns true if visible.
     */
    virtual bool IsVisible() const;

    /**
     *  @brief Makes the item invisible and not interactable.
     *  @details Calls MakeInvisible and MakeInactive.
     *  @see Item::MakeInvisible
     *  @see Item::MakeInactive
     */
    virtual void Hide();
    /**
     *  @brief Makes the item visible and interactable.
     *  @details Calls MakeVisible and MakeActive.
     *  @see Item::MakeVisible
     *  @see Item::MakeActive
     */
    virtual void Show();

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
     *  @details Lambda, put in your own. Not called while the item is inactive.
     */
    std::function<void()> onClick;
    /**
     *  @brief Launches every frame the cursor is over the item.
     *  @details Lambda, put in your own. Not called while the item is inactive.
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
     *  @brief Sets the item's priority.
     *  @details Lower number means higher priority: 0 is the top. Higher-priority
     *           items are drawn later, so they appear on top, and get clicks first.
     *           Default is 3. The GUI re-sorts on its own only after its own
     *           priority methods, so call GUI::SortOrder after using this.
     *  @param value new priority
     *  @throws std::invalid_argument if value is negative
     */
    virtual void SetPriority(int value);
    /**
     *  @brief Raises the priority by one (decreases the number).
     *  @details If priority is 0, does nothing.
     *  @see SetPriority
     */
    virtual void MoveUpPriority();
    /**
     *  @brief Lowers the priority by one (increases the number).
     *  @see SetPriority
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
     *  @brief Returns the internal library identifier of the item.
     *  @details Roughly the name of the class.
     *  @returns Internal ID
     */
    std::string GetFxID() const;
    /**
     *  @brief Returns the internal library identifier of the item.
     *  @details The same as GetFxID, but static.
     *  @returns Internal ID
     *  @see Item::GetFxID
     *  @note Static members are inherited: every widget must define its own,
     *        or it reports its base class's name.
     */
    static const std::string GetClassID();

protected:
    std::string ID; ///< ID used by the user, variable.
    float xAnchor; ///< top left x coordinate.
    float yAnchor; ///< top left y coordinate.
    float height; ///< vertical length of the item
    float width; ///< horizontal length of the item

    bool focused; ///< whether the item is currently doing something; was it the last item clicked.
    bool visible; ///< invisible items are still interactable
    bool inactive; ///< inactive items are not interactable
    bool eatsClick; ///< if false, allows the click to continue under it
    size_t priority; ///< drawing order; see SetPriority.

    bool screenBased; ///< if true, item stays fixed on screen and ignores the camera.
    const std::string fxID; ///< ID for the library.
    float timer = 0.0f; ///< in case a widget needs to measure out time. Fully internal.
};

}

#endif // ITEM_H
