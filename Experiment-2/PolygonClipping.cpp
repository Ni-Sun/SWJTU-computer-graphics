#include "PolygonClipping.h"
// 裁剪矩形
enum ClipEdge {
    LEFT,
    RIGHT,
    BOTTOM,
    TOP
};

//顶点p1和p2与裁剪矩形clipWindow中边edge交点
POINT intersect(const POINT& p1, const POINT& p2, ClipEdge edge, const RECT& clipWindow) {
    POINT res = {0, 0};
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
//这部分根据前面Liang-Barsky求解：
    if (edge == LEFT || edge == RIGHT) {
        double clipX = (edge == LEFT) ? clipWindow.left : clipWindow.right;
        if (dx != 0) {
            res.y = p1.y + dy * (clipX - p1.x) / dx;
        } else {
            res.y = p1.y;
        }
        res.x = (LONG)clipX;
    } else { 
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

// 点p是否在裁剪多边形clipWindow中边edge内侧
bool isInside(const POINT& p, ClipEdge edge, const RECT& clipWindow) {
    switch (edge) {
        case LEFT:   return p.x >= clipWindow.left;//左边
        case RIGHT:  return p.x <= clipWindow.right;//右边
        case TOP:    return p.y >= clipWindow.top; //注意：TOP是“画布”视觉上在上面
        case BOTTOM: return p.y <= clipWindow.bottom;//注意：BUTTON是“画布”视觉上在下面
    }
    return false;
}

// 对矩形裁剪框clipWindow的边edge，处理多边形每一条边（s-p)
std::vector<POINT> clipAgainstEdge(const std::vector<POINT>& subjectPolygon, ClipEdge edge, const RECT& clipWindow) {
    std::vector<POINT> outputList;
    if (subjectPolygon.empty()) {
        return outputList;
    }

    POINT s = subjectPolygon.back(); //取多边形最后一个点
    for (const auto& p : subjectPolygon) {  //对多边形每一个点
        bool s_inside = isInside(s, edge, clipWindow);
        bool p_inside = isInside(p, edge, clipWindow);
        // 2个点都在，p输出
        if (s_inside && p_inside) {
            outputList.push_back(p);
        }
        //s在而p不在，交点输出
        else if (s_inside && !p_inside) {
            outputList.push_back(intersect(s, p, edge, clipWindow));
        }
        //s不在而p在，先输出交点，再输出p
        else if (!s_inside && p_inside) {
            outputList.push_back(intersect(s, p, edge, clipWindow));
            outputList.push_back(p);
        }
        //2个点都不在内侧：舍弃s和p
        s = p; // 遍历下一条边
    }
    return outputList;//处理结束后多边形点
}

// Sutherland-Hodgman
std::vector<POINT> sutherlandHodgmanClip(const std::vector<POINT>& subjectPolygon, const RECT& clipWindow) {
    // 遍历4个裁剪边：
    std::vector<POINT> clipped = clipAgainstEdge(subjectPolygon, LEFT, clipWindow);
    clipped = clipAgainstEdge(clipped, RIGHT, clipWindow);
    clipped = clipAgainstEdge(clipped, TOP, clipWindow);
    clipped = clipAgainstEdge(clipped, BOTTOM, clipWindow);
    
    return clipped;
    //最终多边形
}
