#include "harness.hpp"

#include <CPPFX/properties.hpp>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

/**********************************************************************
 *  Tests for CPPFX::Font that need a real atlas, and therefore a
 *  window.
 *
 *  These live in their own binary rather than alongside the headless
 *  tests, for a reason that is not obvious: raylib's UnloadFontDefault
 *  clears the default font's glyphs and recs but leaves texture.id set.
 *  So once any test in a process has opened and closed a window, every
 *  later "no GL context" assertion is testing a stale, half-torn-down
 *  default font instead of a clean one. Separate processes keep both
 *  sets honest.
 *
 *  Every test opens its own 64x64 window and closes it again. If no
 *  display is available the test skips rather than fails, so a headless
 *  runner without xvfb reports honestly instead of going red.
 **********************************************************************/

using CPPFX::Font;

namespace {

/// Absolute path to tests/assets, injected by CMake so ctest's working
/// directory does not matter.
std::string Asset(const std::string& name) {
    return std::string(CPPFX_TEST_ASSETS) + "/" + name;
}

const char* FONT_A   = "test_a.ttf";
const char* FONT_B   = "test_b.ttf";
const char* NOT_FONT = "notafont.txt";

/// True when InitWindow has any chance of succeeding. raylib logs a warning
/// and returns on failure rather than exiting, but checking first keeps the
/// output clean.
bool DisplayAvailable() {
    if (std::getenv("CPPFX_TESTS_NO_WINDOW") != nullptr) return false;
#if defined(__linux__)
    return std::getenv("DISPLAY") != nullptr ||
           std::getenv("WAYLAND_DISPLAY") != nullptr;
#else
    return true;
#endif
}

/// Opens a window for the duration of a test and closes it afterwards,
/// including when a check throws.
struct WindowFixture {
    WindowFixture() {
        SetTraceLogLevel(LOG_NONE);          // raylib's chatter is not our output
        InitWindow(64, 64, "CPPFX tests");
    }
    ~WindowFixture() {
        if (IsWindowReady()) CloseWindow();
    }
    bool ok() const { return IsWindowReady(); }
};

const int CJK   = 0x4E00;   // no Latin font has this
const int EURO  = 0x20AC;

}

/// Opens a window, or skips the test if the environment cannot.
#define NEEDS_WINDOW(name)                                                     \
    if (!DisplayAvailable()) SKIP("no display");                               \
    WindowFixture name;                                                        \
    if (!name.ok()) SKIP("window could not be created")

// --- loading -------------------------------------------------------------

TEST(font_loads_from_path) {
    NEEDS_WINDOW(window);

    Font f;
    f.SetFilePath(Asset(FONT_A));
    f.SetLoadSize(48);
    f.LoadFont();

    CHECK(f.IsFontValid());
    CHECK(!f.IsDefaultFont());
    CHECK_EQ(f.GetBaseSize(), 48);
    CHECK_EQ(f.GetLoadedPath(), Asset(FONT_A));
}

TEST(font_load_size_defaults_to_font_size) {
    NEEDS_WINDOW(window);

    Font f(36.0f);
    f.LoadFont(Asset(FONT_A));
    CHECK_EQ(f.GetBaseSize(), 36);
}

/// The one that matters: raylib quietly substitutes the default font when it
/// cannot parse a file, and that font passes IsFontValid. Loading a text file
/// must throw rather than silently succeed - and must not leave the shared
/// default atlas filtered.
TEST(font_rejects_the_default_font_fallback) {
    NEEDS_WINDOW(window);

    Font f;
    CHECK_THROWS_AS(f.LoadFont(Asset(NOT_FONT)), std::runtime_error);
    CHECK(f.IsDefaultFont());
    CHECK_EQ(f.GetShareCount(), 0L);
    CHECK_EQ(f.GetLoadedPath(), std::string(""));
}

TEST(font_unload_falls_back_to_default) {
    NEEDS_WINDOW(window);

    Font f;
    f.LoadFont(Asset(FONT_A));
    CHECK(!f.IsDefaultFont());

    f.UnloadFont();
    CHECK(f.IsDefaultFont());
    CHECK_EQ(f.GetShareCount(), 0L);
    CHECK_EQ(f.GetLoadedPath(), std::string(""));
    CHECK_NEAR(f.MeasureTextWidth("Hello") > 0.0f ? 1.0f : 0.0f, 1.0f, 0.001f);
}

