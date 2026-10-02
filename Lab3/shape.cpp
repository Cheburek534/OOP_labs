#include "shape.h"
#include <cmath> 

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
    Rectangle(hdc, xs1, ys1, xs2, ys2);
    SelectObject(hdc, oldBrush);
}

void EllipseShape::Show(HDC hdc) {
    HBRUSH oldBrush;
    HBRUSH newBrush = NULL;

    // Перевіряємо, чи малюється зараз гумовий слід
    if (GetROP2(hdc) == R2_NOTXORPEN) {
        // Беремо порожній пензель, щоб не затирати фон білим кольором
        oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    }
    else {
        // Фінальний малюнок - рожева заливка
        newBrush = CreateSolidBrush(RGB(255, 192, 203));
        oldBrush = (HBRUSH)SelectObject(hdc, newBrush);
    }

    long rx = std::abs(xs2 - xs1);
    long ry = std::abs(ys2 - ys1);
    Ellipse(hdc, xs1 - rx, ys1 - ry, xs1 + rx, ys1 + ry);

    SelectObject(hdc, oldBrush);

    // Видаляємо пензель лише якщо ми його реально створювали
    if (newBrush != NULL) {
        DeleteObject(newBrush);
    }
}