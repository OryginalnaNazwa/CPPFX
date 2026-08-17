#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <functional>     // for function
#include <iostream>       // for operator<<, basic_ostream, char_traits, cerr
#include <string>         // for string, allocator
#include "raylib.h"       // for Color, BLACK, LIGHTGRAY, DARKGRAY, GRAY
#include <memory>         // for shared_ptr
#include <vector>         // for vector
#include <set>            // for set

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

    /**
     *  @brief Places content within a box according to this alignment.
     *  @param x left edge of the box
     *  @param y top edge of the box
     *  @param width width of the box
     *  @param height height of the box
     *  @param contentWidth width of the thing being placed
     *  @param contentHeight height of the thing being placed
     *  @returns Top-left corner for the content.
     *  @note Content larger than the box gives a position outside it. That is
     *        deliberate - it overflows rather than being clamped.
     */
    Vector2 GetAlignedXY(float x, float y, float width, float height,
                  float contentWidth, float contentHeight) const;

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

/**
 *  @class Font
 *  @brief Wrapper over raylib's fonts. Draws and measures text itself.
 *  @details Holds everything DrawTextEx needs except the position and the text:
 *           the glyph atlas, size, spacing and tint. Widgets call DrawText and
 *           MeasureText on it rather than unpacking it into raylib calls.
 *
 *           The atlas is reference counted. Load once, assign the Font to as
 *           many widgets as you like, and it is unloaded exactly once when the
 *           last of them is gone.
 *
 *           When no font has been loaded it falls back to raylib's default
 *           font, so a default constructed Font is always drawable.
 *  @see ::DrawTextEx
 */
class Font : public Property {
public:

    /**
     *  @enum Charset
     *  @brief Ready-made codepoint sets for LoadFont.
     *  @details Every preset includes printable ASCII. LATIN_EXTENDED is the
     *           default and covers Polish, Czech, Hungarian, Croatian,
     *           Romanian, Turkish, the Baltics and the Nordics.
     */
    enum class Charset {
        ASCII,           ///< U+0020-U+007E, 95 glyphs
        LATIN_1,         ///< ASCII + U+00A0-U+00FF
        LATIN_EXTENDED,  ///< LATIN_1 + U+0100-U+017F (default)
        CYRILLIC,        ///< ASCII + U+0400-U+04FF
        GREEK            ///< ASCII + U+0370-U+03FF
    };

    /**
     *  @brief Default constructor.
     *  @details Sets font's size to 20, colour to black and charset to
     *           LATIN_EXTENDED. Does not touch the GPU, so it is safe to
     *           construct before InitWindow.
     */
    Font() : Property("Font", BLACK), fontSize(20.0f), spacing(-1.0f), lineSpacing(0.0f), loadSize(0) {
        AppendPreset(codepoints, Charset::LATIN_EXTENDED);
        NormaliseCharset(codepoints);
    }

    /**
     *  @brief Constructor for custom size.
     *  @param fS size of the font to be set.
     *  @warning throws a warning if size is 0.
     */
    Font(float fS) : Font() {
        if (fS < 0.0f) {
            throw std::invalid_argument("In Font: Negative font size.");
        }
        if (fS == 0.0f) {
            CPPFX_WARN("Font size set to 0. It will not be visible.");
        }
        fontSize = fS;
    }

    /**
     *  @brief Constructor for custom colour.
     *  @param c colour to be set.
     */
    Font(const Color& c) : Font() {
        colour.SetColour(c);
    }

    /**
     *  @brief Constructor for custom size and colour.
     *  @param fS size of the font to be set.
     *  @param c colour to be set.
     *  @warning throws a warning if size is 0.
     */
    Font(float fS, const Color& c) : Font(fS) {
        colour.SetColour(c);
    }

    /**
     *  @brief Sets the file path for later loading.
     *  @param path path to the file; file name
     *  @throws std::invalid_argument if path is empty
     */
    void SetFilePath(const std::string& path);
    std::string GetFilePath() const;
    void ClearFilePath();

