#include "editor.h"
#include <sstream> // [ДОДАТКОВЕ ЗАВДАННЯ 3]: Для роботи з рядковими потоками std::wstringstream

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
    ClearAllShapes(); // Використовуємо уніфіковану функцію очищення
    if (pcshape) {
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
    case 0: return new PointShape();
    case 1: return new LineShape();
    case 2: return new RectShape();
    case 3: return new EllipseShape();
    case 4: return new LineOOShape();
    case 5: return new CubeShape();
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

    // 0: Точка
    HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBlackBrush);
    Ellipse(hdcMem, 6, 6, 11, 11);
    SelectObject(hdcMem, hOldBrush); DeleteObject(hBlackBrush);

    // 1: Лінія
    MoveToEx(hdcMem, 16 + 3, 12, NULL); LineTo(hdcMem, 16 + 13, 3);

    // 2: Прямокутник
    hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 32 + 3, 3, 32 + 13, 13);
    SelectObject(hdcMem, hOldBrush);

    // 3: Еліпс
    HBRUSH hPinkBrush = CreateSolidBrush(RGB(255, 192, 203));
    hOldBrush = (HBRUSH)SelectObject(hdcMem, hPinkBrush);
    Ellipse(hdcMem, 48 + 2, 4, 48 + 14, 12);
    SelectObject(hdcMem, hOldBrush); DeleteObject(hPinkBrush);

    // 4: Лінія з кружечками
    MoveToEx(hdcMem, 64 + 4, 11, NULL); LineTo(hdcMem, 64 + 12, 4);
    hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Ellipse(hdcMem, 64 + 2, 9, 64 + 6, 13);
    Ellipse(hdcMem, 64 + 10, 2, 64 + 14, 6);
    SelectObject(hdcMem, hOldBrush);

    // 5: Куб
    hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 80 + 2, 6, 80 + 11, 14);
    Rectangle(hdcMem, 80 + 5, 2, 80 + 14, 10);
    MoveToEx(hdcMem, 80 + 2, 6, NULL);   LineTo(hdcMem, 80 + 5, 2);
    MoveToEx(hdcMem, 80 + 11, 6, NULL);  LineTo(hdcMem, 80 + 14, 2);
    MoveToEx(hdcMem, 80 + 2, 14, NULL);  LineTo(hdcMem, 80 + 5, 10);
    MoveToEx(hdcMem, 80 + 11, 14, NULL); LineTo(hdcMem, 80 + 14, 10);
    SelectObject(hdcMem, hOldBrush);

    SelectObject(hdcMem, hOldPen); DeleteObject(hBlackPen);
    SelectObject(hdcMem, hOldBmp); DeleteDC(hdcMem); ReleaseDC(hWnd, hdcScreen);

    ImageList_AddMasked(hImageList, hBmp, maskColor); DeleteObject(hBmp);

    TBBUTTON tbb[6]; ZeroMemory(tbb, sizeof(tbb));
    tbb[0].iBitmap = 0; tbb[0].idCommand = ID_OBJECT_POINT;   tbb[0].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED; tbb[0].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[1].iBitmap = 1; tbb[1].idCommand = ID_OBJECT_LINE;    tbb[1].fsState = TBSTATE_ENABLED;                   tbb[1].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[2].iBitmap = 2; tbb[2].idCommand = ID_OBJECT_RECT;    tbb[2].fsState = TBSTATE_ENABLED;                   tbb[2].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[3].iBitmap = 3; tbb[3].idCommand = ID_OBJECT_ELLIPSE; tbb[3].fsState = TBSTATE_ENABLED;                   tbb[3].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[4].iBitmap = 4; tbb[4].idCommand = ID_OBJECT_LINEOO;  tbb[4].fsState = TBSTATE_ENABLED;                   tbb[4].fsStyle = TBSTYLE_CHECKGROUP;
    tbb[5].iBitmap = 5; tbb[5].idCommand = ID_OBJECT_CUBE;    tbb[5].fsState = TBSTATE_ENABLED;                   tbb[5].fsStyle = TBSTYLE_CHECKGROUP;

    hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0, hWnd, (HMENU)1001, GetModuleHandle(NULL), NULL);
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

        delete tempShape; tempShape = nullptr;
        if (hRubberPen) { DeleteObject(hRubberPen); hRubberPen = nullptr; }

        selectedShapeIndex = -1;
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void MyEditor::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) {
            if (i == selectedShapeIndex) {
                HPEN hHighlightPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
                HPEN hOldPen = (HPEN)SelectObject(hdc, hHighlightPen);
                int oldBkMode = SetBkMode(hdc, TRANSPARENT);

                pcshape[i]->Show(hdc);

                SetBkMode(hdc, oldBkMode);
                SelectObject(hdc, hOldPen);
                DeleteObject(hHighlightPen);
            }
            else {
                pcshape[i]->Show(hdc);
            }
        }
    }
    EndPaint(hWnd, &ps);
}

bool MyEditor::SaveToFile(const std::wstring& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!file.is_open()) return false;

    auto toUtf8 = [](const std::wstring& wstr) -> std::string {
        if (wstr.empty()) return "";
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
        return strTo;
        };

    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    file.write((char*)bom, sizeof(bom));

    static const WCHAR csvHeader[] = { 0x041D, 0x0430, 0x0437, 0x0432, 0x0430, ';', 'x', '1', ';', 'y', '1', ';', 'x', '2', ';', 'y', '2', '\n', 0 };
    file << toUtf8(csvHeader);

    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) {
            std::wstring rowW = pcshape[i]->ToCSV() + L"\n";
            file << toUtf8(rowW);
        }
    }

    file.flush();
    file.close();
    return true;
}

