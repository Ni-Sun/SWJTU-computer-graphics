#include "Bezier.h"

extern vector<Bezier> beziers;

RECT Bezier::rect = {400, 530, 500, 560};
COLORREF Bezier::color = RGB(0, 0, 255);

void Draw_bezier(HWND hWnd, vector<POINT> &arr)
{
    if (arr.size() < 2) { 
        return;
    }
    beziers.emplace_back(arr);
    arr.clear();
    InvalidateRect(hWnd, NULL, TRUE);
}
