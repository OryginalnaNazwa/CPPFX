#include "harness.hpp"

#include <CPPFX/properties.hpp>

#include <stdexcept>
#include <string>

/**********************************************************************
 *  Tests for CPPFX::Alignment.
 *
 *  Pure arithmetic and string lookup - no window, no font, so this
 *  runs headless in CI like the Colour tests do.
 *
 *  The box is at (100, 200) and is 300 x 80. The content is 50 x 20.
 *  Every expected coordinate below is exact in binary floating point,
 *  so CHECK_EQ on floats is safe here - no epsilon needed.
 **********************************************************************/

using CPPFX::Alignment;
using A = CPPFX::Alignment::Alignments;

namespace {

constexpr float BX = 100.0f, BY = 200.0f, BW = 300.0f, BH = 80.0f;
constexpr float CW =  50.0f, CH =  20.0f;

/// The nine cells, with where the content's top-left should land.
struct Cell { A value; const char* name; float x; float y; };

constexpr Cell CELLS[] = {
    { A::TOP_LEFT,      "TOP_LEFT",      100.0f, 200.0f },
    { A::TOP_CENTRE,    "TOP_CENTRE",    225.0f, 200.0f },
    { A::TOP_RIGHT,     "TOP_RIGHT",     350.0f, 200.0f },
    { A::CENTRE_LEFT,   "CENTRE_LEFT",   100.0f, 230.0f },
    { A::CENTRE,        "CENTRE",        225.0f, 230.0f },
    { A::CENTRE_RIGHT,  "CENTRE_RIGHT",  350.0f, 230.0f },
    { A::BOTTOM_LEFT,   "BOTTOM_LEFT",   100.0f, 260.0f },
    { A::BOTTOM_CENTRE, "BOTTOM_CENTRE", 225.0f, 260.0f },
    { A::BOTTOM_RIGHT,  "BOTTOM_RIGHT",  350.0f, 260.0f },
};

}

// --- placement -----------------------------------------------------------

TEST(alignment_places_content_in_every_cell) {
    for (const auto& cell : CELLS) {
        Alignment a(cell.value);
        CHECK_EQ(a.GetAlignedX(BX, CW, BW), cell.x);
        CHECK_EQ(a.GetAlignedY(BY, CH, BH), cell.y);
    }
}

TEST(alignment_xy_agrees_with_the_single_axes) {
    // GetAlignedXY takes box before content; GetAlignedX takes content
    // before box. This is the test that catches that swap going wrong.
    for (const auto& cell : CELLS) {
        Alignment a(cell.value);
        Vector2 at = a.GetAlignedXY(BX, BY, BW, BH, CW, CH);
        CHECK_EQ(at.x, cell.x);
        CHECK_EQ(at.y, cell.y);
    }
}

TEST(alignment_content_filling_the_box_sits_at_the_origin) {
    for (const auto& cell : CELLS) {
        Alignment a(cell.value);
        CHECK_EQ(a.GetAlignedX(BX, BW, BW), BX);
        CHECK_EQ(a.GetAlignedY(BY, BH, BH), BY);
    }
}

TEST(alignment_oversized_content_overflows_rather_than_clamping) {
    // Documented behaviour: the content is allowed outside the box.
    CHECK_EQ(Alignment(A::TOP_RIGHT).GetAlignedX(BX, 400.0f, BW), 0.0f);
    CHECK_EQ(Alignment(A::CENTRE).GetAlignedX(BX, 400.0f, BW), 50.0f);
    CHECK_EQ(Alignment(A::BOTTOM_LEFT).GetAlignedY(BY, 100.0f, BH), 180.0f);
}

TEST(alignment_zero_sizes_are_legal) {
    CHECK_EQ(Alignment(A::CENTRE).GetAlignedX(BX, 0.0f, 0.0f), BX);
    CHECK_EQ(Alignment(A::CENTRE_RIGHT).GetAlignedX(BX, 0.0f, BW), 400.0f);
    CHECK_EQ(Alignment(A::BOTTOM_CENTRE).GetAlignedY(BY, 0.0f, BH), 280.0f);
}

TEST(alignment_rejects_negative_sizes) {
    Alignment a(A::CENTRE);
    CHECK_THROWS_AS(a.GetAlignedX(BX, -1.0f, BW), std::invalid_argument);
    CHECK_THROWS_AS(a.GetAlignedX(BX, CW, -1.0f), std::invalid_argument);
    CHECK_THROWS_AS(a.GetAlignedY(BY, -1.0f, BH), std::invalid_argument);
    CHECK_THROWS_AS(a.GetAlignedY(BY, CH, -1.0f), std::invalid_argument);
}

// --- the grid ------------------------------------------------------------

TEST(alignment_is_in_exactly_one_column_and_one_row) {
    for (const auto& cell : CELLS) {
        Alignment a(cell.value);
        CHECK_EQ(a.IsLeftAlignment() + a.IsCentreColumnAlignment() + a.IsRightAlignment(), 1);
        CHECK_EQ(a.IsTopAlignment() + a.IsCentreRowAlignment() + a.IsBottomAlignment(), 1);
    }
}

