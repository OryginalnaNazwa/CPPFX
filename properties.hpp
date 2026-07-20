#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <functional>     // for function
#include <iostream>       // for operator<<, basic_ostream, char_traits, cerr
#include <string>         // for string, allocator
#include <unordered_set>  // for unordered_set
#include "raylib.h"       // for Color, BLACK, LIGHTGRAY, DARKGRAY, GRAY


/******************************************************************
 *  @file properties
 *  @brief This file contains the properties used in the FX items.
 *  @details And also some helpers.
 ******************************************************************/

namespace CPPFX {

#define GREY GRAY //a hack to make British spelling work with Raylib colours.
#define DARKGREY DARKGRAY
#define LIGHTGREY LIGHTGRAY

#ifdef NDEBUG
#define CPPFX_WARN(msg)
#else
#define CPPFX_WARN(msg) std::cerr << "[CPPFX Warning] " << msg << "\n"
#endif

#define CPPFX_THROW(ex, msg) throw ex(std::string("In ") + this->fxID + " " + this->ID + ": " + msg)
#define CPPFX_THROW_NO_ITEM(ex, fxID, ID, msg) throw ex(std::string("In ") + fxID + " " + ID + ": " + msg)

/**
 *  @class Colour
 *  @brief Wrapper over raylib colours.
 *  @details Used in every widget and property.
 */
class Colour {
public:

    /**
     *  @brief Default constructor.
     *  @details Sets colour as light grey.
     */
    Colour() : name("LIGHTGREY"), value(LIGHTGREY) {}
    /**
     *  @brief Constructor for setting the colour by name.
     *  @param colour name of the colour.
     *  @details Normalises the name before commencing. Sets the value too.
     */
    Colour(const std::string& colour) : name(colour), value(StringToColour(Normalise(colour))) {}
    /**
     *  @brief Constructor for setting colour by value.
     *  @param colour value of the colour.
     *  @details Sets the name too.
     */
    Colour(const Color& colour) : name(ColourToString(colour)), value(colour) {}

    /**
     *  @brief Sets colour using its name.
     *  @param colour colour's name
     *  @details Ignores capitalisation of letters. Both GRAY and GREY work. Only Raylib colours are accepted.
     *  @throws std::invalid_argument If the colour doesn't exist.
     */
    void SetColour(const std::string& colour);
    /**
     *  @brief Sets colour using its value.
     *  @param colour colour's value - {R, G, B} or Raylib's defines. Only raylib colours are accepted.
     */
    void SetColour(Color colour);

    /**
     *  @brief Gets colour name.
     *  @details Despite the hack, the only returned names are the raylib one's.
     *  @return A string that's the colour's name.
     */
    std::string GetColourString() const;
    /**
     *  @brief Gets colour.
     *  @return Raylib's colour value.
     */
    Color GetColour() const;

private:
    std::string name; ///< colour's name
    Color value; ///< Raylib's colour

    /**
     *  @brief Gets valid colours.
     *  @return A set of names of valid colours.
     */
    static const std::unordered_set<std::string>& ValidColours();
    /**
     *  @brief Gets an RGB value based on a colour's name.
     *  @details Accepts only Raylib's colours. Both GREY and GRAY work.
     *  @param str A string with a colour's name.
     *  @returns colour's value
     *  @throws std::invalid_argument if colour is not a valid one
     */
    static Color StringToColour(const std::string& str);
    /**
     *  @brief Gets the name of a colour based on the value.
     *  @details Accepts only raylib colours.
     *  @param c color value
     *  @returns name of the colour
     *  @throws std::invalid_argument if colour is not a valid one
     */
    static std::string ColourToString(Color c);
    /// Normalises a colour potential name for further validity check.
    static std::string Normalise(const std::string& str);
};

/**
 *  @class Property
 *  @brief Base for other properties
 *  @note Is it even needed?
 */
class Property {
public:
    Colour colour;

    Property(const std::string& i) : fxID(i) {}
    Property(const std::string& i, const Color& colour) : colour(colour), fxID(i)  {}
    Property(const std::string& i, const std::string& colour) : colour(colour), fxID(i) {}

