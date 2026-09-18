#include "module2.h"
#include "module2.rh"

static char* g_outText = nullptr;
static int g_maxLen = 0;

static INT_PTR CALLBACK Work2(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
    case WM_INITDIALOG:
        SetDlgItemTextA(hDlg, IDC_EDIT1, "");
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemTextA(hDlg, IDC_EDIT1, g_outText, g_maxLen);
            EndDialog(hDlg, 1);
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, 0);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int Func_MOD2(HWND hWnd, char* outText, int maxLen) {
    g_outText = outText;
    g_maxLen = maxLen;
    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, Work2);
}