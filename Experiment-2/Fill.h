#pragma once
#include <windows.h>
#include <vector>
#include "Point.h"
#include <vector>

// Forward declarations from main.cpp
extern std::vector<class Line> lines;
extern std::vector<class Circle> circles;
extern std::vector<class Rect> rects;
extern std::vector<class Triangle> triangles;
extern std::vector<class Parallelogram> parallelograms;
extern std::vector<class Rhombus> rhombuses;
extern std::vector<class Bezier> beziers;


const COLORREF FILL_COLOR = RGB(211, 211, 211); // Light gray

class Scanline_fill {
public:
    static RECT rect;
    static COLORREF color;
};

class Seed_fill {
public:
    static RECT rect;
    static COLORREF color;
};

// Struct to hold information about a filled shape for persistence
struct FilledShape {
    int type;
    int index;
    bool use_scanline;
    POINT seed_point; // Used for seed fill
};

// Global vector to store all filled shapes
extern std::vector<FilledShape> filled_shapes;

void fillShape(HDC hdc, int shape_type, int shape_index, bool use_scanline, const POINT& click_pos);

