#ifndef CPPFX_H
#define CPPFX_H

#include "items.h"


namespace CPPFX {

/**
 * @class Primary controller class for GUI elements.
 */
class GUI {
public:

    GUI() : screenBased(false) {
        for (auto& id : FXIDs) {
            ItemsCounter.insert({id, 0});
        }
    }

    /**
     *  @brief Primary UI drawing and interaction loop.
     *  @details Put in your main loop.
     *  @param camera Camera
     */
    void DoUI(const Camera2D& camera);

    /**
     *  @brief Adds an item based on the ID.
     *  @details Calls an appropriate function. Creates a default dropdown holding string values.
     *  @param fxID the internal ID of th item to be added.
     *  @throws std::invalid_argument If the ID doesn't exist.
     */
    void AddItem(const std::string& fxID, const std::string& ID = "");
    /**
     *  @fn AddX
     *  @brief Creates an X
     *  @param ID ID of the created item. Defaults to "", the library creates the ID then.
     *  @returns raw pointer to X
     */
    TextField* AddTextField(const std::string& ID = "");
    Label* AddLabel(const std::string& ID = "");
    Button* AddButton(const std::string& ID = "");
    CheckBox* AddCheckBox(const std::string& ID = "");
    AnchorPane* AddAnchorPane(const std::string& ID = "");
    VBox* AddVBox(const std::string& ID = "");
    HBox* AddHBox(const std::string& ID = "");
    Spinner* AddSpinner(const std::string& ID = "");
    EditableSpinner* AddEditableSpinner(const std::string& ID = "");
    Workspace* AddWorkspace(const std::string& ID = "");
    PasswordField* AddPasswordField(const std::string& ID = "");
    ProgressIndicator* AddProgressIndicator(const std::string& ID = "");
    ProgressBar* AddProgressBar(const std::string& ID = "");
    PressedButton* AddPressedButton(const std::string& ID = "");
    PieChart* AddPieChart(const std::string& ID = "");

    /**
     *  @brief Creates a dropdown.
     *  @param valueType the type of value the dropdown will be holding.
     */
    template <typename T>
    DropDown<T>* AddDropDown(const std::string& ID = "") {
        std::unique_ptr<Item> dropdown = std::make_unique<DropDown<T>>();
        CreateItemID(dropdown, ID);
        auto pointer = static_cast<DropDown<T>*>(dropdown.get());
        CreateItem(dropdown);
        return pointer;
    }
    template <typename T>
    List<T>* AddList(const std::string& ID = "") {
        std::unique_ptr<Item> alist = std::make_unique<List<T>>();
        CreateItemID(alist, ID);
        auto pointer = static_cast<List<T>*>(alist.get());
        CreateItem(alist);
        return pointer;
    }

    /**
     *  @brief Removes an item.
     *  @details Safe removes from all containers.
     */
    void RemoveItem(const std::string& ID);

    template <typename T>
    T* GetItem(const std::string& ID) {
        try {
            auto* ptr = dynamic_cast<T*>(Items.at(ID).get());
            if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a " + typeid(T).name());
            return ptr;
        } catch (const std::out_of_range&) {
            throw std::out_of_range("No item with ID " + ID + " exists");
        }
    }
    /**
     *  @brief Gets an usable Text Field of specified ID
     *  @param ID ID of the wanted text field
     *  @return Pointer to the specified text field
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     *  @throws std::runtime_error if the item isn't a TextField.
     */
    TextField* GetTextField(const std::string& ID);

    /**
     *  @brief Gets an usable Label of specified ID
     *  @param ID ID of the wanted Label
     *  @return Pointer to the specified Label
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     */
    Label* GetLabel(const std::string& ID);

    /**
     *  @brief Gets an usable Button of specified ID
     *  @param ID ID of the wanted button
     *  @return Pointer to the specified button
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     */
    Button* GetButton(const std::string& ID);

    /**
     *  @brief Gets an usable CheckBox of specified ID
     *  @param ID ID of the wanted checkbox
     *  @return Pointer to the specified checkbox
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     */
    CheckBox* GetCheckBox(const std::string& ID);

