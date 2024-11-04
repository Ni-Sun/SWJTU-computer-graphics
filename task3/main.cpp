// #include <windows.h>
// #include <bits/stdc++.h>
// using namespace std;
// typedef pair<int,int> pii;
// const int N = 4000;
// const double eps = 1e-6;
// int cnt = 0, state=1;

// LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow = SW_SHOWNORMAL)
// {
//     WNDCLASS wc = {0};
//     wc.lpfnWndProc = WndProc;
//     wc.hInstance = hInstance;
//     wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
//     wc.lpszClassName = (LPSTR) "SimplePaint";

//     RegisterClass(&wc);

//     HWND hWnd = CreateWindow((LPSTR) "SimplePaint", (LPSTR) "Simple Paint", WS_OVERLAPPEDWINDOW,
//                              CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

//     ShowWindow(hWnd, nCmdShow);

//     MSG msg;
//     while (GetMessage(&msg, NULL, 0, 0))
//     {
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }

//     return msg.wParam;
// }


// vector<POINT> Bresenham_circle(HDC hdc, int xc, int yc, int radius, COLORREF color) {
//     int x = 0;
//     int y = radius;
//     int d = 3 - 2 * radius;
//     vector<POINT> res;
//     auto fun=[&](HDC hdc, int x, int y, COLORREF color)
//     {
//         SetPixel(hdc, x, y, color);
//         res.emplace_back(POINT{x,y});
//     };
//     while (x <= y) {
//         fun(hdc, xc + x, yc + y, color);
//         fun(hdc, xc - x, yc + y, color);
//         fun(hdc, xc + x, yc - y, color);
//         fun(hdc, xc - x, yc - y, color);
//         fun(hdc, xc + y, yc + x, color);
//         fun(hdc, xc - y, yc + x, color);
//         fun(hdc, xc + y, yc - x, color);
//         fun(hdc, xc - y, yc - x, color);

//         if (d < 0)
//             d = d + 4 * x + 6;
//         else
//         {
//             d = d + 4 * (x - y) + 10;
//             y--;
//         }
//         x++;
//     }
//     return res;
// }


// // Bresenham 算法绘制直线
// void Bresenham_Line(HDC hdc, POINT A, POINT B, COLORREF color)
// {
//     auto [x1, y1] = A;
//     auto [x2, y2] = B;
//     int dx = abs(x1 - x2), dy = abs(y1 - y2);
//     int sx = (x1 < x2 ? 1 : -1);
//     int sy = (y1 < y2 ? 1 : -1);
//     int err = dx - dy;
//     int x = x1, y = y1;

//     while (1)
//     {
//         SetPixel(hdc,x,y,color);
//         if (x == x2 && y == y2)
//             break;
//         int e2 = 2 * err;
//         if (e2 > -dy)
//         {
//             err -= dy;
//             x += sx;
//         }
//         if (e2 < dx)
//         {
//             err += dx;
//             y += sy;
//         }
//     }
// }

// LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     switch (message)
//     {
//     case WM_CREATE:
//         //
//         break;
//     case WM_PAINT:
//     {
//         PAINTSTRUCT ps;
//         HDC hdc = BeginPaint(hWnd, &ps);
//         FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

//         // code here1
//         Bresenham_circle(hdc,5,5,10,RGB(255,242,45));
//         EndPaint(hWnd, &ps);
//         break;
//     }
//     case WM_LBUTTONDOWN:
//     {
//         POINT T;
//         GetCursorPos(&T);
        
//         break;
//     }
//     case WM_MOUSEMOVE:
//         //
//         break;
//     case WM_COMMAND:
//         //
//         break;
//     case WM_DESTROY:
//         PostQuitMessage(0);
//         break;
//     default:
//         return DefWindowProc(hWnd, message, wParam, lParam);
//     }

//     return 0;
// }

























#include <windows.h>
#include <vector>
#include <algorithm>
#include "debug.h"
using std::vector;

// 绘制多边形填充
void FillPolygon(HDC hdc, POINT P, COLORREF fill_color, vector<POINT> &res) {
    COLORREF cur_color=GetPixel(hdc,P.x,P.y);
    if(cur_color != fill_color)
    {
        res.push_back(POINT{P.x,P.y});
        SetPixel(hdc,P.x,P.y,fill_color);
        FillPolygon(hdc, POINT{P.x + 1, P.y}, fill_color, res);
        FillPolygon(hdc, POINT{P.x - 1, P.y}, fill_color, res);
        FillPolygon(hdc, POINT{P.x, P.y + 1}, fill_color, res);
        FillPolygon(hdc, POINT{P.x, P.y - 1}, fill_color, res);
    }
}



// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        int l=300, r=305, b=300, t=305;

        MoveToEx(hdc,l,b,nullptr);
        LineTo(hdc,l,t);
        LineTo(hdc,r,t);
        LineTo(hdc,r,b);
        LineTo(hdc,l,b);
        vector<POINT> res;
        FillPolygon(hdc,POINT{l+r>>1, t+b>>1},RGB(0,0,0),res);

        ReleaseDC(hWnd,hdc);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), NULL, (LPSTR)"PolygonFill" };
    RegisterClass(&wc);

    HWND hWnd = CreateWindow((LPSTR)"PolygonFill", (LPSTR)"Polygon Fill", WS_OVERLAPPEDWINDOW, 100, 100, 400, 400, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

