#include "Triangle.h"
#include <cmath>
#include <algorithm>
#include "debug.h"

extern vector<Triangle> triangles;

RECT Triangle::rect = {180,0,240,30};
COLORREF Triangle::color = RGB(255,180,30);

void Draw_triangle(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc = GetDC(hWnd);
    HBRUSH hBrush = CreateSolidBrush(Triangle::color);
    HBRUSH hOld = (HBRUSH)SelectObject(hdc, hBrush);
    SetBkMode(hdc, TRANSPARENT);

    // 三角形创建
    auto A = arr[0];
    auto B = arr[1];
    int l = min(A.x, B.x), r = max(A.x, B.x);
    int t = min(A.y, B.y), b = max(A.y, B.y);
    POINT p1 = {(l + r) / 2, t}; 
    POINT p2 = {l, b}; 
    POINT p3 = {r, b}; 

    POINT pts[3] = {p1, p2, p3};
    Polygon(hdc, pts, 3);

    SelectObject(hdc, hOld);
    DeleteObject(hBrush);
    ReleaseDC(hWnd, hdc);

    arr.clear();
    triangles.emplace_back(p1, p2, p3);
}
