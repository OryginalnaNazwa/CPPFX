#include "../include/CPPFX/properties.hpp"
#include <ctype.h>          // for toupper
#include <algorithm>        // for transform
#include <stdexcept>        // for invalid_argument
#include <unordered_map>    // for unordered map
#include <charconv>         // for from_chars
#include <algorithm>        // for sort, unique
#include <memory>           // for shared_ptr
#include <sstream>          // for ostringstream
#include <set>              // for set
#include <vector>           // for vector

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
    name = ColourToString(value);
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
    name = ColourToString(value);
}

void Colour::SetDec() {
    hex = false;
    name = ColourToString(value);
}

void Colour::SetLiteralBase(bool hex) {
    this->hex = hex;
    name = ColourToString(value);
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

Vector2 Alignment::GetAlignedXY(float x, float y, float width, float height,
                         float contentWidth, float contentHeight) const {
    return Vector2{ GetAlignedX(x, contentWidth,  width),
                    GetAlignedY(y, contentHeight, height) };
}

//------Font--------

void CPPFX::Font::AppendCodepoint(std::string& text, int codepoint) {
    int size = 0;
    const char* utf8 = ::CodepointToUTF8(codepoint, &size);
    if (utf8 != nullptr && size > 0) {
        text.append(utf8, static_cast<std::size_t>(size));
    }
}

void CPPFX::Font::PopBackCodepoint(std::string& text) {
    if (text.empty()) return;
    // continuation bytes are 10xxxxxx - strip them, then the lead byte
    while (text.size() > 1 &&
           (static_cast<unsigned char>(text.back()) & 0xC0) == 0x80) {
        text.pop_back();
    }
    text.pop_back();
}


void CPPFX::Font::SetFilePath(const std::string& path) {
    if (path.empty()) {
        throw std::invalid_argument("In Font: The file name is empty.");
    }
    filePath = path;
}

std::string CPPFX::Font::GetFilePath() const {
    return filePath;
}

void CPPFX::Font::ClearFilePath() {
    filePath = "";
}

void CPPFX::Font::LoadFont() {
    if (filePath.empty()) {
        throw std::runtime_error("In Font: No file path set.");
    }
    LoadFont(filePath);
}

void CPPFX::Font::LoadFont(const std::string& fileName) {
    if (fileName.empty()) {
        throw std::invalid_argument("In Font: The file name is empty.");
    }
     if (!::FileExists(fileName.c_str())) {
        throw std::runtime_error("In Font: No file at " + fileName +
                                 " (working directory is " +
                                 ::GetWorkingDirectory() + ").");
    }
    if (codepoints.empty()) {
        AppendPreset(codepoints, Charset::LATIN_EXTENDED);
        NormaliseCharset(codepoints);
    }

    int size = (loadSize > 0) ? loadSize : static_cast<int>(fontSize);
    if (size <= 0) size = 20;

    ::Font loaded = ::LoadFontEx(fileName.c_str(), size,
                                 codepoints.data(),
                                 static_cast<int>(codepoints.size()));

    if (!::IsFontValid(loaded)) {
        ::UnloadFont(loaded);
        throw std::runtime_error("In Font: Font at " + fileName +
                                 " did not load correctly.");
    }

    // smoother when drawn at a size other than the one it was baked at
    ::SetTextureFilter(loaded.texture, TEXTURE_FILTER_BILINEAR);

    // The deleter is what makes this work: when the last Font sharing this
    // atlas goes away, the atlas is unloaded, exactly once.
    font = std::shared_ptr<::Font>(new ::Font(loaded),
                                   [](::Font* f) { ::UnloadFont(*f); delete f; });
    warnedCodepoints.clear();

#ifndef NDEBUG
    const std::vector<int> missing = ValidateCharset();
    if (!missing.empty()) {
        std::ostringstream out;
        out << fileName << " has no glyph for " << missing.size()
            << " requested codepoint(s):";
        std::size_t shown = 0;
        for (int cp : missing) {
            if (shown++ >= 12) { out << " ..."; break; }
            std::string glyph;
            AppendCodepoint(glyph, cp);
            out << " " << glyph;
        }
        CPPFX_WARN(out.str());
        for (int cp : missing) warnedCodepoints.insert(cp);
    }
#endif
}

void CPPFX::Font::UnloadFont() {
    font.reset();               // drops one share; unloads only if it was the last
    warnedCodepoints.clear();
}

bool CPPFX::Font::IsFontValid() const {
    return font && ::IsFontValid(*font);
}

bool CPPFX::Font::IsDefaultFont() const {
    return !font;
}

long CPPFX::Font::GetShareCount() const {
    return font ? static_cast<long>(font.use_count()) : 0L;
}

void CPPFX::Font::SetFont(const ::Font& newFont) {
    if (!::IsFontValid(newFont)) {
        throw std::invalid_argument("In Font: New font was not loaded correctly.");
    }
    // no-op deleter: somebody else owns this atlas, we only borrow it
    font = std::shared_ptr<::Font>(new ::Font(newFont),
                                   [](::Font* f) { delete f; });
    warnedCodepoints.clear();
}

::Font CPPFX::Font::GetFont() const {
    return Resolve();
}

void CPPFX::Font::ClearFont() {
    UnloadFont();
}

::Font CPPFX::Font::Resolve() const {
    return font ? *font : ::GetFontDefault();
}

void CPPFX::Font::SetFontSize(float size) {
    if (size < 0.0f) {
        throw std::invalid_argument("In Font: Negative font size.");
    }
    if (size == 0.0f) {
        CPPFX_WARN("Font size set to 0. It will not be visible.");
    }
    fontSize = size;
}

float CPPFX::Font::GetFontSize() const {
    return fontSize;
}

void CPPFX::Font::SetLoadSize(int size) {
    if (size < 0) {
        throw std::invalid_argument("In Font: Negative load size.");
    }
    loadSize = size;
    ReloadIfLoaded();
}

int CPPFX::Font::GetLoadSize() const {
    return loadSize;
}

int CPPFX::Font::GetBaseSize() const {
    return Resolve().baseSize;
}

float CPPFX::Font::GetScaleFactor() const {
    const ::Font f = Resolve();
    return (f.baseSize > 0) ? (fontSize / static_cast<float>(f.baseSize)) : 1.0f;
}

void CPPFX::Font::SetSpacing(float spacing) {
    this->spacing = spacing;
}

float CPPFX::Font::GetSpacing() const {
    // matches raylib's own DrawText, which uses fontSize / 10
    return (spacing < 0.0f) ? (fontSize / 10.0f) : spacing;
}

void CPPFX::Font::SetAutoSpacing() {
    spacing = -1.0f;
}

bool CPPFX::Font::IsAutoSpacing() const {
    return spacing < 0.0f;
}

void CPPFX::Font::SetLineSpacing(float spacing) {
    lineSpacing = spacing;
}

float CPPFX::Font::GetLineSpacing() const {
    return lineSpacing;
}

void CPPFX::Font::ApplyLineSpacing() const {
    // raylib keeps this in a global, so never assume it survived another widget
    ::SetTextLineSpacing(static_cast<int>(lineSpacing));
}

void CPPFX::Font::AppendPreset(std::vector<int>& target, Charset preset) {
    switch (preset) {
        case Charset::LATIN_EXTENDED:
            for (int c = 0x0100; c <= 0x017F; ++c) target.push_back(c);
            [[fallthrough]];
        case Charset::LATIN_1:
            for (int c = 0x00A0; c <= 0x00FF; ++c) target.push_back(c);
            break;
        case Charset::CYRILLIC:
            for (int c = 0x0400; c <= 0x04FF; ++c) target.push_back(c);
            break;
        case Charset::GREEK:
            for (int c = 0x0370; c <= 0x03FF; ++c) target.push_back(c);
            break;
        case Charset::ASCII:
        default:
            break;
    }
}

void CPPFX::Font::NormaliseCharset(std::vector<int>& target) {
    // ASCII is never optional - losing space or '?' is a miserable afternoon
    for (int c = 0x20; c <= 0x7E; ++c) target.push_back(c);
    std::sort(target.begin(), target.end());
    target.erase(std::unique(target.begin(), target.end()), target.end());
}

void CPPFX::Font::SetCharset(Charset preset) {
    std::vector<int> cps;
    AppendPreset(cps, preset);
    NormaliseCharset(cps);
    codepoints = cps;
    ReloadIfLoaded();
}

void CPPFX::Font::AddCharset(Charset preset) {
    AppendPreset(codepoints, preset);
    NormaliseCharset(codepoints);
    ReloadIfLoaded();
}

void CPPFX::Font::SetCharset(const std::string& sampleText) {
    if (sampleText.empty()) {
        throw std::invalid_argument("In Font: The charset sample is empty.");
    }
    int count = 0;
    int* cps = ::LoadCodepoints(sampleText.c_str(), &count);
    std::vector<int> result(cps, cps + count);   // LoadCodepoints does not dedupe
    ::UnloadCodepoints(cps);

    NormaliseCharset(result);
    codepoints = result;
    ReloadIfLoaded();
}

void CPPFX::Font::AddCharset(const std::string& sampleText) {
    if (sampleText.empty()) {
        throw std::invalid_argument("In Font: The charset sample is empty.");
    }
    int count = 0;
    int* cps = ::LoadCodepoints(sampleText.c_str(), &count);
    codepoints.insert(codepoints.end(), cps, cps + count);
    ::UnloadCodepoints(cps);

    NormaliseCharset(codepoints);
    ReloadIfLoaded();
}

void CPPFX::Font::SetCharset(const std::vector<int>& codepoints) {
    if (codepoints.empty()) {
        throw std::invalid_argument("In Font: The charset is empty.");
    }
    std::vector<int> result = codepoints;
    NormaliseCharset(result);
    this->codepoints = result;
    ReloadIfLoaded();
}

std::vector<int> CPPFX::Font::GetCharset() const {
    return codepoints;
}

void CPPFX::Font::ClearCharset() {
    SetCharset(Charset::LATIN_EXTENDED);
}

void CPPFX::Font::ReloadIfLoaded() {
    // only meaningful when we loaded it ourselves and still know from where
    if (font && !filePath.empty()) {
        LoadFont(filePath);
    }
}

bool CPPFX::Font::HasGlyph(int codepoint) const {
    const ::Font f = Resolve();
    if (f.texture.id == 0 || f.glyphCount <= 0) return false;

    const int i = ::GetGlyphIndex(f, codepoint);
    if (i < 0 || i >= f.glyphCount) return false;

    // GetGlyphIndex falls back to '?' on a miss, so check we got what we asked for
    if (f.glyphs[i].value != codepoint) return false;

    if (codepoint == ' ' || codepoint == 0x00A0 || codepoint == 0x00AD) return true;

    // present in the atlas but rasterised empty - the file had no such glyph
    return f.recs[i].width > 0.0f || f.glyphs[i].advanceX > 0;
}

std::vector<int> CPPFX::Font::FindMissingGlyphs(const std::string& text) const {
    std::set<int> missing;
    const char* ptr = text.c_str();
    while (*ptr != '\0') {
        int size = 0;
        const int cp = ::GetCodepointNext(ptr, &size);
        if (cp != '\n' && cp != '\t' && !HasGlyph(cp)) {
            missing.insert(cp);
        }
        ptr += (size > 0) ? size : 1;
    }
    return std::vector<int>(missing.begin(), missing.end());
}

bool CPPFX::Font::CanRender(const std::string& text) const {
    return FindMissingGlyphs(text).empty();
}

std::vector<int> CPPFX::Font::ValidateCharset() const {
    std::vector<int> missing;
    if (!font) return missing;
    for (int cp : codepoints) {
        if (!HasGlyph(cp)) missing.push_back(cp);
    }
    return missing;
}

void CPPFX::Font::WarnAboutMissingGlyphs(const std::string& text,
                                         const std::string& context) const {
#ifdef NDEBUG
    (void)text;
    (void)context;
#else
    const std::vector<int> missing = FindMissingGlyphs(text);
    std::ostringstream out;
    std::size_t fresh = 0;

    for (int cp : missing) {
        if (!warnedCodepoints.insert(cp).second) continue;   // already moaned about it
        std::string glyph;
        AppendCodepoint(glyph, cp);
        out << " " << glyph << " (U+" << std::uppercase << std::hex << cp
            << std::dec << ")";
        ++fresh;
    }

    if (fresh > 0) {
        CPPFX_WARN((context.empty() ? std::string("Font") : context)
                   << " cannot render:" << out.str()
                   << ". Widen the charset before loading, or use a font that has them.");
    }
#endif
}

void CPPFX::Font::DrawText(const std::string& text, float x, float y,
                           const Color& tint) const {
    const ::Font f = Resolve();
    if (f.texture.id == 0) return;   // no GL context yet, nothing to draw with
    ApplyLineSpacing();
    ::DrawTextEx(f, text.c_str(), Vector2{x, y}, fontSize, GetSpacing(), tint);
}

void CPPFX::Font::DrawText(const std::string& text, float x, float y) const {
    DrawText(text, x, y, colour.GetColour());
}

void CPPFX::Font::DrawText(const std::string& text, const Vector2& position) const {
    DrawText(text, position.x, position.y, colour.GetColour());
}

void CPPFX::Font::DrawTextPro(const std::string& text, const Vector2& position,
                              const Vector2& origin, float rotation) const {
    const ::Font f = Resolve();
    if (f.texture.id == 0) return;
    ApplyLineSpacing();
    ::DrawTextPro(f, text.c_str(), position, origin, rotation,
                  fontSize, GetSpacing(), colour.GetColour());
}

Vector2 CPPFX::Font::GetInkSize(const std::string& text) const {
    return Vector2{ MeasureTextWidth(text), GetCapHeight() };
}

void CPPFX::Font::DrawTextAt(const std::string& text, const Vector2& inkTopLeft,
                             const Color& tint) const {
    // caller gave us where the ink goes; DrawTextEx wants the line box top
    DrawText(text, inkTopLeft.x, inkTopLeft.y - GetCapOffset(), tint);
}

void CPPFX::Font::DrawTextAt(const std::string& text, const Vector2& inkTopLeft) const {
    DrawTextAt(text, inkTopLeft, colour.GetColour());
}

Vector2 CPPFX::Font::MeasureText(const std::string& text) const {
    const ::Font f = Resolve();
    if (f.texture.id == 0) return Vector2{0.0f, 0.0f};
    ApplyLineSpacing();
    return ::MeasureTextEx(f, text.c_str(), fontSize, GetSpacing());
}

float CPPFX::Font::MeasureTextWidth(const std::string& text) const {
    return MeasureText(text).x;
}

float CPPFX::Font::MeasureTextHeight(const std::string& text) const {
    return MeasureText(text).y;
}

float CPPFX::Font::GetCapHeight() const {
    const ::Font f = Resolve();
    if (f.texture.id == 0) return 0.0f;
    const int i = ::GetGlyphIndex(f, 'H');
    return f.recs[i].height * GetScaleFactor();
}

float CPPFX::Font::GetCapOffset() const {
    const ::Font f = Resolve();
    if (f.texture.id == 0) return 0.0f;
    const int i = ::GetGlyphIndex(f, 'H');
    return static_cast<float>(f.glyphs[i].offsetY) * GetScaleFactor();
}

float CPPFX::Font::GetVerticalCentreOffset(float boxHeight) const {
    return ((boxHeight - GetCapHeight()) / 2.0f) - GetCapOffset();
}

void CPPFX::Font::DrawAligned(const std::string& text, const Alignment& alignment,
                              float x, float y, float width, float height,
                              const Color& tint) const {
    const Vector2 ink = GetInkSize(text);
    DrawTextAt(text, alignment.GetAlignedXY(x, y, width, height, ink.x, ink.y), tint);
}

void CPPFX::Font::DrawAligned(const std::string& text, const Alignment& alignment,
                              float x, float y, float width, float height) const {
    DrawAligned(text, alignment, x, y, width, height, colour.GetColour());
}
