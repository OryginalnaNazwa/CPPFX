#include "harness.hpp"

#include <CPPFX/items.hpp>

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

/**********************************************************************
 *  Tests for ProgressItem, ProgressIndicator, ProgressBar and the
 *  HasPercentages mixin.
 *
 *  Headless: nothing here draws. The drawing is checked pixel by pixel
 *  in gl/test_progress_gl.cpp.
 *
 *  The probes below only widen access to protected helpers so their
 *  maths can be checked directly. They must name Item themselves:
 *  it's a virtual base, so the most derived class constructs it, and
 *  the widgets' own Item(...) calls are skipped.
 **********************************************************************/

namespace fx = CPPFX;

namespace {

struct IndicatorProbe : fx::ProgressIndicator {
    IndicatorProbe() : Item("ProgressIndicator", 100.0f, 100.0f) {}
    using fx::ProgressIndicator::GetSpinAngle;
    using fx::ProgressItem::GetFilledCount;
};

struct BarProbe : fx::ProgressBar {
    BarProbe() : Item("ProgressBar") {}
    using fx::ProgressBar::GetMaxBarMargin;
    using fx::ProgressBar::GetEffectiveBarMargin;
    using fx::ProgressBar::GetMaxGap;
    using fx::ProgressBar::GetEffectiveGap;
    using fx::ProgressBar::GetInnerRectangle;
    using fx::ProgressBar::GetSegmentWidth;
    using fx::ProgressBar::GetSweepSegments;
    using fx::ProgressBar::GetSweepPhase;
    using fx::ProgressItem::GetFilledCount;
};

struct PercentProbe : fx::HasPercentages {
    PercentProbe() : HasPercentages(false) {}
    using fx::HasPercentages::FormatPercentage;
};

/// Catches what CPPFX_WARN prints for as long as it lives.
struct CerrCapture {
    std::ostringstream out;
    std::streambuf* old;
    CerrCapture() : old(std::cerr.rdbuf(out.rdbuf())) {}
    ~CerrCapture() { std::cerr.rdbuf(old); }
    bool Contains(const std::string& s) const { return out.str().find(s) != std::string::npos; }
    bool Empty() const { return out.str().empty(); }
};

/// The message of whatever invalid_argument f throws, or "" if it doesn't.
template <typename F>
std::string InvalidArgumentMessage(F f) {
    try { f(); } catch (const std::invalid_argument& e) { return e.what(); }
    return "";
}

}

// --- ProgressItem: value and state ---------------------------------------

TEST(progress_starts_indefinite_at_zero) {
    fx::ProgressBar bar;
    CHECK(bar.IsIndefinite());
    CHECK_NEAR(bar.GetProgress(), 0.0f, 1e-6);
    // Neither end counts while there's no value to show.
    CHECK(!bar.IsComplete());
    CHECK(!bar.IsNotStarted());
}

TEST(progress_set_progress_makes_it_definite) {
    fx::ProgressBar bar;
    bar.SetProgress(0.4f);
    CHECK(!bar.IsIndefinite());
    CHECK_NEAR(bar.GetProgress(), 0.4f, 1e-6);
}

TEST(progress_set_progress_accepts_both_ends) {
    fx::ProgressBar bar;
    bar.SetProgress(0.0f);
    CHECK(bar.IsNotStarted());
    bar.SetProgress(1.0f);
    CHECK(bar.IsComplete());
}

TEST(progress_set_progress_rejects_out_of_range) {
    fx::ProgressBar bar;
    bar.SetProgress(0.5f);
    CHECK_THROWS_AS(bar.SetProgress(-0.01f), std::out_of_range);
    CHECK_THROWS_AS(bar.SetProgress(1.01f), std::out_of_range);
    // -1 used to mean indefinite. It's just out of range now.
    CHECK_THROWS_AS(bar.SetProgress(-1.0f), std::out_of_range);
    CHECK_NEAR(bar.GetProgress(), 0.5f, 1e-6);   // a refused value changes nothing
}

TEST(progress_set_progress_rejects_nan) {
    fx::ProgressBar bar;
    CHECK_THROWS_AS(bar.SetProgress(std::numeric_limits<float>::quiet_NaN()), std::out_of_range);
}

TEST(progress_increase_clamps_at_both_ends) {
    fx::ProgressBar bar;
    bar.SetProgress(0.9f);
    bar.IncreaseProgress(0.5f);
    CHECK_NEAR(bar.GetProgress(), 1.0f, 1e-6);
    bar.IncreaseProgress(-5.0f);
    CHECK_NEAR(bar.GetProgress(), 0.0f, 1e-6);
}

