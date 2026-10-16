#include "my_table.h"
#include "Resource.h" 

MyTable* MyTable::instance = nullptr;

MyTable::MyTable() : hDlg(NULL), hParent(NULL), hList(NULL) {
    instance = this;
}

MyTable::~MyTable() {
    Close();
    instance = nullptr;
}

void MyTable::Show(HINSTANCE hInstance, HWND parent) {
    hParent = parent;

    if (!hDlg) {
        hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TABLE), hParent, DlgProc);
    }

    if (hDlg) {
        ShowWindow(hDlg, SW_SHOW);
        SetFocus(hDlg);
    }
}

void MyTable::Close() {
    if (hDlg) {
        DestroyWindow(hDlg);
        hDlg = NULL;
    }
}

void MyTable::InitListView() {
    hList = GetDlgItem(hDlg, IDC_LISTVIEW);

    ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    lvc.fmt = LVCFMT_LEFT;

    lvc.cx = 120;
    static WCHAR strName[] = { 0x041D, 0x0430, 0x0437, 0x0432, 0x0430, 0 }; // "Назва"
    lvc.pszText = strName;
    ListView_InsertColumn(hList, 0, &lvc);

    lvc.cx = 50;
    lvc.pszText = (LPWSTR)L"x1"; ListView_InsertColumn(hList, 1, &lvc);
    lvc.pszText = (LPWSTR)L"y1"; ListView_InsertColumn(hList, 2, &lvc);
    lvc.pszText = (LPWSTR)L"x2"; ListView_InsertColumn(hList, 3, &lvc);
    lvc.pszText = (LPWSTR)L"y2"; ListView_InsertColumn(hList, 4, &lvc);
}

void MyTable::UpdateData(const std::vector<TableRowData>& data) {
    if (!hList) return;

    ListView_DeleteAllItems(hList);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;

    for (size_t i = 0; i < data.size(); i++) {
        lvi.iItem = (int)i; 
        lvi.iSubItem = 0;   
        lvi.pszText = (LPWSTR)data[i].name.c_str();
        ListView_InsertItem(hList, &lvi); 

        std::wstring sx1 = std::to_wstring(data[i].x1);
        std::wstring sy1 = std::to_wstring(data[i].y1);
        std::wstring sx2 = std::to_wstring(data[i].x2);
        std::wstring sy2 = std::to_wstring(data[i].y2);

        ListView_SetItemText(hList, (int)i, 1, (LPWSTR)sx1.c_str());
        ListView_SetItemText(hList, (int)i, 2, (LPWSTR)sy1.c_str());
        ListView_SetItemText(hList, (int)i, 3, (LPWSTR)sx2.c_str());
        ListView_SetItemText(hList, (int)i, 4, (LPWSTR)sy2.c_str());
    }
}

// 
INT_PTR CALLBACK MyTable::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

    case WM_INITDIALOG:
        if (instance) {
            instance->hDlg = hDlg; 
            instance->InitListView();
        }
        return (INT_PTR)TRUE;

    case WM_NOTIFY: {
        LPNMHDR pnmh = (LPNMHDR)lParam;

        if (pnmh->idFrom == IDC_LISTVIEW) {
            LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;

            if (pnmh->code == NM_CLICK && lpnmitem->iItem != -1) {
                if (instance && instance->hParent) {
                    SendMessage(instance->hParent, WM_USER_HIGHLIGHT_SHAPE, (WPARAM)lpnmitem->iItem, 0);
                }
            }
            else if (pnmh->code == NM_DBLCLK && lpnmitem->iItem != -1) {
                if (instance && instance->hParent) {
                    SendMessage(instance->hParent, WM_USER_DELETE_SHAPE, (WPARAM)lpnmitem->iItem, 0);
                }
            }
        }
        break;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDOK) {
            if (instance) instance->Close();
            return (INT_PTR)TRUE;
        }
        break;

    case WM_CLOSE:
        if (instance) instance->Close();
        return (INT_PTR)TRUE;
    }

    return (INT_PTR)FALSE;
}
