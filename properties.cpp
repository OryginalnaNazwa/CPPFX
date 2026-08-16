#include "properties.hpp"
#include <ctype.h>    // for toupper
#include <algorithm>  // for transform
#include <stdexcept>  // for invalid_argument
#include <unordered_map> // for unordered map
#include <charconv> // for from_chars

using namespace CPPFX;

// --- Properties ---

// --- Colour ---

struct Colour::ColourLayout {
        int width;       // 2 = hex, 3 = dec
        int channels;    // 3 = no alpha, 4 = alpha given
};

std::string Colour::Normalise(const std::string& str) {
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

Colour::ColourLayout Colour::DetectLayout(const std::string& s) {
    static const std::string MODEL_HEX = "#RrrGggBbbAaa";
    static const std::string MODEL_DEC = "#RrrrGgggBbbbAaaa";
    const auto Fail = [&s]() {
        throw std::invalid_argument("Invalid colour: " + s + ". Model colour: "
            + MODEL_HEX + " (hex) or " + MODEL_DEC + " (dec). Red Green Blue Alpha."
              " Alpha is optional and defaults to opaque.");
    };

    if (s.size() < 2 || s.front() != '#' || s.at(1) != 'R') Fail();

    // 'G' is not a digit in either base, so the first one is the green label.
    const std::size_t g = s.find('G');
    if (g != 4 && g != 5) Fail();

    ColourLayout layout{static_cast<int>(g) - 2, 0};

    // Each channel costs one label plus `width` digits; the '#' is the +1.
    const int stride = layout.width + 1;
    if (s.size() == (size_t)(1 + (3 * stride)))         layout.channels = 3;
    else if (s.size() == (size_t)(1 + ((4 * stride))))  layout.channels = 4;
    else                                    Fail();

    return layout;
}

Color Colour::ParseLiteral(const std::string& s) {
    const ColourLayout layout = DetectLayout(s);
    const int base = (layout.width == 2) ? 16 : 10;
    const char labels[4] = {'R', 'G', 'B', 'A'};

    unsigned char values[4] = {0, 0, 0, 255};   // alpha defaults to opaque
    std::size_t pos = 1;

    for (int i = 0; i < layout.channels; ++i) {
        if (s.at(pos++) != labels[i])
            throw std::invalid_argument("Invalid colour: " + s + ". Expected '"
                + labels[i] + "' at index " + std::to_string(pos - 1) + ".");

        unsigned parsed{};
        const char* first = s.data() + pos;
        const char* last  = first + layout.width;
        const auto [ptr, ec] = std::from_chars(first, last, parsed, base);
        if (ec != std::errc{} || ptr != last || parsed > 255)
            throw std::invalid_argument("Invalid colour: " + s + ". Bad "
                + labels[i] + " channel.");

        values[i] = static_cast<unsigned char>(parsed);
        pos += layout.width;
    }

    return Color{values[0], values[1], values[2], values[3]};
}

Color Colour::StringToColour(const std::string& s) {
    if (s.empty()) throw std::invalid_argument("Colour cannot be empty.");
    if (s.front() == '#') return ParseLiteral(s); // custom colours start with #

    static const std::unordered_map<std::string, Color> colourMap = {
        {"BLANK",       BLANK},
        {"WHITE",       WHITE},
        {"BLACK",       BLACK},
        {"RED",         RED},
        {"GREEN",       GREEN},
        {"BLUE",        BLUE},
        {"YELLOW",      YELLOW},
        {"ORANGE",      ORANGE},
        {"PURPLE",      PURPLE},
        {"PINK",        PINK},
        {"RAYWHITE",    RAYWHITE},
        {"DARKGRAY",    DARKGRAY},  {"DARKGREY",    DARKGRAY},
        {"GRAY",        GRAY},      {"GREY",        GRAY},
        {"LIGHTGRAY",   LIGHTGRAY}, {"LIGHTGREY",   LIGHTGRAY},
        {"DARKGREEN",   DARKGREEN},
        {"DARKBLUE",    DARKBLUE},
        {"DARKPURPLE",  DARKPURPLE},
        {"DARKBROWN",   DARKBROWN},
        {"BROWN",       BROWN},
        {"BEIGE",       BEIGE},
        {"MAGENTA",     MAGENTA},
        {"VIOLET",      VIOLET},
        {"SKYBLUE",     SKYBLUE},
        {"LIME",        LIME},
        {"GOLD",        GOLD},
        {"MAROON",      MAROON}
    };

    const auto it = colourMap.find(s);
    if (it != colourMap.end()) return it->second;

    throw std::out_of_range("No colour found.");
}

std::string Colour::ColourToString(Color c) {
    static const std::pair<Color, const char*> NAMED_COLOURS[] = {
        {BLANK,      "BLANK"},
        {WHITE,      "WHITE"},
        {BLACK,      "BLACK"},
        {RED,        "RED"},
        {GREEN,      "GREEN"},
        {BLUE,       "BLUE"},
        {YELLOW,     "YELLOW"},
        {ORANGE,     "ORANGE"},
        {PURPLE,     "PURPLE"},
        {PINK,       "PINK"},
        {RAYWHITE,   "RAYWHITE"},
        {DARKGRAY,   "DARKGRAY"},
        {GRAY,       "GRAY"},
        {LIGHTGRAY,  "LIGHTGRAY"},
        {DARKGREEN,  "DARKGREEN"},
        {DARKBLUE,   "DARKBLUE"},
        {DARKPURPLE, "DARKPURPLE"},
        {DARKBROWN,  "DARKBROWN"},
        {BROWN,      "BROWN"},
        {BEIGE,      "BEIGE"},
        {MAGENTA,    "MAGENTA"},
        {VIOLET,     "VIOLET"},
        {SKYBLUE,    "SKYBLUE"},
        {LIME,       "LIME"},
        {GOLD,       "GOLD"},
        {MAROON,     "MAROON"}
        // British spellings are skipped
    };

    for (const auto& [value, colourName] : NAMED_COLOURS)
        if (c.r == value.r && c.g == value.g && c.b == value.b && c.a == value.a)
            return colourName;

    char buf[20];
    if (hex) {
        if (c.a == 255) std::snprintf(buf, sizeof(buf), "#R%02XG%02XB%02X", c.r, c.g, c.b); // alpha is optional
        else            std::snprintf(buf, sizeof(buf), "#R%02XG%02XB%02XA%02X", c.r, c.g, c.b, c.a);
    } else {
        if (c.a == 255) std::snprintf(buf, sizeof(buf), "#R%03uG%03uB%03u", c.r, c.g, c.b);
        else            std::snprintf(buf, sizeof(buf), "#R%03uG%03uB%03uA%03u", c.r, c.g, c.b, c.a);
    }
    return buf;
};

void Colour::SetColour(const std::string& col) {
    const std::string norm = Normalise(col);
    value     = StringToColour(norm);
    name = norm;
}

void Colour::SetColour(Color col) {
    value   = col;
    name    = ColourToString(col);
}

std::string Colour::GetColourString() const {
    return name;
}

Color Colour::GetColour() const {
    return value;
}

void Colour::SetHex() {
    hex = true;
}

void Colour::SetDec() {
    hex = false;
}

void Colour::SetLiteralBase(bool hex) {
    this->hex = hex;
}

bool Colour::IsHex() const {
    return hex;
}

bool Colour::operator==(const Colour& other) const {
    return value.r == other.value.r && value.g == other.value.g
        && value.b == other.value.b && value.a == other.value.a;
}

bool Colour::operator!=(const Colour& other) const {
    return !(*this == other);
}

void Colour::Fade(float factor) {
    if (factor < 0.0f) factor = 0.0f;
    if (factor > 1.0f) factor = 1.0f;

    value.a *= factor;
    name = ColourToString(value);
}

unsigned char Colour::Lerp(unsigned char a, unsigned char b, float t) {
    return static_cast<unsigned char>(a + (static_cast<float>(b) - a) * t + 0.5f);
}

void Colour::Blend(const Color& target, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    const Color from = value;
    const Color to   = target;

    value = Color{
        Lerp(from.r, to.r, t),
        Lerp(from.g, to.g, t),
        Lerp(from.b, to.b, t),
        Lerp(from.a, to.a, t)
    };
    name = ColourToString(value);
}

void Colour::Blend(const Colour& target, float t) {
    Blend(target.GetColour(), t);
}

// --- Property ---

std::string Property::GetFxID() const {
    return fxID;
}

// --- Border ---
void Border::DrawMyself(float x, float y, float width, float height) const {
    if (thickness > 0.0f) {
        if (drawMyself) {
            drawMyself(x, y, width, height);
        } else {
            DrawRectangleLinesEx({x - thickness, y - thickness, width + (2 * thickness), height + (2 * thickness)}, thickness, colour.GetColour());
        }
    }
}

void Border::SetDrawingMethod(const std::function<void(float x, float y, float width, float height)>& drawMyself) {
    this->drawMyself = drawMyself;
}

void Border::RemoveDrawingMethod() {
    this->drawMyself = nullptr;
}

void Border::SetThickness(float thickness) {
    if (thickness < 0.0f) {
        throw std::invalid_argument("Range error: Negative thickness.");
    } else {
        this->thickness = thickness;
    }
}

float Border::GetThickness() const {
    return thickness;
}

//------Font--------

void CPPFX::Font::SetFontSize(float size) {
    if (size < 0.0f) {
        throw std::invalid_argument("Negative font size.");
    } else {
        if (size == 0.0f) {
            CPPFX_WARN("Font size set to 0. It will not be visible.");
        }
        fontSize = size;
    }
}

float CPPFX::Font::GetFontSize() const {
    return fontSize;
}

//--- Alignment ---

//--- Setters ---
void Alignment::SetAlignment(const Alignments& alignment) {
    this->alignment = alignment;
}

void Alignment::SetAlignment(const std::string& alignment) {
    this->alignment = StringToAlignment(alignment);
}

Alignment::Alignments Alignment::GetAlignment() const {
    return this->alignment;
}

std::string Alignment::GetAlignmentString() const {
    return AlignmentToString(this->alignment);
}

Alignment::Alignments Alignment::StringToAlignment(const std::string& alignment_string) const {
    static const std::unordered_map<std::string, Alignments> map = {
        {"TOP_CENTRE", TOP_CENTRE}, {"TOP_CENTER", TOP_CENTRE}, {"TOP", TOP_CENTRE}, {"ABOVE", TOP_CENTRE}, {"UP", TOP_CENTRE},
        {"TOP_LEFT",   TOP_LEFT},
        {"TOP_RIGHT",  TOP_RIGHT},
        {"CENTRE_LEFT",  CENTRE_LEFT}, {"CENTER_LEFT", CENTRE_LEFT}, {"LEFT", CENTRE_LEFT},
        {"CENTRE",       CENTRE}, {"CENTER", CENTRE}, {"MIDDLE", CENTRE},
        {"CENTRE_RIGHT", CENTRE_RIGHT}, {"CENTER_RIGHT", CENTRE_RIGHT}, {"RIGHT", CENTRE_RIGHT},
        {"BOTTOM_LEFT",   BOTTOM_LEFT},
        {"BOTTOM_CENTRE", BOTTOM_CENTRE}, {"BOTTOM_CENTER", BOTTOM_CENTRE}, {"BOTTOM", BOTTOM_CENTRE}, {"DOWN", BOTTOM_CENTRE}, {"UNDER", BOTTOM_CENTRE},
        {"BOTTOM_RIGHT",  BOTTOM_RIGHT},
    };

    std::string normal = alignment_string;
    std::transform(normal.begin(), normal.end(), normal.begin(), ::toupper);

    auto it = map.find(normal);
    if (it != map.end()) return it->second;
    throw std::invalid_argument("No alignment of such name found: " + alignment_string);
}

std::string Alignment::AlignmentToString(const Alignments& alignment) const {
    switch (alignment) {
        case Alignments::TOP_LEFT:      return "TOP_LEFT";
        case Alignments::TOP_CENTRE:    return "TOP_CENTRE";
        case Alignments::TOP_RIGHT:     return "TOP_RIGHT";
        case Alignments::CENTRE_LEFT:   return "CENTRE_LEFT";
        case Alignments::CENTRE:        return "CENTRE";
        case Alignments::CENTRE_RIGHT:  return "CENTRE_RIGHT";
        case Alignments::BOTTOM_LEFT:   return "BOTTOM_LEFT";
        case Alignments::BOTTOM_CENTRE: return "BOTTOM_CENTRE";
        case Alignments::BOTTOM_RIGHT:  return "BOTTOM_RIGHT";
        default: throw std::invalid_argument("No alignment found");
    }
}

bool Alignment::IsRightAlignment() const {
    return alignment == Alignment::Alignments::TOP_RIGHT || alignment == Alignment::Alignments::CENTRE_RIGHT || alignment == Alignment::Alignments::BOTTOM_RIGHT;
}

bool Alignment::IsLeftAlignment() const {
    return alignment == Alignment::Alignments::TOP_LEFT || alignment == Alignment::Alignments::CENTRE_LEFT || alignment == Alignment::Alignments::BOTTOM_LEFT;
}

bool Alignment::IsCentreAlignment() const {
    return alignment == Alignment::Alignments::TOP_CENTRE || alignment == Alignment::Alignments::CENTRE || alignment == Alignment::Alignments::BOTTOM_CENTRE;
}

bool Alignment::IsBottomAlignment() const {
    return alignment == Alignment::Alignments::BOTTOM_CENTRE || alignment == Alignment::Alignments::BOTTOM_LEFT || alignment == Alignment::Alignments::BOTTOM_RIGHT;
}

bool Alignment::IsTopAlignment() const {
    return alignment == Alignment::Alignments::TOP_CENTRE || alignment == Alignment::Alignments::TOP_LEFT || alignment == Alignment::Alignments::TOP_RIGHT;
}

float Alignment::GetAlignedX(float x, float width, float objectWidth) const {
    if (width < 0.0f) {
        throw std::invalid_argument("Cannot align x: negative width");
    }
    if (objectWidth < 0.0f) {
        throw std::invalid_argument("Cannot align x: negative object's width");
    }
    switch (alignment) {
        case TOP_LEFT:
        case CENTRE_LEFT:
        case BOTTOM_LEFT: return x; // left is the default
        case TOP_CENTRE:
        case CENTRE:
        case BOTTOM_CENTRE: return x + (objectWidth / 2.0f) - (width / 2.0f); // move to the half of the object, move back by half of the alignee
        case TOP_RIGHT:
        case CENTRE_RIGHT:
        case BOTTOM_RIGHT: return (x + objectWidth) - width; // move to the far right, go back by alignee's width
        default: return 0; // should't happen
    }
}

float Alignment::GetAlignedY(float y, float height, float objectHeight) const {
    if (height < 0.0f) {
        throw std::invalid_argument("Cannot align y: negative height");
    }
    if (objectHeight < 0.0f) {
        throw std::invalid_argument("Cannot align y: negative object's height");
    }
    switch (alignment) {
        case TOP_LEFT:
        case TOP_CENTRE:
        case TOP_RIGHT:  return y; // top is the default
        case CENTRE_LEFT:
        case CENTRE:
        case CENTRE_RIGHT: return y + (objectHeight / 2.0f) - (height / 2.0f); // move to the half of the object, move back by half of the alignee
        case BOTTOM_CENTRE:
        case BOTTOM_LEFT:
        case BOTTOM_RIGHT: return (y + objectHeight) - height; // move to the far down, go back by alignee's height
        default: return 0; // should't happen
    }
}
