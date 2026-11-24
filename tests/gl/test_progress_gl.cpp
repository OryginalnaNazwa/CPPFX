#include "harness.hpp"

#include <CPPFX/items.hpp>

#include <cmath>
#include <cstdlib>
#include <string>

/**********************************************************************
 *  Drawing tests for ProgressIndicator and ProgressBar.
 *
 *  Each test draws into a render texture and reads pixels back. The
 *  sample points are picked well inside or well outside each shape, so
 *  rasterisation details at the edges can't make a test flaky.
 *
 *  Indicators sit at (50, 50) with the default 100 x 100 size, so the
 *  centre is (100, 100) and the radius 50. Angles follow raylib: 0 is
 *  3 o'clock and they grow clockwise on screen, so -90 is 12 o'clock.
 *
 *  Bars sit at (0, 0), 200 x 40, margin 5: the inner bar spans
 *  x 5 .. 195. The background is BLUE and the bar RED throughout.
 **********************************************************************/

namespace fx = CPPFX;

namespace {

bool DisplayAvailable() {
    if (std::getenv("CPPFX_TESTS_NO_WINDOW") != nullptr) return false;
#if defined(__linux__)
    return std::getenv("DISPLAY") != nullptr ||
           std::getenv("WAYLAND_DISPLAY") != nullptr;
#else
    return true;
#endif
}

struct WindowFixture {
    WindowFixture() {
        SetTraceLogLevel(LOG_NONE);
        InitWindow(64, 64, "CPPFX tests");
    }
    ~WindowFixture() {
        if (IsWindowReady()) CloseWindow();
    }
    bool ok() const { return IsWindowReady(); }
};

/// Somewhere to draw, and a way to read the result back.
struct Canvas {
    RenderTexture2D target;
    Image image{};

    Canvas(int w, int h) : target(LoadRenderTexture(w, h)) {}
    ~Canvas() {
        if (image.data != nullptr) UnloadImage(image);
        UnloadRenderTexture(target);
    }

    template <typename F>
    void Draw(F draw) {
        BeginTextureMode(target);
        ClearBackground(BLACK);
        draw();
        EndTextureMode();
        if (image.data != nullptr) UnloadImage(image);
        image = LoadImageFromTexture(target.texture);
        ImageFlipVertical(&image);   // render textures come back upside down
    }

    Color At(float x, float y) const {
        return GetImageColor(image, (int)std::lround(x), (int)std::lround(y));
    }
    /// "r,g,b", so a failing CHECK_EQ prints the colour it got.
    std::string Rgb(float x, float y) const {
        const Color c = At(x, y);
        return std::to_string(c.r) + "," + std::to_string(c.g) + "," + std::to_string(c.b);
    }
};

const std::string RED_   = "230,41,55";    // raylib's RED
const std::string BLUE_  = "0,121,241";    // raylib's BLUE
const std::string GREEN_ = "0,228,48";     // raylib's GREEN
const std::string BLACK_ = "0,0,0";

/// A point at an angle and distance from the indicator's centre.
struct Polar { float x, y; };
Polar At(float degrees, float distance) {
    return {100.0f + (std::cos(degrees * DEG2RAD) * distance),
            100.0f + (std::sin(degrees * DEG2RAD) * distance)};
}

/// Configures in place: copying a widget would leave its nested settings
/// and its round border pointing at the original.
void Setup(fx::ProgressIndicator& indicator, fx::ProgressIndicator::Shapes shape) {
    indicator.SetXY(50.0f, 50.0f);
    indicator.SetShape(shape);
    indicator.colour.SetColour(RED);
    indicator.trackColour.SetColour(BLUE);
    indicator.DoNotShowPercentage();
}

void Setup(fx::ProgressBar& bar) {
    bar.SetXY(0.0f, 0.0f);
    bar.SetDimensions(200.0f, 40.0f);
    bar.colour.SetColour(BLUE);
    bar.barColour.SetColour(RED);
    bar.DoNotShowPercentage();
}

}

#define NEEDS_WINDOW(name)                                                     \
    if (!DisplayAvailable()) SKIP("no display");                               \
    WindowFixture name;                                                        \
    if (!name.ok()) SKIP("window could not be created")

// --- RING and CIRCLE -----------------------------------------------------