TEST(progress_decrease_is_increase_reversed) {
    fx::ProgressBar bar;
    bar.SetProgress(0.5f);
    bar.DecreaseProgress(0.2f);
    CHECK_NEAR(bar.GetProgress(), 0.3f, 1e-6);
    bar.DecreaseProgress(-0.2f);                  // negative decrease goes up
    CHECK_NEAR(bar.GetProgress(), 0.5f, 1e-6);
    bar.DecreaseProgress(2.0f);
    CHECK_NEAR(bar.GetProgress(), 0.0f, 1e-6);
}

TEST(progress_first_increase_counts_from_zero) {
    // The waiting-then-working case: indefinite until the first step,
    // then counting up from 0.
    fx::ProgressBar bar;
    bar.IncreaseProgress(0.1f);
    CHECK(!bar.IsIndefinite());
    CHECK_NEAR(bar.GetProgress(), 0.1f, 1e-6);
}

TEST(progress_indefinite_keeps_the_last_value) {
    fx::ProgressBar bar;
    bar.SetProgress(0.6f);
    bar.MakeIndefinite();
    CHECK(bar.IsIndefinite());
    CHECK_NEAR(bar.GetProgress(), 0.6f, 1e-6);
    CHECK_EQ(bar.GetProgressPercentage(), 60);
    bar.MakeDefinite();
    CHECK_NEAR(bar.GetProgress(), 0.6f, 1e-6);   // resumes where it stalled
}

TEST(progress_increase_resumes_from_the_last_value) {
    fx::ProgressBar bar;
    bar.SetProgress(0.6f);
    bar.MakeIndefinite();
    bar.IncreaseProgress(0.1f);
    CHECK(!bar.IsIndefinite());
    CHECK_NEAR(bar.GetProgress(), 0.7f, 1e-6);
}

TEST(progress_set_indefinite_goes_both_ways) {
    fx::ProgressBar bar;
    bar.SetIndefinite(false);
    CHECK(!bar.IsIndefinite());
    bar.SetIndefinite(true);
    CHECK(bar.IsIndefinite());
}

TEST(progress_complete_and_not_started_are_false_while_indefinite) {
    fx::ProgressBar bar;
    bar.SetProgress(1.0f);
    bar.MakeIndefinite();
    CHECK(!bar.IsComplete());
    bar.SetProgress(0.0f);
    bar.MakeIndefinite();
    CHECK(!bar.IsNotStarted());
}

TEST(progress_in_between_is_neither_complete_nor_not_started) {
    fx::ProgressBar bar;
    bar.SetProgress(0.5f);
    CHECK(!bar.IsComplete());
    CHECK(!bar.IsNotStarted());
}

// --- ProgressItem: counting parts ----------------------------------------

TEST(progress_percentage_does_not_lose_a_point_to_float_error) {
    // 0.53f * 100 is 52.9999962 in float, which used to truncate to 52.
    // Same for 0.59f. They're the only two of the 101 whole percentages.
    fx::ProgressBar bar;
    bar.SetProgress(0.53f);
    CHECK_EQ(bar.GetProgressPercentage(), 53);
    bar.SetProgress(0.59f);
    CHECK_EQ(bar.GetProgressPercentage(), 59);
}

TEST(progress_percentage_never_rounds_up_to_done) {
    fx::ProgressBar bar;
    bar.SetProgress(0.995f);
    CHECK_EQ(bar.GetProgressPercentage(), 99);
    bar.SetProgress(0.9999f);
    CHECK_EQ(bar.GetProgressPercentage(), 99);
    bar.SetProgress(1.0f);
    CHECK_EQ(bar.GetProgressPercentage(), 100);
}

TEST(progress_percentage_every_whole_step_is_exact) {
    // Every n / 100 has to come back as n, whatever float does to it.
    fx::ProgressBar bar;
    for (int n = 0; n <= 100; ++n) {
        bar.SetProgress((float)n / 100.0f);
        CHECK_EQ(bar.GetProgressPercentage(), n);
    }
}

TEST(progress_filled_count_of_segments) {
    BarProbe bar;
    bar.SetProgress(0.53f);                        // 52.9999962 of 100 segments
    CHECK_EQ(bar.GetFilledCount(100), 53);
    bar.SetProgress(0.7f);
    CHECK_EQ(bar.GetFilledCount(10), 7);
    bar.SetProgress(0.69f);
    CHECK_EQ(bar.GetFilledCount(10), 6);
    bar.SetProgress(0.0f);
    CHECK_EQ(bar.GetFilledCount(10), 0);
    bar.SetProgress(1.0f);
    CHECK_EQ(bar.GetFilledCount(10), 10);
}