    /**
     *  @brief Loads the font from the stored path.
     *  @details Wrapper over raylib's LoadFontEx, using the stored charset and
     *           load size. Must be called after InitWindow.
     *  @see ::LoadFontEx
     *  @throws std::runtime_error if the path is empty or the font did not load.
     *  @warning throws a warning listing any requested glyphs the file lacks.
     */
    void LoadFont();
    /**
     *  @brief Loads the font from the parameter.
     *  @details Doesn't update the stored path.
     *  @param fileName path to the file
     *  @throws std::invalid_argument if fileName is empty
     *  @throws std::runtime_error if the font did not load properly.
     */
    void LoadFont(const std::string& fileName);
    /**
     *  @brief Releases this Font's share of the atlas.
     *  @details Falls back to the default font afterwards. The atlas itself is
     *           only unloaded once every Font sharing it has done the same, so
     *           this never pulls the texture out from under another widget.
     *  @see ::UnloadFont
     */
    void UnloadFont();
    /**
     *  @brief Checks if a font has been loaded and is valid.
     *  @returns true if this Font holds a valid atlas. False means the default
     *           font is in use, which still draws.
     *  @see ::IsFontValid
     */
    bool IsFontValid() const;
    /**
     *  @brief Checks whether the default font is being used.
     *  @returns true if no font has been loaded.
     */
    bool IsDefaultFont() const;
    /**
     *  @brief Counts how many Fonts currently share this atlas.
     *  @details Mostly for debugging. 0 means the default font is in use.
     *  @returns Reference count.
     */
    long GetShareCount() const;

    /**
     *  @brief Sets the font from an already loaded raylib font.
     *  @details Assumes the atlas is owned elsewhere, so it is never unloaded
     *           from here. Unload it wherever you loaded it.
     *  @param font new font to be used by this one.
     *  @throws std::invalid_argument if the new font is not valid.
     */
    void SetFont(const ::Font& font);
    /**
     *  @brief Gets the raylib font actually used for drawing.
     *  @returns The loaded atlas, or the default font if none was loaded.
     */
    ::Font GetFont() const;
    void ClearFont();

    /**
     *  @brief Sets font size.
     *  @param size size to be set.
     *  @throws std::invalid_argument if the size is negative.
     *  @warning throws a warning if size is 0.
     */
    void SetFontSize(float size);
    /**
     *  @brief Gets font size.
     *  @details This is the em size, the numerator of fontSize / baseSize. It
     *           is the height of the line box, not of any particular letter.
     *           For the height a reader actually sees, use GetCapHeight.
     *  @returns Size of the font.
     */
    float GetFontSize() const;

    /**
     *  @brief Sets the size the glyph atlas is rasterised at.
     *  @details Independent of font size. Drawing far above the load size looks
     *           blurry, far below it wastes memory. 0 means "match font size at
     *           load time". Reloads the font if one is already loaded from a path.
     *  @param size load size, or 0 for automatic
     *  @throws std::invalid_argument if size is negative.
     */
    void SetLoadSize(int size);
    int GetLoadSize() const;
    /**
     *  @brief Gets the size the current atlas was actually rasterised at.
     *  @returns baseSize of the raylib font in use.
     */
    int GetBaseSize() const;

    /**
     *  @brief Sets horizontal spacing between characters.
     *  @details Negative values mean automatic, which reproduces raylib's
     *           DrawText spacing of fontSize / 10.
     *  @param spacing spacing in pixels, or a negative value for automatic
     */
    void SetSpacing(float spacing);
    /**
     *  @brief Gets the spacing actually used for drawing.
     *  @returns Spacing in pixels, with automatic already resolved.
     */
    float GetSpacing() const;
    void SetAutoSpacing();
    bool IsAutoSpacing() const;

    /**
     *  @brief Sets extra vertical gap between lines of multiline text.
     *  @param spacing extra pixels between lines; 0 means lines sit exactly
     *         fontSize apart
     */
    void SetLineSpacing(float spacing);
    float GetLineSpacing() const;

