#ifndef CPPFX_H
#define CPPFX_H

#include <stddef.h>       // for size_t
#include <map>            // for map
#include <memory>         // for allocator, unique_ptr, make_unique
#include <stdexcept>      // for out_of_range, runtime_error
#include <string>         // for string, basic_string, operator+, char_traits
#include <typeinfo>       // for type_info
#include <unordered_set>  // for unordered_set, operator==, _Node_const_iter...
#include <vector>         // for vector
#include "base_item.hpp"    // for Item
#include "properties.hpp"   // for Colour
#include "raylib.h"       // for Camera2D, Vector2, Color
/*// declarations
namespace CPPFX { class AnchorPane; }
namespace CPPFX { class Button; }
namespace CPPFX { class CheckBox; }
namespace CPPFX { class Circle; }
namespace CPPFX { class Container; }
namespace CPPFX { class EditableSpinner; }
namespace CPPFX { class HBox; }
namespace CPPFX { class Label; }
namespace CPPFX { class Line; }
namespace CPPFX { class PasswordField; }
namespace CPPFX { class PieChart; }
namespace CPPFX { class PressedButton; }
namespace CPPFX { class ProgressBar; }
namespace CPPFX { class ProgressIndicator; }
namespace CPPFX { class Rectangle; }
namespace CPPFX { class Spinner; }
namespace CPPFX { class Square; }
namespace CPPFX { class TextField; }
namespace CPPFX { class VBox; }
namespace CPPFX { class Workspace; }
namespace CPPFX { template <typename T> class DropDown; }
namespace CPPFX { template <typename T> class List; }
namespace CPPFX { class RadioGroup; }*/
#include "items.hpp" // for items. Needs it, otherwise you'd need to link more files than just this one.

/******************************************************************************
 *  @file CPPFX
 *  @brief This file contains the controller class for use in raylib's loop.
 *****************************************************************************/

namespace CPPFX {

#define CPPFX_VERSION_MAJOR 0
#define CPPFX_VERSION_MINOR 5
#define CPPFX_VERSION_PATCH 0
#define CPPFX_VERSION ((CPPFX_VERSION_MAJOR * 10000) + (CPPFX_VERSION_MINOR * 100) + CPPFX_VERSION_PATCH)
extern const char* CPPFX_VERSION_STRING;

#define CPPFX_THROW_GUI(ex, msg) throw ex(std::string("In GUI ") + this->ID + ": " + msg)

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
     *  @brief Primary UI drawing and interaction loop.
     *  @details Put in your main loop.
     *  @param camera Camera
     */
    void DoUI(const Camera2D& camera);

    /**
     *  @brief Adds item of given type.
     *  @details If the parameter is empty, ID is created automatically.
     *  @param ID ID of the created item, defaults to empty.
     *  @returns a pointer to the newly created item.
     */
    template <typename T>
    T* AddItem(const std::string& ID = "") {
        std::unique_ptr<Item> item = std::make_unique<T>();
        CreateItemID(item, ID);
        auto pointer = dynamic_cast<T*>(item.get());
        CreateItem(item);
        return pointer;
    }
    /**
     *  @brief Creates the item.
     *  @param ID ID of the created item. Defaults to "", the library creates the ID then.
     *  @returns raw pointer to the created item.
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
     */
    template <typename T>
    DropDown<T>* AddDropDown(const std::string& ID = "") {
        return AddItem<DropDown<T>>(ID);
    }
    /**
     *  @copydoc AddTextField
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
     *  @brief Removes an item.
     *  @details Safe removes from all containers.
     *  @param ID ID of the item to be removed.
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     */
    void RemoveItem(const std::string& ID);
    /**
     *  @brief Removes an item.
     *  @details Deletes the pointer.
     *  @details Safe removes from all containers.
     *  @param item pointer to the item to be deleted.
     *  @throws std::invalid_argument if the item doesn't exist.
     *  @throws std::out_of_range if the item doesn't exist in this GUI.
     */
    void RemoveItem(Item*& item);

