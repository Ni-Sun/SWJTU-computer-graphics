#include "Curve.h"
extern vector<Curve> curves;
// shift Curve button right to make room for Triangle/Parallelogram
RECT Curve::rect={360,0,420,30};
COLORREF Curve::color=RGB(23,205,255);
RECT Curve::bezier_rect={400, 530, 500, 560};
COLORREF Curve::bezier_color=RGB(255, 255, 255);

void Draw_curve(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc=GetDC(hWnd);
    HPEN hPen=CreatePen(PS_SOLID,2,Curve::color);
    SelectObject(hdc,hPen);
    // copy points locally BEFORE any clears to avoid using cleared vector
    if (arr.size() < 3) {
        // not enough points, cleanup and return
        DeleteObject(hPen);
        ReleaseDC(hWnd, hdc);
        return;
    }
    POINT A = arr[0];
    POINT B = arr[1];
    POINT C = arr[2];

    int x1 = A.x, y1 = A.y;
    int x2 = B.x, y2 = B.y;
    int x3 = C.x, y3 = C.y;

    auto fun = [&](double x) {
        double res = ((x - x2) * (x - x3) / (double)(x1 - x2) / (double)(x1 - x3)) * y1
                   + ((x - x1) * (x - x3) / (double)(x2 - x1) / (double)(x2 - x3)) * y2
                   + ((x - x1) * (x - x2) / (double)(x3 - x1) / (double)(x3 - x2)) * y3;
        return int(res + 0.5);
    };

    int l = std::min({x1, x2, x3});
    int r = std::max({x1, x2, x3});
    MoveToEx(hdc, l, fun(l), nullptr);
    for (int x = l; x <= r; x++) {
        int y = fun(x);
        LineTo(hdc, x, y);
    }

    // append the curve to storage using the copied points
    curves.emplace_back(A, B, C);

    // clear the input arr as original logic intended
    arr.clear();

    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
}

void Draw_bezier_curve(HWND hWnd, vector<POINT> &arr)
{
    if (arr.size() < 3) {
        return;
    }
    curves.emplace_back(arr[0], arr[1], arr[2], 1);
    arr.clear();
    InvalidateRect(hWnd, NULL, TRUE);
}