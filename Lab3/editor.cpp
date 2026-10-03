#include "editor.h"
#include "resource.h"

MyEditor::MyEditor(HWND h) : hWnd(h), shapeCount(0), currentTool(0), isDrawing(false), tempShape(nullptr), hRubberPen(nullptr) {
    for (int i = 0; i < MAX_SHAPES; i++) {
        pcshape[i] = nullptr;
    }
}

MyEditor::~MyEditor() {
    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) delete pcshape[i];
    }
    if (tempShape) delete tempShape;
    if (hRubberPen) DeleteObject(hRubberPen);
}

Shape* MyEditor::CreateShapeFactory(int tool) {
    switch (tool) {
    case 0: return new PointShape();
    case 1: return new LineShape();
    case 2: return new RectShape();
    case 3: return new EllipseShape();
    }
    return nullptr;
}

void MyEditor::CreateToolbar() {
    InitCommonControls();

    HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 4, 0);

    HDC hdcScreen = GetDC(hWnd);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, 64, 16);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    COLORREF maskColor = RGB(255, 0, 255);
    HBRUSH hBgBrush = CreateSolidBrush(maskColor);
    RECT rcFull = { 0, 0, 64, 16 };
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

    SelectObject(hdcMem, hOldPen);
    DeleteObject(hBlackPen);
    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(hWnd, hdcScreen);

    ImageList_AddMasked(hImageList, hBmp, maskColor);
    DeleteObject(hBmp);

    TBBUTTON tbb[4];
    ZeroMemory(tbb, sizeof(tbb));

    tbb[0].iBitmap = 0;  tbb[0].idCommand = ID_OBJECT_POINT;   tbb[0].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED; tbb[0].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[1].iBitmap = 1;  tbb[1].idCommand = ID_OBJECT_LINE;    tbb[1].fsState = TBSTATE_ENABLED; tbb[1].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[2].iBitmap = 2;  tbb[2].idCommand = ID_OBJECT_RECT;    tbb[2].fsState = TBSTATE_ENABLED; tbb[2].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[3].iBitmap = 3;  tbb[3].idCommand = ID_OBJECT_ELLIPSE; tbb[3].fsState = TBSTATE_ENABLED; tbb[3].fsStyle = TBSTYLE_CHECKGROUP;

    hToolBar = CreateWindowEx(
        0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd, (HMENU)1001, GetModuleHandle(NULL), NULL
    );

    SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hToolBar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
    SendMessage(hToolBar, TB_ADDBUTTONS, (WPARAM)4, (LPARAM)&tbb);
    SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);
}

void MyEditor::OnLButtonDown(LPARAM lParam) {
    if (shapeCount < MAX_SHAPES) {
        isDrawing = true;
        startX = endX = LOWORD(lParam);
        startY = endY = HIWORD(lParam);
        tempShape = CreateShapeFactory(currentTool);
        hRubberPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
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
        if (pcshape[i]) pcshape[i]->Show(hdc);
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
    }

    if (hToolBar) {
        SendMessage(hToolBar, TB_CHECKBUTTON, ID_OBJECT_POINT + currentTool, MAKELONG(TRUE, 0));
    }
}

void MyEditor::OnInitMenuPopup(WPARAM wParam, LPARAM lParam) {
    HMENU hMenu = (HMENU)wParam;

    CheckMenuItem(hMenu, ID_OBJECT_POINT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_OBJECT_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_OBJECT_RECT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_OBJECT_ELLIPSE, MF_BYCOMMAND | MF_UNCHECKED);

    CheckMenuItem(hMenu, ID_OBJECT_POINT + currentTool, MF_BYCOMMAND | MF_CHECKED);
}

LRESULT MyEditor::OnNotify(LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;

    if (pnmh->code == TTN_GETDISPINFOW) {
        LPTOOLTIPTEXTW lpttt = (LPTOOLTIPTEXTW)lParam;
        lpttt->hinst = NULL;

        static WCHAR strPoint[] = { 0x041A, 0x0440, 0x0430, 0x043F, 0x043A, 0x0430, 0 }; // Крапка
        static WCHAR strLine[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, 0 }; // Лінія
        static WCHAR strRect[] = { 0x041F, 0x0440, 0x044F, 0x043C, 0x043E, 0x043A, 0x0443, 0x0442, 0x043D, 0x0438, 0x043A, 0 }; // Прямокутник
        static WCHAR strEllipse[] = { 0x0415, 0x043B, 0x0456, 0x043F, 0x0441, 0 }; // Еліпс

        switch (lpttt->hdr.idFrom) {
        case ID_OBJECT_POINT:   lpttt->lpszText = strPoint; break;
        case ID_OBJECT_LINE:    lpttt->lpszText = strLine; break;
        case ID_OBJECT_RECT:    lpttt->lpszText = strRect; break;
        case ID_OBJECT_ELLIPSE: lpttt->lpszText = strEllipse; break;
        }
    }
    else if (pnmh->code == TTN_GETDISPINFOA) {
        LPTOOLTIPTEXTA lpttt = (LPTOOLTIPTEXTA)lParam;
        lpttt->hinst = NULL;

        static char strPointA[] = { (char)0xCA, (char)0xF0, (char)0xE0, (char)0xEF, (char)0xEA, (char)0xE0, 0 };
        static char strLineA[] = { (char)0xCB, (char)0xB3, (char)0xED, (char)0xB3, (char)0xFF, 0 };
        static char strRectA[] = { (char)0xCF, (char)0xF0, (char)0xFF, (char)0xEC, (char)0xEE, (char)0xEA, (char)0xF3, (char)0xF2, (char)0xED, (char)0xE8, (char)0xEA, 0 };
        static char strEllipseA[] = { (char)0xC5, (char)0xEB, (char)0xB3, (char)0xEF, (char)0xF1, 0 };

        switch (lpttt->hdr.idFrom) {
        case ID_OBJECT_POINT:   lpttt->lpszText = strPointA; break;
        case ID_OBJECT_LINE:    lpttt->lpszText = strLineA; break;
        case ID_OBJECT_RECT:    lpttt->lpszText = strRectA; break;
        case ID_OBJECT_ELLIPSE: lpttt->lpszText = strEllipseA; break;
        }
    }
    return 0;
}