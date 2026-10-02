#include "framework.h" 
#include "resource.h"  
#include "editor.h"

#pragma comment(lib, "comctl32.lib") 

HINSTANCE hInst;
WCHAR szTitle[100] = L"Lab3 - Інтерфейс користувача";
WCHAR szWindowClass[100] = L"Lab3Class";

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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

    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB3);

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

    MyEditor* editor = (MyEditor*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (message) {
    case WM_CREATE:
        editor = new MyEditor(hWnd);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)editor);
        editor->CreateToolbar();
        break;

    case WM_LBUTTONDOWN:  if (editor) editor->OnLButtonDown(lParam); break;
    case WM_MOUSEMOVE:    if (editor) editor->OnMouseMove(lParam); break;
    case WM_LBUTTONUP:    if (editor) editor->OnLButtonUp(lParam); break;
    case WM_PAINT:        if (editor) editor->OnPaint(); break;

    case WM_COMMAND:
        if (editor) editor->OnCommand(wParam);
        if (LOWORD(wParam) == 105) DestroyWindow(hWnd);
        break;

    case WM_INITMENUPOPUP: if (editor) editor->OnInitMenuPopup(wParam, lParam); break;
    case WM_NOTIFY:        if (editor) return editor->OnNotify(lParam); break;

    case WM_DESTROY:
        if (editor) delete editor;
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}