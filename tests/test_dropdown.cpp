#include "harness.hpp"

#include <CPPFX/items.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

/**********************************************************************
 *  Tests for CPPFX::DropDown.
 *
 *  Headless: nothing here draws, and nothing measures text. That second
 *  part matters - TextItem constructs with expandsToTextAutomatically
 *  set to true, and ExpandToText goes through Font::GetInkSize, which
 *  needs the default font raylib only loads in InitWindow. Every fixture
 *  below turns it off first. Leave it on and the suite dies in CI with
 *  no useful message.
 *
 *  The geometry figures are written out longhand rather than reusing the
 *  helpers they check, so a wrong helper cannot agree with itself.
 **********************************************************************/

using CPPFX::DropDown;

namespace {

/// Floats here are sums of small exact binary values, but a tolerance
/// keeps a failure message readable instead of showing 68.000001.
bool Near(float a, float b) {
    return (a > b ? a - b : b - a) < 0.001f;
}

/// Renders an order as one string, so a failing CHECK_EQ prints both
/// sequences instead of just saying two vectors differed.
std::string Joined(const std::vector<std::string>& v) {
    std::string out;
    for (const auto& s : v) {
        if (!out.empty()) out += ", ";
        out += s;
    }
    return out;
}

/**
 *  A dropdown with known geometry, used by most tests below.
 *
 *      header        y   0 .. 100
 *      headerBorder      100 .. 110
 *      listBorder top    110 .. 120
 *      headerDivider     120 .. 124
 *      row 0             124 .. 144
 *      divider           144 .. 146
 *      row 1             146 .. 166
 *      divider           166 .. 168
 *      row 2             168 .. 188
 *
 *  So the list top is 120, the row pitch is 22, and the content height
 *  is 4 + 3*20 + 2*2 = 68.
 */
DropDown<int> MakeFixture() {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetXY(0.0f);
    dd.SetWidth(200.0f);
    dd.SetHeight(100.0f);
    dd.headerBorder.SetThickness(10.0f);
    dd.listBorder.SetThickness(10.0f);
    dd.SetHeaderDividerThickness(4.0f);
    dd.SetDividerThickness(2.0f);
    dd.SetItemInListHeight(20.0f);
    dd.AddItem("first", 1);
    dd.AddItem("second", 2);
    dd.AddItem("third", 3);
    return dd;
}

/// Opens the dropdown and burns the justOpened flag, so the next
/// DoFocusAction is treated as a fresh click rather than the one that
/// opened it.
void OpenAndSettle(DropDown<int>& dd) {
    dd.Focus();
    dd.DoFocusAction(0.0f, {0.0f, 0.0f});
    CHECK(dd.IsFocused());
}

}

// --- geometry ------------------------------------------------------------

TEST(dropdown_content_height_counts_dividers_between_rows_only) {
    DropDown<int> dd = MakeFixture();
    // header divider + three rows + two dividers. Not three dividers:
    // nothing is drawn after the last row.
    CHECK(Near(dd.GetListContentHeight(), 4.0f + (3.0f * 20.0f) + (2.0f * 2.0f)));
}

TEST(dropdown_content_height_of_one_item_has_no_dividers) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetHeaderDividerThickness(4.0f);
    dd.SetDividerThickness(2.0f);
    dd.SetItemInListHeight(20.0f);
    dd.AddItem("only", 1);
    CHECK(Near(dd.GetListContentHeight(), 24.0f));
}

TEST(dropdown_empty_list_measures_the_stub) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetHeaderDividerThickness(4.0f);
    dd.SetDividerThickness(2.0f);
    dd.SetItemInListHeight(20.0f);
    // An open but empty dropdown still shows the header divider, so the
    // click registers visibly instead of appearing to do nothing.
    CHECK(Near(dd.GetListContentHeight(), 4.0f));
}

TEST(dropdown_total_list_height_adds_the_list_border_twice) {
    DropDown<int> dd = MakeFixture();
    CHECK(Near(dd.GetTotalListHeight(), dd.GetListContentHeight() + 20.0f));
}

TEST(dropdown_opening_grows_total_height_by_the_list) {
    DropDown<int> dd = MakeFixture();
    const float closed = dd.GetTotalHeight();
    dd.Focus();
    const float open = dd.GetTotalHeight();
    // Asserts the relationship rather than the absolute figure, so this
    // survives a change to how the outer border is accounted for.
    CHECK(Near(open - closed, dd.GetTotalListHeight()));
}

