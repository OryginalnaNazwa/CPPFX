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
    Colour c;
    c.SetColour("GREY");
    CHECK_EQ(c.GetColourString(), std::string("GRAY"));

    // RED is {230, 41, 55} - E6, 29, 37 in hex
    c.SetColour("#e62937");
    CHECK_EQ(c.GetColourString(), std::string("RED"));
    c.SetColour("#re6g29b37");
    CHECK_EQ(c.GetColourString(), std::string("RED"));
}

// --- comparison ----------------------------------------------------------

TEST(colour_blank_is_not_black) {
    CHECK(Colour(BLANK) != Colour(BLACK));
}

TEST(colour_equality_ignores_how_it_was_built) {
    // Same value reached different ways must compare equal.
    CHECK(Colour("GRAY") == Colour(GRAY));
    CHECK(Colour("GREY") == Colour("GRAY"));
    CHECK(Colour(RED) == Colour("RED"));
    CHECK(Colour("#828282") == Colour(GRAY));
}

TEST(colour_equality_ignores_the_literal_base) {
    Colour a(RED), b(RED);
    a.SetHex();
    b.SetDec();
    CHECK(a == b);
}

// --- plain literals ------------------------------------------------------

TEST(colour_hex_literals) {
    CHECK(Same(Colour("#FF0000").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#00000080").GetColour(), Color{0, 0, 0, 128}));
    CHECK(Same(Colour("#1e90ff").GetColour(), Color{30, 144, 255, 255}));   // lowercase digits
}

TEST(colour_dec_literals) {
    CHECK(Same(Colour("#255000000").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#000000000128").GetColour(), Color{0, 0, 0, 128}));
}

TEST(colour_alpha_defaults_to_opaque) {
    CHECK_EQ(int(Colour("#FF0000").GetColour().a), 255);
    CHECK_EQ(int(Colour("#255000000").GetColour().a), 255);
}

TEST(colour_digit_count_decides_the_base) {
    // All of these are plain digits; only the count says hex or dec.
    CHECK(Same(Colour("#102030").GetColour(), Color{0x10, 0x20, 0x30, 255}));
    CHECK(Same(Colour("#10203040").GetColour(), Color{0x10, 0x20, 0x30, 0x40}));
    CHECK(Same(Colour("#010020030").GetColour(), Color{10, 20, 30, 255}));
    CHECK(Same(Colour("#010020030040").GetColour(), Color{10, 20, 30, 40}));
}

// --- labelled literals ---------------------------------------------------

TEST(colour_labelled_hex_literals) {
    CHECK(Same(Colour("#RFFG00B00").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#R00G00B00A80").GetColour(), Color{0, 0, 0, 128}));
}

TEST(colour_labelled_dec_literals) {
    CHECK(Same(Colour("#R255G000B000").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#R000G000B000A128").GetColour(), Color{0, 0, 0, 128}));
}

TEST(colour_labelled_alpha_defaults_to_opaque) {
    CHECK_EQ(int(Colour("#RFFG00B00").GetColour().a), 255);
    CHECK_EQ(int(Colour("#R255G000B000").GetColour().a), 255);
}

TEST(colour_labelled_hex_is_not_mistaken_for_plain_dec) {
    // Both are 9 characters after the '#'; the leading R tells them apart.
    CHECK(Same(Colour("#RFFG00B00").GetColour(), Color{255, 0, 0, 255}));
    CHECK(Same(Colour("#255000000").GetColour(), Color{255, 0, 0, 255}));
}

// --- all forms -----------------------------------------------------------

TEST(colour_all_forms_give_the_same_value) {
    const Colour expected(Color{30, 144, 255, 128});
    CHECK(Colour("#1E90FF80") == expected);
    CHECK(Colour("#030144255128") == expected);
    CHECK(Colour("#R1EG90BFFA80") == expected);
    CHECK(Colour("#R030G144B255A128") == expected);
}

TEST(colour_literal_matching_a_named_colour_reports_the_name) {
    CHECK_EQ(Colour("#E62937").GetColourString(), std::string("RED"));
    CHECK_EQ(Colour("#230041055").GetColourString(), std::string("RED"));
    CHECK_EQ(Colour("#RE6G29B37").GetColourString(), std::string("RED"));
    CHECK_EQ(Colour("#R230G041B055").GetColourString(), std::string("RED"));
}

// --- reporting -----------------------------------------------------------

TEST(colour_unnamed_value_reports_as_a_plain_literal) {
    Colour c(Color{1, 2, 3, 255});
    // Default base is decimal, three digits per channel, alpha omitted at 255.
    CHECK_EQ(c.GetColourString(), std::string("#001002003"));
}

TEST(colour_alpha_is_reported_when_not_opaque) {
    Colour c(Color{1, 2, 3, 200});
    CHECK_EQ(c.GetColourString(), std::string("#001002003200"));
    c.SetHex();
    CHECK_EQ(c.GetColourString(), std::string("#010203C8"));
}

TEST(colour_base_flag_is_reflected_in_the_name) {
    Colour c(Color{1, 2, 3, 255});
    c.SetDec();
    CHECK_EQ(c.GetColourString(), std::string("#001002003"));
    c.SetHex();
    CHECK_EQ(c.GetColourString(), std::string("#010203"));
}

TEST(colour_literal_round_trips_in_both_bases) {
    Colour original(Color{1, 2, 3, 200});
    original.SetDec();
    CHECK(Colour(original.GetColourString()) == original);
    original.SetHex();
    CHECK(Colour(original.GetColourString()) == original);
}

TEST(colour_base_flag_does_not_affect_input) {
    // Whatever the reporting base, every literal form parses.
    Colour c;
    c.SetHex();
    c.SetColour("#255000000");
    CHECK(Same(c.GetColour(), Color{255, 0, 0, 255}));
    c.SetColour("#R255G000B000");
    CHECK(Same(c.GetColour(), Color{255, 0, 0, 255}));

    c.SetDec();
    c.SetColour("#FF0000");
    CHECK(Same(c.GetColour(), Color{255, 0, 0, 255}));
    c.SetColour("#RFFG00B00");
    CHECK(Same(c.GetColour(), Color{255, 0, 0, 255}));
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

TEST(colour_rejects_malformed_plain_literals) {
    CHECK_THROWS_AS(Colour("#"), std::invalid_argument);              // nothing after the hash
    CHECK_THROWS_AS(Colour("#12345"), std::invalid_argument);         // 5 digits
    CHECK_THROWS_AS(Colour("#1234567"), std::invalid_argument);       // 7 digits
    CHECK_THROWS_AS(Colour("#1234567890"), std::invalid_argument);    // 10 digits
    CHECK_THROWS_AS(Colour("#ZZ0000"), std::invalid_argument);        // Z is not a hex digit
    CHECK_THROWS_AS(Colour("#GG0000"), std::invalid_argument);        // G is not a hex digit
    CHECK_THROWS_AS(Colour("#2550000AB"), std::invalid_argument);     // letters in a dec literal
    CHECK_THROWS_AS(Colour("#999000000"), std::invalid_argument);     // channel over 255
    CHECK_THROWS_AS(Colour("#-10000"), std::invalid_argument);        // sign is not a digit
}

TEST(colour_rejects_malformed_labelled_literals) {
    CHECK_THROWS_AS(Colour("#R"), std::invalid_argument);             // label, no digits
    CHECK_THROWS_AS(Colour("#R12G34"), std::invalid_argument);        // blue missing
    CHECK_THROWS_AS(Colour("#R1G2B3"), std::invalid_argument);        // matches neither layout
    CHECK_THROWS_AS(Colour("#RZZG00B00"), std::invalid_argument);     // Z is not a hex digit
    CHECK_THROWS_AS(Colour("#R999G000B000"), std::invalid_argument);  // channel over 255
    CHECK_THROWS_AS(Colour("#RFFG00X00"), std::invalid_argument);     // wrong label
    CHECK_THROWS_AS(Colour("#RFFG00B00X00"), std::invalid_argument);  // wrong alpha label
}

// --- fade ----------------------------------------------------------------

TEST(colour_fade_identity) {
    Colour c(RED);
    c.FadeTo(1.0f);
    CHECK_EQ(Show(c.GetColour()), Show(RED));
}

TEST(colour_fade_to_transparent) {
    Colour c(RED);
    c.FadeTo(0.0f);
    CHECK_EQ(int(c.GetColour().a), 0);
    // RGB must be left alone
    CHECK_EQ(int(c.GetColour().r), int(RED.r));
    CHECK_EQ(int(c.GetColour().g), int(RED.g));
    CHECK_EQ(int(c.GetColour().b), int(RED.b));
}

TEST(colour_fade_rounds) {
    Colour c(RED);
    c.FadeTo(0.5f);
    // 255 * 0.5 = 127.5, rounded up rather than truncated.
    CHECK_EQ(int(c.GetColour().a), 128);
}

TEST(colour_fade_clamps) {
    Colour low(RED);
    low.FadeTo(-1.0f);
    CHECK_EQ(int(low.GetColour().a), 0);

    Colour high(RED);
    high.FadeTo(2.0f);
    CHECK_EQ(int(high.GetColour().a), 255);
}

TEST(colour_fade_changes_the_reported_name) {
    Colour c(RED);
    c.FadeTo(0.5f);
    // A half-transparent RED is not RED any more.
    CHECK(c.GetColourString() != std::string("RED"));
}

TEST(colour_static_fade_matches_fade_to) {
    Colour member(RED);
    member.FadeTo(0.25f);

    CHECK_EQ(Show(Colour::Fade(RED, 0.25f)), Show(member.GetColour()));
    CHECK(Colour::Fade(Colour(RED), 0.25f) == member);
}

TEST(colour_static_fade_leaves_the_input_alone) {
    const Colour original(RED);
    const Colour faded = Colour::Fade(original, 0.0f);
    CHECK_EQ(Show(original.GetColour()), Show(RED));
    CHECK_EQ(int(faded.GetColour().a), 0);
}

TEST(colour_static_fade_keeps_the_literal_base) {
    Colour original(Color{1, 2, 3, 255});
    original.SetHex();
    CHECK(Colour::Fade(original, 0.5f).IsHex());
}

// --- blend ---------------------------------------------------------------

TEST(colour_blend_endpoints) {
    Colour start(RED);
    start.BlendTo(BLUE, 0.0f);
    CHECK_EQ(Show(start.GetColour()), Show(RED));

    Colour end(RED);
    end.BlendTo(BLUE, 1.0f);
    CHECK_EQ(Show(end.GetColour()), Show(BLUE));
}

TEST(colour_blend_midpoint_rounds) {
    Colour c(Color{0, 0, 0, 0});
    c.BlendTo(Color{255, 255, 255, 255}, 0.5f);
    CHECK_EQ(Show(c.GetColour()), Show(Color{128, 128, 128, 128}));
}

TEST(colour_blend_clamps) {
    Colour low(RED);
    low.BlendTo(BLUE, -0.5f);
    CHECK_EQ(Show(low.GetColour()), Show(RED));

    Colour high(RED);
    high.BlendTo(BLUE, 5.0f);
    CHECK_EQ(Show(high.GetColour()), Show(BLUE));
}

TEST(colour_blend_includes_alpha) {
    Colour c(Color{0, 0, 0, 255});
    c.BlendTo(Color{0, 0, 0, 0}, 1.0f);
    CHECK_EQ(int(c.GetColour().a), 0);
}

TEST(colour_blend_accepts_both_overloads) {
    Colour a(RED);
    a.BlendTo(BLUE, 1.0f);

    Colour b(RED);
    b.BlendTo(Colour(BLUE), 1.0f);

    CHECK(a == b);
}

TEST(colour_blend_changes_the_reported_name) {
    Colour c(RED);
    c.BlendTo(BLUE, 1.0f);
    CHECK_EQ(c.GetColourString(), std::string("BLUE"));
}

TEST(colour_static_blend_matches_blend_to) {
    Colour member(RED);
    member.BlendTo(BLUE, 0.3f);

    CHECK_EQ(Show(Colour::Blend(RED, BLUE, 0.3f)), Show(member.GetColour()));
    CHECK(Colour::Blend(Colour(RED), Colour(BLUE), 0.3f) == member);
}

TEST(colour_static_blend_leaves_the_inputs_alone) {
    const Colour from(RED), to(BLUE);
    (void)Colour::Blend(from, to, 0.5f);
    CHECK_EQ(Show(from.GetColour()), Show(RED));
    CHECK_EQ(Show(to.GetColour()), Show(BLUE));
}

// --- lerp ----------------------------------------------------------------

TEST(colour_lerp_endpoints_and_midpoint) {
    CHECK_EQ(int(Colour::Lerp(0, 255, 0.0f)), 0);
    CHECK_EQ(int(Colour::Lerp(0, 255, 1.0f)), 255);
    // Rounds rather than truncates, so the midpoint goes up.
    CHECK_EQ(int(Colour::Lerp(0, 255, 0.5f)), 128);
}

TEST(colour_lerp_goes_downwards_too) {
    CHECK_EQ(int(Colour::Lerp(255, 0, 1.0f)), 0);
    CHECK_EQ(int(Colour::Lerp(200, 100, 0.5f)), 150);
}

TEST(colour_lerp_clamps) {
    CHECK_EQ(int(Colour::Lerp(100, 200, -1.0f)), 100);
    CHECK_EQ(int(Colour::Lerp(100, 200, 2.0f)), 200);
    CHECK_EQ(int(Colour::Lerp(200, 100, 2.0f)), 100);   // would go below the target unclamped
}

TEST(colour_lerp_to_changes_only_its_channel) {
    const Color start{10, 20, 30, 40};

    Colour r(start); r.LerpTo('r', 255, 1.0f);
    CHECK_EQ(Show(r.GetColour()), Show(Color{255, 20, 30, 40}));

    Colour g(start); g.LerpTo('g', 255, 1.0f);
    CHECK_EQ(Show(g.GetColour()), Show(Color{10, 255, 30, 40}));

    Colour b(start); b.LerpTo('b', 255, 1.0f);
    CHECK_EQ(Show(b.GetColour()), Show(Color{10, 20, 255, 40}));

    Colour a(start); a.LerpTo('a', 255, 1.0f);
    CHECK_EQ(Show(a.GetColour()), Show(Color{10, 20, 30, 255}));
}

TEST(colour_lerp_to_matches_lerp) {
    Colour c(Color{100, 0, 0, 255});
    c.LerpTo('r', 200, 0.3f);
    CHECK_EQ(int(c.GetColour().r), int(Colour::Lerp(100, 200, 0.3f)));
}

TEST(colour_lerp_to_ignores_capitalisation) {
    Colour lower(RED), upper(RED);
    lower.LerpTo('g', 255, 1.0f);
    upper.LerpTo('G', 255, 1.0f);
    CHECK(lower == upper);
}

TEST(colour_lerp_to_clamps) {
    Colour c(Color{100, 0, 0, 255});
    c.LerpTo('r', 200, 5.0f);
    CHECK_EQ(int(c.GetColour().r), 200);
}

TEST(colour_lerp_to_rejects_unknown_channels) {
    Colour c(RED);
    CHECK_THROWS_AS(c.LerpTo('x', 0, 0.5f), std::invalid_argument);
    CHECK_THROWS_AS(c.LerpTo('\0', 0, 0.5f), std::invalid_argument);
    CHECK_THROWS_AS(c.LerpTo(static_cast<char>(0xE9), 0, 0.5f), std::invalid_argument); // negative if char is signed
    // A rejected call must leave the colour alone.
    CHECK_EQ(Show(c.GetColour()), Show(RED));
}