    /**
     *  @brief Sets the charset from a preset.
     *  @details Reloads the font if one is already loaded from a path.
     *  @param preset the preset to use
     */
    void SetCharset(Charset preset);
    /**
     *  @brief Adds a preset to the current charset.
     */
    void AddCharset(Charset preset);
    /**
     *  @brief Sets the charset from a sample of text.
     *  @details Every distinct character in the sample is baked into the atlas,
     *           on top of ASCII. This is the easy way in:
     *           SetCharset("AaCcEeLlNnOoSsZzZz" with the diacritics you need).
     *  @param sampleText text containing the characters that must be renderable
     *  @throws std::invalid_argument if sampleText is empty
     *  @see ::LoadCodepoints
     */
    void SetCharset(const std::string& sampleText);
    /**
     *  @brief Sets the charset from raw Unicode codepoints.
     *  @details The escape hatch. Prefer the preset or sample text overloads.
     *  @param codepoints codepoints to bake into the atlas
     *  @throws std::invalid_argument if codepoints is empty
     */
    void SetCharset(const std::vector<int>& codepoints);
    /**
     *  @brief Adds a sample of text to the current charset.
     */
    void AddCharset(const std::string& sampleText);
    std::vector<int> GetCharset() const;
    /**
     *  @brief Resets the charset to LATIN_EXTENDED.
     */
    void ClearCharset();

    /**
     *  @brief Checks whether a single codepoint can actually be drawn.
     *  @details Catches both a codepoint missing from the atlas and one baked
     *           in as a blank because the font file had no such glyph.
     *  @param codepoint codepoint to check
     *  @returns true if the glyph will render as itself
     */
    bool HasGlyph(int codepoint) const;
    /**
     *  @brief Finds every codepoint in the text this font cannot draw.
     *  @param text text to check
     *  @returns Sorted, deduplicated missing codepoints. Empty if all fine.
     */
    std::vector<int> FindMissingGlyphs(const std::string& text) const;
    /**
     *  @brief Checks whether the whole string can be drawn.
     *  @param text text to check
     *  @returns true if every character will render as itself
     */
    bool CanRender(const std::string& text) const;
    /**
     *  @brief Checks the loaded atlas against the requested charset.
     *  @details Run after loading to find glyphs the font file simply lacks.
     *  @returns Codepoints that were requested but came back blank.
     */
    std::vector<int> ValidateCharset() const;
    /**
     *  @brief Warns once per missing codepoint found in the text.
     *  @details Meant for text mutation points such as SetText, not for the
     *           draw loop. Compiles to nothing under NDEBUG.
     *  @param text text to check
     *  @param context item ID or similar, included in the message
     */
    void WarnAboutMissingGlyphs(const std::string& text,
                                const std::string& context = "") const;

    /**
     *  @brief Draws text with this font.
     *  @details Wrapper over DrawTextEx, using this font's size, spacing and
     *           colour. Position is the top-left of the text's line box.
     *  @param text text to draw
     *  @param x left edge
     *  @param y top of the line box
     *  @see ::DrawTextEx
     */
    void DrawText(const std::string& text, float x, float y) const;
    void DrawText(const std::string& text, const Vector2& position) const;
    /**
     *  @brief Draws text with an overridden tint.
     *  @param tint colour to use instead of this font's colour
     */
    void DrawText(const std::string& text, float x, float y, const Color& tint) const;
    /**
     *  @brief Draws rotated text.
     *  @param origin rotation origin, relative to position
     *  @param rotation rotation in degrees
     *  @see ::DrawTextPro
     */
    void DrawTextPro(const std::string& text, const Vector2& position,
                     const Vector2& origin, float rotation) const;

    /**
     *  @brief Gets the visible size of the text.
     *  @details Width as measured, height as cap height rather than the line
     *           box, so alignment centres what a reader actually sees.
     *  @param text text to measure
     *  @returns Ink width and height in pixels.
     */
    Vector2 GetInkSize(const std::string& text) const;
    /**
     *  @brief Draws text anchored by the top-left of its visible ink.
     *  @details Unlike DrawText, which anchors the line box, this compensates
     *           for the empty space above the capitals. Pair it with
     *           Alignment and GetInkSize.
     *  @param text text to draw
     *  @param inkTopLeft where the top-left of the visible text should land
     */
    void DrawTextAt(const std::string& text, const Vector2& inkTopLeft) const;
    void DrawTextAt(const std::string& text, const Vector2& inkTopLeft,
                    const Color& tint) const;

