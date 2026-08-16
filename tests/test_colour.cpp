#include "harness.hpp"

#include <CPPFX/properties.hpp>

#include <stdexcept>
#include <string>

/**********************************************************************
 *  Tests for CPPFX::Colour.
 *
 *  None of this touches raylib beyond the Color struct and the colour
 *  defines, so it runs headless - no window, no xvfb, works in CI.
 **********************************************************************/

using CPPFX::Colour;

namespace {

/// raylib's Color has no operator==, so compare the channels.
bool Same(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

/// Printable form, so CHECK_EQ can show what a failing colour actually was.
std::string Show(Color c) {
    return "{" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", "
               + std::to_string(c.b) + ", " + std::to_string(c.a) + "}";
}

}

// --- names ---------------------------------------------------------------

TEST(colour_default_is_light_grey) {
    Colour c;
    CHECK_EQ(c.GetColourString(), std::string("LIGHTGRAY"));
    CHECK(Same(c.GetColour(), LIGHTGRAY));
}

TEST(colour_name_to_value) {
    CHECK_EQ(Show(Colour("RED").GetColour()), Show(RED));
    CHECK_EQ(Show(Colour("BLANK").GetColour()), Show(BLANK));
}

TEST(colour_value_to_name) {
    CHECK_EQ(Colour(RED).GetColourString(), std::string("RED"));
    CHECK_EQ(Colour(BLANK).GetColourString(), std::string("BLANK"));
}

TEST(colour_british_spellings_normalise) {
    CHECK_EQ(Colour("GREY").GetColourString(), std::string("GRAY"));
    CHECK_EQ(Colour("DARKGREY").GetColourString(), std::string("DARKGRAY"));
    CHECK_EQ(Colour("LIGHTGREY").GetColourString(), std::string("LIGHTGRAY"));
}

TEST(colour_ignores_capitalisation) {
    CHECK(Same(Colour("red").GetColour(), RED));
    CHECK(Same(Colour("Red").GetColour(), RED));
    CHECK(Same(Colour("rEd").GetColour(), RED));
    CHECK(Same(Colour("grey").GetColour(), GRAY));
}

TEST(colour_setter_normalises_like_the_constructor) {
    // NOTE: currently fails. SetColour stores the input verbatim
    // (name = norm) instead of running it back through ColourToString,
    // so the setter and the constructor disagree.
    Colour c;
    c.SetColour("GREY");
    CHECK_EQ(c.GetColourString(), std::string("GRAY"));

    c.SetColour("#RFFG00B00");
    CHECK_EQ(c.GetColourString(), std::string("RED"));
}

// --- comparison ----------------------------------------------------------

TEST(colour_blank_is_not_black) {
    CHECK(Colour(BLANK) != Colour(BLACK));
}

TEST(colour_equality_ignores_names) {
    // Same value reached two different ways must compare equal.
    CHECK(Colour("GRAY") == Colour(GRAY));
    CHECK(Colour("GREY") == Colour("GRAY"));
    CHECK(Colour(RED) == Colour("RED"));
}

// --- literals ------------------------------------------------------------

TEST(colour_hex_literals) {
    CHECK(Same(Colour("#RFFG00B00").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#R00G00B00A80").GetColour(), Color{0, 0, 0, 128}));
}

TEST(colour_dec_literals) {
    CHECK(Same(Colour("#R255G000B000").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#R000G000B000A128").GetColour(), Color{0, 0, 0, 128}));
}

TEST(colour_both_bases_give_the_same_value) {
    CHECK(Colour("#RFFG00B00") == Colour("#R255G000B000"));
}

TEST(colour_alpha_defaults_to_opaque) {
    CHECK_EQ(int(Colour("#RFFG00B00").GetColour().a), 255);
    CHECK_EQ(int(Colour("#R255G000B000").GetColour().a), 255);
}

TEST(colour_unnamed_value_reports_as_a_literal) {
    Colour c(Color{1, 2, 3, 255});
    // Default base is decimal, three digits per channel, alpha omitted at 255.
    CHECK_EQ(c.GetColourString(), std::string("#R001G002B003"));
}

TEST(colour_literal_round_trips) {
    const Colour original(Color{1, 2, 3, 200});
    CHECK(Colour(original.GetColourString()) == original);
}

TEST(colour_base_flag_is_reflected_in_the_name) {
    // NOTE: currently fails. hex is only read inside ColourToString, which
    // runs when the colour is set - GetColourString just returns the stored
    // name, so flipping the base after the fact changes nothing.
    Colour c(Color{1, 2, 3, 255});
    c.SetDec();
    CHECK_EQ(c.GetColourString(), std::string("#R001G002B003"));
    c.SetHex();
    CHECK_EQ(c.GetColourString(), std::string("#R01G02B03"));
}

TEST(colour_base_flag_does_not_affect_input) {
    // Whatever the reporting base, both literal forms parse.
    Colour c;
    c.SetHex();
    c.SetColour("#R255G000B000");
    CHECK(Same(c.GetColour(), RED));

    c.SetDec();
    c.SetColour("#RFFG00B00");
    CHECK(Same(c.GetColour(), RED));
}

TEST(colour_literal_base_setters_agree) {
    Colour c;
    c.SetLiteralBase(true);
    CHECK(c.IsHex());
    c.SetLiteralBase(false);
    CHECK(!c.IsHex());
    c.SetHex();
    CHECK(c.IsHex());
    c.SetDec();
    CHECK(!c.IsHex());
}

// --- errors --------------------------------------------------------------

TEST(colour_rejects_empty_input) {
    CHECK_THROWS_AS(Colour(""), std::invalid_argument);
}

TEST(colour_rejects_unknown_names) {
    CHECK_THROWS_AS(Colour("NOTACOLOUR"), std::out_of_range);
    CHECK_THROWS_AS(Colour("REDD"), std::out_of_range);
}

TEST(colour_rejects_malformed_literals) {
    CHECK_THROWS_AS(Colour("#"), std::invalid_argument);           // nothing after the hash
    CHECK_THROWS_AS(Colour("#R12G34"), std::invalid_argument);     // blue missing
    CHECK_THROWS_AS(Colour("#R1G2B3"), std::invalid_argument);     // matches neither layout
    CHECK_THROWS_AS(Colour("#RZZG00B00"), std::invalid_argument);  // Z is not a hex digit
    CHECK_THROWS_AS(Colour("#R999G000B000"), std::invalid_argument); // channel over 255
}

// --- fade ----------------------------------------------------------------

TEST(colour_fade_identity) {
    Colour c(RED);
    c.Fade(1.0f);
    CHECK_EQ(Show(c.GetColour()), Show(RED));
}

TEST(colour_fade_to_transparent) {
    Colour c(RED);
    c.Fade(0.0f);
    CHECK_EQ(int(c.GetColour().a), 0);
    // RGB must be left alone
    CHECK_EQ(int(c.GetColour().r), int(RED.r));
    CHECK_EQ(int(c.GetColour().g), int(RED.g));
    CHECK_EQ(int(c.GetColour().b), int(RED.b));
}

TEST(colour_fade_clamps) {
    Colour low(RED);
    low.Fade(-1.0f);
    CHECK_EQ(int(low.GetColour().a), 0);

    Colour high(RED);
    high.Fade(2.0f);
    CHECK_EQ(int(high.GetColour().a), 255);
}

TEST(colour_fade_updates_the_name) {
    Colour c(RED);
    c.Fade(0.5f);
    // A half-transparent RED is not RED any more.
    CHECK(c.GetColourString() != std::string("RED"));
}

// --- blend ---------------------------------------------------------------

TEST(colour_blend_endpoints) {
    Colour start(RED);
    start.Blend(BLUE, 0.0f);
    CHECK_EQ(Show(start.GetColour()), Show(RED));

    Colour end(RED);
    end.Blend(BLUE, 1.0f);
    CHECK_EQ(Show(end.GetColour()), Show(BLUE));
}

TEST(colour_blend_clamps) {
    Colour low(RED);
    low.Blend(BLUE, -0.5f);
    CHECK_EQ(Show(low.GetColour()), Show(RED));

    Colour high(RED);
    high.Blend(BLUE, 5.0f);
    CHECK_EQ(Show(high.GetColour()), Show(BLUE));
}

TEST(colour_blend_includes_alpha) {
    Colour c(Color{0, 0, 0, 255});
    c.Blend(Color{0, 0, 0, 0}, 1.0f);
    CHECK_EQ(int(c.GetColour().a), 0);
}

TEST(colour_blend_accepts_both_overloads) {
    Colour a(RED);
    a.Blend(BLUE, 1.0f);

    Colour b(RED);
    b.Blend(Colour(BLUE), 1.0f);

    CHECK(a == b);
}

TEST(colour_blend_updates_the_name) {
    Colour c(RED);
    c.Blend(BLUE, 1.0f);
    CHECK_EQ(c.GetColourString(), std::string("BLUE"));
}

TEST(colour_lerp_endpoints_and_midpoint) {
    CHECK_EQ(int(Colour::Lerp(0, 255, 0.0f)), 0);
    CHECK_EQ(int(Colour::Lerp(0, 255, 1.0f)), 255);
    // Rounds rather than truncates, so the midpoint goes up.
    CHECK_EQ(int(Colour::Lerp(0, 255, 0.5f)), 128);
}
