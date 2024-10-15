#include <windows.h>
#include <bits/stdc++.h>
using namespace std;
typedef pair<int,int> pii;
const int N = 4000;
const double eps = 1e-6;
int cnt = 0, state=1;
POINT A, B;
set<pii> s1,s2,s3;
RECT btnRect1 = {0, 0, 120, 30};   // Bresenham
RECT btnRect2 = {150, 0, 270, 30};  // MidPoint

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow = SW_SHOWNORMAL)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = (LPSTR) "SimplePaint";

    RegisterClass(&wc);

    HWND hWnd = CreateWindow((LPSTR) "SimplePaint", (LPSTR) "Simple Paint", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// DDA 算法绘制直线
void DDA_Line(HDC hdc, POINT A, POINT B, COLORREF color)
{
    auto [x1, y1] = A;
    auto [x2, y2] = B;
    int dx = x2 - x1, dy = y2 - y1;
    int step = max(abs(dx), abs(dy));
    double tx = double(dx) / double(step);
    double ty = double(dy) / double(step);
    double x = x1, y = y1;
    while (1)
    {
        s1.emplace(int(x+0.5),int(y+0.5));
        // SetPixel(hdc,int(x+0.5),int(y+0.5),color);
        x += tx, y += ty;
        if (fabs(x - x2) <= eps || fabs(y - y2) <= eps)
            break;
    }
}

// Bresenham 算法绘制直线
void Bresenham_Line(HDC hdc, POINT A, POINT B, COLORREF color)
{
    auto [x1, y1] = A;
    auto [x2, y2] = B;
    int dx = abs(x1 - x2), dy = abs(y1 - y2);
    int sx = (x1 < x2 ? 1 : -1);
    int sy = (y1 < y2 ? 1 : -1);
    int err = dx - dy;
    int x = x1, y = y1;

    while (1)
    {
        s2.emplace(x,y);
        // SetPixel(hdc,x,y,color);
        if (x == x2 && y == y2)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

// 中点画线法绘制直线
void MidPoint_Line(HDC hdc, POINT A, POINT B, COLORREF color)
{
    auto [x1,y1]=A;
    auto [x2,y2]=B;
	int x = x1, y = y1;
	int a = -1*abs(y2 - y1), b = abs(x2 - x1);
    int sx=(x2>x1 ? 1 : -1);
    int sy=(y2>y1 ? 1 : -1);

	int d, d1, d2;
	if (-a <= b)		// 斜率绝对值 <= 1
	{
		d = 2 * a + b;
		d1 = 2 * a;
		d2 = 2 * (a + b);
		while(x != x2)
		{
			if (d < 0)
				y += sy, d += d2;
			else
				d += d1;
			x += sx;
            s3.emplace(x,y);
            // SetPixel(hdc,x,y,color);
		}
	}
	else				// 斜率绝对值 > 1
	{
		d = 2 * b + a;
		d1 = 2 * b;
		d2 = 2 * (a + b);
		while(y != y2)
		{
			if(d < 0)
				d += d1;
			else
				x += sx, d += d2;
			y += sy;
            s3.emplace(x,y);
		}
	}
}

void Draw_Line(HDC hdc, COLORREF color)
{
    auto fun=[&](const set<pii> &s, COLORREF color)
    {
        for(auto [x,y]:s)
            SetPixel(hdc,x,y,color);
    };
    fun(s1,RGB(255,142,142));   // 玫瑰红
    fun(s2,RGB(115,219,73));    // 绿色
    fun(s3,RGB(73,60,219));     // 蓝色
}

void Draw_Title(HWND hWnd, HDC hdc)
{
    // 设置字体
    HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFont);
    // 设置文字颜色
    SetTextColor(hdc, RGB(0,0,0));
    // 设置背景为透明
    SetBkMode(hdc,TRANSPARENT);

    
    // Bresenham
    HBRUSH hBtnBrush1 = CreateSolidBrush(RGB(115,219,73));
    FillRect(hdc, &btnRect1, hBtnBrush1);
    DeleteObject(hBtnBrush1);
    TextOut(hdc, 15,8,LPSTR("Bresenham"),9);

    // MidPoint
    HBRUSH hBtnBrush2 = CreateSolidBrush(RGB(73,60,219));
    FillRect(hdc, &btnRect2, hBtnBrush2);
    DeleteObject(hBtnBrush2);
    TextOut(hdc, 170,8,LPSTR("MidPoint"),8);
}

void (*funArr[])(HDC, POINT, POINT, COLORREF) = {DDA_Line, Bresenham_Line, MidPoint_Line};

void Set_Point(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    if (cnt % 2 == 0)
        GetCursorPos(&A);
    else
    {
        GetCursorPos(&B);
        // MidPoint_Line(hdc, A, B,RGB(22,255,163));
        funArr[state](hdc,A,B,RGB(0,0,0));
        InvalidateRect(hWnd, nullptr, 1); // 发送WM_PAINT消息
    }
    cnt++;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        //
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        Draw_Line(hdc,RGB(0,0,0)); // 黑色
        Draw_Title(hWnd, hdc);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        POINT T;
        GetCursorPos(&T);
        if(PtInRect(&btnRect1,T))
        {
            state=1;    // Bresenham
            cnt+=(cnt&1);
            MessageBox(hWnd,(LPSTR)"Bresenham Line!",(LPSTR)"Note",MB_OK);
        }
        else if(PtInRect(&btnRect2,T))
        {
            state=2;    // MidPoint
            cnt+=(cnt&1);
            MessageBox(hWnd,(LPSTR)"MidPoint Line!",(LPSTR)"Note",MB_OK);
        }
        else
            Set_Point(hWnd);
        break;
    }
    case WM_MOUSEMOVE:
        //
        break;
    case WM_COMMAND:
        //
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
