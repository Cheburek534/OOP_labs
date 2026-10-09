#include "shape.h"
#include <cmath> 

void Shape::Set(long x1, long y1, long x2, long y2) {
    xs1 = x1;
    ys1 = y1;
    xs2 = x2;
    ys2 = y2;
}


void PointShape::Show(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, xs2 - 2, ys2 - 2, xs2 + 2, ys2 + 2);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void LineShape::Show(HDC hdc) {
    MoveToEx(hdc, xs1, ys1, NULL); 
    LineTo(hdc, xs2, ys2);        
}

void RectShape::Show(HDC hdc) {
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    long rx = std::abs(xs2 - xs1);
    long ry = std::abs(ys2 - ys1);

    Rectangle(hdc, xs1 - rx, ys1 - ry, xs1 + rx, ys1 + ry);

    SelectObject(hdc, oldBrush); 
}

void EllipseShape::Show(HDC hdc) {
    HBRUSH currentBrush = NULL;
    HBRUSH oldBrush = NULL;

    if (GetROP2(hdc) == R2_NOTXORPEN) {
        currentBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else {
        currentBrush = CreateSolidBrush(RGB(255, 192, 203));
    }

    oldBrush = (HBRUSH)SelectObject(hdc, currentBrush);

    Ellipse(hdc, xs1, ys1, xs2, ys2);

    SelectObject(hdc, oldBrush);

    if (GetROP2(hdc) != R2_NOTXORPEN) {
        DeleteObject(currentBrush);
    }
}

void LineOOShape::Show(HDC hdc) {
    
    long origX1 = xs1, origY1 = ys1;
    long origX2 = xs2, origY2 = ys2;

    LineShape::Set(origX1, origY1, origX2, origY2);
    LineShape::Show(hdc);

    const long R = 5; 
    EllipseShape::Set(origX1 - R, origY1 - R, origX1 + R, origY1 + R);
    EllipseShape::Show(hdc);

    EllipseShape::Set(origX2 - R, origY2 - R, origX2 + R, origY2 + R);
    EllipseShape::Show(hdc);

    Shape::Set(origX1, origY1, origX2, origY2);
}

void CubeShape::Show(HDC hdc) {
    long origX1 = xs1, origY1 = ys1;
    long origX2 = xs2, origY2 = ys2;

    long dx = (origX2 - origX1) / 3;
    long dy = (origY2 - origY1) / 3;

    long f_left = origX1, f_top = origY1;
    long f_right = origX2 - dx, f_bottom = origY2;

    long b_left = f_left + dx, b_top = f_top - dy;
    long b_right = f_right + dx, b_bottom = f_bottom - dy;

    
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, f_left, f_top, f_right, f_bottom);


    Rectangle(hdc, b_left, b_top, b_right, b_bottom);
    SelectObject(hdc, oldBrush);

  
    LineShape::Set(f_left, f_top, b_left, b_top);
    LineShape::Show(hdc);

    LineShape::Set(f_right, f_top, b_right, b_top);
    LineShape::Show(hdc);

    LineShape::Set(f_left, f_bottom, b_left, b_bottom);
    LineShape::Show(hdc);

    LineShape::Set(f_right, f_bottom, b_right, b_bottom);
    LineShape::Show(hdc);

    Shape::Set(origX1, origY1, origX2, origY2);
}
