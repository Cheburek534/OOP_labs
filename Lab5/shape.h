#pragma once

#include <windows.h>
#include <string> // [НОВЕ У ЛАБ 5]: Необхідно для роботи з рядками std::wstring

// ============================================================================
// БАЗОВИЙ АБСТРАКТНИЙ КЛАС
// ============================================================================
class Shape {
protected:
    long xs1, ys1, xs2, ys2;

public:
    virtual ~Shape() {}

    void Set(long x1, long y1, long x2, long y2);
    virtual void Show(HDC hdc) = 0;

    // ========================================================================
    // [НОВЕ У ЛАБ 5]: Блок методів для роботи з таблицею та файлами
    // ========================================================================
    
    // Чисто віртуальний метод для отримання назви фігури українською мовою.
    // Кожен нащадок зобов'язаний його перевизначити.
    virtual std::wstring GetName() const = 0;

    // Геттери для безпечного отримання координат (інкапсуляція)
    long GetX1() const { return xs1; }
    long GetY1() const { return ys1; }
    long GetX2() const { return xs2; }
    long GetY2() const { return ys2; }

    // Метод серіалізації: перетворює об'єкт у рядок формату CSV: "Назва;x1;y1;x2;y2"
    virtual std::wstring ToCSV() const;
};


class PointShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    std::wstring GetName() const override; 
};

class LineShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    std::wstring GetName() const override; 
};

class RectShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    std::wstring GetName() const override; 
};

class EllipseShape : virtual public Shape {
public:
    void Show(HDC hdc) override;
    std::wstring GetName() const override; 
};

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc) override;
    std::wstring GetName() const override; 
};

class CubeShape : public RectShape, public LineShape {
public:
    void Show(HDC hdc) override;
    std::wstring GetName() const override; 
};
