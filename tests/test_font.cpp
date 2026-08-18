#include "harness.hpp"

#include <CPPFX/properties.hpp>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

/**********************************************************************
 *  Tests for CPPFX::Font that need no GL context.
 *
 *  Everything here runs before any window exists, so it covers the
 *  default-font fallback, argument validation, charset bookkeeping and
 *  the UTF-8 helpers. Anything that needs a real atlas lives in
 *  tests/gl/test_font_gl.cpp, in a separate binary - see the comment
 *  at the top of that file for why the split is not optional.
 **********************************************************************/

namespace fx = CPPFX;

namespace {

bool Has(const std::vector<int>& cps, int cp) {
    return std::find(cps.begin(), cps.end(), cp) != cps.end();
}

/// Charsets are promised sorted and deduplicated. This checks both at once.
bool SortedUnique(const std::vector<int>& cps) {
    return std::adjacent_find(cps.begin(), cps.end(),
                              std::greater_equal<int>()) == cps.end();
}

const int A_ACUTE   = 0x00E1;   // á, in LATIN_1
const int L_STROKE  = 0x0142;   // ł, in LATIN_EXTENDED only
const int CYR_A     = 0x0430;   // а
const int GREEK_A   = 0x03B1;   // α
const int EURO      = 0x20AC;   // €, three bytes in UTF-8

}

// --- defaults ------------------------------------------------------------

TEST(font_default_state) {
    fx::Font f;
    CHECK(f.IsDefaultFont());
    CHECK(!f.IsFontValid());
    CHECK_EQ(f.GetShareCount(), 0L);
    CHECK_NEAR(f.GetFontSize(), 20.0f, 0.001f);
    CHECK_EQ(f.GetLoadSize(), 0);
    CHECK_NEAR(f.GetLineSpacing(), 0.0f, 0.001f);
    CHECK_EQ(f.GetFilePath(), std::string(""));
}

TEST(font_default_spacing_is_automatic) {
    fx::Font f;
    CHECK(f.IsAutoSpacing());
    CHECK_NEAR(f.GetSpacing(), 2.0f, 0.001f);   // fontSize / 10, as raylib does

    f.SetFontSize(50.0f);
    CHECK_NEAR(f.GetSpacing(), 5.0f, 0.001f);   // follows the size while automatic

    f.SetSpacing(3.0f);
    CHECK(!f.IsAutoSpacing());
    CHECK_NEAR(f.GetSpacing(), 3.0f, 0.001f);

    f.SetAutoSpacing();
    CHECK(f.IsAutoSpacing());
    CHECK_NEAR(f.GetSpacing(), 5.0f, 0.001f);
}

TEST(font_size_constructors) {
    fx::Font sized(32.0f);
    CHECK_NEAR(sized.GetFontSize(), 32.0f, 0.001f);

    fx::Font tinted(RED);
    CHECK_NEAR(tinted.GetFontSize(), 20.0f, 0.001f);
    CHECK_EQ(tinted.colour.GetColourString(), std::string("RED"));

    fx::Font both(12.0f, BLUE);
    CHECK_NEAR(both.GetFontSize(), 12.0f, 0.001f);
    CHECK_EQ(both.colour.GetColourString(), std::string("BLUE"));
}

// --- validation ----------------------------------------------------------

TEST(font_rejects_negative_sizes) {
    fx::Font f;
    CHECK_THROWS_AS(f.SetFontSize(-1.0f), std::invalid_argument);
    CHECK_THROWS_AS(f.SetLoadSize(-1),    std::invalid_argument);
    CHECK_THROWS_AS(fx::Font(-1.0f),          std::invalid_argument);

    CHECK_NEAR(f.GetFontSize(), 20.0f, 0.001f);   // unchanged after the throw
}

TEST(font_zero_size_is_allowed_but_warned) {
    fx::Font f;
    f.SetFontSize(0.0f);                          // warns in debug, does not throw
    CHECK_NEAR(f.GetFontSize(), 0.0f, 0.001f);
}

