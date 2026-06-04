#include "mixins.h"

using namespace CPPFX;

// --- Circular ---

bool Circular::WasIClicked(const Vector2& mousePosition) const {
    if (CheckCollisionPointCircle(mousePosition, {TranslateXToCentre(), TranslateYToCentre()}, GetRadius())) return true;
    return false;
}

bool Circular::WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const {
    float radius = GetRadius() * camera.zoom;

    if (CheckCollisionPointCircle(mousePosition, {TranslateXToCentre(), TranslateYToCentre()}, radius)) return true;
    return false;
}

void Circular::MakeRadiusBased() {
    xyIsCentre = true;
}

void Circular::MakeTopLeftBased() {
    xyIsCentre = false;
}

void Circular::SetCoordinatesBase(bool flag) {
    xyIsCentre = flag;
}

bool Circular::IsAnchoredAtCentre() const {
    return xyIsCentre;
}

float Circular::TranslateXToCentre() const {
    if (xyIsCentre) {
        return xAnchor;
    } else {
        return xAnchor + GetRadius();
    }
}

float Circular::TranslateYToCentre() const {
    if (xyIsCentre) {
        return yAnchor;
    } else {
        return yAnchor + GetRadius();
    }
}

Vector2 Circular::TranslateXYToCentre() const {
    if (xyIsCentre) {
        return {xAnchor, yAnchor};
    } else {
        return {xAnchor + GetRadius(), yAnchor + GetRadius()};
    }
}

float Circular::TranslateXToCorner() const {
    if (xyIsCentre) {
        return xAnchor - GetRadius();
    } else {
        return xAnchor;
    }
}

float Circular::TranslateYToCorner() const {
    if (xyIsCentre) {
        return yAnchor - GetRadius();
    } else {
        return yAnchor;
    }
}

Vector2 Circular::TranslateXYToCorner() const {
    if (xyIsCentre) {
        return {xAnchor - GetRadius(), yAnchor - GetRadius()};
    } else {
        return {xAnchor, yAnchor};
    }
}

float Circular::GetRadius() const {
    return width / 2.0f;
}

void Circular::SetRadius(float radius) {
    if (radius < 0.0f) {
        throw std::invalid_argument("In " + fxID + " " + ID + ": Negative radius");
    }
    if (radius == 0.0f) {
        CPPFX_WARN("In " + fxID + ID + ": radius equals 0. Will not be visible and may break stuff.");
    }
    SetWidth(radius * 2.0f);
}

float Circular::GetXTransformed() const {
    if (xyIsCentre) {
        return TranslateXToCorner();
    } else {
        return TranslateXToCentre();
    }
}

float Circular::GetYTransformed() const {
    if (xyIsCentre) {
        return TranslateYToCorner();
    } else {
        return TranslateYToCentre();
    }
}

Vector2 Circular::GetXYTransformed() const {
    if (xyIsCentre) {
        return TranslateXYToCorner();
    } else {
        return TranslateXYToCentre();
    }
}

// --- PersistentState ---

void PersistentState::SetPressed(bool flag) {
    pressed = flag;
}

void PersistentState::Press() {
    pressed = true;
}

void PersistentState::Unpress() {
    pressed = false;
}

void PersistentState::SwitchPress() {
    pressed = !pressed;
}

bool PersistentState::IsPressed() const {
    return pressed;
}

// --- Padded ---

void Padded::SetPadding(float value) {
    if (value < 0.0f) {
        throw std::invalid_argument("In " + fxID + " " + ID + ": Negative padding.");
    }
    padding = value;
}

float Padded::GetPadding() const {
    return padding;
}
