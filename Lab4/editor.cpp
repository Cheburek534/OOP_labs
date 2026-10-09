#include "editor.h"

MyEditor::MyEditor()
    : hWnd(NULL), hToolBar(NULL), shapeCount(0), currentTool(0),
    isDrawing(false), startX(0), startY(0), endX(0), endY(0),
    tempShape(nullptr), hRubberPen(nullptr)
{
    pcshape = new Shape * [MAX_SHAPES];

    for (int i = 0; i < MAX_SHAPES; i++) {
        pcshape[i] = nullptr;
    }
}

MyEditor::~MyEditor() {
    if (pcshape) {
        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i]) {
                delete pcshape[i];
                pcshape[i] = nullptr;
            }
        }
        delete[] pcshape;
        pcshape = nullptr;
    }

    if (tempShape) {
        delete tempShape;
        tempShape = nullptr;
    }

    if (hRubberPen) {
        DeleteObject(hRubberPen);
        hRubberPen = nullptr;
    }
}


void MyEditor::Start(HWND h) {
    hWnd = h;        
    CreateToolbar(); 
}


Shape* MyEditor::CreateShapeFactory(int tool) {
    switch (tool) {
    case 0: return new PointShape();   // Фігура 1: Крапка
    case 1: return new LineShape();    // Фігура 2: Лінія
    case 2: return new RectShape();    // Фігура 3: Прямокутник
    case 3: return new EllipseShape(); // Фігура 4: Еліпс
    case 4: return new LineOOShape();  // Фігура 5: Лінія з кружечками 
    case 5: return new CubeShape();    // Фігура 6: Каркас куба
    }
    return nullptr;
}


void MyEditor::CreateToolbar() {
    InitCommonControls();

    HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 6, 0);

    HDC hdcScreen = GetDC(hWnd);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, 96, 16);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    
    COLORREF maskColor = RGB(255, 0, 255);
    HBRUSH hBgBrush = CreateSolidBrush(maskColor);
    RECT rcFull = { 0, 0, 96, 16 };
    FillRect(hdcMem, &rcFull, hBgBrush);
    DeleteObject(hBgBrush);

    HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hBlackPen);

    HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBlackBrush);
    Ellipse(hdcMem, 6, 6, 11, 11);
    SelectObject(hdcMem, hOldBrush);
    DeleteObject(hBlackBrush);

    MoveToEx(hdcMem, 16 + 3, 12, NULL);
    LineTo(hdcMem, 16 + 13, 3);

    hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 32 + 3, 3, 32 + 13, 13);
    SelectObject(hdcMem, hOldBrush);

    HBRUSH hPinkBrush = CreateSolidBrush(RGB(255, 192, 203));
    hOldBrush = (HBRUSH)SelectObject(hdcMem, hPinkBrush);
    Ellipse(hdcMem, 48 + 2, 4, 48 + 14, 12);
    SelectObject(hdcMem, hOldBrush);
    DeleteObject(hPinkBrush);

    MoveToEx(hdcMem, 64 + 4, 11, NULL);
    LineTo(hdcMem, 64 + 12, 4);
    hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Ellipse(hdcMem, 64 + 2, 9, 64 + 6, 13);
    Ellipse(hdcMem, 64 + 10, 2, 64 + 14, 6);
    SelectObject(hdcMem, hOldBrush);

    hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 80 + 2, 6, 80 + 11, 14);
    Rectangle(hdcMem, 80 + 5, 2, 80 + 14, 10);
    MoveToEx(hdcMem, 80 + 2, 6, NULL);  LineTo(hdcMem, 80 + 5, 2);
    MoveToEx(hdcMem, 80 + 11, 6, NULL); LineTo(hdcMem, 80 + 14, 2);
    MoveToEx(hdcMem, 80 + 2, 14, NULL); LineTo(hdcMem, 80 + 5, 10);
    MoveToEx(hdcMem, 80 + 11, 14, NULL);LineTo(hdcMem, 80 + 14, 10);
    SelectObject(hdcMem, hOldBrush);

    SelectObject(hdcMem, hOldPen);
    DeleteObject(hBlackPen);
    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(hWnd, hdcScreen);

    ImageList_AddMasked(hImageList, hBmp, maskColor);
    DeleteObject(hBmp);

    TBBUTTON tbb[6];
    ZeroMemory(tbb, sizeof(tbb));

    tbb[0].iBitmap = 0; tbb[0].idCommand = ID_OBJECT_POINT;   tbb[0].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED; tbb[0].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[1].iBitmap = 1; tbb[1].idCommand = ID_OBJECT_LINE;    tbb[1].fsState = TBSTATE_ENABLED;                   tbb[1].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[2].iBitmap = 2; tbb[2].idCommand = ID_OBJECT_RECT;    tbb[2].fsState = TBSTATE_ENABLED;                   tbb[2].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[3].iBitmap = 3; tbb[3].idCommand = ID_OBJECT_ELLIPSE; tbb[3].fsState = TBSTATE_ENABLED;                   tbb[3].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[4].iBitmap = 4; tbb[4].idCommand = ID_OBJECT_LINEOO;  tbb[4].fsState = TBSTATE_ENABLED;                   tbb[4].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[5].iBitmap = 5; tbb[5].idCommand = ID_OBJECT_CUBE;    tbb[5].fsState = TBSTATE_ENABLED;                   tbb[5].fsStyle = TBSTYLE_CHECKGROUP;

   
    hToolBar = CreateWindowEx(
        0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd, (HMENU)1001, GetModuleHandle(NULL), NULL
    );

    SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hToolBar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
    SendMessage(hToolBar, TB_ADDBUTTONS, (WPARAM)6, (LPARAM)&tbb);
    SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);
}



