#include "base_item.hpp"

#include <stdexcept>  // for invalid_argument

using namespace CPPFX;

// --- Item ---

void Item::DrawMyself(float elapsedTime, const Camera2D& camera) const {
    float savedX = xAnchor, savedY = yAnchor;
    xAnchor = camera.target.x + (xAnchor - camera.offset.x) / camera.zoom; // this is very ugly. Probably needs a rework with internal draw taking actual parameters.
    yAnchor = camera.target.y + (yAnchor - camera.offset.y) / camera.zoom;
    DrawMyself(elapsedTime);
    xAnchor = savedX;
    yAnchor = savedY;
}

bool Item::WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const {
    float height = this->height * camera.zoom; // works. Not sure why. Praise the Omnissiah.
    float width = this->width * camera.zoom;
    float xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

bool Item::WasIClicked(const Vector2& mousePosition) const {
    float xClick = mousePosition.x, yClick = mousePosition.y;
    if (xClick >= xAnchor && xClick <= (xAnchor + width) && yClick >= yAnchor && yClick <= (yAnchor + height) ) {
        return true;
    }
    return false;
}

void Item::DoFocusAction(float elapsedTime, const Vector2& mousePosition) {
    return DoFocusAction(elapsedTime);
}

void Item::DoFocusAction(float elapsedTime, const Vector2& mousePosition, const Camera2D& camera) {
    return DoFocusAction(elapsedTime, mousePosition);
}

void Item::DoFocusAction(float elapsedTime, const Camera2D& camera) {
    return DoFocusAction(elapsedTime);
}

void Item::DoPassiveAction(float elapsedTime) {
    return;
}

void Item::DoPassiveAction(float elapsedTime, const Camera2D& camera) {
    return DoPassiveAction(elapsedTime);
}

// --- Helpers ---

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
    xAnchor = x;
}
void Item::SetY(float y) {
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

void Item::SetXY(const Vector2& xy) {
    SetX(xy.x);
    SetY(xy.y);
}

void Item::SetHeight(float value) {
    if (value < 0.0f) {
        throw std::invalid_argument("In item " + ID + ": Negative height.");
    } else {
        height = value;
    }
}

void Item::SetWidth(float value) {
    if (value < 0.0f) {
        throw std::invalid_argument("In item " + ID + ": Negative width.");
    } else {
        width = value;
    }
}

void Item::SetPriority(int value) {
    if (value < 0.0f) {
        throw std::invalid_argument("In item " + ID + ": Negative priority.");
    } else {
        priority = (size_t)(value);
    }
}

void Item::SetID(const std::string& id) {
    if (id.empty()) {
        throw std::invalid_argument("In item: " + ID + ": New ID would be empty.");
    }
    ID = id;
}

void Item::Focus() {
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
    inactive = true;
    visible = false;
}

void Item::Show() {
    inactive = false;
    visible = true;
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
    return height + (2 * border.GetThickness());
}

float Item::GetWidth() const {
    return width;
}

float Item::GetTotalWidth() const {
    return width + (2 * border.GetThickness());
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