TEST(progress_filled_count_every_step_of_ten) {
    BarProbe bar;
    for (int n = 0; n <= 10; ++n) {
        bar.SetProgress((float)n / 10.0f);
        CHECK_EQ(bar.GetFilledCount(10), n);
    }
}

TEST(progress_filled_count_in_thousandths) {
    // What DrawValue formats: floored to a tenth of a percent.
    BarProbe bar;
    bar.SetProgress(0.2905f);
    CHECK_EQ(bar.GetFilledCount(1000), 290);
    bar.SetProgress(0.9996f);
    CHECK_EQ(bar.GetFilledCount(1000), 999);
}

// --- HasPercentages ------------------------------------------------------

TEST(percentages_whole_numbers_have_no_decimal) {
    CHECK_EQ(PercentProbe::FormatPercentage(0.0f), std::string("0%"));
    CHECK_EQ(PercentProbe::FormatPercentage(0.42f), std::string("42%"));
    CHECK_EQ(PercentProbe::FormatPercentage(1.0f), std::string("100%"));
}

TEST(percentages_float_error_does_not_add_a_decimal) {
    // 0.15f * 100 is 15.0000010 in float, and 0.53f * 100 is 52.9999962.
    // Neither is a whole number, so they used to print as "15.0%" and "53.0%".
    CHECK_EQ(PercentProbe::FormatPercentage(0.15f), std::string("15%"));
    CHECK_EQ(PercentProbe::FormatPercentage(0.30f), std::string("30%"));
    CHECK_EQ(PercentProbe::FormatPercentage(0.53f), std::string("53%"));
    CHECK_EQ(PercentProbe::FormatPercentage(0.60f), std::string("60%"));
}

TEST(percentages_one_decimal_place) {
    CHECK_EQ(PercentProbe::FormatPercentage(0.425f), std::string("42.5%"));
    CHECK_EQ(PercentProbe::FormatPercentage(0.12345f), std::string("12.3%"));
    CHECK_EQ(PercentProbe::FormatPercentage(0.0005f), std::string("0.1%"));   // rounds, for PieChart
}

TEST(percentages_every_whole_step_formats_cleanly) {
    for (int n = 0; n <= 100; ++n) {
        CHECK_EQ(PercentProbe::FormatPercentage((float)n / 100.0f), std::to_string(n) + "%");
    }
}

TEST(percentages_flag_goes_both_ways) {
    PercentProbe p;
    CHECK(!p.IsShowingPercentage());
    p.DoShowPercentage();
    CHECK(p.IsShowingPercentage());
    p.DoNotShowPercentage();
    CHECK(!p.IsShowingPercentage());
    p.SetShowingPercentage(true);
    CHECK(p.IsShowingPercentage());
}

TEST(percentages_defaults_differ_per_widget) {
    fx::ProgressBar bar;
    fx::ProgressIndicator indicator;
    fx::PieChart chart;
    CHECK(bar.IsShowingPercentage());
    CHECK(indicator.IsShowingPercentage());
    CHECK(!chart.IsShowingPercentage());
}

// --- ProgressIndicator: shapes -------------------------------------------

TEST(indicator_defaults) {
    fx::ProgressIndicator indicator;
    CHECK_EQ(indicator.GetShape(), fx::ProgressIndicator::DOTS);
    CHECK_EQ(indicator.GetIndefiniteShape(), fx::ProgressIndicator::DOTS);
    CHECK(indicator.IsClockwise());
    CHECK_NEAR(indicator.GetStartAngle(), -90.0f, 1e-6);
    CHECK_NEAR(indicator.GetWidth(), 100.0f, 1e-6);
    CHECK_NEAR(indicator.GetHeight(), 100.0f, 1e-6);
    CHECK_EQ(indicator.GetFxID(), std::string("ProgressIndicator"));
    CHECK(indicator.trackColour.GetColour().a == 0);
}

TEST(indicator_every_shape_can_be_set) {
    fx::ProgressIndicator indicator;
    for (auto shape : {fx::ProgressIndicator::DOTS, fx::ProgressIndicator::ARC,
                       fx::ProgressIndicator::CIRCLE, fx::ProgressIndicator::RING}) {
        indicator.SetShape(shape);
        CHECK_EQ(indicator.GetShape(), shape);
    }
}

TEST(indicator_shape_names_round_trip) {
    fx::ProgressIndicator indicator;
    for (std::string name : {"DOTS", "ARC", "CIRCLE", "RING"}) {
        indicator.SetShape(name);
        CHECK_EQ(indicator.GetShapeString(), name);
    }
}

