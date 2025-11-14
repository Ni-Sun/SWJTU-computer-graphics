#pragma once
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

struct Bezier
{
    POINT A,B,C;
    static RECT rect;
    static COLORREF color;
    Bezier()
    {
        A=B=C={0,0};
    }
    Bezier(POINT a, POINT b, POINT c)
    {
        A=a, B=b, C=c;
    }
};

void Draw_bezier(HWND hWnd, vector<POINT> &arr);