TEST(gl_ring_fills_clockwise_from_twelve) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.SetProgress(0.25f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    // Default thickness 0.2: the ring spans 40 .. 50 from the centre.
    auto filled = At(-45.0f, 45.0f), early = At(-80.0f, 45.0f);
    auto past = At(45.0f, 45.0f), opposite = At(135.0f, 45.0f), before = At(-100.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(filled.x, filled.y), RED_);
    CHECK_EQ(canvas.Rgb(early.x, early.y), RED_);
    CHECK_EQ(canvas.Rgb(past.x, past.y), BLUE_);
    CHECK_EQ(canvas.Rgb(opposite.x, opposite.y), BLUE_);
    CHECK_EQ(canvas.Rgb(before.x, before.y), BLUE_);   // counterclockwise of the start
    CHECK_EQ(canvas.Rgb(100.0f, 100.0f), BLACK_);       // the hole stays a hole
}

TEST(gl_ring_fills_counterclockwise) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.MakeCounterclockwise();
    indicator.SetProgress(0.25f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    auto filled = At(-135.0f, 45.0f), notFilled = At(-45.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(filled.x, filled.y), RED_);
    CHECK_EQ(canvas.Rgb(notFilled.x, notFilled.y), BLUE_);
}

TEST(gl_start_angle_moves_the_fill) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.SetStartAngle(0.0f);            // 3 o'clock
    indicator.SetProgress(0.25f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    auto filled = At(45.0f, 45.0f), notFilled = At(-45.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(filled.x, filled.y), RED_);
    CHECK_EQ(canvas.Rgb(notFilled.x, notFilled.y), BLUE_);
}

TEST(gl_ring_thickness_moves_the_inner_edge) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.SetProgress(0.25f);
    auto inside = At(-45.0f, 30.0f);

    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(inside.x, inside.y), BLACK_);   // 30 is inside the 0.2 ring

    indicator.ring.SetThickness(0.5f);                  // now spans 25 .. 50
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(inside.x, inside.y), RED_);
}

TEST(gl_circle_fills_like_a_pie) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::CIRCLE);
    indicator.SetProgress(0.25f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    auto filled = At(-45.0f, 25.0f), notFilled = At(135.0f, 25.0f);
    CHECK_EQ(canvas.Rgb(filled.x, filled.y), RED_);
    CHECK_EQ(canvas.Rgb(notFilled.x, notFilled.y), BLUE_);
}

TEST(gl_fill_at_both_ends) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    auto a = At(-45.0f, 45.0f), b = At(135.0f, 45.0f);

    indicator.SetProgress(0.0f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(a.x, a.y), BLUE_);
    CHECK_EQ(canvas.Rgb(b.x, b.y), BLUE_);

    indicator.SetProgress(1.0f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(a.x, a.y), RED_);
    CHECK_EQ(canvas.Rgb(b.x, b.y), RED_);
}

TEST(gl_track_is_not_drawn_under_the_fill) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.colour.SetColour(Fade(RED, 0.5f));   // see-through fill
    indicator.SetProgress(0.25f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    // Half of raylib's RED (230, 41, 55) over black has a blue of about
    // 27. With the track drawn full underneath, it would be about 148.
    auto filled = At(-45.0f, 45.0f);
    CHECK(canvas.At(filled.x, filled.y).b < 60);
    CHECK(canvas.At(filled.x, filled.y).r > 100);
}

TEST(gl_no_track_by_default) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    indicator.SetXY(50.0f, 50.0f);
    indicator.SetShape(fx::ProgressIndicator::RING);
    indicator.DoNotShowPercentage();
    indicator.SetProgress(0.25f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    auto notFilled = At(135.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(notFilled.x, notFilled.y), BLACK_);
}

// --- indefinite fallbacks ------------------------------------------------

TEST(gl_indefinite_circle_falls_back_to_dots) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::CIRCLE);
    indicator.SetProgress(0.5f);
    indicator.MakeIndefinite();
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    // The leading dot starts at 12 o'clock, orbiting at 50 - 50/6.
    const float orbit = 50.0f - (50.0f / 6.0f);
    auto head = At(-90.0f, orbit);
    CHECK_EQ(canvas.Rgb(head.x, head.y), RED_);
    // And there's no pie or track left over from the shape.
    auto pie = At(135.0f, 25.0f);
    CHECK_EQ(canvas.Rgb(pie.x, pie.y), BLACK_);
}

