#include "Fill.h"
#include "Line.h"
#include "Circle.h"
#include "Rect.h"
#include "Triangle.h"
#include "Parallelogram.h"
#include "Rhombus.h"
#include "Bezier.h"
#include <algorithm>
#include <vector>
#include <list>

// 根据点到c圆心距离判断是否在圆内，用于检查选中
bool isInsideCircle(const POINT& p, const Circle& c) {
    double dx = p.x - c.O.x;
    double dy = p.y - c.O.y;
    return (dx * dx + dy * dy) < (c.r * c.r);
}

// 判断p是否在折线段之间
bool isInsidePolygon(const POINT& p, const std::vector<POINT>& polygon) {
    if (polygon.size() < 3) return false;
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if (((polygon[i].y > p.y) != (polygon[j].y > p.y)) &&
            (p.x < (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (double)(polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

RECT scanline_button_rect = {500, 530, 570, 560};
COLORREF scanline_button_color = RGB(211, 211, 211);

RECT seed_button_rect = {570, 530, 640, 560};
COLORREF seed_button_color = RGB(211, 211, 211);

std::vector<FilledShape> filled_shapes;

struct Edge {
    double ymax;
    double x;
    double dx;
    Edge* next;
};
//栅栏扫描线填充法
void scanlineFill(HDC hdc, const std::vector<POINT>& vertices, COLORREF fillColor) {
    if (vertices.size() < 3) return;

    int ymin = vertices[0].y, ymax = vertices[0].y;
    for (const auto& p : vertices) {
        if (p.y < ymin) ymin = p.y;
        if (p.y > ymax) ymax = p.y;
    }

    std::vector<Edge*> AET;
    std::vector<std::list<Edge>> NET(ymax - ymin + 1);

    for (size_t i = 0; i < vertices.size(); ++i) {
        POINT p1 = vertices[i];
        POINT p2 = vertices[(i + 1) % vertices.size()];

        if (p1.y == p2.y) continue;

        int y_start = std::min(p1.y, p2.y);
        int y_end = std::max(p1.y, p2.y);
        double x_start = (p1.y < p2.y) ? p1.x : p2.x;
        double dx = (double)(p2.x - p1.x) / (p2.y - p1.y);

        Edge edge = { (double)y_end, x_start, dx, nullptr };
        NET[y_start - ymin].push_back(edge);
    }

    for (int y = ymin; y < ymax; ++y) {
        for (auto it = NET[y - ymin].begin(); it != NET[y - ymin].end(); ++it) {
            AET.push_back(new Edge(*it));
        }

        AET.erase(std::remove_if(AET.begin(), AET.end(), [y](Edge* e) {
            bool remove = e->ymax <= y;
            if (remove) delete e;
            return remove;
        }), AET.end());

        std::sort(AET.begin(), AET.end(), [](Edge* a, Edge* b) {
            return a->x < b->x;
        });

        for (size_t i = 0; i < AET.size(); i += 2) {
            if (i + 1 < AET.size()) {
                for (int x = (int)ceil(AET[i]->x); x < (int)floor(AET[i + 1]->x); ++x) {
                    SetPixel(hdc, x, y, fillColor);
                }
            }
        }

        for (auto& edge : AET) {
            edge->x += edge->dx;
        }
    }
     for (auto edge : AET) {
        delete edge;
    }
}

void seedFill(HDC hdc, int x, int y, COLORREF fillColor, COLORREF boundaryColor) {
    COLORREF interiorColor = GetPixel(hdc, x, y);

    if (interiorColor == boundaryColor || interiorColor == fillColor) {
        return;
    }

    std::stack<POINT> pixels;
    pixels.push({x, y});

    while (!pixels.empty()) {
        POINT current = pixels.top();
        pixels.pop();

        int cx = current.x;
        int cy = current.y;

        if (cx < 0 || cx >= 900 || cy < 0 || cy >= 600) {
            continue;
        }

        if (GetPixel(hdc, cx, cy) == interiorColor) {
            SetPixel(hdc, cx, cy, fillColor);
            pixels.push({cx + 1, cy});
            pixels.push({cx - 1, cy});
            pixels.push({cx, cy + 1});
            pixels.push({cx, cy - 1});
        }
    }
}

void fillShape(HDC hdc, int shape_type, int shape_index, bool use_scanline, const POINT& click_pos) {
    std::vector<POINT> vertices;
    COLORREF boundaryColor = 0;

    // 已经涂色，跳过
    for (const auto& filled : filled_shapes) {
        if (filled.type == shape_type && filled.index == shape_index) {
            return; 
        }
    }

    bool is_inside = false;

    switch (shape_type) {
        case 1: { // Circle
            //遍历圆向量，依次检查每个圆
            if (shape_index < circles.size()) {
                Circle& c = circles[shape_index];
                boundaryColor = Circle::color;
                if (isInsideCircle(click_pos, c)) {
                    is_inside = true;
                }
            }
            break;
        }
        case 2: { // Rect
            if (shape_index < rects.size()) {
                Rect& r = rects[shape_index];
                boundaryColor = Rect::color;
                vertices.push_back({r.left, r.top});
                vertices.push_back({r.right, r.top});
                vertices.push_back({r.right, r.bottom});
                vertices.push_back({r.left, r.bottom});
                if (isInsidePolygon(click_pos, vertices)) {
                    is_inside = true;
                }
            }
            break;
        }
        case 8: { // Triangle
            if (shape_index < triangles.size()) {
                Triangle& t = triangles[shape_index];
                boundaryColor = Triangle::color;
                vertices.push_back(t.A);
                vertices.push_back(t.B);
                vertices.push_back(t.C);
                if (isInsidePolygon(click_pos, vertices)) {
                    is_inside = true;
                }
            }
            break;
        }
        case 9: { // Parallelogram
            if (shape_index < parallelograms.size()) {
                Parallelogram& p = parallelograms[shape_index];
                boundaryColor = Parallelogram::color;
                vertices.push_back(p.A);
                vertices.push_back(p.B);
                vertices.push_back(p.C);
                vertices.push_back(p.D);
                if (isInsidePolygon(click_pos, vertices)) {
                    is_inside = true;
                }
            }
            break;
        }
        case 11: { // Rhombus
            if (shape_index < rhombuses.size()) {
                Rhombus& r = rhombuses[shape_index];
                boundaryColor = Rhombus::color;
                vertices.push_back(r.A);
                vertices.push_back(r.B);
                vertices.push_back(r.C);
                vertices.push_back(r.D);
                if (isInsidePolygon(click_pos, vertices)) {
                    is_inside = true;
                }
            }
            break;
        }
    }

    if (is_inside) {
        if (use_scanline && !vertices.empty()) {
            scanlineFill(hdc, vertices, FILL_COLOR);
            filled_shapes.push_back({shape_type, shape_index, use_scanline, click_pos});
        } else if (!use_scanline) {
            seedFill(hdc, click_pos.x, click_pos.y, FILL_COLOR, boundaryColor);
            filled_shapes.push_back({shape_type, shape_index, use_scanline, click_pos});
        }
    }
}

void redrawFill(HDC hdc, const FilledShape& shape)
{
    std::vector<POINT> vertices;
    COLORREF boundaryColor = 0;

    HBRUSH hBrush = CreateSolidBrush(FILL_COLOR);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    switch (shape.type) {
        case 1: { // Circle
            if (shape.index < circles.size()) {
                Circle& c = circles[shape.index];
                if (!shape.use_scanline) {
                    // Use Ellipse to redraw filled circle
                    Ellipse(hdc, c.O.x - c.r, c.O.y - c.r, c.O.x + c.r, c.O.y + c.r);
                }
            }
            break;
        }
        case 2: { // Rect
            if (shape.index < rects.size()) {
                Rect& r = rects[shape.index];
                vertices.push_back({r.left, r.top});
                vertices.push_back({r.right, r.top});
                vertices.push_back({r.right, r.bottom});
                vertices.push_back({r.left, r.bottom});
            }
            break;
        }
        case 8: { // Triangle
            if (shape.index < triangles.size()) {
                Triangle& t = triangles[shape.index];
                vertices.push_back(t.A);
                vertices.push_back(t.B);
                vertices.push_back(t.C);
            }
            break;
        }
        case 9: { // Parallelogram
            if (shape.index < parallelograms.size()) {
                Parallelogram& p = parallelograms[shape.index];
                vertices.push_back(p.A);
                vertices.push_back(p.B);
                vertices.push_back(p.C);
                vertices.push_back(p.D);
            }
            break;
        }
        case 11: { // Rhombus
            if (shape.index < rhombuses.size()) {
                Rhombus& r = rhombuses[shape.index];
                vertices.push_back(r.A);
                vertices.push_back(r.B);
                vertices.push_back(r.C);
                vertices.push_back(r.D);
            }
            break;
        }
    }

    if (shape.use_scanline && !vertices.empty()) {
        scanlineFill(hdc, vertices, FILL_COLOR);
    } else if (!shape.use_scanline && !vertices.empty()) {
       
        Polygon(hdc, vertices.data(), vertices.size());
    }

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}