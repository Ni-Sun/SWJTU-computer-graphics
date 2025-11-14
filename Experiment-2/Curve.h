#pragma once
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;
// 二次函数
struct Curve
{
    POINT A,B,C;
    int type = 0; // 0 for Lagrange, 1 for Bezier
    static RECT rect;
    static COLORREF color;
    static RECT bezier_rect;
    static COLORREF bezier_color;
    Curve()
    {
        A=B=C={0,0};
    }
    Curve(POINT a, POINT b, POINT c, int curve_type = 0)
    {
        A=a, B=b, C=c;
        type = curve_type;
    }
};

void Draw_curve(HWND hWnd, vector<POINT> &arr);
void Draw_bezier_curve(HWND hWnd, vector<POINT> &arr);