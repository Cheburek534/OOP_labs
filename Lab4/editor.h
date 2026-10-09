#pragma once
#include <windows.h> 
#include <commctrl.h> 
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

    Shape* CreateShapeFactory(int tool);

public:
    MyEditor();

    ~MyEditor();

    void Start(HWND h);

    void CreateToolbar();

    void OnInitMenuPopup(WPARAM wParam, LPARAM lParam);

    LRESULT OnNotify(LPARAM lParam);

    void OnLButtonDown(LPARAM lParam); 
    void OnMouseMove(LPARAM lParam);  
    void OnLButtonUp(LPARAM lParam);   
    void OnPaint();                    
    void OnCommand(WPARAM wParam);     
};