void MyEditor::OnLButtonDown(LPARAM lParam) {
    if (shapeCount < MAX_SHAPES) {
        isDrawing = true;
        startX = endX = LOWORD(lParam);
        startY = endY = HIWORD(lParam);

        tempShape = CreateShapeFactory(currentTool);

        hRubberPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
    }
}

void MyEditor::OnMouseMove(LPARAM lParam) {
    if (isDrawing && tempShape) {
        HDC hdc = GetDC(hWnd);
        SetROP2(hdc, R2_NOTXORPEN);   
        SetBkMode(hdc, TRANSPARENT);  

        HPEN oldPen = (HPEN)SelectObject(hdc, hRubberPen);

        tempShape->Set(startX, startY, endX, endY);
        tempShape->Show(hdc);

        endX = LOWORD(lParam);
        endY = HIWORD(lParam);

        tempShape->Set(startX, startY, endX, endY);
        tempShape->Show(hdc);

        SelectObject(hdc, oldPen);
        ReleaseDC(hWnd, hdc);
    }
}

void MyEditor::OnLButtonUp(LPARAM lParam) {
    if (isDrawing) {
        isDrawing = false;

        pcshape[shapeCount] = CreateShapeFactory(currentTool);
        pcshape[shapeCount]->Set(startX, startY, endX, endY);
        shapeCount++;

        delete tempShape;
        tempShape = nullptr;

        if (hRubberPen) {
            DeleteObject(hRubberPen);
            hRubberPen = nullptr;
        }

        InvalidateRect(hWnd, NULL, TRUE);
    }
}


void MyEditor::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) {
            pcshape[i]->Show(hdc);
        }
    }

    EndPaint(hWnd, &ps);
}

void MyEditor::OnCommand(WPARAM wParam) {
    int wmId = LOWORD(wParam);
    switch (wmId) {
    case ID_OBJECT_POINT:   currentTool = 0; break;
    case ID_OBJECT_LINE:    currentTool = 1; break;
    case ID_OBJECT_RECT:    currentTool = 2; break;
    case ID_OBJECT_ELLIPSE: currentTool = 3; break;
    case ID_OBJECT_LINEOO:  currentTool = 4; break; 
    case ID_OBJECT_CUBE:    currentTool = 5; break; 
    }

    if (hToolBar) {
        SendMessage(hToolBar, TB_CHECKBUTTON, ID_OBJECT_POINT + currentTool, MAKELONG(TRUE, 0));
    }
}

void MyEditor::OnInitMenuPopup(WPARAM wParam, LPARAM lParam) {
    HMENU hMenu = (HMENU)wParam;

    for (int i = 0; i < 6; i++) {
        CheckMenuItem(hMenu, ID_OBJECT_POINT + i, MF_BYCOMMAND | MF_UNCHECKED);
    }

    CheckMenuItem(hMenu, ID_OBJECT_POINT + currentTool, MF_BYCOMMAND | MF_CHECKED);
}

LRESULT MyEditor::OnNotify(LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;

    if (pnmh->code == TTN_GETDISPINFOW) {
        LPTOOLTIPTEXTW lpttt = (LPTOOLTIPTEXTW)lParam;
        lpttt->hinst = NULL;

        static WCHAR strPoint[] = { 0x041A, 0x0440, 0x0430, 0x043F, 0x043A, 0x0430, 0 }; // "Крапка"
        static WCHAR strLine[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, 0 };          // "Лінія"
        static WCHAR strRect[] = { 0x041F, 0x0440, 0x044F, 0x043C, 0x043E, 0x043A, 0x0443, 0x0442, 0x043D, 0x0438, 0x043A, 0 }; // "Прямокутник"
        static WCHAR strEllipse[] = { 0x0415, 0x043B, 0x0456, 0x043F, 0x0441, 0 };       // "Еліпс"
        static WCHAR strLineOO[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, ' ', 0x0437, ' ', 0x043A, 0x0440, 0x0443, 0x0436, 0x0435, 0x0447, 0x043A, 0x0430, 0x043C, 0x0438, 0 }; // "Лінія з кружечками"
        static WCHAR strCube[] = { 0x041A, 0x0430, 0x0440, 0x043A, 0x0430, 0x0441, ' ', 0x043A, 0x0443, 0x0431, 0x0430, 0 }; // "Каркас куба"

        switch (lpttt->hdr.idFrom) {
        case ID_OBJECT_POINT:   lpttt->lpszText = strPoint;   break;
        case ID_OBJECT_LINE:    lpttt->lpszText = strLine;    break;
        case ID_OBJECT_RECT:    lpttt->lpszText = strRect;    break;
        case ID_OBJECT_ELLIPSE: lpttt->lpszText = strEllipse; break;
        case ID_OBJECT_LINEOO:  lpttt->lpszText = strLineOO;  break;
        case ID_OBJECT_CUBE:    lpttt->lpszText = strCube;    break;
        }
    }
    return 0;
}
