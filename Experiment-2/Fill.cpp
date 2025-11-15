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

RECT scanline_button_rect = {500, 530, 570, 560};
COLORREF scanline_button_color = RGB(211, 211, 211);

RECT seed_button_rect = {570, 530, 640, 560};
COLORREF seed_button_color = RGB(211, 211, 211);

struct Edge {
    double ymax;
    double x;
    double dx;
    Edge* next;
};

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
    std::list<POINT> queue;
    queue.push_back({x, y});

    while (!queue.empty()) {
        POINT p = queue.front();
        queue.pop_front();

        COLORREF current_color = GetPixel(hdc, p.x, p.y);
        if (current_color != boundaryColor && current_color != fillColor) {
            SetPixel(hdc, p.x, p.y, fillColor);
            queue.push_back({p.x + 1, p.y});
            queue.push_back({p.x - 1, p.y});
            queue.push_back({p.x, p.y + 1});
            queue.push_back({p.x, p.y - 1});
        }
    }
}

void fillShape(HDC hdc, int shape_type, int shape_index, bool use_scanline, const POINT& click_pos) {
    std::vector<POINT> vertices;
    COLORREF boundaryColor = 0;

    switch (shape_type) {
        case 1: { // Circle
            if (shape_index < circles.size()) {
                Circle& c = circles[shape_index];
                boundaryColor = Circle::color;
                 if (!use_scanline) {
                    seedFill(hdc, c.O.x, c.O.y, FILL_COLOR, boundaryColor);
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
            }
            break;
        }
    }

    if (use_scanline && !vertices.empty()) {
        scanlineFill(hdc, vertices, FILL_COLOR);
    } else if (!use_scanline && !vertices.empty()) {
        POINT center = {0, 0};
        for(const auto& p : vertices) {
            center.x += p.x;
            center.y += p.y;
        }
        center.x /= vertices.size();
        center.y /= vertices.size();
        seedFill(hdc, center.x, center.y, FILL_COLOR, boundaryColor);
    }
}
