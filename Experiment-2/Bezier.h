#pragma once
#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

struct Bezier
{
    vector<POINT> points;
    static RECT rect;
    static COLORREF color;
    Bezier()
    {
    }
    Bezier(const vector<POINT> &p)
    {
        points = p;
    }
};

void Draw_bezier(HWND hWnd, vector<POINT> &arr);