TEST(gl_indefinite_ring_falls_back_to_the_arc_when_asked) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.SetIndefiniteShape(fx::ProgressIndicator::ARC);
    indicator.MakeIndefinite();
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    // At t = 0 the head is at 12 o'clock and the 90-degree tail trails
    // back to 9 o'clock.
    auto onArc = At(-135.0f, 45.0f), offArc = At(-45.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(onArc.x, onArc.y), RED_);
    CHECK_EQ(canvas.Rgb(offArc.x, offArc.y), BLUE_);    // the track behind the arc
    CHECK_EQ(canvas.Rgb(100.0f, 100.0f), BLACK_);
}

TEST(gl_dots_shape_ignores_the_value) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    const float orbit = 50.0f - (50.0f / 6.0f);
    auto head = At(-90.0f, orbit);

    indicator.SetProgress(0.1f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    const std::string atTenPercent = canvas.Rgb(head.x, head.y);
    indicator.SetProgress(0.9f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(head.x, head.y), atTenPercent);
    CHECK_EQ(atTenPercent, RED_);
}

// --- DOTS ----------------------------------------------------------------

TEST(gl_dots_fade_behind_the_head) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    indicator.colour.SetColour(WHITE);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    const float orbit = 50.0f - (50.0f / 6.0f);
    auto head = At(-90.0f, orbit);     // k = 0
    auto next = At(-135.0f, orbit);    // k = 1, just behind it
    auto last = At(-45.0f, orbit);     // k = 7, the faintest, just ahead
    const int h = canvas.At(head.x, head.y).r;
    const int n = canvas.At(next.x, next.y).r;
    const int l = canvas.At(last.x, last.y).r;
    CHECK_EQ(h, 255);
    CHECK(n < h);
    CHECK(l < n);
    CHECK(l > 0);   // no dot disappears - the old alpha of 0 for the first dot
}

TEST(gl_dots_trail_flips_counterclockwise) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    indicator.colour.SetColour(WHITE);
    indicator.MakeCounterclockwise();
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    const float orbit = 50.0f - (50.0f / 6.0f);
    auto behind = At(-45.0f, orbit);   // behind the head when turning counterclockwise
    auto ahead = At(-135.0f, orbit);
    CHECK(canvas.At(behind.x, behind.y).r > canvas.At(ahead.x, ahead.y).r);
}

TEST(gl_dots_keep_the_colours_own_alpha) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    indicator.colour.SetColour(Fade(WHITE, 0.5f));
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    const float orbit = 50.0f - (50.0f / 6.0f);
    auto head = At(-90.0f, orbit);
    const int h = canvas.At(head.x, head.y).r;
    CHECK(h > 100 && h < 156);   // half of white over black, not full white
}

TEST(gl_dots_stay_inside_the_circle) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    indicator.colour.SetColour(WHITE);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });
    // The head is at 12 o'clock. The old code orbited at the full radius,
    // so its dot stuck out above the top edge at y = 50.
    CHECK_EQ(canvas.Rgb(100.0f, 47.0f), BLACK_);
}

TEST(gl_dots_turn_with_time) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    indicator.colour.SetColour(WHITE);
    canvas.Draw([&] { indicator.DrawMyself(0.25f); });   // a quarter of a 1 s turn

    const float orbit = 50.0f - (50.0f / 6.0f);
    auto head = At(0.0f, orbit);                            // now at 3 o'clock
    CHECK_EQ((int)canvas.At(head.x, head.y).r, 255);
}

TEST(gl_dots_trail_of_zero_leaves_only_the_head_bright) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::DOTS);
    indicator.colour.SetColour(WHITE);
    indicator.dots.SetTrailLength(0);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    const float orbit = 50.0f - (50.0f / 6.0f);
    auto next = At(-135.0f, orbit), last = At(-45.0f, orbit);
    // Everything but the head is at the minimum alpha.
    CHECK_EQ((int)canvas.At(next.x, next.y).r, (int)canvas.At(last.x, last.y).r);
    CHECK(canvas.At(next.x, next.y).r < 60);
}

// --- ARC -----------------------------------------------------------------

TEST(gl_arc_spins) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::ARC);
    indicator.trackColour.SetColour(BLANK);
    canvas.Draw([&] { indicator.DrawMyself(0.25f); });   // head now at 3 o'clock

    auto onArc = At(-45.0f, 45.0f), offArc = At(-135.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(onArc.x, onArc.y), RED_);
    CHECK_EQ(canvas.Rgb(offArc.x, offArc.y), BLACK_);
}

TEST(gl_arc_length_and_thickness) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::ARC);
    indicator.trackColour.SetColour(BLANK);
    indicator.arc.SetLength(180.0f);
    indicator.arc.SetThickness(0.5f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    // Head at 12, tail at 6, along the left: 25 .. 50 from the centre.
    auto left = At(180.0f, 30.0f), right = At(0.0f, 30.0f);
    CHECK_EQ(canvas.Rgb(left.x, left.y), RED_);
    CHECK_EQ(canvas.Rgb(right.x, right.y), BLACK_);
}

