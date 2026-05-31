#include "properties.h"

using namespace CPPFX;

// --- Properties ---

std::string Colour::Normalise(const std::string& str) {
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

const std::unordered_set<std::string>& Colour::ValidColours() {
    static const std::unordered_set<std::string> colours = {
        "WHITE", "BLACK", "RED", "GREEN", "BLUE",
        "YELLOW", "ORANGE", "PURPLE", "PINK",
        "RAYWHITE", "DARKGRAY", "GRAY", "LIGHTGRAY",
        "DARKGREEN", "DARKBLUE", "DARKPURPLE", "DARKBROWN",
        "BROWN", "BEIGE", "MAGENTA", "VIOLET", "SKYBLUE",
        "LIME", "GOLD", "MAROON", "GREY", "DARKGREY", "LIGHTGREY"
    };
    return colours;
}

Color Colour::StringToColour(const std::string& s) {
    if (s == "WHITE")       return WHITE;
    if (s == "BLACK")       return BLACK;
    if (s == "RED")         return RED;
    if (s == "GREEN")       return GREEN;
    if (s == "BLUE")        return BLUE;
    if (s == "YELLOW")      return YELLOW;
    if (s == "ORANGE")      return ORANGE;
    if (s == "PURPLE")      return PURPLE;
    if (s == "PINK")        return PINK;
    if (s == "RAYWHITE")    return RAYWHITE;
    if (s == "DARKGRAY")    return DARKGRAY;
    if (s == "DARKGREY")    return DARKGRAY;
    if (s == "GRAY")        return GRAY;
    if (s == "GREY")        return GRAY;
    if (s == "LIGHTGRAY")   return LIGHTGRAY;
    if (s == "LIGHTGREY")   return LIGHTGRAY;
    if (s == "DARKGREEN")   return DARKGREEN;
    if (s == "DARKBLUE")    return DARKBLUE;
    if (s == "DARKPURPLE")  return DARKPURPLE;
    if (s == "DARKBROWN")   return DARKBROWN;
    if (s == "BROWN")       return BROWN;
    if (s == "BEIGE")       return BEIGE;
    if (s == "MAGENTA")     return MAGENTA;
    if (s == "VIOLET")      return VIOLET;
    if (s == "SKYBLUE")     return SKYBLUE;
    if (s == "LIME")        return LIME;
    if (s == "GOLD")        return GOLD;
    if (s == "MAROON")      return MAROON;
    throw std::invalid_argument("Unknown colour: " + s);
}

std::string Colour::ColourToString(Color c) {
    // Raylib Color is {r, g, b, a} unsigned chars
    if (c.r == WHITE.r       && c.g == WHITE.g       && c.b == WHITE.b)       return "WHITE";
    if (c.r == BLACK.r       && c.g == BLACK.g       && c.b == BLACK.b)       return "BLACK";
    if (c.r == RED.r         && c.g == RED.g         && c.b == RED.b)         return "RED";
    if (c.r == GREEN.r       && c.g == GREEN.g       && c.b == GREEN.b)       return "GREEN";
    if (c.r == BLUE.r        && c.g == BLUE.g        && c.b == BLUE.b)        return "BLUE";
    if (c.r == YELLOW.r      && c.g == YELLOW.g      && c.b == YELLOW.b)      return "YELLOW";
    if (c.r == ORANGE.r      && c.g == ORANGE.g      && c.b == ORANGE.b)      return "ORANGE";
    if (c.r == PURPLE.r      && c.g == PURPLE.g      && c.b == PURPLE.b)      return "PURPLE";
    if (c.r == PINK.r        && c.g == PINK.g        && c.b == PINK.b)        return "PINK";
    if (c.r == RAYWHITE.r    && c.g == RAYWHITE.g    && c.b == RAYWHITE.b)    return "RAYWHITE";
    if (c.r == DARKGRAY.r    && c.g == DARKGRAY.g    && c.b == DARKGRAY.b)    return "DARKGRAY";
    if (c.r == GRAY.r        && c.g == GRAY.g        && c.b == GRAY.b)        return "GREY";
    if (c.r == LIGHTGRAY.r   && c.g == LIGHTGRAY.g   && c.b == LIGHTGRAY.b)   return "LIGHTGRAY";
    if (c.r == DARKGREEN.r   && c.g == DARKGREEN.g   && c.b == DARKGREEN.b)   return "DARKGREEN";
    if (c.r == DARKBLUE.r    && c.g == DARKBLUE.g    && c.b == DARKBLUE.b)    return "DARKBLUE";
    if (c.r == DARKPURPLE.r  && c.g == DARKPURPLE.g  && c.b == DARKPURPLE.b)  return "DARKPURPLE";
    if (c.r == DARKBROWN.r   && c.g == DARKBROWN.g   && c.b == DARKBROWN.b)   return "DARKBROWN";
    if (c.r == BROWN.r       && c.g == BROWN.g       && c.b == BROWN.b)       return "BROWN";
    if (c.r == BEIGE.r       && c.g == BEIGE.g       && c.b == BEIGE.b)       return "BEIGE";
    if (c.r == MAGENTA.r     && c.g == MAGENTA.g     && c.b == MAGENTA.b)     return "MAGENTA";
    if (c.r == VIOLET.r      && c.g == VIOLET.g      && c.b == VIOLET.b)      return "VIOLET";
    if (c.r == SKYBLUE.r     && c.g == SKYBLUE.g     && c.b == SKYBLUE.b)     return "SKYBLUE";
    if (c.r == LIME.r        && c.g == LIME.g        && c.b == LIME.b)        return "LIME";
    if (c.r == GOLD.r        && c.g == GOLD.g        && c.b == GOLD.b)        return "GOLD";
    if (c.r == MAROON.r      && c.g == MAROON.g      && c.b == MAROON.b)      return "MAROON";
    // Fallback: hex string for custom colours
    char buf[10];
    snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X", c.r, c.g, c.b, c.a);
    return std::string(buf);
}

void Colour::SetColour(const std::string& col) {
    const std::string norm = Normalise(col);
    if (ValidColours().count(norm) == 0)
        throw std::invalid_argument("Invalid colour name: " + col);
    value     = StringToColour(norm);
    name = norm;
}

void Colour::SetColour(Color col) {
    value     = col;
    name = ColourToString(col); // resolves to name or hex fallback
}

// --- Setters ---



// --- Getters ---

std::string Colour::GetColourString() const {
    return name;
}

Color Colour::GetColour() const {
    return value;
}

std::string Property::GetFxID() const {
    return fxID;
}

// --- Border ---
void Border::DrawMyself(const float& x, const float& y, const float& width, const float& height) const {
    if (thickness > 0) {
        if (drawMyself) {
            drawMyself(x, y, width, height);
        } else {
            DrawRectangleLinesEx({x - thickness, y - thickness, width + thickness, height + thickness}, thickness, colour.GetColour());
        }
    }
}

void Border::SetDrawingMethod(const std::function<void(const float& x, const float& y, const float& width, const float& height)>& drawMyself) {
    this->drawMyself = drawMyself;
}

void Border::RemoveDrawingMethod() {
    this->drawMyself = nullptr;
}

// --- Setters ---
void Border::SetThickness(const float& thickness) {
    if (thickness < 0) {
        throw std::range_error("Range error: Negative thickness.");
    } else {
        this->thickness = thickness;
    }
}

// --- Getters ---
float Border::GetThickness() const {
    return thickness;
}

//------Font--------

void CPPFX::Font::SetFontSize(const float& size) {
    if (size < 0) {
        throw std::invalid_argument("Negative font size.");
    } else {
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
    std::string normal = "";
    for (auto& c : alignment_string) {
        normal += toupper(c); //normalisation
    }

    if (normal == "TOP_CENTRE") {
        return Alignments::TOP_CENTRE;
    } else if (normal == "TOP_LEFT") {
        return Alignments::TOP_LEFT;
    } else if (normal == "TOP_RIGHT") {
        return Alignments::TOP_RIGHT;
    } else if (normal == "BOTTOM_LEFT") {
        return Alignments::BOTTOM_LEFT;
    } else if (normal == "BOTTOM_RIGHT") {
        return Alignments::BOTTOM_RIGHT;
    } else if (normal == "BOTTOM_CENTRE") {
        return Alignments::BOTTOM_CENTRE;
    } else if (normal == "CENTRE_LEFT") {
        return Alignments::CENTRE_LEFT;
    } else if (normal == "CENTRE_RIGHT") {
        return Alignments::CENTRE_RIGHT;
    } else if (normal == "CENTRE") {
        return Alignments::CENTRE;
    } else {
        throw std::invalid_argument("No alignment of such name found");
    }
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