    /**
     *  @brief Returns name of the property.
     *  @returns Name of the property.
     */
    std::string GetFxID() const;

protected:
    std::string fxID; ///< internal use ID

};

/**
 *  @class Border
 *  @brief The outline around an item.
 *  @details Draws outside the item, thickness radiating outwards, rather than into the item by default. Allows to override default behaviour by a lambda.
 */
class Border : public Property {
public:

    /**
     *  @brief Default constructor.
     *  @details Sets colour to black and thickness to 0, turning off border by default in all items.
     */
    Border() : Property("Border", BLACK), thickness(0), drawMyself(nullptr) {}

    /**
     * @brief Sets the thickness value.
     * @param thickness new thickness value.
     * @throw std::invalid_argument If thickness is negative.
     */
    void SetThickness(float thickness);
    /**
     *  @brief Gets thickness.
     *  @returns thickness value.
     */
    float GetThickness() const;

    /**
     *  @brief Sets drawing method to one specified.
     *  @param drawMyself how the border should draw itself around an item. Takes in item's basic dimensions as parameters.
     */
    void SetDrawingMethod(const std::function<void(float x, float y, float width, float height)>& drawMyself);
    /**
     *  @brief Sets custom drawing method to null.
     *  @details Goes back to default drawing method.
     *  @see Border::DrawMyself for default drawing method.
     */
    void RemoveDrawingMethod();

    /**
     *  @brief Draws border around an item.
     *  @details Default border draws around and outwards an item, i.e. x - thickness is the end point of border. Draws only when thickness is greater than 0.
     *  @param x item's x coordinate
     *  @param y item's y coordinate
     *  @param width item's width
     *  @param height item's height
     */
    void DrawMyself(float x, float y, float width, float height) const;

private:
    float thickness; ///< thickness of the border, radiating outward
    std::function<void(float x, float y, float width, float height)> drawMyself; ///< drawing method
};

/**
 *  @class Font
 *  @brief Wrapper over raylib's fonts.
 */
class Font : public Property {
public:

    /**
     *  @brief Default constructor.
     *  @details Sets font's size to 20 and colour to black.
     */
    Font() : Property("Font", BLACK), fontSize(20.0f) {}
    /**
     *  @brief Constructor for custom size.
     *  @details Sets colour to black.
     *  @param fS size of the font to be set.
     *  @warning throws a warning if size is 0.
     */
    Font(float fS) : Property("Font", BLACK), fontSize(fS) {
        if (fS == 0.0f) {
            CPPFX_WARN("Font size set to 0. It will not be visible.");
        }
    }
    /**
     *  @brief Constructor for custom colour.
     *  @details Sets font's size to 20.
     *  @param c colour to be set.
     */
    Font(const Color& c) : Property("Font", c), fontSize(20.0f) {}
    /**
     *  @brief Constructor for custom size.
     *  @details Sets colour to black.
     *  @param fS size of the font to be set.
     *  @param c colour to be set.
     *  @warning throws a warning if size is 0.
     */
    Font(float fS, const Color& c) : Property("Font", c), fontSize(fS) {
        if (fS == 0.0f) {
            CPPFX_WARN("Font size set to 0. It will not be visible.");
        }
    }

    /**
     *  @brief Sets font size.
     *  @param size size to be set.
     *  @throws std::invalid_argument if the size is negative.
     *  @warning throws a warning if size is 0.
     */
    void SetFontSize(float size);
    /**
     *  @brief Gets font size.
     *  @returns Size of the font.
     */
    float GetFontSize() const;

private:
    float fontSize; ///< distance from the bottom to the top of the letter
    //std::string name; //TODO ///< name of the font to be used for loading custom ones
};

/**
 *  @class Alignment
 *  @brief 3 x 3 alignment grid for text display or containers arrangement.
 */
class Alignment : public Property {
public:
    enum Alignments {
        TOP_LEFT,       TOP_CENTRE,     TOP_RIGHT,
        CENTRE_LEFT,    CENTRE,         CENTRE_RIGHT,
        BOTTOM_LEFT,    BOTTOM_CENTRE,  BOTTOM_RIGHT,
        //aliases (convenience and reuse of axes)
        LEFT = CENTRE_LEFT,
        RIGHT = CENTRE_RIGHT,
        TOP = TOP_CENTRE,
        UP = TOP_CENTRE,
        ABOVE = TOP_CENTRE,
        BOTTOM = BOTTOM_CENTRE,
        DOWN = BOTTOM_CENTRE,
        UNDER = BOTTOM_CENTRE,
        MIDDLE = CENTRE,
        TOP_CENTER = TOP_CENTRE,
        CENTER_LEFT = CENTRE_LEFT,
        CENTER = CENTRE,
        BOTTOM_CENTER = BOTTOM_CENTRE,
        CENTER_RIGHT = CENTRE_RIGHT
    }; ///< alignments matrix

