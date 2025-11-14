#include "Select_point.h"
#define DEBUG
extern vector<POINT> Cross_points;
RECT Select_point::rect={480,0,540,30};
COLORREF Select_point::color=RGB(245,85,211);

// 选取目标点附近的交点(有多个就取最近的)
POINT Near_by_cross_points(POINT P)
{
    auto [x0,y0]=P;
    double mn=1e9;
    POINT Q;
    for(auto [x,y]:Cross_points)
    {
        double dis=pow(x0-x,2) + pow(y0-y,2);
        if(dis<mn)
        {
            mn=dis;//mn为距离选中点（x0,y0)最近
            //交点，该交点与(x0,y0)之间距离
            Q={x,y};
        }
    }
    if(mn<=10000)
        return Q;
    return {-1,-1};     // 附近没有交点
}

// 在窗口中显示该交点坐标
void Show_near_by_point(HWND hWnd, POINT bt)
{
    POINT P=Near_by_cross_points(bt);
    if(P.x<0)
        return;
    std::wstring wstr = L"(" + std::to_wstring(P.x) + L"," + std::to_wstring(P.y) + L")";
    // 使用 overlay 机制，在 WM_PAINT 中统一绘制，避免被后续重绘擦除
    overlay_text = wstr;
    overlay_pt = P;
    overlay_show = true;
    // 请求重绘以显示 overlay
    InvalidateRect(hWnd, NULL, TRUE);
    // 仍输出窄字符串到 debug 控制台
    string dbg = "(" + to_string(P.x) + "," + to_string(P.y) + ")\n";
    debug(dbg);
}