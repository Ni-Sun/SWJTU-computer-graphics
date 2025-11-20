#include "Show.h"
#include "Triangle.h"
#include "Parallelogram.h"
#include "Rhombus.h"
#include "Select_tangent.h"
#include "Line.h"
#include "Circle.h"
#include "Rect.h"
#include "Curve.h"
#include "Poly.h"
#include "Select_point.h"
#include "Select_delete.h"
#include "Select_save.h"
#include "Select_open.h"
#include "Select_center.h"
#include "Select_perp.h"
#include "Bezier.h"
#include "Fill.h"
#include "Select_linestyle.h"
#include "Select_linewidth.h"

extern vector<Line> lines;
extern vector<Circle> circles;
extern vector<Rect> rects;
extern vector<Curve> curves;
extern vector<Poly> polylines;
extern vector<Triangle> triangles;
extern vector<Parallelogram> parallelograms;
extern vector<Rhombus> rhombuses;
extern vector<Bezier> beziers;

// selection globals exposed from main.cpp
extern int selected_type;
extern int selected_index;

void Show_text(HWND hWnd, int x, int y, std::wstring str)
{
    HDC hdc = GetDC(hWnd);
    SetBkMode(hdc, TRANSPARENT);
    TextOutW(hdc, x+10, y+10, str.c_str(), (int)str.length());
    ReleaseDC(hWnd, hdc);
}

