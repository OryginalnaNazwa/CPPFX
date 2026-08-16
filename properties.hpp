#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <functional>     // for function
#include <iostream>       // for operator<<, basic_ostream, char_traits, cerr
#include <string>         // for string, allocator
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
 *  @details Used in every widget and property. Accepts named raylib colours
 *           and custom literals in the form #RrrGggBbb[Aaa] (hex) or
 *           #RrrrGgggBbbb[Aaaa] (dec). Alpha is optional and defaults to opaque.
 */
class Colour {
public:

    /**
     *  @brief Default constructor.
     *  @details Sets colour as light grey.
     */
    Colour() : name("LIGHTGRAY"), value(LIGHTGRAY) {}
    /**
     *  @brief Constructor for setting the colour by name or literal.
     *  @param colour name of the colour, or a custom literal.
     *  @details Normalises the input. Sets the value too. The stored name is
     *           always the canonical one, so Colour("grey") reports "GRAY".
     *  @throws std::invalid_argument if the input is empty or the literal is malformed
     *  @throws std::out_of_range if the name isn't a known colour
     */
    explicit Colour(const std::string& colour) {
                value = StringToColour(Normalise(colour));
                name  = ColourToString(value);
            }
    /**
     *  @brief Constructor for setting colour by value.
     *  @param colour value of the colour.
     *  @details Sets the name too - a known name if the value matches one,
     *           otherwise a custom literal.
     */
    Colour(const Color& colour) : value(colour) {
        name = ColourToString(colour);
    }

    /**
     *  @brief Sets colour using its name or a custom literal.
     *  @param colour colour's name, or #RrrGggBbb[Aaa] / #RrrrGgggBbbb[Aaaa]
     *  @details Ignores capitalisation. Both GRAY and GREY work.
     *  @throws std::invalid_argument if the input is empty or the literal is malformed
     *  @throws std::out_of_range if the name isn't a known colour
     */
    void SetColour(const std::string& colour);
    /**
     *  @brief Sets colour using its value.
     *  @param colour colour's value - {R, G, B, A} or Raylib's defines.
     */
    void SetColour(Color colour);

    /**
     *  @brief Gets colour name.
     *  @details Returns the canonical raylib spelling for known colours -
     *           never the British aliases. Custom colours come back as literals.
     *  @return A string that's the colour's name.
     */
    std::string GetColourString() const;
    /**
     *  @brief Gets colour.
     *  @return Raylib's colour value.
     */
    Color GetColour() const;

    /**
     *  @brief Sets custom colour literals to be reported in hexadecimal.
     *  @details Affects GetColourString only, and only for colours with no
     *           name - #R1EG90BFF rather than #R030G144B255. Both forms are
     *           always accepted on input.
     *  @see SetLiteralBase
     */
    void SetHex();
    /**
     *  @brief Sets custom colour literals to be reported in decimal.
     *  @details Affects GetColourString only, and only for colours with no
     *           name - #R030G144B255 rather than #R1EG90BFF. Both forms are
     *           always accepted on input.
     *  @see SetLiteralBase
     */
    void SetDec();
    /**
     *  @brief Sets the base used when reporting custom colour literals.
     *  @param base true for hexadecimal, false for decimal.
     *  @details Convenience for the two previous methods, for when the base
     *           is held in a variable.
     *  @see SetHex
     *  @see SetDec
     */
    void SetLiteralBase(bool hex);
    /**
     *  @brief Checks which base custom colour literals are reported in.
     *  @returns true if hexadecimal, false if decimal.
     */
    bool IsHex() const;

    /**
     *  @brief Compares two colours by value.
     *  @param other the colour to compare against
     *  @returns true if RGBA match
     *  @details Names are ignored - a colour built from "GRAY" equals one
     *           built from "#R80G80B80".
     */
    bool operator==(const Colour& other) const;
    bool operator!=(const Colour& other) const;

    /**
     *  @brief Reduces the colour's opacity.
     *  @details Clamps the factor
     *  @param factor 0.0 fully transparent, 1.0 unchanged
     */
    void Fade(float factor);

    /**
     *  @brief Blends this colour towards another.
     *  @param target colour to blend towards
     *  @param t 0.0 leaves the colour unchanged, 1.0 makes it the target
     *  @details Clamps t. Blends all four channels, alpha included.
     */
    void Blend(const Color& target, float t);
    void Blend(const Colour& target, float t);

    static unsigned char Lerp(unsigned char a, unsigned char b, float t); ///< lerps from one colour to the other. One colour channel

private:
    std::string name;   ///< colour's canonical name, or a custom literal
    Color value;        ///< Raylib's colour
    bool hex = false;   ///< whether it holds the string value in hex or dec form

    /// How the literal is laid out: digits per channel, and how many channels.
    struct ColourLayout;

    /**
     *  @brief Gets a colour value from a name or custom literal.
     *  @param str A colour's name, or a hex/dec literal.
     *  @returns colour's value {R, G, B, A}
     *  @throws std::invalid_argument if str is empty or the literal is malformed
     *  @throws std::out_of_range if the name isn't a known colour
     */
    Color StringToColour(const std::string& str);
    /**
     *  @brief Gets the name of a colour from its value.
     *  @details Falls back to a custom literal when the value isn't a known colour.
     *  @param c color value
     *  @returns canonical name, or a literal
     */
    std::string ColourToString(Color c);
    /// Normalises a potential colour name for further validity check.
    static std::string Normalise(const std::string& str);
    static ColourLayout DetectLayout(const std::string& s);
    static Color ParseLiteral(const std::string& s);
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
     *  @param height height of the object to align, for properly centering it
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
