#include "mixins.h"

using namespace CPPFX;

bool Circular::WasIClicked(const Vector2& mousePosition) const {
    if (xyIsCentre) {
        if (CheckCollisionPointCircle(mousePosition, {xAnchor, yAnchor}, width)) return true;
        return false;
    } else {
        if (CheckCollisionPointCircle(mousePosition, {xAnchor + (width / 2), yAnchor + (width / 2)}, width / 2)) return true;
        return false;
    }
}

bool Circular::WasIClicked(const Vector2& mousePosition, const Camera2D& camera) const {
    float width = this->width * camera.zoom;
    if (xyIsCentre) {
        if (CheckCollisionPointCircle(mousePosition, {xAnchor, yAnchor}, width)) return true;
        return false;
    } else {
        if (CheckCollisionPointCircle(mousePosition, {xAnchor + (width / 2), yAnchor + (width / 2)}, width / 2)) return true;
        return false;
    }
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
    return width / 2;
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

void Padded::SetPadding(float value) {
    if (value < 0) {
        throw std::invalid_argument("In " + ID + "Negative padding.");
    }
    padding = value;
}

float Padded::GetPadding() const {
    return padding;
}
