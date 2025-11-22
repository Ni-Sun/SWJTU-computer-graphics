#include "Parallelogram.h"
#include <algorithm>

extern vector<Parallelogram> parallelograms;

RECT Parallelogram::rect = {240,0,300,30};
COLORREF Parallelogram::color = RGB(120,200,180);

void Draw_parallelogram(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc = GetDC(hWnd);
    HBRUSH hBrush = CreateSolidBrush(Parallelogram::color);
    HBRUSH hOld = (HBRUSH)SelectObject(hdc, hBrush);
    SetBkMode(hdc, TRANSPARENT);

    auto P = arr[0];
    auto Q = arr[1];
    int l = min(P.x, Q.x), r = max(P.x, Q.x);
    int t = min(P.y, Q.y), b = max(P.y, Q.y);
    int w = r - l;
    int shift = w/4; 

    POINT a = {l + shift, t};
    POINT bpt = {r + shift, t};
    POINT c = {r, b};
    POINT d = {l, b};

    POINT pts[4] = {a, bpt, c, d};
    Polygon(hdc, pts, 4);

    SelectObject(hdc, hOld);
    DeleteObject(hBrush);
    ReleaseDC(hWnd, hdc);

    arr.clear();
    parallelograms.emplace_back(a,bpt,c,d);
}
