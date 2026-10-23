#include "framework.h"
#include "Object2.h"
#include <vector>
#include <string>
#include <random>   
#include <iomanip>  
#include <sstream>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"Object2 - Генератор даних";
WCHAR szWindowClass[MAX_LOADSTRING] = L"Object2Class";

std::vector<double> g_data;

UINT WM_OBJ2_READY = 0;
UINT WM_OBJ2_DONE = 0;

struct ParamsDTO {
    int n;
    double minVal;
    double maxVal;
};

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CopyToClipboard(HWND hWnd, const std::vector<double>& data) {
    if (!OpenClipboard(hWnd)) return;

    EmptyClipboard();

    std::wstringstream wss;
    for (double val : data) {
        wss << val << L"\n";
    }
    std::wstring textStr = wss.str();

    size_t memSize = (textStr.length() + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, memSize);

    if (hMem) {
        void* ptr = GlobalLock(hMem);
        memcpy(ptr, textStr.c_str(), memSize);
        GlobalUnlock(hMem);

        SetClipboardData(CF_UNICODETEXT, hMem);
    }

    CloseClipboard();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    WM_OBJ2_READY = RegisterWindowMessage(L"MSG_OBJ2_READY");
    WM_OBJ2_DONE = RegisterWindowMessage(L"MSG_OBJ2_DONE_COPY");

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
        CW_USEDEFAULT, 0, 400, 400, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

    case WM_CREATE: {
        HWND hLab6 = FindWindowW(L"Lab6Class", NULL);
        if (hLab6) {
            PostMessage(hLab6, WM_OBJ2_READY, 0, 0);
        }
        break;
    }

    case WM_COPYDATA: {
        PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

        if (pcds->dwData == 1) {
            ParamsDTO* params = (ParamsDTO*)pcds->lpData;

            g_data.clear();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dis(params->minVal, params->maxVal);

            for (int i = 0; i < params->n; i++) {
                g_data.push_back(dis(gen));
            }
            CopyToClipboard(hWnd, g_data);

            InvalidateRect(hWnd, NULL, TRUE);

            HWND hLab6 = FindWindowW(L"Lab6Class", NULL);
            if (hLab6) {
                PostMessage(hLab6, WM_OBJ2_DONE, 0, 0);
            }
        }
        return 1; 
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (!g_data.empty()) {
            int startX = 10, startY = 10;
            int x = startX, y = startY;
            int columns = 5;
            int colWidth = 70; 
            int rowHeight = 20; 
            int currentItem = 0;

            for (double val : g_data) {
                std::wstringstream wss;
                wss << std::fixed << std::setprecision(2) << val;
                std::wstring text = wss.str();

                TextOutW(hdc, x, y, text.c_str(), text.length());

                currentItem++;
                if (currentItem % columns == 0) {
                    x = startX;
                    y += rowHeight; 
                }
                else {
                    x += colWidth;  
                }
            }
        }
        else {
            std::wstring msg = L"Очікування параметрів від Lab6...";
            TextOutW(hdc, 10, 10, msg.c_str(), msg.length());
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