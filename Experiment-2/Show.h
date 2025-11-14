#pragma once
#include <windows.h>
#include <bits/stdc++.h>
#include "Point.h"
#include "Line.h"
#include "Circle.h"
#include "Rect.h"
#include "Curve.h"
#include "Poly.h"
#include "Bezier.h"
#include "Select_point.h"
#include "Fill.h"
using namespace std;

void Show_text(HWND hWnd, int x, int y, std::wstring str);

void Draw_Title(HWND hWnd, HDC hdc);

// 显示所有图案
void Show_graphics(HWND hWnd, HDC hdc);

// overlay text drawn by WM_PAINT when needed
extern std::wstring overlay_text;
extern POINT overlay_pt;
extern bool overlay_show;
// selected primitive: -1 none, 0 line,1 circle,2 rect,3 curve,4 poly
extern int selected_type;
extern int selected_index;