TEST(dropdown_list_top_clears_both_borders) {
    DropDown<int> dd = MakeFixture();
    CHECK(Near(dd.GetListTop(), 120.0f));
}

TEST(dropdown_row_pitch_is_row_plus_divider) {
    DropDown<int> dd = MakeFixture();
    CHECK(Near(dd.GetRowPitch(), 22.0f));
}

// --- hit testing ---------------------------------------------------------

TEST(dropdown_index_at_finds_every_row) {
    DropDown<int> dd = MakeFixture();
    CHECK_EQ(dd.GetIndexAt(134.0f), 0);   // middle of row 0
    CHECK_EQ(dd.GetIndexAt(156.0f), 1);
    CHECK_EQ(dd.GetIndexAt(178.0f), 2);   // middle of the last row
}

TEST(dropdown_index_at_handles_row_edges) {
    DropDown<int> dd = MakeFixture();
    CHECK_EQ(dd.GetIndexAt(124.0f), 0);   // first pixel of row 0
    CHECK_EQ(dd.GetIndexAt(143.9f), 0);   // last pixel of row 0
    CHECK_EQ(dd.GetIndexAt(146.0f), 1);   // first pixel of row 1
}

TEST(dropdown_dividers_are_dead_zones) {
    DropDown<int> dd = MakeFixture();
    CHECK_EQ(dd.GetIndexAt(122.0f), -1);  // the header divider
    CHECK_EQ(dd.GetIndexAt(145.0f), -1);  // between rows 0 and 1
    CHECK_EQ(dd.GetIndexAt(167.0f), -1);  // between rows 1 and 2
}

TEST(dropdown_index_at_rejects_outside_the_list) {
    DropDown<int> dd = MakeFixture();
    CHECK_EQ(dd.GetIndexAt(50.0f), -1);    // on the header
    CHECK_EQ(dd.GetIndexAt(115.0f), -1);   // inside the borders
    CHECK_EQ(dd.GetIndexAt(188.0f), -1);   // one past the last row
    CHECK_EQ(dd.GetIndexAt(5000.0f), -1);  // far below - must not clamp
}

TEST(dropdown_index_at_survives_a_zero_pitch) {
    // listItemHeight and dividerThickness both zero would divide by zero.
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetItemInListHeight(0.0f);
    dd.SetDividerThickness(0.0f);
    dd.AddItem("first", 1);
    CHECK_EQ(dd.GetIndexAt(200.0f), -1);
}

TEST(dropdown_index_at_on_empty_list) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetItemInListHeight(20.0f);
    CHECK_EQ(dd.GetIndexAt(150.0f), -1);
}

TEST(dropdown_click_picks_the_row_under_the_cursor) {
    DropDown<int> dd = MakeFixture();
    OpenAndSettle(dd);
    dd.DoFocusAction(0.0f, {100.0f, 156.0f});   // row 1
    CHECK_EQ(dd.GetCurrentLabel(), std::string("second"));
    CHECK_EQ(dd.GetCurrentValue(), 2);
    CHECK(!dd.IsFocused());                      // picking closes it
}

TEST(dropdown_click_on_the_header_closes_it) {
    DropDown<int> dd = MakeFixture();
    OpenAndSettle(dd);
    dd.DoFocusAction(0.0f, {100.0f, 50.0f});
    CHECK(!dd.IsFocused());
}

TEST(dropdown_opening_click_does_not_close_it) {
    // Focus() and the first DoFocusAction land in the same frame, so
    // without justOpened the header click would shut it immediately.
    DropDown<int> dd = MakeFixture();
    dd.Focus();
    dd.DoFocusAction(0.0f, {100.0f, 50.0f});
    CHECK(dd.IsFocused());
}

TEST(dropdown_click_outside_closes_without_picking) {
    DropDown<int> dd = MakeFixture();
    dd.SetCurrent("first");
    OpenAndSettle(dd);
    dd.DoFocusAction(0.0f, {900.0f, 156.0f});   // past the right edge
    CHECK(!dd.IsFocused());
    CHECK_EQ(dd.GetCurrentLabel(), std::string("first"));
}