// --- sharing -------------------------------------------------------------

TEST(font_atlas_is_reference_counted) {
    NEEDS_WINDOW(window);

    Font original;
    original.LoadFont(Asset(FONT_A));
    CHECK_EQ(original.GetShareCount(), 1L);

    {
        Font copy = original;
        CHECK_EQ(original.GetShareCount(), 2L);
        CHECK_EQ(copy.GetShareCount(),     2L);
        CHECK_EQ(copy.GetFont().texture.id, original.GetFont().texture.id);

        copy.UnloadFont();                       // one share released
        CHECK_EQ(original.GetShareCount(), 1L);
        CHECK(original.IsFontValid());           // the atlas survived
    }

    CHECK_EQ(original.GetShareCount(), 1L);
    CHECK(original.IsFontValid());
}

TEST(font_copy_outliving_the_original_keeps_the_atlas) {
    NEEDS_WINDOW(window);

    Font survivor;
    {
        Font original;
        original.LoadFont(Asset(FONT_A));
        survivor = original;
    }
    CHECK(survivor.IsFontValid());
    CHECK_EQ(survivor.GetShareCount(), 1L);
    CHECK(survivor.MeasureTextWidth("Hello") > 0.0f);
}

// --- borrowed fonts ------------------------------------------------------

TEST(font_borrowed_atlas_is_not_reloaded_over) {
    NEEDS_WINDOW(window);

    ::Font raw = ::LoadFontEx(Asset(FONT_A).c_str(), 32, nullptr, 0);
    CHECK(::IsFontValid(raw));

    Font f;
    f.SetFilePath(Asset(FONT_B));                // a stale path from earlier setup
    f.SetFont(raw);
    CHECK(f.IsFontValid());
    CHECK_EQ(f.GetLoadedPath(), std::string(""));   // borrowed, not owned

    const unsigned int before = f.GetFont().texture.id;
    f.SetCharset(Font::Charset::GREEK);          // must not reload from filePath
    CHECK_EQ(f.GetFont().texture.id, before);

    ::UnloadFont(raw);                           // we still own it
}

TEST(font_reload_uses_the_file_actually_loaded) {
    NEEDS_WINDOW(window);

    Font f;
    f.SetFilePath(Asset(FONT_A));
    f.LoadFont();
    f.LoadFont(Asset(FONT_B));                   // overload does not store the path
    CHECK_EQ(f.GetLoadedPath(), Asset(FONT_B));

    f.SetCharset(Font::Charset::GREEK);          // triggers a reload
    CHECK_EQ(f.GetLoadedPath(), Asset(FONT_B));  // still B, not the stored A
    CHECK_EQ(f.GetFilePath(),   Asset(FONT_A));  // and the stored path is untouched
}

TEST(font_load_size_change_reloads_the_atlas) {
    NEEDS_WINDOW(window);

    Font f;
    f.LoadFont(Asset(FONT_A));
    f.SetLoadSize(64);
    CHECK_EQ(f.GetBaseSize(), 64);
    CHECK(f.IsFontValid());
}

// --- glyph queries -------------------------------------------------------

TEST(font_reports_glyphs_it_has) {
    NEEDS_WINDOW(window);

    Font f;
    f.SetCharset("ąćęłńóśźż");
    f.LoadFont(Asset(FONT_A));

    CHECK(f.HasGlyph('A'));
    CHECK(f.HasGlyph(' '));
    CHECK(f.HasGlyph(0x0142));                   // ł, asked for
    CHECK(!f.HasGlyph(CJK));                     // never asked for
    CHECK(f.CanRender("Hello ąćę"));
    CHECK(!f.CanRender("Hello 一"));
}

TEST(font_finds_missing_glyphs) {
    NEEDS_WINDOW(window);

    Font f;
    f.SetCharset(Font::Charset::ASCII);
    f.LoadFont(Asset(FONT_A));

    const std::vector<int> missing = f.FindMissingGlyphs("aa一b一");
    CHECK_EQ(missing.size(), std::size_t(1));    // deduplicated
    CHECK_EQ(missing.front(), CJK);
}