void MyEditor::ClearAllShapes() {
    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) {
            delete pcshape[i];
            pcshape[i] = nullptr;
        }
    }
    shapeCount = 0;
    selectedShapeIndex = -1; 
    if (hWnd) InvalidateRect(hWnd, NULL, TRUE); 
}

bool MyEditor::LoadFromFile(const std::wstring& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    if (fileSize <= 0) {
        file.close();
        return false;
    }
    file.seekg(0, std::ios::beg);

    unsigned char bom[3] = { 0 };
    file.read((char*)bom, 3);
    if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)) {
        file.seekg(0, std::ios::beg);
    }

    ClearAllShapes();

    auto utf8ToWstring = [](const std::string& str) -> std::wstring {
        if (str.empty()) return L"";
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
        if (sizeNeeded <= 0) {
            sizeNeeded = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0);
            std::wstring wstr(sizeNeeded, 0);
            MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], sizeNeeded);
            return wstr;
        }
        std::wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], sizeNeeded);
        return wstr;
        };

    std::string lineA;
    if (!std::getline(file, lineA)) {
        file.close();
        return false;
    }

    while (std::getline(file, lineA) && shapeCount < MAX_SHAPES) {
        if (!lineA.empty() && lineA.back() == '\r') lineA.pop_back();
        if (lineA.empty()) continue;

        std::wstring lineW = utf8ToWstring(lineA);
        std::wstringstream ss(lineW);
        std::wstring name, sX1, sY1, sX2, sY2;

        if (std::getline(ss, name, L';') &&
            std::getline(ss, sX1, L';') &&
            std::getline(ss, sY1, L';') &&
            std::getline(ss, sX2, L';') &&
            std::getline(ss, sY2, L';'))
        {
            long x1 = std::stol(sX1);
            long y1 = std::stol(sY1);
            long x2 = std::stol(sX2);
            long y2 = std::stol(sY2);

            Shape* newShape = nullptr;

            static const WCHAR namePoint[] = { 0x041A, 0x0440, 0x0430, 0x043F, 0x043A, 0x0430, 0 }; // Крапка
            static const WCHAR nameLine[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, 0 };          // Лінія
            static const WCHAR nameRect[] = { 0x041F, 0x0440, 0x044F, 0x043C, 0x043E, 0x043A, 0x0443, 0x0442, 0x043D, 0x0438, 0x043A, 0 }; // Прямокутник
            static const WCHAR nameEllipse[] = { 0x0415, 0x043B, 0x0456, 0x043F, 0x0441, 0 };       // Еліпс
            static const WCHAR nameLineOO[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, ' ', 0x0437, ' ', 0x043A, 0x0443, 0x0436, 0x0435, 0x0447, 0x043A, 0x0430, 0x043C, 0x0438, 0 }; // Лінія з кружечками
            static const WCHAR nameCube[] = { 0x041A, 0x0430, 0x0440, 0x043A, 0x0430, 0x0441, ' ', 0x043A, 0x0443, 0x0431, 0x0430, 0 }; // Каркас куба

            if (name == namePoint)        newShape = new PointShape();
            else if (name == nameLine)    newShape = new LineShape();
            else if (name == nameRect)    newShape = new RectShape();
            else if (name == nameEllipse) newShape = new EllipseShape();
            else if (name == nameLineOO)  newShape = new LineOOShape();
            else if (name == nameCube)    newShape = new CubeShape();

            if (newShape) {
                newShape->Set(x1, y1, x2, y2);
                pcshape[shapeCount++] = newShape;
            }
        }
    }

    file.close();
    if (hWnd) InvalidateRect(hWnd, NULL, TRUE);
    return true;
}

void MyEditor::HighlightShape(int index) {
    if (index >= 0 && index < shapeCount) {
        selectedShapeIndex = index;
    }
    else {
        selectedShapeIndex = -1;
    }
    InvalidateRect(hWnd, NULL, TRUE);
}

void MyEditor::DeleteShape(int index) {
    if (index >= 0 && index < shapeCount && pcshape[index] != nullptr) {
        delete pcshape[index];
        pcshape[index] = nullptr;

        for (int i = index; i < shapeCount - 1; i++) {
            pcshape[i] = pcshape[i + 1];
        }
        pcshape[shapeCount - 1] = nullptr;
        shapeCount--;
        selectedShapeIndex = -1;
        InvalidateRect(hWnd, NULL, TRUE);
    }
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
    if (hToolBar) SendMessage(hToolBar, TB_CHECKBUTTON, ID_OBJECT_POINT + currentTool, MAKELONG(TRUE, 0));
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
        static WCHAR strPoint[] = { 0x041A, 0x0440, 0x0430, 0x043F, 0x043A, 0x0430, 0 };
        static WCHAR strLine[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, 0 };
        static WCHAR strRect[] = { 0x041F, 0x0440, 0x044F, 0x043C, 0x043E, 0x043A, 0x0443, 0x0442, 0x043D, 0x0438, 0x043A, 0 };
        static WCHAR strEllipse[] = { 0x0415, 0x043B, 0x0456, 0x043F, 0x0441, 0 };
        static WCHAR strLineOO[] = { 0x041B, 0x0456, 0x043D, 0x0456, 0x044F, ' ', 0x0437, ' ', 0x043A, 0x0440, 0x0443, 0x0436, 0x0435, 0x0447, 0x043A, 0x0430, 0x043C, 0x0438, 0 };
        static WCHAR strCube[] = { 0x041A, 0x0430, 0x0440, 0x043A, 0x0430, 0x0441, ' ', 0x043A, 0x0443, 0x0431, 0x0430, 0 };
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
