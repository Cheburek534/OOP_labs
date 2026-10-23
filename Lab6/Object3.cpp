#include "framework.h"
#include "Object3.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"Object3 - Графік функції";
WCHAR szWindowClass[MAX_LOADSTRING] = L"Object3Class";

std::vector<double> g_data;

UINT WM_OBJ3_READY = 0;
UINT MSG_DRAW_GRAPH = 0;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void ReadFromClipboard(HWND hWnd) {
    if (!OpenClipboard(hWnd)) return;

    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
        if (hMem) {
            LPCWSTR ptr = (LPCWSTR)GlobalLock(hMem);
            if (ptr) {
                g_data.clear();
                std::wstringstream wss(ptr);
                double val;

                while (wss >> val) {
                    g_data.push_back(val);
                }
                GlobalUnlock(hMem);
            }
        }
    }
    CloseClipboard();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    WM_OBJ3_READY = RegisterWindowMessage(L"MSG_OBJ3_READY");
    MSG_DRAW_GRAPH = RegisterWindowMessage(L"MSG_DRAW_GRAPH");

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(107));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(108));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 600, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == MSG_DRAW_GRAPH) {
        ReadFromClipboard(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }

    switch (message) {

    case WM_CREATE: {
        HWND hLab6 = FindWindowW(L"Lab6Class", NULL);
        if (hLab6) {
            PostMessage(hLab6, WM_OBJ3_READY, 0, 0);
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (g_data.size() > 1) {
            RECT rect;
            GetClientRect(hWnd, &rect);

            int padL = 50, padR = 30, padT = 30, padB = 40;
            int drawWidth = rect.right - padL - padR;
            int drawHeight = rect.bottom - padT - padB;

            double minY = *std::min_element(g_data.begin(), g_data.end());
            double maxY = *std::max_element(g_data.begin(), g_data.end());

            if (maxY - minY == 0) { maxY += 1.0; minY -= 1.0; }

            int n = (int)g_data.size();

            double scaleX = (double)drawWidth / (n - 1);
            double scaleY = (double)drawHeight / (maxY - minY);

            HPEN hAxisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hAxisPen);

            MoveToEx(hdc, padL, padT, NULL);
            LineTo(hdc, padL, padT + drawHeight);

            MoveToEx(hdc, padL, padT + drawHeight, NULL);
            LineTo(hdc, padL + drawWidth, padT + drawHeight);

            SetBkMode(hdc, TRANSPARENT);
            WCHAR buf[32];

            swprintf(buf, 32, L"%.1f", maxY);
            TextOutW(hdc, 5, padT - 10, buf, lstrlenW(buf));

            swprintf(buf, 32, L"%.1f", minY);
            TextOutW(hdc, 5, padT + drawHeight - 10, buf, lstrlenW(buf));

            swprintf(buf, 32, L"%d", n - 1);
            TextOutW(hdc, padL + drawWidth - 10, padT + drawHeight + 10, buf, lstrlenW(buf));

            SelectObject(hdc, hOldPen);
            DeleteObject(hAxisPen);

            HPEN hGraphPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
            SelectObject(hdc, hGraphPen);

            for (int i = 0; i < n; i++) {
                int scrX = padL + (int)(i * scaleX);

                int scrY = padT + drawHeight - (int)((g_data[i] - minY) * scaleY);

                if (i == 0) {
                    MoveToEx(hdc, scrX, scrY, NULL);
                }
                else {
                    LineTo(hdc, scrX, scrY);
                }
            }

            SelectObject(hdc, hOldPen);
            DeleteObject(hGraphPen);
        }
        else {
            std::wstring msg = L"Очікування даних від Object2 (буфер обміну)...";
            TextOutW(hdc, 20, 20, msg.c_str(), msg.length());
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
