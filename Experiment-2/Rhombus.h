#pragma once
#include <windows.h>
#include <vector>
using namespace std;
struct Rhombus{
    POINT A,B,C,D;
    static RECT rect;
    static COLORREF color;
    Rhombus(){A=B=C=D={0,0};}
    Rhombus(POINT a, POINT b, POINT c, POINT d):A(a),B(b),C(c),D(d){}
};

void Draw_rhombus(HWND hWnd, vector<POINT> &arr);