void Draw_Title(HWND hWnd, HDC hdc)
{
    // 设置字体（使用宽字符 API）
    HFONT hFont = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    // 设置文字颜色
    SetTextColor(hdc, RGB(0,0,0));
    // 设置背景为透明
    SetBkMode(hdc,TRANSPARENT);

    
    auto draw=[&](COLORREF color, RECT rect, std::wstring wstr)
    {
        HBRUSH hBtnBrush = CreateSolidBrush(color);
        FillRect(hdc, &rect, hBtnBrush);
        DeleteObject(hBtnBrush);
        TextOutW(hdc, rect.left + 5, rect.top + 8, wstr.c_str(), (int)wstr.length());
    };
    draw(Line::color, Line::rect, L"直线");            // 直线
    draw(Circle::color, Circle::rect, L"圆");         // 圆
    draw(Rect::color, Rect::rect, L"矩形");            // 矩形
    draw(Triangle::color, Triangle::rect, L"三角");
    draw(Parallelogram::color, Parallelogram::rect, L"平行");
    draw(Rhombus::color, Rhombus::rect, L"菱形");
    draw(Curve::color, Curve::rect, L"曲线");         // 曲线
    draw(Poly::color, Poly::rect, L"折线");           // 折线
    draw(Select_point::color, Select_point::rect, L"选择");  // 选点
    // 工具按钮（使用各自的静态 rect/color，避免手工坐标不一致导致覆盖）
    draw(Select_delete::color, Select_delete::rect, L"删除");
    draw(Select_save::color, Select_save::rect, L"保存");
    draw(Select_open::color, Select_open::rect, L"打开");
    draw(Select_center::color, Select_center::rect, L"圆心");
    draw(Select_perp::color, Select_perp::rect, L"垂线");
    draw(Select_tangent::color, Select_tangent::rect, L"切线");
    draw(Line::midpoint_color, Line::midpoint_rect, L"中点画线");
    draw(Line::bresenham_color, Line::bresenham_rect, L"Bresenham");
    draw(Circle::midpoint_color, Circle::midpoint_rect, L"中点，圆");
    draw(Circle::bresenham_color, Circle::bresenham_rect, L"Bres，圆");
    draw(Bezier::color, Bezier::rect, L"Bezier");
    draw(scanline_button_color, scanline_button_rect, L"扫描线");
    draw(seed_button_color, seed_button_rect, L"种子");
    draw(linestyle_button_color, linestyle_button_rect, L"修改线型");
    draw(linewidth_button_color, linewidth_button_rect, L"修改线宽");
    // draw()
    // 恢复并删除临时字体//
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void Show_graphics(HWND hWnd, HDC hdc)
{
    // Redraw filled shapes for persistence
    for (const auto& shape : filled_shapes) {
        redrawFill(hdc, shape);
    }

    vector<POINT> arr;
    for(auto &l: lines)
    {
        HPEN hPen = CreatePen(l.dashed ? PS_DASH : PS_SOLID, l.dashed ? 1 : 2, Line::color);
        HPEN hOld = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, l.s.x, l.s.y, nullptr);
        LineTo(hdc, l.e.x, l.e.y);
        SelectObject(hdc, hOld);
        DeleteObject(hPen);
    }
    for(auto &cir: circles)
    {
        HPEN hPen = CreatePen(cir.dashed ? PS_DASH : PS_SOLID, cir.dashed ? 1 : 2, Circle::color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Ellipse(hdc, cir.O.x - cir.r, cir.O.y - cir.r, cir.O.x + cir.r, cir.O.y + cir.r);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
    // 如果有 overlay 文本需要显示（例如选点显示最近交点），在标题区显示
    if (overlay_show && !overlay_text.empty()) {
        // 在 overlay_pt 附近绘制文本（宽字符）
        SetTextColor(hdc, RGB(0,0,0));
        SetBkMode(hdc, TRANSPARENT);
        TextOutW(hdc, overlay_pt.x + 10, overlay_pt.y + 10, overlay_text.c_str(), (int)overlay_text.length());
    }
    // Draw existing rects
    for(auto &r: rects)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, Rect::color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, r.left, r.top, r.right, r.bottom);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
    // Draw existing triangles
    for(auto &tri: triangles)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, Triangle::color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        POINT pts[3] = {tri.A, tri.B, tri.C};
        Polygon(hdc, pts, 3);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
    // Draw existing parallelograms
    for(auto &pg: parallelograms)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, Parallelogram::color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        POINT pts[4] = {pg.A, pg.B, pg.C, pg.D};
        Polygon(hdc, pts, 4);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
    // Draw existing rhombuses
    for(auto &rh: rhombuses)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, Rhombus::color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        POINT pts[4] = {rh.A, rh.B, rh.C, rh.D};
        Polygon(hdc, pts, 4);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
    // Draw existing curves
    for(auto &cur: curves)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, Curve::color);
        HPEN hOld = (HPEN)SelectObject(hdc, hPen);
        auto [x1,y1]=cur.A;
        auto [x2,y2]=cur.B;
        auto [x3,y3]=cur.C;
        auto fun=[&](double x)
        {
            double res=((x-x2)*(x-x3)/(x1-x2)/(x1-x3))*y1
                     + ((x-x1)*(x-x3)/(x2-x1)/(x2-x3))*y2
                     + ((x-x1)*(x-x2)/(x3-x1)/(x3-x2))*y3;
            return int(res+0.5);
        };
        int l=min({x1,x2,x3});
        int r=max({x1,x2,x3});
        MoveToEx(hdc,l,fun(l),nullptr);
        for(int x=l;x<=r;x++)
        {
            int y=fun(x);
            LineTo(hdc,x,y);
        }
        SelectObject(hdc, hOld);
        DeleteObject(hPen);
    }
    for(auto &pl:polylines)
    {
        // Draw polyline points
        if(pl.p.size()>=2){
            HPEN hPen = CreatePen(PS_SOLID, 2, Poly::color);
            HPEN hOld = (HPEN)SelectObject(hdc, hPen);
            MoveToEx(hdc, pl.p[0].x, pl.p[0].y, NULL);
            for(size_t i=1;i<pl.p.size();++i) LineTo(hdc, pl.p[i].x, pl.p[i].y);
            SelectObject(hdc, hOld);
            DeleteObject(hPen);
        }
    }

    for(auto &bz: beziers)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, Bezier::color);
        HPEN hOld = (HPEN)SelectObject(hdc, hPen);
        POINT p0 = bz.A;
        POINT p1 = bz.B;
        POINT p2 = bz.C;

        MoveToEx(hdc, p0.x, p0.y, nullptr);
        for (double t = 0.01; t <= 1.0; t += 0.01)
        {
            double x = (1 - t) * (1 - t) * p0.x + 2 * t * (1 - t) * p1.x + t * t * p2.x;
            double y = (1 - t) * (1 - t) * p0.y + 2 * t * (1 - t) * p1.y + t * t * p2.y;
            LineTo(hdc, (int)x, (int)y);
        }
        SelectObject(hdc, hOld);
        DeleteObject(hPen);
    }

    if(selected_type!=-1 && selected_index>=0)
    {
        HPEN hDashPen = CreatePen(PS_DASH, 2, RGB(0,0,0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hDashPen);
        switch(selected_type)
        {
            case 0: // line
                if(selected_index < (int)lines.size()){
                    auto &L = lines[selected_index];
                    MoveToEx(hdc, L.s.x, L.s.y, NULL);
                    LineTo(hdc, L.e.x, L.e.y);
                }
                break;
            case 1: // circle
                if(selected_index < (int)circles.size()){
                    auto &C = circles[selected_index];
                    int r = C.r;
                    Ellipse(hdc, C.O.x - r, C.O.y - r, C.O.x + r, C.O.y + r);
                }
                break;
            case 2: // rect
                if(selected_index < (int)rects.size()){
                    auto &R = rects[selected_index];
                    Rectangle(hdc, R.left, R.top, R.right, R.bottom);
                }
                break;
            case 3: // curve
                if(selected_index < (int)curves.size()){
                    auto &C = curves[selected_index];
                    int l = min({C.A.x, C.B.x, C.C.x});
                    int r = max({C.A.x, C.B.x, C.C.x});
                    auto fun=[&](double x){
                        double res=((x-C.B.x)*(x-C.C.x)/(C.A.x-C.B.x)/(C.A.x-C.C.x))*C.A.y
                                 + ((x-C.A.x)*(x-C.C.x)/(C.B.x-C.A.x)/(C.B.x-C.C.x))*C.B.y
                                 + ((x-C.A.x)*(x-C.B.x)/(C.C.x-C.A.x)/(C.C.x-C.B.x))*C.C.y;
                        return int(res+0.5);
                    };
                    MoveToEx(hdc, l, fun(l), NULL);
                    for(int x=l;x<=r;x++) LineTo(hdc, x, fun(x));
                }
                break;
            case 8: // triangle
                if(selected_index < (int)triangles.size()){
                    auto &T = triangles[selected_index];
                    POINT pts[3] = {T.A, T.B, T.C};
                    Polygon(hdc, pts, 3);
                }
                break;
            case 9: // parallelogram
                if(selected_index < (int)parallelograms.size()){
                    auto &P = parallelograms[selected_index];
                    POINT pts[4] = {P.A, P.B, P.C, P.D};
                    Polygon(hdc, pts, 4);
                }
                break;
            case 11: // rhombus
                if(selected_index < (int)rhombuses.size()){
                    auto &rh = rhombuses[selected_index];
                    POINT pts[4] = {rh.A, rh.B, rh.C, rh.D};
                    Polygon(hdc, pts, 4);
                }
                break;
            case 4: // poly
                if(selected_index < (int)polylines.size()){
                    auto &P = polylines[selected_index];
                    if(P.p.size()>=2){
                        MoveToEx(hdc, P.p[0].x, P.p[0].y, NULL);
                        for(size_t i=1;i<P.p.size();i++) LineTo(hdc, P.p[i].x, P.p[i].y);
                    }
                }
                break;
        }
        SelectObject(hdc, hOldPen);
        DeleteObject(hDashPen);
    }
}