// --- border --------------------------------------------------------------

TEST(gl_round_border_is_a_ring) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::ProgressIndicator indicator;
    Setup(indicator, fx::ProgressIndicator::RING);
    indicator.trackColour.SetColour(BLANK);
    indicator.border.SetThickness(4.0f);
    indicator.border.colour.SetColour(GREEN);
    indicator.SetProgress(0.0f);
    canvas.Draw([&] { indicator.DrawMyself(0.0f); });

    auto onBorder = At(90.0f, 52.0f);
    CHECK_EQ(canvas.Rgb(onBorder.x, onBorder.y), GREEN_);
    // The old border was a filled disc, which would paint all of this.
    CHECK_EQ(canvas.Rgb(100.0f, 100.0f), BLACK_);
    auto inRing = At(90.0f, 45.0f);
    CHECK_EQ(canvas.Rgb(inRing.x, inRing.y), BLACK_);
}

TEST(gl_round_border_is_drawn_around_a_circle_too) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 200);
    fx::Circle circle;
    circle.SetXY(50.0f, 50.0f);
    circle.SetWidth(100.0f);
    circle.colour.SetColour(Fade(RED, 0.5f));
    circle.border.SetThickness(4.0f);
    circle.border.colour.SetColour(GREEN);
    canvas.Draw([&] { circle.DrawMyself(0.0f); });

    auto onBorder = At(90.0f, 52.0f);
    CHECK_EQ(canvas.Rgb(onBorder.x, onBorder.y), GREEN_);
    // A see-through circle no longer shows a green disc behind it: half
    // of RED over black has a green of about 20, over GREEN about 134.
    CHECK(canvas.At(100.0f, 100.0f).g < 60);
}

// --- ProgressBar: FILL ---------------------------------------------------

TEST(gl_bar_fills_from_the_left) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.SetProgress(0.5f);                    // filled to x = 100
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(50.0f, 20.0f), RED_);
    CHECK_EQ(canvas.Rgb(95.0f, 20.0f), RED_);
    CHECK_EQ(canvas.Rgb(105.0f, 20.0f), BLUE_);
    CHECK_EQ(canvas.Rgb(2.0f, 20.0f), BLUE_);   // the margin
    CHECK_EQ(canvas.Rgb(50.0f, 2.0f), BLUE_);
}

TEST(gl_bar_indefinite_does_not_draw_the_fill) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.SetProgress(1.0f);
    bar.MakeIndefinite();
    // At t = 0 a WRAP sweep is just off the left edge, so nothing shows.
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(100.0f, 20.0f), BLUE_);
    CHECK_EQ(canvas.Rgb(180.0f, 20.0f), BLUE_);
}

TEST(gl_bar_segments_fill_whole_segments) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.Segmented();
    bar.SetProgress(0.75f);   // 7.5 of 10: seven drawn
    canvas.Draw([&] { bar.DrawMyself(0.0f); });

    // Segment i spans 5 + 19.1 i .. + 18.1, with 1 px gaps.
    CHECK_EQ(canvas.Rgb(14.0f, 20.0f), RED_);    // segment 0
    CHECK_EQ(canvas.Rgb(128.0f, 20.0f), RED_);   // segment 6
    CHECK_EQ(canvas.Rgb(148.0f, 20.0f), BLUE_);  // segment 7 isn't reached yet
    CHECK_EQ(canvas.Rgb(23.0f, 20.0f), BLUE_);   // the gap after segment 0
}

TEST(gl_bar_clamped_gap_still_draws_segments) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.Segmented();
    bar.SetSegments(2, 1000.0f);   // max gap is 188, leaving two 1 px segments
    bar.SetProgress(1.0f);
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(5.0f, 20.0f), RED_);     // first segment at the left edge
    CHECK_EQ(canvas.Rgb(194.0f, 20.0f), RED_);   // second at the right edge
    CHECK_EQ(canvas.Rgb(100.0f, 20.0f), BLUE_);  // the huge gap between
}

TEST(gl_bar_margin_too_big_draws_nothing_inside_out) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.SetBarMargin(30.0f);      // clamped to 20: the inner bar has no height
    bar.SetProgress(1.0f);
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(100.0f, 20.0f), BLUE_);
    CHECK_EQ(canvas.Rgb(100.0f, 5.0f), BLUE_);
}

