#include "Rhombus.h"
#include <algorithm>

extern std::vector<Rhombus> rhombuses;

RECT Rhombus::rect = {300, 0, 360, 30};
COLORREF Rhombus::color = RGB(100, 150, 255);

void Draw_rhombus(HWND hWnd, std::vector<POINT> &arr)
{
    HDC hdc = GetDC(hWnd);
    HBRUSH hBrush = CreateSolidBrush(Rhombus::color);
    HBRUSH hOld = (HBRUSH)SelectObject(hdc, hBrush);
    SetBkMode(hdc, TRANSPARENT);

    auto P1 = arr[0];
    auto P2 = arr[1];
    int l = min(P1.x, P2.x), r = max(P1.x, P2.x);
    int t = min(P1.y, P2.y), b = max(P1.y, P2.y);
//v1-v4是菱形的4个点，l,r,t,b为左、右、下、上4条中轴线，用最大外接矩形左上、右下顶点确定一个菱形。
    POINT v1 = {(l + r) / 2, t};
    POINT v2 = {r, (t + b) / 2};
    POINT v3 = {(l + r) / 2, b};
    POINT v4 = {l, (t + b) / 2};

    POINT pts[4] = {v1, v2, v3, v4};
    Polygon(hdc, pts, 4);

    SelectObject(hdc, hOld);
    DeleteObject(hBrush);
    ReleaseDC(hWnd, hdc);

    arr.clear();
    rhombuses.emplace_back(v1, v2, v3, v4);
}