    /**
     *  @brief Default constructor
     *  @details Sets alignment to top left.
     */
    Alignment() : Property("Alignment"), alignment(TOP_LEFT) {}
    /**
     *  @brief Constructor for setting alignment using enum value.
     */
    Alignment(const Alignments& a) : Property("Alignment"), alignment(a) {}
    /**
     *  @brief Constructor for setting alignment using string.
     */
    Alignment(const std::string& a) : Property("Alignment"), alignment(StringToAlignment(a)) {}

    /**
     *  @brief Sets the alignment based on a string.
     *  @details Normalises by toupper only.
     *  @param alignment string of the alignment
     *  @throws std::invalid_argument if the parameter is not in the Alignments enum
     */
    void SetAlignment(const std::string& alignment);
    /**
     *  @brief Sets the alignment based on the value.
     *  @param alignment value to be set
     */
    void SetAlignment(const Alignments& alignment);
    /**
     *  @brief Gets the current alignment as a value
     *  @returns An enum value corresponding to the current alignment.
     */
    Alignments GetAlignment() const;
    /**
     *  @brief Gets the current alignment as a string.
     *  @returns A string corresponding to the current alignment.
     */
    std::string GetAlignmentString() const;

    /**
     *  @brief Checks whether the current alignment is X_RIGHT.
     *  @returns true if right
     */
    bool IsRightAlignment() const;
    /**
     *  @brief Checks whether the current alignment is X_LEFT.
     *  @returns true if left
     */
    bool IsLeftAlignment() const;
    /**
     *  @brief Checks whether the current alignment is X_CENTRE.
     *  @returns true if centre
     */
    bool IsCentreAlignment() const;
    /**
     *  @brief Checks whether the current alignment is BOTTOM_X.
     *  @returns true if bottom
     */
    bool IsBottomAlignment() const;
    /**
     *  @brief Checks whether the current alignment is TOP_X.
     *  @returns true if top
     */
    bool IsTopAlignment() const;

    /**
     *  @brief Gets x coordinate aligned according to current alignment.
     *  @details Assumes top left is the default and doesn't have to do anything.
     *  @param x x coordinate
     *  @param width width of the object to align, for properly centering it
     *  @param objectWidth width of the space in which aligning is happening
     *  @returns x aligned to the current alignment
     *  @throws std::invalid_argument if width or objectWidth is negative
     */
    float GetAlignedX(float x, float width, float objectWidth) const;
    /**
     *  @brief Gets y coordinate aligned according to current alignment.
     *  @details Assumes top left is the default and doesn't have to do anything.
     *  @param y y coordinate
     *  @param width width of the object to align, for properly centering it
     *  @param objectHeight height of the space in which aligning is happening
     *  @returns y aligned to the current alignment
     *  @throws std::invalid_argument if height or objectHeight is negative
     */
    float GetAlignedY(float y, float height, float objectHeight) const;

private:
    std::string AlignmentToString(const Alignments& alignment) const;
    /**
     *  @brief Gets alignment based on a name.
     *  @details Take in aliases, but overwrites them.
     *  @param alignment name of the alignment
     *  @returns Alignment
     */
    Alignments StringToAlignment(const std::string& alignment) const;

    Alignments alignment; ///< current alignment
};

}
#endif // PROPERTIES_H