TEST(font_rejects_empty_paths) {
    fx::Font f;
    CHECK_THROWS_AS(f.SetFilePath(""), std::invalid_argument);
    CHECK_THROWS_AS(f.LoadFont(""),    std::invalid_argument);
    CHECK_THROWS_AS(f.LoadFont(),      std::runtime_error);   // no path set
}

TEST(font_rejects_missing_file) {
    fx::Font f;
    f.SetFilePath("definitely/not/a/font.ttf");
    CHECK_EQ(f.GetFilePath(), std::string("definitely/not/a/font.ttf"));
    CHECK_THROWS_AS(f.LoadFont(), std::runtime_error);
    CHECK(f.IsDefaultFont());                     // failure left it alone

    f.ClearFilePath();
    CHECK_EQ(f.GetFilePath(), std::string(""));
}

TEST(font_rejects_invalid_borrowed_font) {
    fx::Font f;
    ::Font empty = {0};
    CHECK_THROWS_AS(f.SetFont(empty), std::invalid_argument);
    CHECK(f.IsDefaultFont());
}

// --- charsets ------------------------------------------------------------

TEST(font_default_charset_is_latin_extended) {
    fx::Font f;
    const std::vector<int> cps = f.GetCharset();
    CHECK(Has(cps, 'A'));
    CHECK(Has(cps, ' '));
    CHECK(Has(cps, A_ACUTE));
    CHECK(Has(cps, L_STROKE));
    CHECK(!Has(cps, CYR_A));
    CHECK(SortedUnique(cps));
}

TEST(font_ascii_preset_is_ascii_only) {
    fx::Font f;
    f.SetCharset(fx::Font::Charset::ASCII);
    const std::vector<int> cps = f.GetCharset();
    CHECK_EQ(cps.size(), std::size_t(95));        // U+0020 to U+007E
    CHECK_EQ(cps.front(), 0x20);
    CHECK_EQ(cps.back(),  0x7E);
}

TEST(font_presets_always_keep_ascii) {
    fx::Font f;
    f.SetCharset(fx::Font::Charset::CYRILLIC);
    CHECK(Has(f.GetCharset(), CYR_A));
    CHECK(Has(f.GetCharset(), 'A'));              // ASCII is never optional
    CHECK(!Has(f.GetCharset(), L_STROKE));        // Set replaces, it does not add

    f.SetCharset(fx::Font::Charset::GREEK);
    CHECK(Has(f.GetCharset(), GREEK_A));
    CHECK(!Has(f.GetCharset(), CYR_A));
}

TEST(font_add_charset_unions) {
    fx::Font f;
    f.SetCharset(fx::Font::Charset::CYRILLIC);
    f.AddCharset(fx::Font::Charset::GREEK);
    const std::vector<int> cps = f.GetCharset();
    CHECK(Has(cps, CYR_A));
    CHECK(Has(cps, GREEK_A));
    CHECK(Has(cps, 'A'));
    CHECK(SortedUnique(cps));
}

TEST(font_charset_from_sample_text) {
    fx::Font f;
    f.SetCharset("żółw");
    const std::vector<int> cps = f.GetCharset();
    CHECK(Has(cps, 0x017C));                      // ż
    CHECK(Has(cps, 0x00F3));                      // ó
    CHECK(Has(cps, 0x0142));                      // ł
    CHECK(Has(cps, 'w'));
    CHECK(Has(cps, 'A'));                         // ASCII still injected
    CHECK(!Has(cps, CYR_A));
    CHECK(SortedUnique(cps));
}

TEST(font_charset_sample_deduplicates) {
    fx::Font f;
    f.SetCharset("aaaąąą");
    const std::vector<int> cps = f.GetCharset();
    CHECK(SortedUnique(cps));
    CHECK_EQ(std::count(cps.begin(), cps.end(), int('a')), std::ptrdiff_t(1));
    CHECK_EQ(std::count(cps.begin(), cps.end(), 0x0105), std::ptrdiff_t(1));
}

