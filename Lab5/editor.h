#pragma once

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <fstream>
#include "shape.h"

#define MAX_SHAPES 103 

#define ID_OBJECT_POINT    32771
#define ID_OBJECT_LINE     32772
#define ID_OBJECT_RECT     32773
#define ID_OBJECT_ELLIPSE  32774
#define ID_OBJECT_LINEOO   32775
#define ID_OBJECT_CUBE     32776

class MyEditor {
private:
    HWND hWnd;
    HWND hToolBar;
    Shape** pcshape;
    int shapeCount;
    int currentTool;
    bool isDrawing;
    long startX, startY, endX, endY;
    Shape* tempShape;
    HPEN hRubberPen;

    MyEditor();
    ~MyEditor();
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    int selectedShapeIndex = -1; 

    Shape* CreateShapeFactory(int tool);

public:
    static MyEditor& getInstance() {
        static MyEditor instance;
        return instance;
    }

    void Start(HWND h);
    void CreateToolbar();
    void OnInitMenuPopup(WPARAM wParam, LPARAM lParam);
    LRESULT OnNotify(LPARAM lParam);

    void OnLButtonDown(LPARAM lParam);
    void OnMouseMove(LPARAM lParam);
    void OnLButtonUp(LPARAM lParam);
    void OnPaint();
    void OnCommand(WPARAM wParam);

    bool SaveToFile(const std::wstring& filename);
    
    void ClearAllShapes();

    bool LoadFromFile(const std::wstring& filename);

    void HighlightShape(int index);
    void DeleteShape(int index);

    int GetShapeCount() const { return shapeCount; }
    Shape* GetShape(int index) const { 
        if (index >= 0 && index < shapeCount) return pcshape[index];
        return nullptr;
    }
};
