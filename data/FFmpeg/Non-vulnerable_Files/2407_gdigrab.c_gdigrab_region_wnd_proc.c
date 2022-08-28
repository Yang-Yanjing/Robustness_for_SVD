static LRESULT CALLBACK
gdigrab_region_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    switch (msg) {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        FrameRect(hdc, &rect, GetStockObject(BLACK_BRUSH));
        rect.left++; rect.top++; rect.right--; rect.bottom--;
        FrameRect(hdc, &rect, GetStockObject(WHITE_BRUSH));
        rect.left++; rect.top++; rect.right--; rect.bottom--;
        FrameRect(hdc, &rect, GetStockObject(BLACK_BRUSH));
        EndPaint(hwnd, &ps);
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}
