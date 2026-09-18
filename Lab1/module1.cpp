#include "module1.h"
#include "module1.rh" 
static int currentPos = 1;

static INT_PTR CALLBACK Work1(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
    case WM_INITDIALOG:
        currentPos = 1;
        SetScrollRange(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL, 1, 100, TRUE);
        SetScrollPos(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL, currentPos, TRUE);
        SetDlgItemInt(hDlg, IDC_STATIC_VAL, currentPos, FALSE);
        return (INT_PTR)TRUE;

    case WM_HSCROLL: {
        int pos = GetScrollPos(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL);
        switch (LOWORD(wParam)) {
        case SB_LINELEFT: pos--; break;
        case SB_LINERIGHT: pos++; break;
        case SB_THUMBTRACK: pos = HIWORD(wParam); break;
        }
        if (pos < 1) pos = 1;
        if (pos > 100) pos = 100;
        SetScrollPos(GetDlgItem(hDlg, IDC_SCROLLBAR1), SB_CTL, pos, TRUE);
        SetDlgItemInt(hDlg, IDC_STATIC_VAL, pos, FALSE);
        currentPos = pos;
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
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

int Func_MOD1(HWND hWnd, int& outValue) {
    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    int result = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Work1);
    if (result == 1) {
        outValue = currentPos;
    }
    return result;
}