    /**
     *  @brief Gets an item of the specified type.
     *  @param ID ID of the item to be returned.
     *  @returns A pointer to the item.
     *  @throws std::runtime_error if the type is incorrect for the given item.
     *  @throws std::out_of_range if no item with given ID exists.
     */
    template <typename T>
    T* GetItem(const std::string& ID) {
        try {
            auto* ptr = dynamic_cast<T*>(Items.at(ID).get());
            if (!ptr) CPPFX_THROW_GUI(std::runtime_error, "Item with the ID " + ID + " is not a " + typeid(T).name());
            return ptr;
        } catch (const std::out_of_range&) {
            CPPFX_THROW_GUI(std::out_of_range, "No item with ID " + ID + " exists");
        }
    }
    /**
     *  @brief Gets an usable item of the specified ID.
     *  @param ID ID of the wanted item.
     *  @return Pointer to the specified item.
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     *  @throws std::runtime_error if the item isn't of the right type.
     */
    TextField* GetTextField(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Label* GetLabel(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Button* GetButton(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    CheckBox* GetCheckBox(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    template <typename T>
    DropDown<T>* GetDropDown(const std::string& ID) {
        return GetItem<DropDown<T>>(ID);
    }
    template <typename T>
    List<T>* GetList(const std::string& ID) {
       return GetItem<List<T>>(ID);
    }
    /**
     *  @copydoc GetTextField
     */
    AnchorPane* GetAnchorPane(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    VBox* GetVBox(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    HBox* GetHBox(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Workspace* GetWorkspace(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Spinner* GetSpinner(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    EditableSpinner* GetEditableSpinner(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    PasswordField* GetPasswordField(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    ProgressIndicator* GetProgressIndicator(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    ProgressBar* GetProgressBar(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    PressedButton* GetPressedButton(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    PieChart* GetPieChart(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Line* GetLine(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Square* GetSquare(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Rectangle* GetRectangle(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    Circle* GetCircle(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    RadioButton* GetRadioButton(const std::string& ID);
    /**
     *  @copydoc GetTextField
     */
    RadioGroup* GetRadioGroup(const std::string& ID);

    /**
     *  @brief Sets the item's priority to highest available.
     *  @details Sorts the items before setting, if needed.
     *  @param ID the ID of the Item which priority will be changed.
     *  @throws std::out_of_range if item with the ID doesn't exist.
     *  @warning warns if the item already has priority 0.
     */
    void SetHighestPriority(const std::string& ID);
    /**
     *  @brief Sets the item's priority to be the highest of all items.
     *  @details Sorts the items before setting, if needed. If another item has priority of 0 already, this item will have new priority equal to it.
     *  @param ID the ID of the Item which priority will be changed.
     *  @throws std::out_of_range if item with the ID doesn't exist.
     *  @warning warns if the item already has priority 0 or if other items already have priority 0.
     */
    void SetAboveHighestPriority(const std::string& ID);
    /**
     *  @brief Sets the item's priority to as low as other items get.
     *  @details Sorts the items before setting, if needed.
     *  @param ID the ID of the Item which priority will be changed.
     *  @throws std::out_of_range if item with the ID doesn't exist.
     */
    void SetLowestPriority(const std::string& ID);
    /**
     *  @brief Sets the item's priority to be the lowest of all items.
     *  @details Sorts the items before setting, if needed.
     *  @param ID the ID of the Item which priority will be changed.
     *  @throws std::out_of_range if item with the ID doesn't exist.
     */
    void SetBelowLowestPriority(const std::string& ID);

    /**
     *  @brief Sorts the drawing order by priority. Sorting is done automatically on first frame after adding an item.
     */
    void SortOrder();

    /**
     *  @brief Sets primary colour of all items.
     *  @details Primary is the base colour inherited after Item.
     *  @param colour colour to be set
     */
    void SetGlobalPrimaryColour(const Colour& colour);
    /// @copydoc SetGlobalPrimaryColour
    void SetGlobalPrimaryColour(const Color& colour);
    /// @copydoc SetGlobalPrimaryColour
    void SetGlobalPrimaryColour(const std::string& colour);

    /**
     * @brief Check how many widgets of this type were created in total.
     * @param fxID ID in internal use; Widget default name
     * @return number of widgets created
     * @throws std::out_of_range if such fxID doesn't exist
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
     * @details {Workspace, Anchorpane, VBox, HBox}
     * @param ID Item to be checked
     * @returns true if is a container.
     * @throws std::out_of_range if item doesn't exist.
     */
    bool IsItemContainer(const std::string& ID) const;

    /**
     *  @brief Checks whether the widget type is a container
     *  @param fxID widget name
     *  @returns true if it is a container
     */
    static bool IsContainer(const std::string& fxID);
    /**
     * @brief Checks whether the ID is one of the library's internal use identificators.
     * @details fxIDs are the names of the classes of widgets.
     * @param fxID string to be checked.
     * @returns true if it is an fxID
     */
    static bool IsFxID(const std::string& fxID);

    /**
     *  @brief Returns a set of fxIDs.
     *  @returns fxIDs.
     */
    static const std::unordered_set<std::string>& GetFxIDs();

    /**
     *  @brief Grabs IDs of all items currently in GUI.
     *  @returns a vector of IDs of all items.
     */
    const std::unordered_set<std::string> GetItemsIDs() const;

    /**
     *  @brief Returns the prefix used to create IDs when user has specified none.
     *  @returns the prefix
     */
    static const std::string& GetAutomaticIDPrefix();

    /**
     *  @brief Checks whether the given ID has been generated automatically.
     *  @returns true if yes.
     */
    static bool IsIDAutomatic(const std::string& ID);

    /**
     *  @brief Gives a set of containers fxIDs.
     *  @returns a set of names of containers.
     */
    static const std::unordered_set<std::string>& GetContainersFxIDs();

    /**
     *  @brief Sets every item coordinate system to Screen based.
     */
    void SetGlobalScreenBased();
    /**
     *  @brief Sets every item coordinate system to World based.
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
    bool screenBased; /// global flag for whether the items are drawn in world or screen coordinates

    std::map<std::string, std::unique_ptr<Item>> Items; /// lookup map by ID
    std::vector<Item*> ItemsInDrawingOrder; /// items in reverse priority order — back is the highest priority. Which is indeed the correct drawing order

    std::map<std::string, size_t> ItemsCounter; /// each new created item increments its counter. Used for auto IDs.

    float elapsedTime = 0.0f; /// total elapsed time of all frames
    bool needsSorting = false; /// lazy sort

    void DrawUI(const Camera2D& camera) const; /// draws the items
    void onMouseClick(const Vector2& mousePos, const Camera2D& camera); /// defocuses nad does onClick
    void DoItemsActions(const Vector2& mousePos, const Camera2D& camera); /// does focused and passive actions
    void DoClickedItemsActions(const Vector2& mousePos, const Camera2D& camera); /// does actions that require mouse input

    void DefocusItems(); /// defocuses all items after a click.

    Container* GetContainer(const std::string& ID); /// returns a polymorphic pointer to Container

    /**
     *  @brief Pushes a pointer to the items in order and moves the unique_ptr to the map.
     *  @details Sets needsSorting to true.
     *  @param item unique pointer to the item created elsewhere.
     */
    void CreateItem(std::unique_ptr<Item>& item);
    /**
     *  @brief Creates an ID for a new item.
     *  @details The default ID follows a pattern: GUI_AUTO_ + fxID + (number of items with the fxID + 1).
     *  @param item unique pointer to the newly created item.
     *  @param ID ID to be given to the new item. If empty, defaults.
     */
    void CreateItemID(std::unique_ptr<Item>& item, const std::string& ID = "");

    static const std::unordered_set<std::string> FXIDs; /// list of widget names

    static const std::string AUTOMATIC_ID_PREFIX; /// how are automatic IDs created

    static const std::unordered_set<std::string> CONTAINERS; /// list of which widgets are containers
};

}

#endif // CPPFX_H
