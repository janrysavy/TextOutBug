#include <windows.h>
#include <usp10.h>

#pragma comment(lib, "Usp10.lib")

void OnPaint(HDC hdc)
{
    // Note that this problem occurs with 'Arial' font that has not entry in
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\FontLink\SystemLink\Arial
    // but works fine with 'Segoe UI' that has entry in the registry.
    // When 'Segoe UI' is copied to 'Arial' entry, the problem disappears after restart.
    HFONT hFont = CreateFont(200, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    wchar_t buff1[] = L"\u2300"; // https://unicodemap.org/details/0x2300/index.html
    wchar_t buff2[] = L"\u2300\u21D2"; // https://unicodemap.org/details/0x21D2/index.html
    TextOutW(hdc, 0, 0, buff1, (int)wcslen(buff1)); // BUG
    TextOutW(hdc, 0, 200, buff2, (int)wcslen(buff2)); // OK
    // Note: ExtTextOutW has the same problem
    HRESULT hr1 = ScriptIsComplex(buff1, (int)wcslen(buff1), SIC_COMPLEX);
    if (hr1 == S_FALSE)
        OutputDebugString(L"1. ScriptIsComplex returned S_FALSE\n");
    HRESULT hr2 = ScriptIsComplex(buff2, (int)wcslen(buff2), SIC_COMPLEX);
    if (hr2 == S_FALSE)
        OutputDebugString(L"2. ScriptIsComplex returned S_FALSE\n");
    SelectObject(hdc, hOld);
    DeleteObject(hFont);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASSW wc = { 0 };
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"myWindowClass";
    wc.lpfnWndProc = WindowProcedure;
    if (!RegisterClassW(&wc))
        return -1;
    CreateWindowW(L"myWindowClass", L"TextOutBug", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 600, 600, NULL, NULL, NULL, NULL);
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