TEST(dropdown_click_on_a_divider_picks_nothing) {
    DropDown<int> dd = MakeFixture();
    dd.SetCurrent("first");
    OpenAndSettle(dd);
    dd.DoFocusAction(0.0f, {100.0f, 145.0f});
    CHECK_EQ(dd.GetCurrentLabel(), std::string("first"));
}

TEST(dropdown_was_i_clicked_covers_the_open_list) {
    DropDown<int> dd = MakeFixture();
    CHECK(!dd.WasIClicked({100.0f, 150.0f}));   // closed: list is not there
    dd.Focus();
    CHECK(dd.WasIClicked({100.0f, 150.0f}));
    CHECK(dd.WasIClicked({100.0f, 50.0f}));     // the header still counts
    CHECK(!dd.WasIClicked({100.0f, 200.0f}));   // below the list
}

// --- ordering ------------------------------------------------------------

TEST(dropdown_natural_order_puts_nine_before_ten) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("item10", 10);
    dd.AddItem("item9", 9);
    dd.AddItem("item1", 1);
    dd.SetOrder(DropDown<int>::natural);
    dd.Sort();
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("item1, item9, item10"));
}

TEST(dropdown_alphabetic_order_does_not) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("item10", 10);
    dd.AddItem("item9", 9);
    dd.AddItem("item1", 1);
    dd.SetOrder(DropDown<int>::alphabetic);
    dd.Sort();
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("item1, item10, item9"));
}

TEST(dropdown_natural_order_breaks_ties_on_leading_zeros) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("item007", 7);
    dd.AddItem("item7", 7);
    dd.SetOrder(DropDown<int>::natural);
    dd.Sort();
    // Numerically equal, so the one with fewer leading zeros comes first.
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("item7, item007"));
}

TEST(dropdown_case_sensitivity_changes_the_order) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("Apple", 1);
    dd.AddItem("banana", 2);
    dd.AddItem("Cherry", 3);

    dd.SetOrder(DropDown<int>::alphabetic);
    dd.Sort();
    // Capitals sort ahead of lowercase in ASCII.
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("Apple, Cherry, banana"));

    dd.SetOrder(DropDown<int>::alphabeticCaseInsensitive);
    dd.Sort();
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("Apple, banana, Cherry"));
}

TEST(dropdown_reverse_orders_are_exact_reverses) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("item10", 10);
    dd.AddItem("item2", 2);
    dd.AddItem("Item1", 1);

    dd.SetOrder(DropDown<int>::natural);
    dd.Sort();
    std::vector<std::string> forward = dd.GetLabelsInOrder();

    dd.SetOrder(DropDown<int>::reverseNatural);
    dd.Sort();
    std::vector<std::string> backward = dd.GetLabelsInOrder();

    std::reverse(backward.begin(), backward.end());
    CHECK_EQ(Joined(backward), Joined(forward));
}

TEST(dropdown_insertion_order_is_recoverable_after_sorting) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("cherry", 3);
    dd.AddItem("apple", 1);
    dd.AddItem("banana", 2);

    dd.SetOrder(DropDown<int>::alphabetic);
    dd.Sort();
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("apple, banana, cherry"));

    // insertionOrder exists precisely so this round trip works.
    dd.SetOrder(DropDown<int>::insertion);
    dd.Sort();
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("cherry, apple, banana"));
}

TEST(dropdown_new_items_land_at_the_end_in_insertion_order) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("a", 1);
    dd.AddItem("b", 2);
    dd.AddItem("c", 3);
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("a, b, c"));
}

TEST(dropdown_sorting_is_lazy) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetOrder(DropDown<int>::alphabetic);
    dd.AddItem("c", 3);
    dd.AddItem("a", 1);
    // Inserting marks it dirty; DoPassiveAction is what actually sorts.
    dd.DoPassiveAction(0.0f);
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("a, c"));
}

TEST(dropdown_custom_order_without_a_comparator_throws) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("a", 1);
    dd.SetOrder(DropDown<int>::custom);
    CHECK_THROWS_AS(dd.Sort(), std::runtime_error);
}

TEST(dropdown_custom_order_uses_the_comparator) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("aaa", 1);
    dd.AddItem("z", 2);
    dd.AddItem("bb", 3);
    dd.SetCustomSort([](const std::string& a, const std::string& b) {
        return a.size() < b.size();
    });
    dd.SetOrder(DropDown<int>::custom);
    dd.Sort();
    CHECK_EQ(Joined(dd.GetLabelsInOrder()), std::string("z, bb, aaa"));
}

