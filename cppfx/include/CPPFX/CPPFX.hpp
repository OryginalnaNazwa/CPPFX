#ifndef CPPFX_H
#define CPPFX_H

#include <cstddef>        // for size_t
#include <unordered_map>  // for unordered_map
#include <memory>         // for allocator, unique_ptr, make_unique
#include <stdexcept>      // for out_of_range, runtime_error
#include <string>         // for string, basic_string, operator+, char_traits
#include <unordered_set>  // for unordered_set, operator==
#include <vector>         // for vector
#include <concepts>       // for concepts

#include "base_item.hpp"  // for Item
#include "properties.hpp" // for Colour
#include "raylib.h"       // for Camera2D, Vector2, Color
#include "items.hpp" // for items. Needs it, otherwise you'd need to include more files than just this one.

/**
 *  @file
 *  @brief Entry point of CPPFX: the GUI controller.
 *  @details The only header users need to include. Defines CPPFX::GUI, which
 *           owns every widget it creates. Call DoUI once per frame between BeginMode2D
 *           and EndMode2D, and Close before CloseWindow.
 *           Also defines the CPPFX_VERSION macros for compile-time checks.
 */


/**
 * @name Version
 * @brief CPPFX version
 * @details CPPFX_VERSION = MAJOR * 10000 + MINOR * 100 + PATCH, so 1.23.4 would be 102304.
 * @{
 */
#define CPPFX_VERSION_MAJOR 0
#define CPPFX_VERSION_MINOR 24
#define CPPFX_VERSION_PATCH 0
#define CPPFX_VERSION ((CPPFX_VERSION_MAJOR * 10000) + (CPPFX_VERSION_MINOR * 100) + CPPFX_VERSION_PATCH)
/** @} */

/**
 *  @brief Throws an exception with ID of the GUI
 *  @param ex exception type
 *  @param msg exception message
 */
#define CPPFX_THROW_GUI(ex, msg) throw ex(std::string("In GUI ") + this->ID + ": " + (msg))

namespace CPPFX {

extern const char* const CPPFX_VERSION_STRING;

/**
 * @class GUI
 * @brief Primary controller class for GUI elements.
 */
class GUI {
public:
    std::string ID = ""; ///< for differentiating between different GUI instances.

    /**
     *  @brief Default constructor for GUI.
     *  @details Screen based starts at false. Sets items counters to 0.
     */
    GUI() : screenBased(false) {
        for (auto& id : FXIDs) {
            ItemsCounter.insert({id, 0}); // sets all the counters to 0
        }
    }

    /**
     *  @brief Calls Close()
     */
    ~GUI() { Close(); }

    /**
     *  @brief Primary UI drawing and interaction loop.
     *  @details Controls everything else.
     *  @param camera raylib's camera instance, the one being passed to BeginMode2D()
     *  @note Use this in your main loop.
     *  @warning Must be between Begin/EndMode2D if screenBased items or GUI is used.
     */
    void DoUI(const Camera2D& camera);

    /**
     *  @brief Closes the GUI
     *  @details Call in OpenGL context (before CloseWindow). Cleans up and destroys items.
     */
    void Close();

