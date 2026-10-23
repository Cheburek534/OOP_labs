#include "framework.h"
#include "Resource.h"
#include <string>

HINSTANCE hInst;
WCHAR szTitle[] = L"Lab6 - Менеджер (Варіант 3)";
WCHAR szWindowClass[] = L"Lab6Class";

int g_n = 10;
double g_Min = 0.0;
double g_Max = 100.0;

struct ParamsDTO {
    int n;
    double minVal;
    double maxVal;
};

UINT WM_OBJ2_READY = 0;
UINT WM_OBJ3_READY = 0;
UINT WM_OBJ2_DONE = 0;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ParamsDlg(HWND, UINT, WPARAM, LPARAM);

bool StartProcess(LPCWSTR exeName) {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessW(exeName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    return false;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    WM_OBJ2_READY = RegisterWindowMessage(L"MSG_OBJ2_READY");
    WM_OBJ3_READY = RegisterWindowMessage(L"MSG_OBJ3_READY");
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
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB6);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        50, 50, 400, 200, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        DWORD err = GetLastError();
        WCHAR errMsg[256];
        swprintf(errMsg, 256, L"Не вдалося створити вікно Lab6! Код помилки: %lu\nПеревірте Lab6.rc", err);
        MessageBoxW(NULL, errMsg, L"Критична помилка", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_OBJ2_READY) {
        HWND hObj2 = FindWindowW(L"Object2Class", NULL);
        if (hObj2) {
            ParamsDTO params = { g_n, g_Min, g_Max };
            COPYDATASTRUCT cds;
            cds.dwData = 1;
            cds.cbData = sizeof(ParamsDTO);
            cds.lpData = &params;

            SendMessage(hObj2, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        }
        return 0;
    }

    if (message == WM_OBJ2_DONE) {
        HWND hObj3 = FindWindowW(L"Object3Class", NULL);
        if (!hObj3) {
            StartProcess(L"Object3.exe");
        }
        else {
            PostMessage(hObj3, RegisterWindowMessage(L"MSG_DRAW_GRAPH"), 0, 0);
        }
        return 0;
    }

    if (message == WM_OBJ3_READY) {
        HWND hObj3 = FindWindowW(L"Object3Class", NULL);
        if (hObj3) {
            PostMessage(hObj3, RegisterWindowMessage(L"MSG_DRAW_GRAPH"), 0, 0);

            HWND hObj2 = FindWindowW(L"Object2Class", NULL);
            if (hObj2) MoveWindow(hObj2, 50, 260, 400, 400, TRUE);
            if (hObj3) MoveWindow(hObj3, 460, 50, 600, 610, TRUE);
        }
        return 0;
    }

    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case ID_START_PROCESS:
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PARAMS), hWnd, ParamsDlg) == IDOK) {
                HWND hObj2 = FindWindowW(L"Object2Class", NULL);
                if (hObj2) {
                    PostMessage(hWnd, WM_OBJ2_READY, 0, 0);
                }
                else {
                    if (!StartProcess(L"Object2.exe")) {
                        MessageBoxW(hWnd, L"Не вдалося знайти Object2.exe! Скомпілюйте його.", L"Помилка", MB_ICONERROR);
                    }
                }
            }
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_DESTROY: {
        HWND hObj2 = FindWindowW(L"Object2Class", NULL);
        HWND hObj3 = FindWindowW(L"Object3Class", NULL);
        if (hObj2) PostMessage(hObj2, WM_CLOSE, 0, 0);
        if (hObj3) PostMessage(hObj3, WM_CLOSE, 0, 0);

        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK ParamsDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        SetDlgItemInt(hDlg, IDC_EDIT_N, g_n, FALSE);
        SetDlgItemTextW(hDlg, IDC_EDIT_MIN, std::to_wstring(g_Min).c_str());
        SetDlgItemTextW(hDlg, IDC_EDIT_MAX, std::to_wstring(g_Max).c_str());
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            g_n = GetDlgItemInt(hDlg, IDC_EDIT_N, NULL, FALSE);

            WCHAR buf[256];
            GetDlgItemTextW(hDlg, IDC_EDIT_MIN, buf, 256);
            g_Min = _wtof(buf);

            GetDlgItemTextW(hDlg, IDC_EDIT_MAX, buf, 256);
            g_Max = _wtof(buf);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
