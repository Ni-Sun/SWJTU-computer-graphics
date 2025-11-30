#include "PolygonClipping.h"

// Define clipping edges
enum ClipEdge {
    LEFT,
    RIGHT,
    BOTTOM,
    TOP
};

// Computes the intersection of a line segment (p1-p2) with a clipping edge.
POINT intersect(const POINT& p1, const POINT& p2, ClipEdge edge, const RECT& clipWindow) {
    POINT res = {0, 0};
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    if (edge == LEFT || edge == RIGHT) {
        double clipX = (edge == LEFT) ? clipWindow.left : clipWindow.right;
        if (dx != 0) {
            res.y = p1.y + dy * (clipX - p1.x) / dx;
        } else {
            res.y = p1.y;
        }
        res.x = (LONG)clipX;
    } else { // TOP or BOTTOM
        double clipY = (edge == TOP) ? clipWindow.top : clipWindow.bottom;
        if (dy != 0) {
            res.x = p1.x + dx * (clipY - p1.y) / dy;
        } else {
            res.x = p1.x;
        }
        res.y = (LONG)clipY;
    }
    return res;
}

// Checks if a point is inside a clipping edge.
bool isInside(const POINT& p, ClipEdge edge, const RECT& clipWindow) {
    switch (edge) {
        case LEFT:   return p.x >= clipWindow.left;
        case RIGHT:  return p.x <= clipWindow.right;
        case TOP:    return p.y >= clipWindow.top; // In GDI, smaller y is higher
        case BOTTOM: return p.y <= clipWindow.bottom;
    }
    return false;
}

// Clips a polygon against a single edge of the clip window.
std::vector<POINT> clipAgainstEdge(const std::vector<POINT>& subjectPolygon, ClipEdge edge, const RECT& clipWindow) {
    std::vector<POINT> outputList;
    if (subjectPolygon.empty()) {
        return outputList;
    }

    POINT s = subjectPolygon.back();
    for (const auto& p : subjectPolygon) {
        bool s_inside = isInside(s, edge, clipWindow);
        bool p_inside = isInside(p, edge, clipWindow);

        // Case 1: Both points are inside
        if (s_inside && p_inside) {
            outputList.push_back(p);
        }
        // Case 2: Start point is inside, end point is outside
        else if (s_inside && !p_inside) {
            outputList.push_back(intersect(s, p, edge, clipWindow));
        }
        // Case 3: Start point is outside, end point is inside
        else if (!s_inside && p_inside) {
            outputList.push_back(intersect(s, p, edge, clipWindow));
            outputList.push_back(p);
        }
        // Case 4: Both points are outside - do nothing

        s = p; // Move to the next edge
    }
    return outputList;
}

// Main Sutherland-Hodgman clipping function
std::vector<POINT> sutherlandHodgmanClip(const std::vector<POINT>& subjectPolygon, const RECT& clipWindow) {
    // Clip against the left edge
    std::vector<POINT> clipped = clipAgainstEdge(subjectPolygon, LEFT, clipWindow);
    // Clip the result against the right edge
    clipped = clipAgainstEdge(clipped, RIGHT, clipWindow);
    // Clip the result against the top edge
    clipped = clipAgainstEdge(clipped, TOP, clipWindow);
    // Clip the final result against the bottom edge
    clipped = clipAgainstEdge(clipped, BOTTOM, clipWindow);
    
    return clipped;
}