TEST(font_charset_from_codepoints) {
    fx::Font f;
    f.SetCharset(std::vector<int>{EURO, CYR_A, EURO});
    const std::vector<int> cps = f.GetCharset();
    CHECK(Has(cps, EURO));
    CHECK(Has(cps, CYR_A));
    CHECK(SortedUnique(cps));
}

TEST(font_rejects_empty_charsets) {
    fx::Font f;
    CHECK_THROWS_AS(f.SetCharset(std::string("")),  std::invalid_argument);
    CHECK_THROWS_AS(f.AddCharset(std::string("")),  std::invalid_argument);
    CHECK_THROWS_AS(f.SetCharset(std::vector<int>{}), std::invalid_argument);
}

TEST(font_clear_charset_restores_default) {
    fx::Font f;
    f.SetCharset(fx::Font::Charset::CYRILLIC);
    f.ClearCharset();
    CHECK(Has(f.GetCharset(), L_STROKE));
    CHECK(!Has(f.GetCharset(), CYR_A));
}

// --- UTF-8 helpers -------------------------------------------------------

TEST(font_append_codepoint_encodes_utf8) {
    std::string text;
    fx::Font::AppendCodepoint(text, 'A');
    CHECK_EQ(text, std::string("A"));
    CHECK_EQ(text.size(), std::size_t(1));

    fx::Font::AppendCodepoint(text, L_STROKE);
    CHECK_EQ(text.size(), std::size_t(3));        // one byte plus two

    fx::Font::AppendCodepoint(text, EURO);
    CHECK_EQ(text.size(), std::size_t(6));        // plus three
}

TEST(font_pop_back_removes_one_character) {
    std::string text;
    fx::Font::AppendCodepoint(text, 'a');
    fx::Font::AppendCodepoint(text, L_STROKE);
    fx::Font::AppendCodepoint(text, EURO);
    CHECK_EQ(text.size(), std::size_t(6));

    fx::Font::PopBackCodepoint(text);                 // the three-byte one
    CHECK_EQ(text.size(), std::size_t(3));
    fx::Font::PopBackCodepoint(text);                 // the two-byte one
    CHECK_EQ(text, std::string("a"));
    fx::Font::PopBackCodepoint(text);
    CHECK(text.empty());
}

TEST(font_pop_back_is_safe_on_empty) {
    std::string text;
    fx::Font::PopBackCodepoint(text);                 // must not read past the end
    CHECK(text.empty());
}

// --- behaviour with no GL context ----------------------------------------
// These pin the guards that keep a fx::Font usable before InitWindow, so that
// constructing widgets at static-init time cannot crash.

TEST(font_measures_zero_without_a_context) {
    fx::Font f;
    CHECK_NEAR(f.MeasureTextWidth("Hello"),  0.0f, 0.001f);
    CHECK_NEAR(f.MeasureTextHeight("Hello"), 0.0f, 0.001f);
    CHECK_NEAR(f.GetCapHeight(),             0.0f, 0.001f);
    CHECK_NEAR(f.GetCapOffset(),             0.0f, 0.001f);
}

TEST(font_ink_size_of_empty_text_is_zero) {
    fx::Font f;
    const Vector2 ink = f.GetInkSize("");
    CHECK_NEAR(ink.x, 0.0f, 0.001f);
    CHECK_NEAR(ink.y, 0.0f, 0.001f);              // never negative, never thrown
}

TEST(font_drawing_without_a_context_is_a_no_op) {
    fx::Font f;
    f.DrawText("Hello", 0.0f, 0.0f);              // must not crash
    f.DrawTextAt("Hello", Vector2{0.0f, 0.0f});
    f.DrawTextPro("Hello", Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}, 45.0f);
    CHECK(true);
}

TEST(font_glyph_queries_are_false_without_a_context) {
    fx::Font f;
    CHECK(!f.HasGlyph('A'));
    CHECK(f.ValidateCharset().empty());           // nothing loaded, nothing to report
}
