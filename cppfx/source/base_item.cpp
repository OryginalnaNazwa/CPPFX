#include "../include/CPPFX/base_item.hpp"

#include <stdexcept>  // for invalid_argument
#include <utility>    // for swap

using namespace CPPFX;

// --- Item ---

bool Item::WasIClicked(Vector2 mousePosition) const {
    float xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

void Item::DoFocusAction(float elapsedTime, Vector2 /*mousePosition*/) {
    return DoFocusAction(elapsedTime);
}

void Item::DoPassiveAction(float /*elapsedTime*/) {
    return;
}

void Item::DoHoverAction(float /*elapsedTime*/, const Vector2 /*mousePosition*/) {
    return;
}

void Item::GetCallbacks(std::vector<Callback*>& out) {
    out.insert(out.end(), {&onClick, &onMove, &onChange, &onFocus, &onResize, &onHover, &onDehover});
}

bool Item::HasArmedCallbacks() {
    std::vector<Callback*> callbacks;
    GetCallbacks(callbacks);
    return std::any_of(callbacks.begin(), callbacks.end(),
                       [](const Callback* c) { return c->IsArmed(); });
}

void Item::DoCallbacks() {
    std::vector<Callback*> callbacks;
    GetCallbacks(callbacks);
    for (auto& cb : callbacks) {
        cb->Do();
    }
}

void Item::DisarmCallbacks() {
    std::vector<Callback*> callbacks;
    GetCallbacks(callbacks);
    for (auto& cb : callbacks) {
        cb->Disarm();
    }
}

void Item::Hover() {
    if (!hovered) onHover.Arm();
    hovered = true;
}

void Item::Dehover() {
    if (hovered) onDehover.Arm();
    hovered = false;
}

bool Item::IsHovered() const {
    return hovered;
}

// --- Helpers ---

void Item::SwapHoverColour() {
    std::swap(colour, hoveredColour);
}

void Item::SetHoverCallbacksToSwap() {
    onHover.Set([this](){ SwapHoverColour(); });
    onDehover.Set([this](){ SwapHoverColour(); });
    if (hovered) SwapHoverColour();
}

void Item::MoveUpPriority() {
    if (priority > (size_t)(0)) {
        priority--;
    }
}

void Item::MoveDownPriority() {
    priority++;
}

// --- Setters ---
void Item::SetX(float x) {
    if (xAnchor != x) onMove.Arm();
    xAnchor = x;
}
void Item::SetY(float y) {
    if (yAnchor != y) onMove.Arm();
    yAnchor = y;
}

void Item::SetXY(float xy) {
    SetX(xy);
    SetY(xy);
}

void Item::SetXY(float x, float y) {
    SetX(x);
    SetY(y);
}

void Item::SetXY(Vector2 xy) {
    SetX(xy.x);
    SetY(xy.y);
}

void Item::SetHeight(float value) {
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative height.");
    }
    if (value != height) onResize.Arm();
    height = value;
}

void Item::SetWidth(float value) {
    if (value < 0.0f) {
        CPPFX_THROW(std::invalid_argument, "Negative width.");
    }
    if (value != width) onResize.Arm();
    width = value;
}

void Item::SetDimensions(float width, float height) {
    SetWidth(width);
    SetHeight(height);
}

void Item::SetDimensions(Vector2 dimensions) {
    SetWidth(dimensions.x);
    SetHeight(dimensions.y);
}

void Item::SetDimensions(float value) {
    SetWidth(value);
    SetHeight(value);
}

void Item::SetPriority(int value) {
    if (value < 0) {
        CPPFX_THROW(std::invalid_argument, "Negative priority.");
    }
    priority = (size_t)(value);
}

void Item::SetID(const std::string& id) {
    if (id.empty()) {
        CPPFX_THROW(std::invalid_argument, "New ID would be empty.");
    }
    ID = id;
}

void Item::Focus() {
    if (!focused) onFocus.Arm();
    focused = true;
}

void Item::Defocus() {
    focused = false;
}

void Item::MakeInactive() {
    inactive = true;
}

void Item::MakeActive() {
    inactive = false;
}

void Item::SetInactive(bool flag) {
    inactive = flag;
}

void Item::MakeInvisible() {
    visible = false;
}

void Item::MakeVisible() {
    visible = true;
}

void Item::SetVisible(bool flag) {
    visible = flag;
}

void Item::Hide() {
    MakeInvisible();
    MakeInactive();
}

void Item::Show() {
    MakeActive();
    MakeVisible();
}

void Item::SetToWorld() {
    screenBased = false;
}

void Item::SetToScreen() {
    screenBased = true;
}

void Item::ConsumeClicks() {
    eatsClick = true;
}

void Item::LetClicksThrough() {
    eatsClick = false;
}

// --- Getters ---

float Item::GetX() const {
    return xAnchor;
}

float Item::GetY() const {
    return yAnchor;
}

Vector2 Item::GetXY() const {
    return {GetX(), GetY()};
}

float Item::GetHeight() const {
    return height;
}

float Item::GetTotalHeight() const {
    return height + (2.0f * border.GetThickness());
}

float Item::GetWidth() const {
    return width;
}

float Item::GetTotalWidth() const {
    return width + (2.0f * border.GetThickness());
}

std::string Item::GetFxID() const {
    return fxID;
}

const std::string Item::GetClassID() {
    return "Item";
}

size_t Item::GetPriority() const {
    return priority;
}

std::string Item::GetID() const {
    return ID;
}

bool Item::IsFocused() const {
    return focused;
}

bool Item::IsInactive() const {
    return inactive;
}

bool Item::IsScreenBased() const {
    return screenBased;
}

bool Item::DoesEatClicks() const {
    return eatsClick;
}

bool Item::IsVisible() const {
    return visible;
}
