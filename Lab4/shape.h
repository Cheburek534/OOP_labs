#pragma once
#include <windows.h> 

class Shape {
protected:
    long xs1, ys1, xs2, ys2;

public:
    virtual ~Shape() {}

    void Set(long x1, long y1, long x2, long y2);

    virtual void Show(HDC hdc) = 0;
};


class PointShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
};

class LineShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
};

class RectShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
};

class EllipseShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
};

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc) override;
};

class CubeShape : public RectShape, public LineShape {
public:
    void Show(HDC hdc) override;
};