TEST(indicator_shape_names_ignore_case) {
    fx::ProgressIndicator indicator;
    indicator.SetShape("ring");
    CHECK_EQ(indicator.GetShape(), fx::ProgressIndicator::RING);
    indicator.SetShape("Arc");
    CHECK_EQ(indicator.GetShape(), fx::ProgressIndicator::ARC);
}

TEST(indicator_rejects_unknown_shapes) {
    fx::ProgressIndicator indicator;
    indicator.SetShape(fx::ProgressIndicator::RING);
    CHECK_THROWS_AS(indicator.SetShape("BAR"), std::invalid_argument);   // gone with the split
    CHECK_THROWS_AS(indicator.SetShape(""), std::invalid_argument);
    CHECK_THROWS_AS(indicator.SetShape((fx::ProgressIndicator::Shapes)42), std::invalid_argument);
    CHECK_EQ(indicator.GetShape(), fx::ProgressIndicator::RING);       // unchanged
}

TEST(indicator_indefinite_shape_takes_only_spinners) {
    fx::ProgressIndicator indicator;
    indicator.SetIndefiniteShape(fx::ProgressIndicator::ARC);
    CHECK_EQ(indicator.GetIndefiniteShape(), fx::ProgressIndicator::ARC);
    indicator.SetIndefiniteShape(fx::ProgressIndicator::DOTS);
    CHECK_EQ(indicator.GetIndefiniteShape(), fx::ProgressIndicator::DOTS);

    CHECK_THROWS_AS(indicator.SetIndefiniteShape(fx::ProgressIndicator::CIRCLE), std::invalid_argument);
    CHECK_THROWS_AS(indicator.SetIndefiniteShape(fx::ProgressIndicator::RING), std::invalid_argument);
    CHECK_THROWS_AS(indicator.SetIndefiniteShape((fx::ProgressIndicator::Shapes)42), std::invalid_argument);
    CHECK_EQ(indicator.GetIndefiniteShape(), fx::ProgressIndicator::DOTS);
}

TEST(indicator_indefinite_shape_by_name) {
    fx::ProgressIndicator indicator;
    indicator.SetIndefiniteShape("arc");
    CHECK_EQ(indicator.GetIndefiniteShapeString(), std::string("ARC"));
    // A known name of the wrong kind fails the same way as an unknown one.
    CHECK_THROWS_AS(indicator.SetIndefiniteShape("RING"), std::invalid_argument);
    CHECK_THROWS_AS(indicator.SetIndefiniteShape("spinny"), std::invalid_argument);
}

TEST(indicator_direction_methods) {
    fx::ProgressIndicator indicator;
    indicator.MakeCounterclockwise();
    CHECK(!indicator.IsClockwise());
    indicator.MakeClockwise();
    CHECK(indicator.IsClockwise());
    indicator.SetClockwise(false);
    CHECK(!indicator.IsClockwise());
}

TEST(indicator_start_angle_is_stored_as_given) {
    fx::ProgressIndicator indicator;
    indicator.SetStartAngle(45.0f);
    CHECK_NEAR(indicator.GetStartAngle(), 45.0f, 1e-6);
    indicator.SetStartAngle(-450.0f);
    CHECK_NEAR(indicator.GetStartAngle(), -450.0f, 1e-6);
}

// --- ProgressIndicator: spin ---------------------------------------------

TEST(indicator_spin_angle_follows_the_duration) {
    IndicatorProbe indicator;
    CHECK_NEAR(indicator.GetSpinAngle(0.0f, 1.0f), 0.0f, 1e-3);
    CHECK_NEAR(indicator.GetSpinAngle(0.25f, 1.0f), 90.0f, 1e-3);
    CHECK_NEAR(indicator.GetSpinAngle(1.0f, 2.0f), 180.0f, 1e-3);
}

TEST(indicator_spin_angle_wraps_each_revolution) {
    IndicatorProbe indicator;
    CHECK_NEAR(indicator.GetSpinAngle(1.25f, 1.0f), 90.0f, 1e-3);
    CHECK_NEAR(indicator.GetSpinAngle(7.25f, 1.0f), 90.0f, 1e-3);
}

TEST(indicator_spin_angle_stays_accurate_after_a_long_session) {
    // Wrapping before scaling keeps this precise. Multiplying first, as
    // the old code did, loses it as elapsed time grows.
    IndicatorProbe indicator;
    CHECK_NEAR(indicator.GetSpinAngle(36000.25f, 1.0f), 90.0f, 0.5);
}

TEST(indicator_spin_angle_reverses_counterclockwise) {
    IndicatorProbe indicator;
    indicator.MakeCounterclockwise();
    CHECK_NEAR(indicator.GetSpinAngle(0.25f, 1.0f), -90.0f, 1e-3);
}