// --- ProgressBar: SWEEP --------------------------------------------------

TEST(gl_sweep_wrap_in_the_middle) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.MakeIndefinite();
    // Phase 0.5 of a 1.5 s pass: the chunk covers 0.375 .. 0.625 of the
    // inner bar, x 76.25 .. 123.75.
    canvas.Draw([&] { bar.DrawMyself(0.75f); });
    CHECK_EQ(canvas.Rgb(100.0f, 20.0f), RED_);
    CHECK_EQ(canvas.Rgb(80.0f, 20.0f), RED_);
    CHECK_EQ(canvas.Rgb(70.0f, 20.0f), BLUE_);
    CHECK_EQ(canvas.Rgb(130.0f, 20.0f), BLUE_);
}

TEST(gl_sweep_wrap_enters_from_the_left_edge) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.MakeIndefinite();
    // Phase 0.1: start at -0.125, so only 0 .. 0.125 shows, x 5 .. 28.75.
    canvas.Draw([&] { bar.DrawMyself(0.15f); });
    CHECK_EQ(canvas.Rgb(10.0f, 20.0f), RED_);
    CHECK_EQ(canvas.Rgb(2.0f, 20.0f), BLUE_);    // cut off at the margin, not drawn into it
    CHECK_EQ(canvas.Rgb(40.0f, 20.0f), BLUE_);
}

TEST(gl_sweep_bounce_turns_at_the_far_end) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.MakeIndefinite();
    bar.sweep.SetMotion(fx::ProgressBar::Sweep::BOUNCE);
    // Phase 1 after one pass: the chunk is flush with the right end,
    // 0.75 .. 1 of the inner bar, x 147.5 .. 195.
    canvas.Draw([&] { bar.DrawMyself(1.5f); });
    CHECK_EQ(canvas.Rgb(190.0f, 20.0f), RED_);
    CHECK_EQ(canvas.Rgb(197.0f, 20.0f), BLUE_);
    CHECK_EQ(canvas.Rgb(140.0f, 20.0f), BLUE_);
}

TEST(gl_sweep_shape_while_definite) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.SetShape(fx::ProgressBar::SWEEP);
    bar.SetProgress(0.9f);
    canvas.Draw([&] { bar.DrawMyself(0.75f); });
    CHECK_EQ(canvas.Rgb(100.0f, 20.0f), RED_);   // the sweep
    CHECK_EQ(canvas.Rgb(50.0f, 20.0f), BLUE_);   // no fill under it
    CHECK_EQ(canvas.Rgb(180.0f, 20.0f), BLUE_);
}

TEST(gl_segmented_sweep_wrap_enters_one_segment_at_a_time) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.Segmented();
    bar.MakeIndefinite();
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(14.0f, 20.0f), RED_);    // segment 0 only
    CHECK_EQ(canvas.Rgb(33.0f, 20.0f), BLUE_);
}

TEST(gl_segmented_sweep_bounce_starts_whole) {
    NEEDS_WINDOW(window);
    Canvas canvas(200, 40);
    fx::ProgressBar bar;
    Setup(bar);
    bar.Segmented();
    bar.MakeIndefinite();
    bar.sweep.SetMotion(fx::ProgressBar::Sweep::BOUNCE);
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    // 0.25 of 10 rounds to 3 segments, all inside from the start.
    CHECK_EQ(canvas.Rgb(14.0f, 20.0f), RED_);    // segment 0
    CHECK_EQ(canvas.Rgb(52.0f, 20.0f), RED_);    // segment 2
    CHECK_EQ(canvas.Rgb(71.0f, 20.0f), BLUE_);   // segment 3
}

TEST(gl_bar_border_is_rectangular) {
    NEEDS_WINDOW(window);
    Canvas canvas(220, 60);
    fx::ProgressBar bar;
    Setup(bar);
    bar.SetXY(10.0f, 10.0f);
    bar.border.SetThickness(4.0f);
    bar.border.colour.SetColour(GREEN);
    bar.SetProgress(0.5f);
    canvas.Draw([&] { bar.DrawMyself(0.0f); });
    CHECK_EQ(canvas.Rgb(8.0f, 30.0f), GREEN_);    // left of the bar
    CHECK_EQ(canvas.Rgb(110.0f, 8.0f), GREEN_);   // above it
    CHECK_EQ(canvas.Rgb(60.0f, 30.0f), RED_);     // the fill is untouched
}
