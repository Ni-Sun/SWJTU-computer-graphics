#pragma once
#include <windows.h>
#include <vector>
using namespace std;
struct Triangle{
    POINT A,B,C;
    static RECT rect;
    static COLORREF color;
    Triangle(){A=B=C={0,0};}
    Triangle(POINT a, POINT b, POINT c):A(a),B(b),C(c){}
};

void Draw_triangle(HWND hWnd, vector<POINT> &arr);