// --- items ---------------------------------------------------------------

TEST(dropdown_rejects_an_empty_label) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    CHECK_THROWS_AS(dd.AddItem("", 1), std::invalid_argument);
}

TEST(dropdown_rejects_a_duplicate_label) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("taken", 1);
    CHECK_THROWS_AS(dd.AddItem("taken", 2), std::out_of_range);
}

TEST(dropdown_removing_a_missing_label_throws) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    CHECK_THROWS_AS(dd.RemoveItem("nothing"), std::out_of_range);
}

TEST(dropdown_renaming_onto_a_taken_label_throws) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("a", 1);
    dd.AddItem("b", 2);
    CHECK_THROWS_AS(dd.ChangeLabel("a", "b"), std::invalid_argument);
}

TEST(dropdown_with_no_pick_throws_on_access) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    CHECK_THROWS_AS(dd.GetCurrentValue(), std::runtime_error);
    CHECK_THROWS_AS(dd.GetCurrentLabel(), std::runtime_error);
}

TEST(dropdown_setting_a_missing_current_throws) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("a", 1);
    CHECK_THROWS_AS(dd.SetCurrent("b"), std::out_of_range);
}

TEST(dropdown_removing_the_current_item_clears_the_pick) {
    DropDown<int> dd = MakeFixture();
    dd.SetCurrent("second");
    dd.RemoveItem("second");
    CHECK_THROWS_AS(dd.GetCurrentLabel(), std::runtime_error);
    CHECK_EQ(dd.GetNumberOfItems(), (size_t)2);
}

TEST(dropdown_removing_another_item_leaves_the_pick_alone) {
    DropDown<int> dd = MakeFixture();
    dd.SetCurrent("second");
    dd.RemoveItem("first");
    CHECK_EQ(dd.GetCurrentLabel(), std::string("second"));
}

TEST(dropdown_renaming_the_current_item_follows_it) {
    DropDown<int> dd = MakeFixture();
    dd.SetCurrent("second");
    dd.ChangeLabel("second", "renamed");
    CHECK_EQ(dd.GetCurrentLabel(), std::string("renamed"));
    CHECK_EQ(dd.GetValue("renamed"), 2);
    CHECK(!dd.IsLabelTaken("second"));
}

TEST(dropdown_picks_the_first_item_when_nothing_is_current) {
    DropDown<int> dd = MakeFixture();
    dd.DoPassiveAction(0.0f);
    CHECK_EQ(dd.GetCurrentLabel(), std::string("first"));
}

TEST(dropdown_first_item_follows_the_order) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.AddItem("cherry", 3);
    dd.AddItem("apple", 1);
    dd.SetOrder(DropDown<int>::alphabetic);
    dd.DoPassiveAction(0.0f);
    // "First" means first as drawn, not first inserted.
    CHECK_EQ(dd.GetCurrentLabel(), std::string("apple"));
}

TEST(dropdown_changing_a_value_keeps_the_label) {
    DropDown<int> dd = MakeFixture();
    dd.ChangeValue("second", 22);
    CHECK_EQ(dd.GetValue("second"), 22);
    CHECK_EQ(dd.GetNumberOfItems(), (size_t)3);
}

// --- per-label colours ---------------------------------------------------

TEST(dropdown_label_colours_go_in_and_come_out) {
    DropDown<int> dd = MakeFixture();
    dd.SetColourToLabel("second", CPPFX::Colour(RED));
    CHECK(dd.GetColoursToLabels().contains("second"));
    CHECK(dd.GetColoursToLabels().at("second") == CPPFX::Colour(RED));
}

TEST(dropdown_setting_a_label_colour_twice_replaces_it) {
    DropDown<int> dd = MakeFixture();
    dd.SetColourToLabel("second", CPPFX::Colour(RED));
    dd.SetColourToLabel("second", CPPFX::Colour(BLUE));
    CHECK(dd.GetColoursToLabels().at("second") == CPPFX::Colour(BLUE));
    CHECK_EQ(dd.GetColoursToLabels().size(), (size_t)1);
}

