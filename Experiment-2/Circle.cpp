#include "Circle.h"
#define DEBUG
extern vector<Line> lines;
extern vector<Circle> circles;
extern vector<POINT> Cross_points;
RECT Circle::rect={60,0,120,30};
COLORREF Circle::color=RGB(255,242,45);
RECT Circle::midpoint_rect={200, 530, 300, 560};
COLORREF Circle::midpoint_color=RGB(255, 255, 0);
RECT Circle::bresenham_rect={300, 530, 400, 560};
COLORREF Circle::bresenham_color=RGB(0, 0, 255);

void Draw_circle(HWND hWnd, vector<POINT> &arr)
{
    HDC hdc=GetDC(hWnd);
    HBRUSH hBrush=CreateSolidBrush(Circle::color);
    SelectObject(hdc,hBrush);
    SetBkMode(hdc,TRANSPARENT);

    auto [x1,y1]=arr[0];
    auto [x2,y2]=arr[1];
    int r=sqrt(pow(x1-x2,2)+pow(y1-y2,2))+0.5;
    Ellipse(hdc,x1-r,y1-r,x1+r,y1+r);
    arr.clear();
    DeleteObject(hBrush);
    ReleaseDC(hWnd,hdc);

    string str;
    str+="O: "+to_string(POINT{x1,y1})+",  r="+to_string(r)+'\n';
    auto Add_point=[&](pair<POINT,POINT> t)
    {
        auto [P,Q]=t;
        if(0<P.x && 0<P.y)
            str+="Cross Points: "+to_string(P)+'\n',
            Cross_points.push_back(P);
        if(0<Q.x && 0<Q.y)
            str+="Cross Points: "+to_string(Q)+'\n',
            Cross_points.push_back(Q);
    };
    Circle A(POINT{x1,y1},r);
    str+="Circle cross line:\n";
    for(auto B:lines)
        Add_point(Circle_cross_line(A,B));
        
    str+="\nCircle_cross_circle: \n";
    for(auto B:circles)
        Add_point(Circle_cross_circle(A,B));
    debug(str);
    circles.push_back(A);
}

void plot_circle_points(HDC hdc, int xc, int yc, int x, int y, COLORREF color) {
    SetPixel(hdc, xc + x, yc + y, color);
    SetPixel(hdc, xc - x, yc + y, color);
    SetPixel(hdc, xc + x, yc - y, color);
    SetPixel(hdc, xc - x, yc - y, color);
    SetPixel(hdc, xc + y, yc + x, color);
    SetPixel(hdc, xc - y, yc + x, color);
    SetPixel(hdc, xc + y, yc - x, color);
    SetPixel(hdc, xc - y, yc - x, color);
}

void Draw_midpoint_circle(HWND hWnd, vector<POINT>& arr) {
    HDC hdc = GetDC(hWnd);
    auto center = arr[0];
    auto point_on_circle = arr[1];
    int r = static_cast<int>(sqrt(pow(point_on_circle.x - center.x, 2) + pow(point_on_circle.y - center.y, 2)));

    int x = 0, y = r;
    int d = 1 - r;

    plot_circle_points(hdc, center.x, center.y, x, y, Circle::midpoint_color);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        plot_circle_points(hdc, center.x, center.y, x, y, Circle::midpoint_color);
    }

    arr.clear();
    ReleaseDC(hWnd, hdc);
    circles.push_back(Circle(center, r));
}

void Draw_bresenham_circle(HWND hWnd, vector<POINT>& arr) {
    HDC hdc = GetDC(hWnd);
    auto center = arr[0];
    auto point_on_circle = arr[1];
    int r = static_cast<int>(sqrt(pow(point_on_circle.x - center.x, 2) + pow(point_on_circle.y - center.y, 2)));

    int x = 0, y = r;
    int d = 3 - 2 * r;

    plot_circle_points(hdc, center.x, center.y, x, y, Circle::bresenham_color);

    while (x < y) {
        x++;
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        plot_circle_points(hdc, center.x, center.y, x, y, Circle::bresenham_color);
    }

    arr.clear();
    ReleaseDC(hWnd, hdc);
    circles.push_back(Circle(center, r));
}