    /**
     *  @brief Measures text with this font.
     *  @details Wrapper over MeasureTextEx.
     *  @param text text to measure
     *  @returns Width and line box height in pixels.
     *  @see ::MeasureTextEx
     */
    Vector2 MeasureText(const std::string& text) const;
    float MeasureTextWidth(const std::string& text) const;
    /**
     *  @brief Measures the line box height of the text.
     *  @details Equals font size for single line text, and grows by
     *           fontSize + lineSpacing per newline.
     */
    float MeasureTextHeight(const std::string& text) const;

    /**
     *  @brief Gets the ink height of a capital letter.
     *  @details Unlike font size, this is what a reader actually sees. Font
     *           size is the em box, which includes empty descender space.
     *  @returns Height of 'H' in pixels at the current font size.
     */
    float GetCapHeight() const;
    /**
     *  @brief Gets the distance from the top of the line box down to the top
     *         of a capital letter.
     */
    float GetCapOffset() const;
    /**
     *  @brief Gets the y offset that optically centres one line of text.
     *  @details Centres the visible ink rather than the line box, so text does
     *           not sit high once a real font is loaded. Use it as
     *           font.DrawText(t, x, yAnchor + font.GetVerticalCentreOffset(height)).
     *  @param boxHeight height of the box to centre within
     *  @returns Offset from the top of the box.
     */
    float GetVerticalCentreOffset(float boxHeight) const;

    /**
     *  @brief Appends a Unicode codepoint to a UTF-8 string.
     *  @details Wrapper over raylib's CodepointToUTF8. Use this instead of casting
     *           to char - a cast truncates anything above U+007F, so 'z' becomes
     *           '|'.
     *  @param text string to append to
     *  @param codepoint Unicode codepoint, e.g. as returned by GetCharPressed
     *  @see ::CodepointToUTF8
     */
    static void AppendCodepoint(std::string& text, int codepoint);

    /**
     *  @brief Removes the last whole character from a UTF-8 string.
     *  @details Strips continuation bytes before the lead byte, so one call removes
     *           one visible character rather than one byte. Safe on empty strings.
     */
    static void PopBackCodepoint(std::string& text);

    /**
     *  @brief Draws text aligned within a box.
     *  @details Places the visible ink, not the line box, so text sits where a
     *           reader expects it. Alignment is a parameter because it belongs
     *           to whatever owns the box, not to the font.
     *  @param text text to draw
     *  @param alignment where to place it within the box
     *  @param x left edge of the box
     *  @param y top edge of the box
     *  @param width width of the box
     *  @param height height of the box
     *  @see Alignment::GetAlignedXY
     */
    void DrawAligned(const std::string& text, const Alignment& alignment,
                     float x, float y, float width, float height) const;
    void DrawAligned(const std::string& text, const Alignment& alignment,
                     float x, float y, float width, float height,
                     const Color& tint) const;

private:
    std::shared_ptr<::Font> font;         ///< atlas; null means use the default font
    std::string filePath;                 ///< path stored for LoadFont
    float fontSize;                       ///< em size; numerator of fontSize / baseSize
    float spacing;                        ///< horizontal gap; negative means automatic
    float lineSpacing;                    ///< extra vertical gap between lines
    int loadSize;                         ///< atlas rasterisation size; 0 means match fontSize
    std::vector<int> codepoints;          ///< charset baked into the atlas
    mutable std::set<int> warnedCodepoints; ///< deduplicates missing glyph warnings

    /**
     *  @brief Gets the font to actually draw with.
     *  @returns The loaded atlas, or raylib's default font.
     */
    ::Font Resolve() const;
    /**
     *  @brief Gets the ratio the atlas is scaled by when drawing.
     *  @returns fontSize / baseSize.
     */
    float GetScaleFactor() const;
    /**
     *  @brief Pushes line spacing into raylib's global text state.
     *  @details SetTextLineSpacing is global, so it is set before every draw
     *           and measure rather than assumed.
     */
    void ApplyLineSpacing() const;
    /**
     *  @brief Reloads the atlas if this Font owns one loaded from a path.
     */
    void ReloadIfLoaded();
    /**
     *  @brief Appends the codepoints of a preset to a vector.
     */
    static void AppendPreset(std::vector<int>& target, Charset preset);
    /**
     *  @brief Adds ASCII, then sorts and deduplicates.
     */
    static void NormaliseCharset(std::vector<int>& target);
};

}
#endif // PROPERTIES_H
