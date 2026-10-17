#include "framework.h"
#include "Resource.h"
#include "editor.h"
#include "my_table.h"
#include <commdlg.h>  

#pragma comment(lib, "comctl32.lib")

HINSTANCE hInst;
WCHAR szTitle[100] = L"Lab5 - Багатовіконний інтерфейс";
WCHAR szWindowClass[100] = L"Lab5Class";

MyTable myTable;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


void UpdateTableDisplay() {
    std::vector<TableRowData> data;
    MyEditor& editor = MyEditor::getInstance();

    for (int i = 0; i < editor.GetShapeCount(); i++) {
        Shape* s = editor.GetShape(i);
        if (s) {
            data.push_back({ s->GetName(), s->GetX1(), s->GetY1(), s->GetX2(), s->GetY2() });
        }
    }
    myTable.UpdateData(data); 
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        HWND hDlg = myTable.GetDialogHandle();
        if (hDlg == NULL || !IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB5);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    MyEditor& editor = MyEditor::getInstance();

    switch (message) {

    case WM_CREATE:
        editor.Start(hWnd);
        break;

    case WM_LBUTTONDOWN:
        editor.OnLButtonDown(lParam);
        break;

    case WM_MOUSEMOVE:
        editor.OnMouseMove(lParam);
        break;

    case WM_LBUTTONUP:
        editor.OnLButtonUp(lParam);
        UpdateTableDisplay();
        break;

    case WM_PAINT:
        editor.OnPaint();
        break;

    case WM_USER_HIGHLIGHT_SHAPE:
        editor.HighlightShape((int)wParam);
        break;

    case WM_USER_DELETE_SHAPE:
        editor.DeleteShape((int)wParam);
        UpdateTableDisplay();
        break;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        case ID_VIEW_TABLE:
            myTable.Show(hInst, hWnd);
            UpdateTableDisplay();
            break;

        case ID_FILE_SAVE_CSV: {
            OPENFILENAMEW ofn;
            WCHAR szFile[260] = { 0 };

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
            ofn.lpstrFilter = L"CSV Файли (*.csv)\0*.csv\0Всі файли (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrDefExt = L"csv";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

            if (GetSaveFileNameW(&ofn) == TRUE) {
                if (editor.SaveToFile(ofn.lpstrFile)) {
                    MessageBoxW(hWnd, L"Дані успішно збережено!", L"Успіх", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    MessageBoxW(hWnd, L"Помилка запису у файл.", L"Помилка", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }

                             
        case ID_FILE_OPEN_CSV: {
            OPENFILENAMEW ofn;
            WCHAR szFile[260] = { 0 }; 

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;       
            ofn.lpstrFile = szFile;     
            ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
            ofn.lpstrFilter = L"CSV Файли (*.csv)\0*.csv\0Всі файли (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameW(&ofn) == TRUE) {
                if (editor.LoadFromFile(ofn.lpstrFile)) {
                    UpdateTableDisplay();

                    MessageBoxW(hWnd, L"Дані успішно завантажено!", L"Успіх", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    MessageBoxW(hWnd, L"Помилка читання або некоректний формат файлу.", L"Помилка", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }

        default:
            editor.OnCommand(wParam);
            break;
        }
        break;
    }

    case WM_INITMENUPOPUP:
        editor.OnInitMenuPopup(wParam, lParam);
        break;

    case WM_NOTIFY:
        return editor.OnNotify(lParam);

    case WM_DESTROY:
        myTable.Close();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