/// Newlines, tabs and carriage returns are layout, not glyphs. CRLF text
/// arrives from clipboards and Windows files constantly and must not be
/// reported as unrenderable.
TEST(font_ignores_layout_characters) {
    NEEDS_WINDOW(window);

    Font f;
    f.LoadFont(Asset(FONT_A));
    CHECK(f.FindMissingGlyphs("first\r\nsecond\tthird").empty());
    CHECK(f.CanRender("first\r\nsecond\tthird"));
}

/// Asking for a codepoint the file has no glyph for is different from not
/// asking for it: it gets baked in blank, and ValidateCharset is what finds it.
TEST(font_validate_charset_finds_blank_glyphs) {
    NEEDS_WINDOW(window);

    Font f;
    f.SetCharset(std::vector<int>{CJK});
    f.LoadFont(Asset(FONT_A));

    const std::vector<int> missing = f.ValidateCharset();
    CHECK_EQ(missing.size(), std::size_t(1));
    CHECK_EQ(missing.front(), CJK);
}

// --- metrics -------------------------------------------------------------

TEST(font_cap_height_is_less_than_font_size) {
    NEEDS_WINDOW(window);

    Font f(40.0f);
    f.LoadFont(Asset(FONT_A));

    const float cap = f.GetCapHeight();
    CHECK(cap > 0.0f);
    CHECK(cap < f.GetFontSize());                // the em box includes descender space
    CHECK(f.GetCapOffset() >= 0.0f);
    CHECK(f.GetCapOffset() < f.GetFontSize());
}

TEST(font_single_line_ink_height_is_cap_height) {
    NEEDS_WINDOW(window);

    Font f(40.0f);
    f.LoadFont(Asset(FONT_A));
    CHECK_NEAR(f.GetInkSize("Hello").y, f.GetCapHeight(), 0.01f);
}

/// The multiline fix: ink height has to grow per line, or centre and bottom
/// alignment place a block of text as though it were one line tall.
TEST(font_multiline_ink_height_grows_per_line) {
    NEEDS_WINDOW(window);

    Font f(40.0f);
    f.LoadFont(Asset(FONT_A));
    f.SetLineSpacing(0.0f);

    const float one   = f.GetInkSize("A").y;
    const float two   = f.GetInkSize("A\nB").y;
    const float three = f.GetInkSize("A\nB\nC").y;

    CHECK_NEAR(two,   one + f.GetFontSize(),         0.01f);
    CHECK_NEAR(three, one + (2.0f * f.GetFontSize()), 0.01f);
}

TEST(font_line_spacing_widens_the_gap) {
    NEEDS_WINDOW(window);

    Font f(40.0f);
    f.LoadFont(Asset(FONT_A));

    f.SetLineSpacing(0.0f);
    const float tight = f.GetInkSize("A\nB").y;
    f.SetLineSpacing(10.0f);
    const float loose = f.GetInkSize("A\nB").y;

    CHECK_NEAR(loose - tight, 10.0f, 0.01f);
}

TEST(font_multiline_width_is_the_widest_line) {
    NEEDS_WINDOW(window);

    Font f;
    f.LoadFont(Asset(FONT_A));
    CHECK_NEAR(f.GetInkSize("A\nWWWW").x, f.MeasureTextWidth("WWWW"), 0.01f);
}

TEST(font_measured_width_grows_with_size_and_spacing) {
    NEEDS_WINDOW(window);

    Font f(20.0f);
    f.SetLoadSize(64);                           // fixed atlas, so only the size varies
    f.LoadFont(Asset(FONT_A));

    const float small = f.MeasureTextWidth("Hello");
    f.SetFontSize(40.0f);
    CHECK(f.MeasureTextWidth("Hello") > small);

    const float automatic = f.MeasureTextWidth("Hello");
    f.SetSpacing(20.0f);
    CHECK(f.MeasureTextWidth("Hello") > automatic);
}

TEST(font_empty_text_measures_zero_with_an_atlas) {
    NEEDS_WINDOW(window);

    Font f;
    f.LoadFont(Asset(FONT_A));
    const Vector2 ink = f.GetInkSize("");
    CHECK_NEAR(ink.x, 0.0f, 0.001f);
    CHECK_NEAR(ink.y, 0.0f, 0.001f);             // not negative
}