    /**
     *  @brief Gets an usable DropDown of specified ID
     *  @param ID ID of the wanted DropDown
     *  @return Pointer to the specified DropDown
     *  @throws std::out_of_range if the item of given ID doesn't exist.
     */
    template <typename T>
    DropDown<T>* GetDropDown(const std::string& ID) {
        try {
            auto* ptr = dynamic_cast<DropDown<T>*>(Items.at(ID).get());
            if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a Dropdown");
            return ptr;
        } catch (const std::out_of_range& e) {
            throw std::out_of_range("No Dropdown with the ID " + ID + " exists");
        }
    }
    template <typename T>
    List<T>* GetList(const std::string& ID) {
       try {
            auto* ptr = dynamic_cast<List<T>*>(Items.at(ID).get());
            if (!ptr) throw std::runtime_error("Item with ID " + ID + " is not a List");
            return ptr;
        } catch (const std::out_of_range& e) {
            throw std::out_of_range("No List with the ID " + ID + " exists");
        }
    }
    /**
     *  @fn Get*
     *  @brief Gets an Item of specified ID
     *  @param ID ID of the item to be grabbed
     *  @returns raw pointer to the item of type *
     *  @throws std::runtime_error if the ID belongs to an item that is not * type.
     *  @throws std::out_of_range if there's no item of ID
     */
    AnchorPane* GetAnchorPane(const std::string& ID);
    VBox* GetVBox(const std::string& ID);
    HBox* GetHBox(const std::string& ID);
    Workspace* GetWorkspace(const std::string& ID);
    Spinner* GetSpinner(const std::string& ID);
    EditableSpinner* GetEditableSpinner(const std::string& ID);
    PasswordField* GetPasswordField(const std::string& ID);
    ProgressIndicator* GetProgressIndicator(const std::string& ID);
    ProgressBar* GetProgressBar(const std::string& ID);
    PressedButton* GetPressedButton(const std::string& ID);
    PieChart* GetPieChart(const std::string& ID);

    /**
     *  @brief Sets the item's priority to highest available.
     *  @details Assumes the Items are sorted.
     *  @param ID the ID of the Item which priority will be changed.
     */
    void SetHighestPriority(const std::string& ID);

    /**
     *  @brief Sets the item's priority to be the highest of all items.
     *  @details Assumes the Items are sorted. If another item has priority of 0 already, this item will have new priority equal to it.
     *  @param ID the ID of the Item which priority will be changed.
     */
    void SetAboveHighestPriority(const std::string& ID);

    /**
     *  @brief Sorts the drawing order by priority. Sorting is done automatically on first frame after adding an item.
     */
    void SortOrder();

    void SetGlobalPrimaryColour(const Colour& colour);
    void SetGlobalPrimaryColour(const Color& colour);
    void SetGlobalPrimaryColour(const std::string& colour);

    /**
     * @brief Check how many widgets of this type were created in total.
     * @param fxID ID in internal use; Widget default name
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
     * @brief Checks whether the item of specified ID is a container.
     * @details {Workspace, Anchorpane, VBox, HBox}
     * @param ID Item to be checked
     * @returns true if is a container.
     * @throws std::out_of_range if item doesn't exist.
     */
    bool IsItemContainer(const std::string& ID) const;
    /**
     * @brief Checks whether the ID is one of the library's internal use identificators.
     * @details fxIDs are the names of the classes of widgets.
     * @param fxID string to be checked.
     * @returns true if it is an fxID
     */
    bool IsFxID(const std::string& fxID) const;

    std::vector<std::string> GetItemsIDs() const;

    void SetGlobalScreenBased();
    void SetGlobalWorldBased();
    /**
     *  @brief Sets whether the items will be screen based or world based.
     *  @param flag true - screen, false - world.
     */
    void SetGlobalCoordinateBase(const bool& flag);
    bool IsGlobalScreenBased() const;

private:
    bool screenBased;

    std::map<std::string, std::unique_ptr<Item>> Items;
    std::vector<Item*> ItemsInDrawingOrder;

    std::map<std::string, size_t> ItemsCounter;

    float dt = 0; ///frame time
    bool needsSorting = false; ///lazy sort

    void DrawUI(const Camera2D& camera) const;
    void onMouseClick(const Vector2& mousePos, const Camera2D& camera);
    void DoItemsActions(const Vector2& mousePos, const Camera2D& camera);
    void DoClickedItemsActions(const Vector2& mousePos, const Camera2D& camera);

    void DefocusItems();

    bool IsContainer(const std::string& fxID) const;

    Container* GetContainer(const std::string& ID);

    void CreateItem(std::unique_ptr<Item>& item);
    /**
     *  @brief Creates an ID for a new item.
     *  @details The default ID follows a pattern: GUI_AUTO_ + fxID + (number of items with the fxID + 1).
     *  @param item unique pointer to the newly created item.
     *  @param ID ID to be given to the new item. If empty, defaults.
     */
    void CreateItemID(std::unique_ptr<Item>& item, const std::string& ID = "");

    static const std::unordered_set<std::string> FXIDs;
    /*{"Label", "Button", "TextField", "CheckBox", "DropDown", "AnchorPane", "VBox", "HBox", "Workspace", "Spinner", "EditableSpinner",
    "PasswordField", "ProgressBar", "ProgressIndicator", "PressedButton", "List", "Chart", "PieChart"}*/

};

}

#endif // CPPFX_H