    /**
     *  @brief Adds item of given type.
     *  @details If the parameter is empty, ID is created automatically.
     *  @param ID ID of the created item, defaults to empty.
     *  @returns a pointer to the newly created item.
     *  @warning Do not use the auto-generated prefix ("GUI_AUTO_") as a parameter, it's reserved. See: GetAutomaticPrefix()
     *  @throws std::invalid_argument if ID is taken
     *  @tparam T widget type, must be derived from Item.
     */
    template <typename T>
    T* AddItem(const std::string& ID = "") requires std::derived_from<T, Item> && std::default_initializable<T> {
        std::unique_ptr<Item> item = std::make_unique<T>();
        CreateItemID(item, ID);
        auto pointer = dynamic_cast<T*>(item.get());
        CreateItem(item);
        return pointer;
    }
    /**
     *  @brief Adds a widget of this type.
     *  @param ID ID of the created item. Defaults to "", the library creates the ID then.
     *  @returns raw pointer to the created item, owned by GUI. Valid until Close or RemoveItem.
     *  @throws std::invalid_argument if ID is taken
     *  @warning Do not use the auto-generated prefix ("GUI_AUTO_") as a parameter, it's reserved.
     *  @see AddItem
     */
    TextField* AddTextField(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Label* AddLabel(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Button* AddButton(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    CheckBox* AddCheckBox(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    AnchorPane* AddAnchorPane(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    VBox* AddVBox(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    HBox* AddHBox(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Spinner* AddSpinner(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    EditableSpinner* AddEditableSpinner(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Workspace* AddWorkspace(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    PasswordField* AddPasswordField(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    ProgressIndicator* AddProgressIndicator(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    ProgressBar* AddProgressBar(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    PressedButton* AddPressedButton(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    PieChart* AddPieChart(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     *  @tparam T type of values held in the DropDown. std::string adds methods for when label and value are the same.
     */
    template <typename T>
    DropDown<T>* AddDropDown(const std::string& ID = "") {
        return AddItem<DropDown<T>>(ID);
    }
    /**
     *  @copydoc AddTextField
     */
    ComboBox* AddComboBox(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    SearchableDropDown* AddSearchableDropDown(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     *  @tparam T type of values held in the list. Must be able to be turned into std::string, either by type or by supplied method.
     */
    template <typename T>
    List<T>* AddList(const std::string& ID = "") {
        return AddItem<List<T>>(ID);
    }
    /**
     *  @copydoc AddTextField
     */
    Line* AddLine(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Square* AddSquare(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Rectangle* AddRectangle(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Circle* AddCircle(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    RadioButton* AddRadioButton(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    RadioGroup* AddRadioGroup(const std::string& ID = "");
    /**
     *  @copydoc AddTextField
     */
    Sprite* AddSprite(const std::string& ID = "");

    /**
     *  @brief Removes an item.
     *  @details Also removes from any containers that hold it.
     *  @param ID ID of the item to be removed.
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     *  @warning Makes any pointer to the item invalid after successful removal.
     */
    void RemoveItem(const std::string& ID);
    /**
     *  @brief Removes an item.
     *  @details Also removes from any containers that hold it.
     *  @param item pointer to the item to be deleted.
     *  @throws std::invalid_argument if the item doesn't exist.
     *  @throws std::out_of_range if there's no such item in this GUI
     *  @warning Makes any pointer to the item invalid after successful removal.
     */
    void RemoveItem(Item* item);

    /**
     *  @brief Gets an item of the specified type.
     *  @param ID ID of the item to be returned.
     *  @tparam T type of the item to be returned
     *  @returns A pointer to the item, owned by GUI, valid until Close or RemoveItem.
     *  @throws std::runtime_error if the type is incorrect for the given item.
     *  @throws std::out_of_range if no item with given ID exists.
     */
    template <typename T>
    T* GetItem(const std::string& ID) const requires std::derived_from<T, Item> {
        try {
            auto* ptr = dynamic_cast<T*>(Items.at(ID).get());
            if (!ptr) CPPFX_THROW_GUI(std::runtime_error, "Item with the ID " + ID + " is not a " + T::GetClassID());
            return ptr;
        } catch (const std::out_of_range&) {
            CPPFX_THROW_GUI(std::out_of_range, "No item with ID " + ID + " exists");
        }
    }
    /**
     *  @brief Gets an item of this widget type.
     *  @param ID ID of the wanted item.
     *  @return Pointer to the specified item, owned by GUI, valid until Close or RemoveItem.
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     *  @throws std::runtime_error if the item isn't of the right type.
     */
    TextField* GetTextField(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Label* GetLabel(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Button* GetButton(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    CheckBox* GetCheckBox(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     *  @tparam T type of value held in DropDown.
     */
    template <typename T>
    DropDown<T>* GetDropDown(const std::string& ID) const {
        return GetItem<DropDown<T>>(ID);
    }
    /**
     *  @copydoc GetTextField
     */
    ComboBox* GetComboBox(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    SearchableDropDown* GetSearchableDropDown(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     *  @tparam T type of value held in the List.
     */
    template <typename T>
    List<T>* GetList(const std::string& ID) const {
       return GetItem<List<T>>(ID);
    }
    /**
     *  @copydoc GetTextField
     */
    AnchorPane* GetAnchorPane(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    VBox* GetVBox(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    HBox* GetHBox(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Workspace* GetWorkspace(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Spinner* GetSpinner(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    EditableSpinner* GetEditableSpinner(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    PasswordField* GetPasswordField(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    ProgressIndicator* GetProgressIndicator(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    ProgressBar* GetProgressBar(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    PressedButton* GetPressedButton(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    PieChart* GetPieChart(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Line* GetLine(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Square* GetSquare(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Rectangle* GetRectangle(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Circle* GetCircle(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     *  @note It's a CheckBox with shape.
     */
    RadioButton* GetRadioButton(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    RadioGroup* GetRadioGroup(const std::string& ID) const;
    /**
     *  @copydoc GetTextField
     */
    Sprite* GetSprite(const std::string& ID) const;

    /**
     *  @brief Gives the item the highest priority any item currently has.
     *  @details The item ties with the current highest; use SetAboveHighestPriority
     *           to put it strictly on top. Sorts the drawing order first, if needed.
     *  @param ID ID of the item whose priority will be changed.
     *  @throws std::out_of_range if no item with the ID exists.
     *  @note Prints a warning (debug builds only) if the item already has priority 0.
     *  @see Item::SetPriority
     */
    void SetHighestPriority(const std::string& ID);
    /**
     *  @brief Gives the item a priority one above the highest any item currently has.
     *  @details Sorts the drawing order first, if needed. If the highest is already 0,
     *           the item ties at 0 instead.
     *  @param ID ID of the item whose priority will be changed.
     *  @throws std::out_of_range if no item with the ID exists.
     *  @note Prints a warning (debug builds only) if the item already has
     *        priority 0, or if it has to tie at 0.
     *  @see Item::SetPriority
     */
    void SetAboveHighestPriority(const std::string& ID);
    /**
     *  @brief Gives the item the lowest priority any item currently has.
     *  @details The item ties with the current lowest; use SetBelowLowestPriority
     *           to put it strictly at the bottom. Sorts the drawing order first, if needed.
     *  @param ID ID of the item whose priority will be changed.
     *  @throws std::out_of_range if no item with the ID exists.
     *  @see Item::SetPriority
     */
    void SetLowestPriority(const std::string& ID);
    /**
     *  @brief Gives the item a priority one below the lowest any item currently has.
     *  @details Sorts the drawing order first, if needed.
     *  @param ID ID of the item whose priority will be changed.
     *  @throws std::out_of_range if no item with the ID exists.
     *  @see Item::SetPriority
     */
    void SetBelowLowestPriority(const std::string& ID);

    /**
     *  @brief Sorts the drawing order by priority.
     *  @details Done automatically on the next DoUI after adding an item or
     *           using the priority methods above. Call it yourself after
     *           changing priority directly on an item (Item::SetPriority),
     *           which the GUI can't detect.
     */
    void SortOrder();

    /**
     *  @brief Sets primary colour of all items.
     *  @details Primary is the Item::colour field. Sets ALL and only currently existing items.
     *  @param colour colour to be set
     */
    void SetGlobalPrimaryColour(const Colour& colour); //TODO make them actually useful
    /// @copydoc SetGlobalPrimaryColour(const Colour&)
    void SetGlobalPrimaryColour(const Color& colour);
    /**
     *  @copydoc SetGlobalPrimaryColour(const Colour&)
     *  @throws std::invalid_argument if colour is empty
     *  @throws std::out_of_range if colour is not one of known colours
     */
    void SetGlobalPrimaryColour(const std::string& colour); //TODO colours are handled now, fix

    /**
     * @brief Checks how many widgets of this type were created in total since creation or last Close.
     * @param fxID widget type name
     * @return number of widgets created
     * @throws std::invalid_argument if such fxID doesn't exist
     */
    size_t GetTotalItemCount(const std::string& fxID) const;

    /**
     * @brief Checks whether ID already exists.
     * @param ID string to be checked.
     * @returns true if already taken.
     */
    bool IsIDTaken(const std::string& ID) const;
    /**
     *  @brief Checks whether the item of the given ID exists.
     *  @details alias for IsIDTaken
     *  @param ID ID of the item to check
     *  @return true if the item exists.
     */
    bool HasItem(const std::string& ID) const;

    /**
     * @brief Checks whether the item of specified ID is a container.
     * @see GetContainersFxIDs
     * @param ID ID of the item to be checked
     * @returns true if the item is a container.
     * @throws std::out_of_range if item doesn't exist.
     */
    bool IsItemContainer(const std::string& ID) const;

    /**
     *  @brief Checks whether the widget type is a container
     *  @param fxID widget type name, e.g "VBox"
     *  @returns true if it is a container
     */
    static bool IsContainer(const std::string& fxID);
    /**
     * @brief Checks whether the ID is one of the library's internal use identifiers.
     * @details fxIDs are the names of the classes of widgets.
     * @param fxID string to be checked.
     * @returns true if it is an fxID
     */
    static bool IsFxID(const std::string& fxID);

    /**
     *  @brief Returns the names of all widget types the GUI can create.
     *  @returns an unordered set of fxIDs.
     */
    static const std::unordered_set<std::string>& GetFxIDs();

    /**
     *  @brief Grabs IDs of all items currently in GUI.
     *  @returns a set of IDs of all items.
     */
    std::unordered_set<std::string> GetItemsIDs() const;

    /**
     *  @brief Returns the prefix used to create IDs when user has specified none.
     *  @returns the prefix (GUI_AUTO_)
     */
    static const std::string& GetAutomaticIDPrefix();

    /**
     *  @brief Checks whether the given ID has been generated automatically.
     *  @details Checks if it starts with the prefix.
     *  @see GetAutomaticIDPrefix
     *  @returns true if yes.
     */
    static bool IsIDAutomatic(const std::string& ID);

    /**
     *  @brief Gives a set of containers fxIDs.
     *  @returns a set of names of containers.
     */
    static const std::unordered_set<std::string>& GetContainersFxIDs();

    /**
     *  @brief Overrides the items so all work in Screen coordinates.
     */
    void SetGlobalScreenBased();
    /**
     *  @brief Turns of the override so every item works in its own coordinate system.
     *  @see SetGlobalScreenBased
     *  @see Item::SetScreenBased
     */
    void SetGlobalWorldBased();
    /**
     *  @brief Sets whether the items will be screen based or world based.
     *  @param flag true - screen, false - world.
     */
    void SetGlobalCoordinateBase(const bool& flag);
    /**
     *  @brief Checks the global coordinate system flag.
     *  @returns true if screen based, false if world.
     */
    bool IsGlobalScreenBased() const;

private:
    bool screenBased; ///< global flag for whether the items are drawn in screen coordinates or their own

    std::unordered_map<std::string, std::unique_ptr<Item>> Items; ///< lookup map by ID
    std::vector<Item*> ItemsInDrawingOrder; ///< items in reverse priority order — back is the highest priority, so that the items with highest priority end up drawn on top

    std::unordered_map<std::string, size_t> ItemsCounter; ///< each newly created item increments its counter. Used for auto IDs.

    float elapsedTime = 0.0f; ///< total elapsed time of all frames
    bool needsSorting = false; ///< lazy sort

    void DrawUI(const Camera2D& camera) const; ///< draws the items
    void OnMouseClick(const Vector2& mousePos, const Camera2D& camera); ///< defocuses and does onClick
    void DoItemsActions(const Vector2& mousePos, const Camera2D& camera); ///< does focused and passive actions and onHover
    void DoClickedItemsActions(const Vector2& mousePos, const Camera2D& camera); ///< does actions that require mouse input

    void DefocusItems(); ///< defocuses all items after a click.

    Container* GetContainer(const std::string& ID) const; ///< returns a polymorphic pointer to Container

    /**
     *  @brief Adds item to the drawing order and moves its ownership into the map
     *  @details Sets needsSorting to true.
     *  @param item unique pointer to the item created elsewhere. Left empty afterwards.
     */
    void CreateItem(std::unique_ptr<Item>& item);
    /**
     *  @brief Creates an ID for a new item.
     *  @details The default ID follows a pattern: GUI_AUTO_ + fxID + number of items of this type created so far (first item doesn't get any number), e.g GUI_AUTO_Label, GUI_AUTO_Label1...
     *  @param item unique pointer to the newly created item.
     *  @param ID ID to be given to the new item. If empty, an automatic ID is generated.
     *  @throws std::invalid_argument on duplicate IDs
     */
    void CreateItemID(std::unique_ptr<Item>& item, const std::string& ID = "");

    static const std::unordered_set<std::string> FXIDs; ///< list of widget names

    static const std::string AUTOMATIC_ID_PREFIX; ///< prefix of every automatic ID

    static const std::unordered_set<std::string> CONTAINERS; ///< list of which widgets are containers
};

}

#endif // CPPFX_H
