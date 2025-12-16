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

// Forward declaration for Circle to be used in isInsideCircle
struct Circle;

// Helper functions defined in Fill.cpp
bool isInsideCircle(const POINT& p, const Circle& c);
bool isInsidePolygon(const POINT& p, const std::vector<POINT>& polygon);

const COLORREF FILL_COLOR = RGB(211, 211, 211); // Light gray

// Declarations for button rectangles, defined in Fill.cpp
extern RECT scanline_button_rect;
extern COLORREF scanline_button_color;
extern RECT seed_button_rect;
extern COLORREF seed_button_color;

// Struct to hold information about a filled shape for persistence
struct FilledShape {
    int type;
    int index;
    bool use_scanline;
    POINT seed_point; // Used for seed fill
};

// Global vector to store all filled shapes
extern std::vector<FilledShape> filled_shapes;

void scanlineFill(HDC hdc, const std::vector<POINT>& vertices, COLORREF fillColor);
void seedFill(HDC hdc, int x, int y, COLORREF fillColor, COLORREF boundaryColor);
void fillShape(HDC hdc, int shape_type, int shape_index, bool use_scanline, const POINT& click_pos);
void redrawFill(HDC hdc, const FilledShape& shape);