TEST(alignment_predicates_agree_with_the_placement) {
    // The column decides x, the row decides y. If a predicate and the
    // arithmetic ever disagree about which, this fails.
    for (const auto& cell : CELLS) {
        Alignment a(cell.value);
        const float x = a.GetAlignedX(BX, CW, BW);
        const float y = a.GetAlignedY(BY, CH, BH);

        if (a.IsLeftAlignment())         CHECK_EQ(x, BX);
        if (a.IsCentreColumnAlignment()) CHECK_EQ(x, BX + (BW - CW) / 2.0f);
        if (a.IsRightAlignment())        CHECK_EQ(x, BX + BW - CW);

        if (a.IsTopAlignment())       CHECK_EQ(y, BY);
        if (a.IsCentreRowAlignment()) CHECK_EQ(y, BY + (BH - CH) / 2.0f);
        if (a.IsBottomAlignment())    CHECK_EQ(y, BY + BH - CH);
    }
}

TEST(alignment_centre_is_the_only_cell_in_both_middles) {
    for (const auto& cell : CELLS) {
        Alignment a(cell.value);
        const bool both = a.IsCentreColumnAlignment() && a.IsCentreRowAlignment();
        CHECK_EQ(both, cell.value == A::CENTRE);
    }
}

// --- names ---------------------------------------------------------------

TEST(alignment_default_is_top_left) {
    CHECK_EQ(Alignment().GetAlignment(), A::TOP_LEFT);
}

TEST(alignment_names_round_trip) {
    for (const auto& cell : CELLS) {
        CHECK_EQ(Alignment(cell.value).GetAlignmentString(), std::string(cell.name));
        CHECK_EQ(Alignment(cell.name).GetAlignment(), cell.value);
    }
}

TEST(alignment_accepts_aliases) {
    CHECK_EQ(Alignment("LEFT").GetAlignment(), A::CENTRE_LEFT);
    CHECK_EQ(Alignment("RIGHT").GetAlignment(), A::CENTRE_RIGHT);
    CHECK_EQ(Alignment("TOP").GetAlignment(), A::TOP_CENTRE);
    CHECK_EQ(Alignment("UNDER").GetAlignment(), A::BOTTOM_CENTRE);
    CHECK_EQ(Alignment("MIDDLE").GetAlignment(), A::CENTRE);
}

TEST(alignment_accepts_american_spelling) {
    CHECK_EQ(Alignment("CENTER").GetAlignment(), A::CENTRE);
    CHECK_EQ(Alignment("TOP_CENTER").GetAlignment(), A::TOP_CENTRE);
    CHECK_EQ(Alignment("CENTER_LEFT").GetAlignment(), A::CENTRE_LEFT);
}

TEST(alignment_ignores_case) {
    CHECK_EQ(Alignment("bottom_right").GetAlignment(), A::BOTTOM_RIGHT);
    CHECK_EQ(Alignment("BoTtOm_RiGhT").GetAlignment(), A::BOTTOM_RIGHT);
}

TEST(alignment_reports_the_canonical_name_not_the_alias) {
    CHECK_EQ(Alignment("UP").GetAlignmentString(), std::string("TOP_CENTRE"));
    CHECK_EQ(Alignment("CENTER").GetAlignmentString(), std::string("CENTRE"));
    CHECK_EQ(Alignment("left").GetAlignmentString(), std::string("CENTRE_LEFT"));
}

TEST(alignment_rejects_bad_names) {
    CHECK_THROWS_AS(Alignment(""), std::invalid_argument);
    CHECK_THROWS_AS(Alignment("SIDEWAYS"), std::invalid_argument);
    CHECK_THROWS_AS(Alignment("TOP LEFT"), std::invalid_argument);   // space, not underscore
    CHECK_THROWS_AS(Alignment("TOP-LEFT"), std::invalid_argument);   // hyphen, not underscore
}

TEST(alignment_handles_non_ascii_without_undefined_behaviour) {
    // A byte above 0x7F must reach toupper as unsigned. If it goes in
    // signed this is UB, and MSVC's debug CRT asserts on it.
    CHECK_THROWS_AS(Alignment("CENTRE\xE2\x80\x99"), std::invalid_argument);
    CHECK_THROWS_AS(Alignment("\xC5\x9ARODEK"), std::invalid_argument);
}

TEST(alignment_string_literal_binds_to_a_const_reference_parameter) {
    // const char* -> std::string -> Alignment would be two user-defined
    // conversions, which is one too many. This only compiles because of
    // the const char* constructor.
    auto takes = [](const Alignment& a) { return a.GetAlignment(); };
    CHECK_EQ(takes("TOP_RIGHT"), A::TOP_RIGHT);
}

// --- setters -------------------------------------------------------------

TEST(alignment_setters_replace_the_value) {
    Alignment a;
    a.SetAlignment(A::BOTTOM_RIGHT);
    CHECK_EQ(a.GetAlignment(), A::BOTTOM_RIGHT);
    a.SetAlignment("top_centre");
    CHECK_EQ(a.GetAlignment(), A::TOP_CENTRE);
}

TEST(alignment_failed_set_leaves_the_value_alone) {
    // StringToAlignment throws before the assignment, so a bad name
    // cannot leave the object holding something half-set.
    Alignment a(A::BOTTOM_RIGHT);
    CHECK_THROWS_AS(a.SetAlignment("NOWHERE"), std::invalid_argument);
    CHECK_EQ(a.GetAlignment(), A::BOTTOM_RIGHT);
}

// --- static conversions --------------------------------------------------

TEST(alignment_conversions_work_without_an_instance) {
    CHECK_EQ(Alignment::AlignmentToString(A::CENTRE_RIGHT), std::string("CENTRE_RIGHT"));
    CHECK_EQ(Alignment::StringToAlignment("bottom"), A::BOTTOM_CENTRE);
}
