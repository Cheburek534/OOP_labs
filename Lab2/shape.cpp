#include "shape.h"

void Shape::Set(long x1, long y1, long x2, long y2) {
    xs1 = x1; ys1 = y1;
    xs2 = x2; ys2 = y2;
}

void PointShape::Show(HDC hdc) {
    SetPixel(hdc, xs2, ys2, RGB(0, 0, 0));
}

void LineShape::Show(HDC hdc) {
    MoveToEx(hdc, xs1, ys1, NULL);
    LineTo(hdc, xs2, ys2);
}

void RectShape::Show(HDC hdc) {
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    long left = xs1 - (xs2 - xs1);
    long top = ys1 - (ys2 - ys1);

    Rectangle(hdc, left, top, xs2, ys2);

    SelectObject(hdc, oldBrush);
}

void EllipseShape::Show(HDC hdc) {
    HBRUSH fillBrush = CreateSolidBrush(RGB(0, 255, 255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, fillBrush);

    Ellipse(hdc, xs1, ys1, xs2, ys2);

    SelectObject(hdc, oldBrush);
    DeleteObject(fillBrush); 
}