TEST(dropdown_removing_an_item_takes_its_colour_with_it) {
    DropDown<int> dd = MakeFixture();
    dd.SetColourToLabel("second", CPPFX::Colour(RED));
    dd.RemoveItem("second");
    CHECK(!dd.GetColoursToLabels().contains("second"));
    // Reusing the name must not inherit the old colour.
    dd.AddItem("second", 2);
    CHECK(!dd.GetColoursToLabels().contains("second"));
}

TEST(dropdown_renaming_an_item_carries_its_colour_across) {
    DropDown<int> dd = MakeFixture();
    dd.SetColourToLabel("second", CPPFX::Colour(RED));
    dd.ChangeLabel("second", "renamed");
    CHECK(!dd.GetColoursToLabels().contains("second"));
    CHECK(dd.GetColoursToLabels().at("renamed") == CPPFX::Colour(RED));
}

TEST(dropdown_clearing_colours_leaves_the_items) {
    DropDown<int> dd = MakeFixture();
    dd.SetColourToLabel("first", CPPFX::Colour(RED));
    dd.SetColourToLabel("second", CPPFX::Colour(BLUE));
    dd.ClearColoursFromLabels();
    CHECK(dd.GetColoursToLabels().empty());
    CHECK_EQ(dd.GetNumberOfItems(), (size_t)3);
}

// --- setters -------------------------------------------------------------

TEST(dropdown_divider_setters_do_not_cross_wires) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    dd.SetDividerThickness(7.0f);
    CHECK(Near(dd.GetDividerThickness(), 7.0f));
    // The neighbour is the point of this test - one setter wrote the
    // other's member for a while.
    CHECK(Near(dd.GetHeaderDividerThickness(), 0.0f));

    dd.SetHeaderDividerThickness(3.0f);
    CHECK(Near(dd.GetHeaderDividerThickness(), 3.0f));
    CHECK(Near(dd.GetDividerThickness(), 7.0f));
}

TEST(dropdown_rejects_negative_measurements) {
    DropDown<int> dd;
    dd.DoNotExpandToTextAutomatically();
    CHECK_THROWS_AS(dd.SetDividerThickness(-1.0f), std::invalid_argument);
    CHECK_THROWS_AS(dd.SetHeaderDividerThickness(-1.0f), std::invalid_argument);
    CHECK_THROWS_AS(dd.SetItemInListHeight(-1.0f), std::invalid_argument);
}

// --- syncing -------------------------------------------------------------

TEST(dropdown_does_not_sync_by_default) {
    DropDown<int> dd = MakeFixture();
    CHECK(!dd.IsSyncingToHeaderAutomatically());
}

TEST(dropdown_automatic_sync_follows_the_header) {
    DropDown<int> dd = MakeFixture();
    dd.listColour.SetColour(GREEN);
    dd.SyncToHeaderAutomatically();
    dd.colour.SetColour(RED);
    dd.DoPassiveAction(0.0f);
    CHECK(dd.listColour == CPPFX::Colour(RED));

    // ... and keeps following, however often the header changes.
    dd.colour.SetColour(BLUE);
    dd.DoPassiveAction(0.0f);
    CHECK(dd.listColour == CPPFX::Colour(BLUE));
}

TEST(dropdown_automatic_sync_stops_when_turned_off) {
    DropDown<int> dd = MakeFixture();
    dd.SyncToHeaderAutomatically();
    dd.colour.SetColour(RED);
    dd.DoPassiveAction(0.0f);

    dd.DoNotSyncToHeaderAutomatically();
    dd.listColour.SetColour(GREEN);
    dd.colour.SetColour(BLUE);
    dd.DoPassiveAction(0.0f);
    CHECK(dd.listColour == CPPFX::Colour(GREEN));
}

TEST(dropdown_one_shot_sync_is_a_starting_point) {
    DropDown<int> dd = MakeFixture();
    dd.colour.SetColour(RED);
    dd.headerBorder.SetThickness(6.0f);
    dd.SyncToHeader();
    CHECK(dd.listColour == CPPFX::Colour(RED));
    CHECK(Near(dd.listBorder.GetThickness(), 6.0f));

    // Adjustments after the call stay put - it is not a mode.
    dd.SetItemInListHeight(12.0f);
    dd.colour.SetColour(BLUE);
    dd.DoPassiveAction(0.0f);
    CHECK(dd.listColour == CPPFX::Colour(RED));
    CHECK(Near(dd.GetItemInListHeight(), 12.0f));
}
