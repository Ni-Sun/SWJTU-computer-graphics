#pragma once
#include <windows.h>
#include <vector>
using namespace std;
struct Parallelogram{
    POINT A,B,C,D; // in order (top-left, top-right, bottom-right, bottom-left)
    static RECT rect;
    static COLORREF color;
    Parallelogram(){A=B=C=D={0,0};}
    Parallelogram(POINT a, POINT b, POINT c, POINT d):A(a),B(b),C(c),D(d){}
};

void Draw_parallelogram(HWND hWnd, vector<POINT> &arr);
