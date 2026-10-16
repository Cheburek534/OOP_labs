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

#define WM_USER_UPDATE_TABLE    (WM_USER + 1) 

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

    int highlightedIndex;

    Shape* CreateShapeFactory(int tool);
    Shape* CreateShapeFromString(const std::wstring& name);

    MyEditor();
    ~MyEditor();
    MyEditor(const MyEditor&) = delete;            
    MyEditor& operator=(const MyEditor&) = delete; 

public:
    static MyEditor& getInstance() {
        static MyEditor instance; // Створюється один раз у потокобезпечному режимі
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

    
    int GetShapeCount() const { return shapeCount; }
    Shape* GetShape(int index) const { return (index >= 0 && index < shapeCount) ? pcshape[index] : nullptr; }

    void HighlightShape(int index); 
    void DeleteShape(int index);    
    
    bool SaveToFile(const std::wstring& filename);
    void LoadFromFile(LPCWSTR filename);
};