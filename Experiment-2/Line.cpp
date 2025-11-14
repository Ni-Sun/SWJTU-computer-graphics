#include "Line.h"
extern vector<Line> lines;
extern vector<Circle> circles;
extern vector<POINT> Cross_points; // 交点
RECT Line::rect={0,0,60,30};
COLORREF Line::color=RGB(212,73,30);
RECT Line::midpoint_rect={0, 530, 100, 560};
COLORREF Line::midpoint_color=RGB(0,255,0);
RECT Line::bresenham_rect={100, 530, 200, 560};
COLORREF Line::bresenham_color=RGB(255,0,0);

void Draw_line(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc=GetDC(hWnd);
    HPEN hPen=CreatePen(PS_SOLID,2,Line::color);
    SelectObject(hdc,hPen);

    auto A=arr[0], B=arr[1];
    MoveToEx(hdc,A.x,A.y,nullptr);
    LineTo(hdc,B.x,B.y);
    arr.clear();
    DeleteObject(hPen);
    ReleaseDC(hWnd,hdc);

    string str;
    Line l1=Line(A,B);
    str+=to_string(A)+"  "+to_string(B)+'\n';
    auto Add_point=[&](pair<POINT,POINT> t)
    {
        auto [P,Q]=t;
        if(l1.left<=P.x && P.x<=l1.right)
            str+="Cross Points: "+to_string(P)+'\n',
            Cross_points.push_back(P);
        if(l1.left<=P.x && P.x<=l1.right)
            str+="Cross Points: "+to_string(Q)+'\n',
            Cross_points.push_back(Q);
    };
    for(auto l2:lines)
        Add_point(make_pair(Line_cross_line(l1,l2),POINT{-1,-1}));
    for(auto C:circles)
        Add_point(Circle_cross_line(C,l1));
    lines.push_back(l1);

    str+="lines :\n";
    for(auto l:lines)
        str+="a="+to_string(l.a)+", b="+to_string(l.b)+", c="+to_string(l.c)+'\n';
    debug(str);
}

void Draw_midpoint_line(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc = GetDC(hWnd);
    HPEN hPen = CreatePen(PS_SOLID, 2, Line::midpoint_color);
    SelectObject(hdc, hPen);

    auto A = arr[0], B = arr[1];
    int x1 = A.x, y1 = A.y;
    int x2 = B.x, y2 = B.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    if (dy > dx) {
        swap(dx, dy);
        int d = 2 * dy - dx;
        int x = x1, y = y1;
        for (int i = 0; i < dx; i++) {
            SetPixel(hdc, x, y, Line::midpoint_color);
            if (d >= 0) {
                x += sx;
                d -= 2 * dy;
            }
            y += sy;
            d += 2 * dx;
        }
    } else {
        int d = 2 * dy - dx;
        int x = x1, y = y1;
        for (int i = 0; i < dx; i++) {
            SetPixel(hdc, x, y, Line::midpoint_color);
            if (d >= 0) {
                y += sy;
                d -= 2 * dx;
            }
            x += sx;
            d += 2 * dy;
        }
    }

    arr.clear();
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
    lines.push_back(Line(A, B));
}

void Draw_bresenham_line(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc = GetDC(hWnd);
    HPEN hPen = CreatePen(PS_SOLID, 2, Line::bresenham_color);
    SelectObject(hdc, hPen);

    auto A = arr[0], B = arr[1];
    int x1 = A.x, y1 = A.y;
    int x2 = B.x, y2 = B.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    if (dy > dx) {
        swap(dx, dy);
        int p = 2 * dy - dx;
        int x = x1, y = y1;
        for (int i = 0; i < dx; i++) {
            SetPixel(hdc, x, y, Line::bresenham_color);
            if (p >= 0) {
                x += sx;
                p -= 2 * dy;
            }
            y += sy;
            p += 2 * dx;
        }
    } else {
        int p = 2 * dy - dx;
        int x = x1, y = y1;
        for (int i = 0; i < dx; i++) {
            SetPixel(hdc, x, y, Line::bresenham_color);
            if (p >= 0) {
                y += sy;
                p -= 2 * dx;
            }
            x += sx;
            p += 2 * dy;
        }
    }

    arr.clear();
    DeleteObject(hPen);
    ReleaseDC(hWnd, hdc);
    lines.push_back(Line(A, B));
}

Point lineprog(POINT P, Line A)
{
    return A.s + ( ((A.e-A.s)*(A.e-A.s)*(P-A.s))/(pow(len(A.e,A.s),2)) );
}