// --- ProgressIndicator: nested settings ----------------------------------

TEST(indicator_dots_defaults) {
    fx::ProgressIndicator indicator;
    CHECK_EQ(indicator.dots.GetCount(), 8);
    CHECK_NEAR(indicator.dots.GetSize(), 1.0f / 6.0f, 1e-6);
    CHECK_EQ(indicator.dots.GetTrailLength(), std::numeric_limits<int>::max());
    CHECK_NEAR(indicator.dots.GetMinimumAlpha(), 0.15f, 1e-6);
    CHECK_NEAR(indicator.dots.GetRevolutionDuration(), 1.0f, 1e-6);
}

TEST(indicator_dots_setters_store_valid_values) {
    fx::ProgressIndicator indicator;
    indicator.dots.SetCount(1);
    CHECK_EQ(indicator.dots.GetCount(), 1);
    indicator.dots.SetSize(0.5f);
    CHECK_NEAR(indicator.dots.GetSize(), 0.5f, 1e-6);
    indicator.dots.SetTrailLength(0);
    CHECK_EQ(indicator.dots.GetTrailLength(), 0);
    indicator.dots.SetMinimumAlpha(0.0f);
    CHECK_NEAR(indicator.dots.GetMinimumAlpha(), 0.0f, 1e-6);
    indicator.dots.SetMinimumAlpha(1.0f);
    CHECK_NEAR(indicator.dots.GetMinimumAlpha(), 1.0f, 1e-6);
    indicator.dots.SetRevolutionDuration(0.01f);
    CHECK_NEAR(indicator.dots.GetRevolutionDuration(), 0.01f, 1e-6);
}

