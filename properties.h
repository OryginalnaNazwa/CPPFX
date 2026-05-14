#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "raylib.h"
#include <string>
#include <unordered_set>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <memory>
#include <functional>

#define GREY GRAY //a hack to make British spelling work with Raylib colours.
#define DARKGREY DARKGRAY
#define LIGHTGREY LIGHTGRAY

/****************************
 * This file contains the properties used in the FX items.
 ****************************/


namespace CPPFX {

class Colour {
public:


    Colour() : name("LIGHTGREY"), value(LIGHTGREY) {}
    Colour(const std::string& colour) : name(colour), value(StringToColour(Normalise(colour))) {}
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
     *  @param colour colour's value - {R, G, B} or Raylib's defines.
     *  @details Accepts also RGB values. Resolves to the string of the value as a name.
     */
    void SetColour(Color colour);

    /**
     *  @brief Gets colour name.
     *  @return A string that's the colour's name.
     */
    std::string GetColourString() const;
    /**
     *  @brief Gets colour.
     *  @return Raylib's colour value.
     */
    Color GetColour() const;

private:
    std::string name; ///colour's name
    Color value; ///Raylib's colour

    /**
     *  @brief Gets valid colours.
     *  @return A set of names of valid colours.
     */
    static const std::unordered_set<std::string>& ValidColours();
    /**
     *  @brief Gets an RGB value based on a colour's name.
     *  @details Accepts only Raylib's colours. Both GREY and GRAY work.
     *  @param str A string with a colour's name.
     */
    static Color StringToColour(const std::string& str);
    static std::string ColourToString(Color c);
    ///Normalises a colour potential name for further validity check.
    static std::string Normalise(const std::string& str);
};

class Property {
public:
    Colour colour;

    Property(const std::string& i) : fxID(i) {}
    Property(const std::string& i, const Color& colour) : colour(colour), fxID(i)  {}
    Property(const std::string& i, const std::string& colour) : colour(colour), fxID(i) {}

    std::string GetFxID() const;

protected:
    std::string fxID; ///internal use ID

};

class Border : public Property {
public:

    Border() : Property("Border", BLACK), thickness(0) {}

    /**
     * @details Sets the thickness value.
     * @param thickness new thickness value.
     * @throw std::range_error If thickness is negative.
     */
    void SetThickness(const float& thickness);
    float GetThickness() const;

    void SetDrawingMethod(const std::function<void(const float& x, const float& y, const float& width, const float& height)>& drawMyself);
    void RemoveDrawingMethod();

    void DrawMyself(const float& x, const float& y, const float& width, const float& height) const;

private:
    float thickness;
    std::function<void(const float& x, const float& y, const float& width, const float& height)> drawMyself;
};

class Font : public Property {
public:
    float fontSize;
    std::string name;

    Font() : Property("Font", BLACK), fontSize(20) {}
    Font(const float& fS) : Property("Font", BLACK), fontSize(fS) {}
};

/**
 *  @class Alignment

 */
class Alignment : public Property {
public:
    enum Alignments {
        TOP_CENTRE,     TOP_LEFT,   TOP_RIGHT,
        CENTRE_LEFT,    CENTRE,     CENTRE_RIGHT,
        BOTTOM_LEFT,    BOTTOM_CENTRE, BOTTOM_RIGHT
    };

    Alignment() : Property("Alignment"), alignment(TOP_LEFT) {}

    /**
     *  @brief Sets the alignment based on a string.
     *  @details Accepts the same names as in the Alignments enum only.
     *  @param alignment string of the alignment
     *  @throws std::invalid_argument if the parameter is not in the Alignments enum
     */
    void SetAlignment(const std::string& alignment);
    void SetAlignment(const Alignments& alignment);
    Alignments GetAlignment() const;
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

private:
    std::string AlignmentToString(const Alignments& alignment) const;
    Alignments StringToAlignment(const std::string& alignment) const;

    Alignments alignment;
};

}
#endif // PROPERTIES_H
