#pragma once
#include <windows.h>
#include <commctrl.h> 
#include "shape.h"

#define MAX_SHAPES 104 

#define ID_OBJECT_POINT   32771
#define ID_OBJECT_LINE    32772
#define ID_OBJECT_RECT    32773
#define ID_OBJECT_ELLIPSE 32774

class MyEditor {
private:
    HWND hWnd;             
    HWND hToolBar;         
    
    Shape* pcshape[MAX_SHAPES]; 
    int shapeCount;        
    
    int currentTool;       
    bool isDrawing;        
    long startX, startY, endX, endY; 
    
    Shape* tempShape;      
    HPEN hRubberPen;       

    Shape* CreateShapeFactory(int tool);

public:
    MyEditor(HWND h);
    ~MyEditor();
    
    void CreateToolbar();
    void OnLButtonDown(LPARAM lParam);
    void OnMouseMove(LPARAM lParam);
    void OnLButtonUp(LPARAM lParam);
    void OnPaint();
    void OnCommand(WPARAM wParam);
    void OnInitMenuPopup(WPARAM wParam, LPARAM lParam);
    LRESULT OnNotify(LPARAM lParam); 
};