TEST(indicator_dots_setters_reject_bad_values) {
    fx::ProgressIndicator indicator;
    CHECK_THROWS_AS(indicator.dots.SetCount(0), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetSize(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetSize(1.0f), std::invalid_argument);    // would orbit at radius 0
    CHECK_THROWS_AS(indicator.dots.SetSize(std::numeric_limits<float>::quiet_NaN()), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetTrailLength(-1), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetMinimumAlpha(-0.01f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetMinimumAlpha(1.01f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetRevolutionDuration(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.dots.SetRevolutionDuration(-1.0f), std::invalid_argument);
    CHECK_EQ(indicator.dots.GetCount(), 8);   // nothing refused got stored
}

TEST(indicator_arc_defaults_and_limits) {
    fx::ProgressIndicator indicator;
    CHECK_NEAR(indicator.arc.GetLength(), 90.0f, 1e-6);
    CHECK_NEAR(indicator.arc.GetThickness(), 0.2f, 1e-6);
    CHECK_NEAR(indicator.arc.GetRevolutionDuration(), 1.0f, 1e-6);

    indicator.arc.SetThickness(1.0f);                       // a wedge is allowed
    CHECK_NEAR(indicator.arc.GetThickness(), 1.0f, 1e-6);
    indicator.arc.SetLength(359.0f);
    CHECK_NEAR(indicator.arc.GetLength(), 359.0f, 1e-6);

    CHECK_THROWS_AS(indicator.arc.SetLength(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.arc.SetLength(360.0f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.arc.SetThickness(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.arc.SetThickness(1.01f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.arc.SetRevolutionDuration(0.0f), std::invalid_argument);
}

TEST(indicator_ring_defaults_and_limits) {
    fx::ProgressIndicator indicator;
    CHECK_NEAR(indicator.ring.GetThickness(), 0.2f, 1e-6);
    indicator.ring.SetThickness(1.0f);
    CHECK_NEAR(indicator.ring.GetThickness(), 1.0f, 1e-6);
    CHECK_THROWS_AS(indicator.ring.SetThickness(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(indicator.ring.SetThickness(-0.5f), std::invalid_argument);
}

TEST(indicator_shared_looking_settings_are_separate) {
    // The point of the nested groups: arc and ring each own a thickness,
    // arc and dots each own a duration, and setting one leaves the other.
    fx::ProgressIndicator indicator;
    indicator.arc.SetThickness(0.5f);
    CHECK_NEAR(indicator.ring.GetThickness(), 0.2f, 1e-6);
    indicator.dots.SetRevolutionDuration(3.0f);
    CHECK_NEAR(indicator.arc.GetRevolutionDuration(), 1.0f, 1e-6);
}

TEST(indicator_nested_errors_name_the_widget) {
    fx::ProgressIndicator indicator;
    indicator.SetID("spinner");
    const std::string message = InvalidArgumentMessage([&] { indicator.dots.SetCount(0); });
    CHECK(message.find("ProgressIndicator") != std::string::npos);
    CHECK(message.find("spinner") != std::string::npos);
}

// --- ProgressBar: shapes and sweep ---------------------------------------

TEST(bar_defaults) {
    fx::ProgressBar bar;
    CHECK_EQ(bar.GetShape(), fx::ProgressBar::FILL);
    CHECK(!bar.IsSegmented());
    CHECK_NEAR(bar.GetBarMargin(), 5.0f, 1e-6);
    CHECK_EQ(bar.GetNumberOfSegments(), 10);
    CHECK_NEAR(bar.GetGapBetweenSegments(), 1.0f, 1e-6);
    CHECK_EQ(bar.GetFxID(), std::string("ProgressBar"));
    CHECK_NEAR(bar.sweep.GetLength(), 0.25f, 1e-6);
    CHECK_NEAR(bar.sweep.GetPassDuration(), 1.5f, 1e-6);
    CHECK_EQ(bar.sweep.GetMotion(), fx::ProgressBar::Sweep::WRAP);
}

TEST(bar_shapes_by_enum_and_name) {
    fx::ProgressBar bar;
    bar.SetShape(fx::ProgressBar::SWEEP);
    CHECK_EQ(bar.GetShapeString(), std::string("SWEEP"));
    bar.SetShape("fill");
    CHECK_EQ(bar.GetShape(), fx::ProgressBar::FILL);
    CHECK_THROWS_AS(bar.SetShape("RING"), std::invalid_argument);
    CHECK_THROWS_AS(bar.SetShape((fx::ProgressBar::Shapes)7), std::invalid_argument);
    CHECK_EQ(bar.GetShape(), fx::ProgressBar::FILL);
}

TEST(bar_segmented_methods) {
    fx::ProgressBar bar;
    bar.Segmented();
    CHECK(bar.IsSegmented());
    bar.Solid();
    CHECK(!bar.IsSegmented());
    bar.SetSegmented(true);
    CHECK(bar.IsSegmented());
}

TEST(bar_sweep_motion_by_enum_and_name) {
    fx::ProgressBar bar;
    bar.sweep.SetMotion(fx::ProgressBar::Sweep::BOUNCE);
    CHECK_EQ(bar.sweep.GetMotionString(), std::string("BOUNCE"));
    bar.sweep.SetMotion("wrap");
    CHECK_EQ(bar.sweep.GetMotion(), fx::ProgressBar::Sweep::WRAP);
    CHECK_THROWS_AS(bar.sweep.SetMotion("WOBBLE"), std::invalid_argument);
    CHECK_THROWS_AS(bar.sweep.SetMotion((fx::ProgressBar::Sweep::Motions)9), std::invalid_argument);
    CHECK_EQ(bar.sweep.GetMotion(), fx::ProgressBar::Sweep::WRAP);
}

TEST(bar_sweep_setters_reject_bad_values) {
    fx::ProgressBar bar;
    bar.sweep.SetLength(1.0f);
    CHECK_NEAR(bar.sweep.GetLength(), 1.0f, 1e-6);
    CHECK_THROWS_AS(bar.sweep.SetLength(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(bar.sweep.SetLength(1.01f), std::invalid_argument);
    CHECK_THROWS_AS(bar.sweep.SetPassDuration(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(bar.sweep.SetPassDuration(std::numeric_limits<float>::quiet_NaN()), std::invalid_argument);
}

TEST(bar_sweep_errors_name_the_widget) {
    fx::ProgressBar bar;
    bar.SetID("loader");
    const std::string message = InvalidArgumentMessage([&] { bar.sweep.SetMotion("WOBBLE"); });
    CHECK(message.find("ProgressBar") != std::string::npos);
    CHECK(message.find("loader") != std::string::npos);
}

TEST(bar_sweep_phase_wrap) {
    BarProbe bar;                                   // pass takes 1.5 s
    CHECK_NEAR(bar.GetSweepPhase(0.0f), 0.0f, 1e-4);
    CHECK_NEAR(bar.GetSweepPhase(0.75f), 0.5f, 1e-4);
    CHECK_NEAR(bar.GetSweepPhase(1.5f), 0.0f, 1e-4);   // wraps back to the start
    CHECK_NEAR(bar.GetSweepPhase(2.25f), 0.5f, 1e-4);
}

TEST(bar_sweep_phase_bounce) {
    BarProbe bar;
    bar.sweep.SetMotion(fx::ProgressBar::Sweep::BOUNCE);
    CHECK_NEAR(bar.GetSweepPhase(0.0f), 0.0f, 1e-4);
    CHECK_NEAR(bar.GetSweepPhase(0.75f), 0.5f, 1e-4);
    CHECK_NEAR(bar.GetSweepPhase(1.5f), 1.0f, 1e-4);   // turns at the far end
    CHECK_NEAR(bar.GetSweepPhase(2.25f), 0.5f, 1e-4);  // on the way back
    CHECK_NEAR(bar.GetSweepPhase(3.0f), 0.0f, 1e-4);
}

TEST(bar_sweep_phase_stays_in_range) {
    BarProbe bar;
    for (auto motion : {fx::ProgressBar::Sweep::WRAP, fx::ProgressBar::Sweep::BOUNCE}) {
        bar.sweep.SetMotion(motion);
        for (int i = 0; i < 200; ++i) {
            const float phase = bar.GetSweepPhase((float)i * 0.037f);
            CHECK(phase >= 0.0f && phase <= 1.0f);
        }
    }
}

TEST(bar_sweep_segments_round_and_clamp) {
    BarProbe bar;                                   // 10 segments
    CHECK_EQ(bar.GetSweepSegments(), 3);            // 2.5 rounds up
    bar.sweep.SetLength(0.01f);
    CHECK_EQ(bar.GetSweepSegments(), 1);            // never less than one
    bar.sweep.SetLength(1.0f);
    CHECK_EQ(bar.GetSweepSegments(), 10);
    bar.SetNumberOfSegments(1);
    CHECK_EQ(bar.GetSweepSegments(), 1);
}

// --- ProgressBar: geometry -----------------------------------------------

TEST(bar_inner_rectangle_sits_inside_the_margin) {
    BarProbe bar;                                   // 200 x 100, margin 5
    bar.SetXY(10.0f, 20.0f);
    const ::Rectangle inner = bar.GetInnerRectangle();
    CHECK_NEAR(inner.x, 15.0f, 1e-4);
    CHECK_NEAR(inner.y, 25.0f, 1e-4);
    CHECK_NEAR(inner.width, 190.0f, 1e-4);
    CHECK_NEAR(inner.height, 90.0f, 1e-4);
}

TEST(bar_segment_width_shares_out_the_inner_width) {
    BarProbe bar;
    // (190 - 9 gaps of 1) / 10
    CHECK_NEAR(bar.GetSegmentWidth(), 18.1f, 1e-4);
    bar.SetNumberOfSegments(1);
    CHECK_NEAR(bar.GetSegmentWidth(), 190.0f, 1e-4);   // no gaps at all
}

TEST(bar_segments_and_gaps_add_up_to_the_inner_width) {
    BarProbe bar;
    for (int n : {1, 2, 3, 7, 10, 25}) {
        bar.SetSegments(n, 2.0f);
        const float total = ((float)n * bar.GetSegmentWidth()) + ((float)(n - 1) * bar.GetEffectiveGap());
        CHECK_NEAR(total, bar.GetInnerRectangle().width, 1e-3);
    }
}

// --- ProgressBar: clamping instead of throwing ---------------------------

TEST(bar_negative_values_still_throw) {
    fx::ProgressBar bar;
    CHECK_THROWS_AS(bar.SetBarMargin(-1.0f), std::invalid_argument);
    CHECK_THROWS_AS(bar.SetGapBetweenSegments(-1.0f), std::invalid_argument);
    CHECK_THROWS_AS(bar.SetNumberOfSegments(0), std::invalid_argument);
    CHECK_THROWS_AS(bar.SetSegments(0, 1.0f), std::invalid_argument);
    CHECK_THROWS_AS(bar.SetSegments(5, -1.0f), std::invalid_argument);
}

TEST(bar_set_segments_changes_nothing_when_it_throws) {
    fx::ProgressBar bar;
    CHECK_THROWS_AS(bar.SetSegments(5, -1.0f), std::invalid_argument);
    CHECK_EQ(bar.GetNumberOfSegments(), 10);
    CHECK_NEAR(bar.GetGapBetweenSegments(), 1.0f, 1e-6);
}

TEST(bar_margin_that_does_not_fit_is_kept_and_clamped) {
    BarProbe bar;                                   // 200 x 100
    CerrCapture cerr;
    bar.SetBarMargin(80.0f);
    CHECK_NEAR(bar.GetBarMargin(), 80.0f, 1e-6);     // stored as given
    CHECK_NEAR(bar.GetMaxBarMargin(), 50.0f, 1e-6);  // half of the shorter side
    CHECK_NEAR(bar.GetEffectiveBarMargin(), 50.0f, 1e-6);
    CHECK(bar.GetInnerRectangle().height >= 0.0f);   // never inside out
}

TEST(bar_margin_checks_the_width_too) {
    BarProbe bar;
    bar.SetDimensions(40.0f, 200.0f);               // tall and narrow
    CerrCapture cerr;
    bar.SetBarMargin(30.0f);
    CHECK_NEAR(bar.GetEffectiveBarMargin(), 20.0f, 1e-6);
    CHECK(bar.GetInnerRectangle().width >= 0.0f);
}

TEST(bar_margin_comes_back_after_a_resize) {
    BarProbe bar;
    CerrCapture cerr;
    bar.SetBarMargin(80.0f);
    bar.SetHeight(300.0f);                           // 200 x 300 now fits 80
    CHECK_NEAR(bar.GetEffectiveBarMargin(), 80.0f, 1e-6);
    bar.SetHeight(20.0f);                            // and shrinking clamps again
    CHECK_NEAR(bar.GetEffectiveBarMargin(), 10.0f, 1e-6);
}

TEST(bar_max_gap_leaves_every_segment_a_pixel) {
    BarProbe bar;
    // (190 inner - 10 segments * 1 px) / 9 gaps
    CHECK_NEAR(bar.GetMaxGap(), 20.0f, 1e-4);
    CerrCapture cerr;
    bar.SetGapBetweenSegments(50.0f);
    CHECK_NEAR(bar.GetGapBetweenSegments(), 50.0f, 1e-6);
    CHECK_NEAR(bar.GetEffectiveGap(), 20.0f, 1e-4);
    CHECK_NEAR(bar.GetSegmentWidth(), 1.0f, 1e-4);
}

TEST(bar_one_segment_takes_any_gap) {
    BarProbe bar;
    bar.SetNumberOfSegments(1);
    CerrCapture cerr;
    bar.SetGapBetweenSegments(10000.0f);
    CHECK(cerr.Empty());                             // nothing to fit, nothing to warn about
    CHECK_NEAR(bar.GetSegmentWidth(), 190.0f, 1e-4);
}

TEST(bar_more_segments_than_pixels_has_no_gap) {
    BarProbe bar;
    CerrCapture cerr;
    bar.SetNumberOfSegments(500);                    // 190 px for 500 segments
    CHECK_NEAR(bar.GetMaxGap(), 0.0f, 1e-6);
    CHECK_NEAR(bar.GetEffectiveGap(), 0.0f, 1e-6);
    CHECK(bar.GetSegmentWidth() > 0.0f);             // sub-pixel, but not negative
}

TEST(bar_order_of_segment_setters_does_not_matter) {
    // Big gap first, then few enough segments for it to fit - the case that
    // used to throw. Both orders must end in the same drawn state.
    BarProbe a, b;
    {
        CerrCapture cerr;
        a.SetGapBetweenSegments(30.0f);
        a.SetNumberOfSegments(4);
    }
    b.SetNumberOfSegments(4);
    b.SetGapBetweenSegments(30.0f);
    CHECK_NEAR(a.GetEffectiveGap(), 30.0f, 1e-6);
    CHECK_NEAR(a.GetEffectiveGap(), b.GetEffectiveGap(), 1e-6);
    CHECK_NEAR(a.GetSegmentWidth(), b.GetSegmentWidth(), 1e-6);
}

TEST(bar_warnings_are_printed) {
#ifdef NDEBUG
    SKIP("CPPFX_WARN compiles to nothing under NDEBUG");
#else
    BarProbe bar;
    bar.SetID("loader");
    {
        CerrCapture cerr;
        bar.SetBarMargin(80.0f);
        CHECK(cerr.Contains("loader"));
        CHECK(cerr.Contains("bar margin"));
    }
    {
        CerrCapture cerr;
        bar.SetGapBetweenSegments(500.0f);
        CHECK(cerr.Contains("gap between segments"));
    }
    {
        CerrCapture cerr;
        bar.SetSegments(4, 500.0f);
        CHECK(cerr.Contains("gap between segments"));
    }
#endif
}

TEST(bar_fitting_values_do_not_warn) {
    BarProbe bar;
    CerrCapture cerr;
    bar.SetBarMargin(10.0f);
    bar.SetGapBetweenSegments(2.0f);
    bar.SetNumberOfSegments(20);
    bar.SetSegments(5, 4.0f);
    CHECK(cerr.Empty());
}

TEST(bar_set_segments_warns_once) {
#ifdef NDEBUG
    SKIP("CPPFX_WARN compiles to nothing under NDEBUG");
#else
    // Calling the two setters in a row would warn about a gap that's about
    // to stop mattering. SetSegments checks only the final state.
    BarProbe bar;
    bar.SetSegments(4, 50.0f);                       // 50 fits 4 segments, not 10
    CerrCapture cerr;
    bar.SetSegments(10, 5.0f);                       // count first would have warned
    CHECK(cerr.Empty());
#endif
}
