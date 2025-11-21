#include <windows.h>
#include <windowsx.h> // 用于 GET_X_LPARAM / GET_Y_LPARAM
#include <bits/stdc++.h>
#include <commdlg.h>
#include "Line.h"
#include "Circle.h"
#include "Rect.h"
#include "Curve.h"
#include "Poly.h"
#include "Triangle.h"
#include "Parallelogram.h"
#include "Rhombus.h"
#include "Select_point.h"
#include "Select_circle.h"
#include "Select_delete.h"
#include "Select_save.h"
#include "Select_open.h"
#include "Select_center.h"
#include "Select_perp.h"
#include "Select_tangent.h"
#include "Bezier.h"
#include "Show.h"
#include "Fill.h"
#include "Select_linestyle.h"
#include "Select_linewidth.h"
// #define DEBUG
#include "debug.h"
using namespace std;
typedef pair<int,int> pii;
const int N = 4000;
const double eps = 1e-6;
int state=0;

vector<POINT> arr;
vector<Line> lines;
vector<Circle> circles;
vector<Rect> rects;
vector<Curve> curves;
vector<Poly> polylines;
vector<Triangle> triangles;
vector<Parallelogram> parallelograms;
vector<Rhombus> rhombuses;
vector<Bezier> beziers;
vector<POINT> Cross_points; // 交点
std::wstring overlay_text = L"";
POINT overlay_pt = {0,0};
bool overlay_show = false;
int selected_type = -1;
int selected_index = -1;



int perp_selected_line = -1;

bool isDragging = false;
bool possibleToggle = false; 
POINT mouse_down_pt = {0,0};
int drag_threshold = 4;
enum TransformMode { TM_NONE=0, TM_MOVE=1, TM_ROTATE=2 };
TransformMode transformMode = TM_NONE;

Line origLine;
Circle origCircle;
Rect origRect;
Curve origCurve;
Poly origPoly;
Rhombus origRhombus;
// 旋转中心点和初始角度
POINT rotate_center = {0,0};
double rotate_init_angle = 0.0;

// 保存/加载辅助函数(使用文件对话框)
static void SaveShapes(HWND hWnd)
{
    wchar_t szFile[MAX_PATH] = L"shapes.txt";
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if(!GetSaveFileNameW(&ofn)) return; // 用户取消

    int needed = WideCharToMultiByte(CP_ACP, 0, szFile, -1, NULL, 0, NULL, NULL);
    std::string path(needed, '\0');
    WideCharToMultiByte(CP_ACP, 0, szFile, -1, &path[0], needed, NULL, NULL);
    if(!path.empty() && path.back()=='\0') path.pop_back();

    std::ofstream ofs(path);
    if(!ofs){ MessageBox(hWnd, "无法创建文件", "Error", MB_OK); return; }
    for(auto &l: lines){ ofs << "LINE " << l.s.x << ' ' << l.s.y << ' ' << l.e.x << ' ' << l.e.y << ' ' << l.dashed << '\n'; }
    for(auto &c: circles){ ofs << "CIRCLE " << c.O.x << ' ' << c.O.y << ' ' << c.r << ' ' << c.dashed << '\n'; }
    for(auto &r: rects){ ofs << "RECT " << r.left << ' ' << r.right << ' ' << r.bottom << ' ' << r.top << '\n'; }
    for(auto &c: curves){ ofs << "CURVE " << c.A.x << ' ' << c.A.y << ' ' << c.B.x << ' ' << c.B.y << ' ' << c.C.x << ' ' << c.C.y << '\n'; }
    for(auto &t: triangles){ ofs << "TRIANGLE " << t.A.x << ' ' << t.A.y << ' ' << t.B.x << ' ' << t.B.y << ' ' << t.C.x << ' ' << t.C.y << '\n'; }
    for(auto &p: polylines){ ofs << "POLY " << p.p.size(); for(auto &pt: p.p) ofs << ' ' << pt.x << ' ' << pt.y; ofs << '\n'; }
    for(auto &q: parallelograms){ ofs << "PARA " << q.A.x << ' ' << q.A.y << ' ' << q.B.x << ' ' << q.B.y << ' ' << q.C.x << ' ' << q.C.y << ' ' << q.D.x << ' ' << q.D.y << '\n'; }
    for(auto &rh: rhombuses){ ofs << "RHOMBUS " << rh.A.x << ' ' << rh.A.y << ' ' << rh.B.x << ' ' << rh.B.y << ' ' << rh.C.x << ' ' << rh.C.y << ' ' << rh.D.x << ' ' << rh.D.y << '\n'; }
    for(auto &bz: beziers){ ofs << "BEZIER " << bz.A.x << ' ' << bz.A.y << ' ' << bz.B.x << ' ' << bz.B.y << ' ' << bz.C.x << ' ' << bz.C.y << '\n'; }
    ofs.close();
}

static void LoadShapes(HWND hWnd)
{
    wchar_t szFile[MAX_PATH] = L"shapes.txt";
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if(!GetOpenFileNameW(&ofn)) return; // 用户取消

    int needed = WideCharToMultiByte(CP_ACP, 0, szFile, -1, NULL, 0, NULL, NULL);
    std::string path(needed, '\0');
    WideCharToMultiByte(CP_ACP, 0, szFile, -1, &path[0], needed, NULL, NULL);
    if(!path.empty() && path.back()=='\0') path.pop_back();

    std::ifstream ifs(path);
    if(!ifs){ MessageBox(hWnd, "无法打开所选文件", "Error", MB_OK); return; }
    lines.clear(); circles.clear(); rects.clear(); curves.clear(); polylines.clear(); Cross_points.clear();
    std::string cmd;
    std::string line_str;
    while(std::getline(ifs, line_str)){
        std::stringstream ss(line_str);
        ss >> cmd;
        if(cmd=="LINE"){ int x1,y1,x2,y2; bool dashed=false; ss>>x1>>y1>>x2>>y2>>dashed; Line l; l.s={x1,y1}; l.e={x2,y2}; l.left=min(x1,x2); l.right=max(x1,x2); l.button=min(y1,y2); l.top=max(y1,y2); l.dashed=dashed; lines.push_back(l); }
        else if(cmd=="CIRCLE"){ int ox,oy,r; bool dashed=false; ss>>ox>>oy>>r>>dashed; circles.emplace_back(POINT{ox,oy}, r); circles.back().dashed = dashed; }
        else if(cmd=="RECT"){ int l,r,b,t; ss>>l>>r>>b>>t; Rect R; R.left=l; R.right=r; R.bottom=b; R.top=t; R.A={l,b}; R.B={r,t}; rects.push_back(R); }
    else if(cmd=="CURVE"){ int x1,y1,x2,y2,x3,y3; ss>>x1>>y1>>x2>>y2>>x3>>y3; curves.emplace_back(POINT{x1,y1}, POINT{x2,y2}, POINT{x3,y3}); }
    else if(cmd=="TRIANGLE"){ int x1,y1,x2,y2,x3,y3; ss>>x1>>y1>>x2>>y2>>x3>>y3; triangles.emplace_back(POINT{x1,y1}, POINT{x2,y2}, POINT{x3,y3}); }
        else if(cmd=="POLY"){ size_t n; ss>>n; Poly P; for(size_t i=0;i<n;i++){ int x,y; ss>>x>>y; P.p.push_back({x,y}); } polylines.push_back(P); }
    else if(cmd=="PARA"){ int x1,y1,x2,y2,x3,y3,x4,y4; ss>>x1>>y1>>x2>>y2>>x3>>y3>>x4>>y4; parallelograms.emplace_back(POINT{x1,y1}, POINT{x2,y2}, POINT{x3,y3}, POINT{x4,y4}); }
    else if(cmd=="RHOMBUS"){ int x1,y1,x2,y2,x3,y3,x4,y4; ss>>x1>>y1>>x2>>y2>>x3>>y3>>x4>>y4; rhombuses.emplace_back(POINT{x1,y1}, POINT{x2,y2}, POINT{x3,y3}, POINT{x4,y4}); }
    else if(cmd=="BEZIER"){ int x1,y1,x2,y2,x3,y3; ss>>x1>>y1>>x2>>y2>>x3>>y3; beziers.emplace_back(POINT{x1,y1}, POINT{x2,y2}, POINT{x3,y3}); }
        else{ continue; }
    }
    ifs.close();
    selected_type=-1; selected_index=-1;
    InvalidateRect(hWnd, NULL, TRUE);
}
// 求交：求直线和直线，直线和圆，圆和圆之间的交点，并给出交点的坐标。
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow = SW_SHOWNORMAL)
{
    WNDCLASSW wc = {0};//填充窗口类
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;//窗口实例为当前应用实例句柄
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = L"SimplePaint";

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(L"SimplePaint", L"WFY simple paint", WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, 900, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
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
        
        Draw_Title(hWnd, hdc);
        Show_graphics(hWnd,hdc);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        // 使用 lParam 获取客户端坐标，避免 GetCursorPos/手动偏移导致坐标不一致
        POINT bt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        mouse_down_pt = bt;
        isDragging = false;
        possibleToggle = true;
        auto note=[&](int s, string str)
        {
            state=s;
            arr.clear();
            MessageBox(hWnd,LPSTR(str.c_str()),LPSTR("Note"),MB_OK);
        };
        arr.push_back(bt);
//菜单栏
        if(PtInRect(&Line::rect,bt))
            note(0,"Line");
        else if(PtInRect(&Circle::rect,bt))
            note(1,"Circle");
        else if(PtInRect(&Rect::rect,bt))
            note(2,"Rect");    //矩形
        else if(PtInRect(&Curve::rect,bt))
            note(3,"Curve");    //曲线
        else if(PtInRect(&Poly::rect,bt))
            note(4,"Poly");     //折线
        else if(PtInRect(&Select_point::rect,bt))
            note(5,"Select_point");
        else if(PtInRect(&Triangle::rect, bt))
            note(8, "Triangle");
        else if(PtInRect(&Parallelogram::rect, bt))
            note(9, "Parallelogram");
        else if(PtInRect(&Rhombus::rect, bt))
            note(11, "Rhombus");
        else if(PtInRect(&Select_center::rect, bt))
            note(6, "Select_center");
        else if(PtInRect(&Select_perp::rect, bt))
            note(7, "Select_perp");
    else if(PtInRect(&Select_delete::rect, bt))
        {
            if(selected_type!=-1 && selected_index>=0)
            {
                switch(selected_type)
                {
                    case 0:
                        if(selected_index < (int)lines.size()) lines.erase(lines.begin()+selected_index);
                        break;
                    case 1:
                        if(selected_index < (int)circles.size()) circles.erase(circles.begin()+selected_index);
                        break;
                    case 2:
                        if(selected_index < (int)rects.size()) rects.erase(rects.begin()+selected_index);
                        break;
                    case 3:
                        if(selected_index < (int)curves.size()) curves.erase(curves.begin()+selected_index);
                        break;
                    case 4:
                        if(selected_index < (int)polylines.size()) polylines.erase(polylines.begin()+selected_index);
                        break;
                    case 11:
                        if(selected_index < (int)rhombuses.size()) rhombuses.erase(rhombuses.begin()+selected_index);
                        break;
                }
                // 清除选择并请求重绘
                selected_type = -1; selected_index = -1;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        else if(PtInRect(&Select_tangent::rect, bt))
            note(10, "Tangent");
        else if(PtInRect(&Line::midpoint_rect, bt))
            note(12, "Midpoint Line");
        else if(PtInRect(&Line::bresenham_rect, bt))
            note(13, "Bresenham Line");
        else if(PtInRect(&Circle::midpoint_rect, bt))
            note(14, "Midpoint Circle");
        else if(PtInRect(&Circle::bresenham_rect, bt))
            note(15, "Bresenham Circle");
        else if(PtInRect(&Bezier::rect, bt))
            note(16, "Bezier");
        else if(PtInRect(&Select_save::rect, bt))
        {
            SaveShapes(hWnd);
        }
        else if(PtInRect(&Select_open::rect, bt))
        {
            LoadShapes(hWnd);
        }
        else if(PtInRect(&scanline_button_rect, bt))
        {
            note(20, "Scanline Fill");
        }
        else if(PtInRect(&seed_button_rect, bt))
        {
            note(21, "Seed Fill");
        }
        else if(PtInRect(&linestyle_button_rect, bt))
        {
            note(22, "Modify Line Style");
        }
        else if(PtInRect(&linewidth_button_rect, bt))
        {
            note(23, "Modify Line Width");
        }
        else
        {
            if(state == 23) 
            {
                bool found = false;
                for (int i = (int)lines.size() - 1; i >= 0; --i) {
                    auto &l = lines[i];
                    RECT bbox = {l.left - 5, l.button - 5, l.right + 5, l.top + 5};
                    if (PtInRect(&bbox, bt)) {
                        double dx = l.e.x - l.s.x;
                        double dy = l.e.y - l.s.y;
                        double length = sqrt(dx * dx + dy * dy);
                        if (length < 1e-6) continue;

                        double nx = -dy / length;
                        double ny = dx / length;

                        const int half_width = 5;
                        POINT v1 = {(long)(l.s.x + half_width * nx), (long)(l.s.y + half_width * ny)};
                        POINT v2 = {(long)(l.e.x + half_width * nx), (long)(l.e.y + half_width * ny)};
                        POINT v3 = {(long)(l.e.x - half_width * nx), (long)(l.e.y - half_width * ny)};
                        POINT v4 = {(long)(l.s.x - half_width * nx), (long)(l.s.y - half_width * ny)};
                                                // 创建一个持久化的平行四边形来代表粗线
                        parallelograms.emplace_back(v1, v2, v3, v4);
                        HDC hdc = GetDC(hWnd);
                        std::vector<POINT> parallelogram_vertices = {v1, v2, v3, v4};
                        scanlineFill(hdc, parallelogram_vertices, RGB(192, 192, 192));
                        ReleaseDC(hWnd, hdc);
                      
                        // 删除原始直线
                        lines.erase(lines.begin() + i);
                        // lines.erase(lines.begin() + i); // Optionally remove the original line

                        found = true;
                        break;
                    }
                }

                if (found) {
                    state = -1; // Reset state
                    InvalidateRect(hWnd, NULL, TRUE);
                    break; // Consume the click
                }

                if(!found) {
                    for (int i = (int)circles.size() - 1; i >= 0; --i) {
                        auto &c = circles[i];
                        long dx = bt.x - c.O.x;
                        long dy = bt.y - c.O.y;
                        if (dx * dx + dy * dy <= (long)c.r * c.r) {
                            // Create a ring by adding two new circles
                            if (c.r > 5) {
                                circles.emplace_back(c.O, c.r - 5);
                            }
                            circles.emplace_back(c.O, c.r + 5);

                            // Remove the original circle
                            circles.erase(circles.begin() + i);

                            found = true;
                            break;
                        }
                    }
                }

                if (found) {
                    state = -1; // Reset state
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break; // Consume the click
            }
            if(state == 22) // Modify Line Style mode
            {
                bool found = false;
                // Iterate in reverse to select the top-most shape
                for(int i = (int)lines.size() - 1; i >= 0; --i)
                {
                    auto &l = lines[i];
                    // Use a slightly larger bounding box for easier selection
                    RECT bbox = {l.left - 5, l.button - 5, l.right + 5, l.top + 5};
                    if(PtInRect(&bbox, bt))
                    {
                        l.dashed = true;
                        found = true;
                        break; 
                    }
                }
                if(!found)
                {
                    for(int i = (int)circles.size() - 1; i >= 0; --i)
                    {
                        auto &c = circles[i];
                        long dx = bt.x - c.O.x;
                        long dy = bt.y - c.O.y;
                        if(dx * dx + dy * dy <= (long)c.r * (long)c.r)
                        {
                            c.dashed = true;
                            found = true;
                            break;
                        }
                    }
                }

                if(found)
                {
                    state = -1; // Reset state after modification
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break; // Consume the click
            }
            if(state==6){ // 显示圆心模式
                overlay_show = false;
                bool found=false;
                for(auto &c: circles){
                    long dx = bt.x - c.O.x;
                    long dy = bt.y - c.O.y;
                    if(dx*dx + dy*dy <= (long)c.r * (long)c.r){
                        // 在这个圆内
                        std::wstring w = L"(" + std::to_wstring(c.O.x) + L"," + std::to_wstring(c.O.y) + L")";
                        overlay_text = w;
                        overlay_pt = c.O;
                        overlay_show = true;
                        InvalidateRect(hWnd, NULL, TRUE);
                        found=true;
                        break;
                    }
                }
                if(!found){ overlay_text = L"不在圆中"; overlay_pt = bt; overlay_show = true; InvalidateRect(hWnd, NULL, TRUE); }
                break; // 消费掉此次点击
            }
            if(state==7){ 
                if(perp_selected_line==-1){
                    for(int i=(int)lines.size()-1;i>=0;--i){
                        auto &L=lines[i];
                        RECT bbox = {L.left-5, L.button-5, L.right+5, L.top+5};
                        if(PtInRect(&bbox, bt)) { perp_selected_line = i;  InvalidateRect(hWnd, NULL, TRUE); break; }
                    }
                    if(perp_selected_line==-1){ overlay_text = L"未选中直线，请重新点击直线"; overlay_pt = bt; overlay_show = true; InvalidateRect(hWnd, NULL, TRUE); }
                    break;
                } else {
                    auto &L = lines[perp_selected_line];
                    double dx = (double)(L.e.x - L.s.x);
                    double dy = (double)(L.e.y - L.s.y);
                    double pdx = -dy;
                    double pdy = dx;
                    double len = sqrt(pdx*pdx + pdy*pdy);
                    if(len < 1e-6) { overlay_text = L"无法生成垂线（原直线零长度）"; overlay_pt = bt; overlay_show = true; InvalidateRect(hWnd, NULL, TRUE); perp_selected_line=-1; break; }
                    pdx /= len; pdy /= len;
                    double K = 2000.0; // 足够长以穿过窗口
                    POINT A = { (int)round(bt.x - pdx*K), (int)round(bt.y - pdy*K) };
                    POINT B = { (int)round(bt.x + pdx*K), (int)round(bt.y + pdy*K) };
                    Line newL(A,B);
                    lines.push_back(newL);
                    overlay_text = L"已绘制垂线";
                    overlay_pt = bt;
                    overlay_show = true;
                    InvalidateRect(hWnd, NULL, TRUE);
                    // 重置状态
                    perp_selected_line = -1;
                    state = -1;
                    break;
                }
            }
            if(state==10){ 
                if(arr.size()==1){
                    overlay_text = L"已选择切点，请再点击选择一个圆";
                    overlay_pt = arr[0];
                    overlay_show = true;
                    InvalidateRect(hWnd, NULL, TRUE);
                } else if(arr.size()==2){
                    POINT P = arr[0];
                    POINT circle_click_pt = arr[1];
                    bool foundCircle=false;
                    for(int i=(int)circles.size()-1; i>=0; --i){
                        auto &C = circles[i];
                        long dx_hit = circle_click_pt.x - C.O.x;
                        long dy_hit = circle_click_pt.y - C.O.y;
                        if(dx_hit*dx_hit + dy_hit*dy_hit <= (long)C.r * (long)C.r){
                            double ox = (double)C.O.x, oy = (double)C.O.y;
                            double px = (double)P.x, py = (double)P.y;
                            double dx_dist = px - ox, dy_dist = py - oy;
                            double d_sq = dx_dist*dx_dist + dy_dist*dy_dist;
                            double r_sq = (double)C.r * (double)C.r;

                            if(d_sq <= r_sq + 1e-6){
                                overlay_text = L"点在圆内部或在圆上，无法生成切线";
                                overlay_pt = P; overlay_show = true; InvalidateRect(hWnd, NULL, TRUE);
                                arr.clear(); state = -1;
                                foundCircle = true;
                                break;
                            }
                            
                            double d = sqrt(d_sq);
                            double theta = atan2(dy_dist, dx_dist);
                            double alpha = acos((double)C.r / d);
                            double ang1 = theta + alpha;
                            double ang2 = theta - alpha;
                            
                            POINT T1 = { (int)round(ox + C.r * cos(ang1)), (int)round(oy + C.r * sin(ang1)) };
                            POINT T2 = { (int)round(ox + C.r * cos(ang2)), (int)round(oy + C.r * sin(ang2)) };
                            
                            lines.emplace_back(P, T1);
                            if(!(T1.x==T2.x && T1.y==T2.y)) lines.emplace_back(P, T2);
                            
                            overlay_text = L"已绘制切线"; overlay_pt = P; overlay_show = true;
                            InvalidateRect(hWnd, NULL, TRUE);
                            arr.clear(); state = -1; foundCircle=true; break;
                        }
                    }
                    if(!foundCircle){
                        overlay_text = L"未选中圆，请重新选择圆";
                        overlay_pt = circle_click_pt;
                        overlay_show = true;
                        InvalidateRect(hWnd, NULL, TRUE);
                        arr.pop_back();
                    }
                }
                break; // 消费掉此次点击
            }
            // 首先尝试命中测试图元(单击选择一个图元)
            bool found=false;
            int preSelType = selected_type;
            int preSelIndex = selected_index;
            // 按相反顺序搜索，以便选中最上面的(最后的)图元
            for(int i=(int)lines.size()-1;i>=0 && !found;--i){
                auto &L=lines[i];
                RECT bbox = {L.left-5, L.button-5, L.right+5, L.top+5};
                if(PtInRect(&bbox, bt)) { selected_type=0; selected_index=i; found=true; }
            }
            for(int i=(int)circles.size()-1;i>=0 && !found;--i){
                auto &C=circles[i];
                RECT bbox = {C.O.x - C.r -5, C.O.y - C.r -5, C.O.x + C.r +5, C.O.y + C.r +5};
                if(PtInRect(&bbox, bt)) { selected_type=1; selected_index=i; found=true; }
            }
            for(int i=(int)rects.size()-1;i>=0 && !found;--i){
                auto &R=rects[i];
                RECT bbox = {min(R.left,R.right)-5, min(R.bottom,R.top)-5, max(R.left,R.right)+5, max(R.bottom,R.top)+5};
                if(PtInRect(&bbox, bt)) { selected_type=2; selected_index=i; found=true; }
            }
            for(int i=(int)curves.size()-1;i>=0 && !found;--i){
                auto &C=curves[i];
                int l = min({C.A.x, C.B.x, C.C.x})-5;
                int r = max({C.A.x, C.B.x, C.C.x})+5;
                int b = min({C.A.y, C.B.y, C.C.y})-5;
                int t = max({C.A.y, C.B.y, C.C.y})+5;
                RECT bbox = {l,b,r,t};
                if(PtInRect(&bbox, bt)) { selected_type=3; selected_index=i; found=true; }
            }
            for(int i=(int)polylines.size()-1;i>=0 && !found;--i){
                auto &P=polylines[i];
                if(P.p.empty()) continue;
                LONG lx = P.p[0].x, rx = P.p[0].x, by = P.p[0].y, ty = P.p[0].y;
                for(auto &pt:P.p){ lx = min<long>(lx, pt.x); rx = max<long>(rx, pt.x); by = min<long>(by, pt.y); ty = max<long>(ty, pt.y); }
                RECT bbox = {lx-5,by-5,rx+5,ty+5};
                if(PtInRect(&bbox, bt)) { selected_type=4; selected_index=i; found=true; }
            }
            //种子填充法
            for(int i=(int)triangles.size()-1;i>=0 && !found;--i){
                auto &T = triangles[i];
                int lx = min({T.A.x, T.B.x, T.C.x}) - 5;
                int rx = max({T.A.x, T.B.x, T.C.x}) + 5;
                int by = min({T.A.y, T.B.y, T.C.y}) - 5;
                int ty = max({T.A.y, T.B.y, T.C.y}) + 5;
                RECT bbox = {lx, by, rx, ty};
                if(PtInRect(&bbox, bt)) { selected_type=8; selected_index=i; found=true; }
            }
            for(int i=(int)parallelograms.size()-1;i>=0 && !found;--i){
                auto &P = parallelograms[i];
                int lx = min({P.A.x, P.B.x, P.C.x, P.D.x}) - 5;
                int rx = max({P.A.x, P.B.x, P.C.x, P.D.x}) + 5;
                int by = min({P.A.y, P.B.y, P.C.y, P.D.y}) - 5;
                int ty = max({P.A.y, P.B.y, P.C.y, P.D.y}) + 5;
                RECT bbox = {lx, by, rx, ty};
                if(PtInRect(&bbox, bt)) { selected_type=9; selected_index=i; found=true; }
            }
            for(int i=(int)rhombuses.size()-1;i>=0 && !found;--i){
                auto &rh = rhombuses[i];
                int lx = min({rh.A.x, rh.B.x, rh.C.x, rh.D.x}) - 5;
                int rx = max({rh.A.x, rh.B.x, rh.C.x, rh.D.x}) + 5;
                int by = min({rh.A.y, rh.B.y, rh.C.y, rh.D.y}) - 5;
                int ty = max({rh.A.y, rh.B.y, rh.C.y, rh.D.y}) + 5;
                RECT bbox = {lx, by, rx, ty};
                if(PtInRect(&bbox, bt)) { selected_type=11; selected_index=i; found=true; }
            }
            
            if(found) {
                if (state == 20 || state == 21) { // Fill modes
                    HDC hdc = GetDC(hWnd);
                    fillShape(hdc, selected_type, selected_index, state == 20, bt);
                    ReleaseDC(hWnd, hdc);
                    state = -1; // Reset state after filling
                    selected_type = -1; // Deselect
                    selected_index = -1;
                } else {
                    // 如果选择更改为不同的图元，请复制原始图元，以便立即拖动
                    if(!(selected_type==preSelType && selected_index==preSelIndex)){
                        // 新选择，而不是单击相同的图元
                        possibleToggle = false;
                        switch(selected_type){
                            case 0: if(selected_index < (int)lines.size()) origLine = lines[selected_index]; break;
                            case 1: if(selected_index < (int)circles.size()) origCircle = circles[selected_index]; break;
                            case 2: if(selected_index < (int)rects.size()) origRect = rects[selected_index]; break;
                            case 3: if(selected_index < (int)curves.size()) origCurve = curves[selected_index]; break;
                            case 4: if(selected_index < (int)polylines.size()) origPoly = polylines[selected_index]; break;
                            case 11: if(selected_index < (int)rhombuses.size()) origRhombus = rhombuses[selected_index]; break;
                        }
                    } else {
                        // 单击了相同的选定图元：如果未拖动，则允许在鼠标弹起时切换
                        possibleToggle = true;
                    }
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break; // 选择或填充完成，跳过下面的绘图创建
            }

            if(arr.size()==2 && state==0)
                Draw_line(hWnd,arr);
            if(arr.size()==2 && state==1)
                Draw_circle(hWnd,arr);
            if(arr.size()==2 && state==2)
                Draw_rect(hWnd,arr);
            if(arr.size()==3 && state==3)
                Draw_curve(hWnd,arr);
            if(arr.size()==3 && state==4)
            {
                Poly p;
                p.p = arr;
                polylines.push_back(p);
                arr.clear();
                InvalidateRect(hWnd, NULL, TRUE);
            }
            if(arr.size()==3 && state==8)
                Draw_triangle(hWnd,arr);
            if(arr.size()==3 && state==9)
                Draw_parallelogram(hWnd,arr);
            if(arr.size()==2 && state==11)
                Draw_rhombus(hWnd,arr);
            if(arr.size()==2 && state==12)
                Draw_midpoint_line(hWnd,arr);
            if(arr.size()==2 && state==13)
                Draw_bresenham_line(hWnd,arr);
            if(arr.size()==2 && state==14)
                Draw_midpoint_circle(hWnd,arr);
            if(arr.size()==2 && state==15)
                Draw_bresenham_circle(hWnd,arr);
            if(arr.size()==3 && state==16)
                Draw_bezier(hWnd,arr);
            if(state==5)   // 在交点附近
                Show_near_by_point(hWnd,bt);

            // string str="Cross points:\n";
            // for(auto P:Cross_points)
            //     str+=to_string(P)+"\n";
            // debug(str);
        }
        break;
    }
    case WM_MOUSEMOVE:
        {
            if(wParam & MK_LBUTTON){
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                int dx = pt.x - mouse_down_pt.x;
                int dy = pt.y - mouse_down_pt.y;
                if(!isDragging && possibleToggle){
                    if(abs(dx) > drag_threshold || abs(dy) > drag_threshold){
                        // 开始拖动
                        isDragging = true; possibleToggle = false; transformMode = TM_MOVE;
                    }
                }
                // 在右键或旋转模式下处理旋转
                if(transformMode==TM_ROTATE && isDragging){
                    // 计算当前角度并从原始角度旋转增量
                    POINT cur = pt;
                    double curAngle = atan2((double)(cur.y - rotate_center.y), (double)(cur.x - rotate_center.x));
                    double delta = curAngle - rotate_init_angle;
                    double cs = cos(delta), sn = sin(delta);
                    switch(selected_type){
                        case 0: if(selected_index < (int)lines.size()){
                            Line &L = lines[selected_index];
                            // 从原始直线旋转端点
                            int x1 = origLine.s.x - rotate_center.x, y1 = origLine.s.y - rotate_center.y;
                            int x2 = origLine.e.x - rotate_center.x, y2 = origLine.e.y - rotate_center.y;
                            L.s.x = rotate_center.x + (int)round(x1*cs - y1*sn);
                            L.s.y = rotate_center.y + (int)round(x1*sn + y1*cs);
                            L.e.x = rotate_center.x + (int)round(x2*cs - y2*sn);
                            L.e.y = rotate_center.y + (int)round(x2*sn + y2*cs);
                            L.left = min(L.s.x,L.e.x); L.right = max(L.s.x,L.e.x); L.button = min(L.s.y,L.e.y); L.top = max(L.s.y,L.e.y);
                        } break;
                        case 1: if(selected_index < (int)circles.size()){
                            Circle &C = circles[selected_index];
                            int ox = origCircle.O.x - rotate_center.x, oy = origCircle.O.y - rotate_center.y;
                            C.O.x = rotate_center.x + (int)round(ox*cs - oy*sn);
                            C.O.y = rotate_center.y + (int)round(ox*sn + oy*cs);
                        } break;
                        case 2: if(selected_index < (int)rects.size()){
                            Rect &R = rects[selected_index];
                            int ax = origRect.A.x - rotate_center.x, ay = origRect.A.y - rotate_center.y;
                            int bx = origRect.B.x - rotate_center.x, by = origRect.B.y - rotate_center.y;
                            R.A.x = rotate_center.x + (int)round(ax*cs - ay*sn);
                            R.A.y = rotate_center.y + (int)round(ax*sn + ay*cs);
                            R.B.x = rotate_center.x + (int)round(bx*cs - by*sn);
                            R.B.y = rotate_center.y + (int)round(bx*sn + by*cs);
                            R.left = min(R.A.x, R.B.x); R.right = max(R.A.x, R.B.x); R.bottom = min(R.A.y, R.B.y); R.top = max(R.A.y, R.B.y);
                        } break;
                        case 3: if(selected_index < (int)curves.size()){
                            Curve &C = curves[selected_index];
                            Point OA = {origCurve.A.x - rotate_center.x, origCurve.A.y - rotate_center.y};
                            Point OB = {origCurve.B.x - rotate_center.x, origCurve.B.y - rotate_center.y};
                            Point OC = {origCurve.C.x - rotate_center.x, origCurve.C.y - rotate_center.y};
                            C.A.x = rotate_center.x + (int)round(OA.x*cs - OA.y*sn); C.A.y = rotate_center.y + (int)round(OA.x*sn + OA.y*cs);
                            C.B.x = rotate_center.x + (int)round(OB.x*cs - OB.y*sn); C.B.y = rotate_center.y + (int)round(OB.x*sn + OB.y*cs);
                            C.C.x = rotate_center.x + (int)round(OC.x*cs - OC.y*sn); C.C.y = rotate_center.y + (int)round(OC.x*sn + OC.y*cs);
                        } break;
                        case 4: if(selected_index < (int)polylines.size()){
                            Poly &P = polylines[selected_index];
                            P.p.resize(origPoly.p.size());
                            for(size_t ii=0; ii<origPoly.p.size(); ++ii){
                                int ox = origPoly.p[ii].x - rotate_center.x;
                                int oy = origPoly.p[ii].y - rotate_center.y;
                                P.p[ii].x = rotate_center.x + (int)round(ox*cs - oy*sn);
                                P.p[ii].y = rotate_center.y + (int)round(ox*sn + oy*cs);
                            }
                        } break;
                        case 11: if(selected_index < (int)rhombuses.size()){
                            Rhombus &rh = rhombuses[selected_index];
                            Point OA = {origRhombus.A.x - rotate_center.x, origRhombus.A.y - rotate_center.y};
                            Point OB = {origRhombus.B.x - rotate_center.x, origRhombus.B.y - rotate_center.y};
                            Point OC = {origRhombus.C.x - rotate_center.x, origRhombus.C.y - rotate_center.y};
                            Point OD = {origRhombus.D.x - rotate_center.x, origRhombus.D.y - rotate_center.y};
                            rh.A.x = rotate_center.x + (int)round(OA.x*cs - OA.y*sn); rh.A.y = rotate_center.y + (int)round(OA.x*sn + OA.y*cs);
                            rh.B.x = rotate_center.x + (int)round(OB.x*cs - OB.y*sn); rh.B.y = rotate_center.y + (int)round(OB.x*sn + OB.y*cs);
                            rh.C.x = rotate_center.x + (int)round(OC.x*cs - OC.y*sn); rh.C.y = rotate_center.y + (int)round(OC.x*sn + OC.y*cs);
                            rh.D.x = rotate_center.x + (int)round(OD.x*cs - OD.y*sn); rh.D.y = rotate_center.y + (int)round(OD.x*sn + OD.y*cs);
                        } break;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                    return 0;
                }
                if(isDragging && transformMode==TM_MOVE && selected_type!=-1 && selected_index>=0){
                    // 从原始位置应用平移
                    switch(selected_type){
                        case 0: // 直线
                            if(selected_index < (int)lines.size()){
                                lines[selected_index].s.x = origLine.s.x + dx;
                                lines[selected_index].s.y = origLine.s.y + dy;
                                lines[selected_index].e.x = origLine.e.x + dx;
                                lines[selected_index].e.y = origLine.e.y + dy;
                                lines[selected_index].left = min(lines[selected_index].s.x, lines[selected_index].e.x);
                                lines[selected_index].right = max(lines[selected_index].s.x, lines[selected_index].e.x);
                                lines[selected_index].button = min(lines[selected_index].s.y, lines[selected_index].e.y);
                                lines[selected_index].top = max(lines[selected_index].s.y, lines[selected_index].e.y);
                            }
                            break;
                        case 1: // 圆
                            if(selected_index < (int)circles.size()){
                                circles[selected_index].O.x = origCircle.O.x + dx;
                                circles[selected_index].O.y = origCircle.O.y + dy;
                            }
                            break;
                        case 2: // 矩形
                            if(selected_index < (int)rects.size()){
                                rects[selected_index].A.x = origRect.A.x + dx;
                                rects[selected_index].A.y = origRect.A.y + dy;
                                rects[selected_index].B.x = origRect.B.x + dx;
                                rects[selected_index].B.y = origRect.B.y + dy;
                                rects[selected_index].left = rects[selected_index].A.x; rects[selected_index].right = rects[selected_index].B.x;
                                rects[selected_index].bottom = rects[selected_index].A.y; rects[selected_index].top = rects[selected_index].B.y;
                            }
                            break;
                        case 3: // 曲线
                            if(selected_index < (int)curves.size()){
                                curves[selected_index].A.x = origCurve.A.x + dx; curves[selected_index].A.y = origCurve.A.y + dy;
                                curves[selected_index].B.x = origCurve.B.x + dx; curves[selected_index].B.y = origCurve.B.y + dy;
                                curves[selected_index].C.x = origCurve.C.x + dx; curves[selected_index].C.y = origCurve.C.y + dy;
                            }
                            break;
                        case 4: // 折线
                            if(selected_index < (int)polylines.size()){
                                for(size_t ii=0; ii<origPoly.p.size(); ++ii){
                                    polylines[selected_index].p[ii].x = origPoly.p[ii].x + dx;
                                    polylines[selected_index].p[ii].y = origPoly.p[ii].y + dy;
                                }
                            }
                            break;
                        case 11: // 菱形
                            if(selected_index < (int)rhombuses.size()){
                                rhombuses[selected_index].A.x = origRhombus.A.x + dx;
                                rhombuses[selected_index].A.y = origRhombus.A.y + dy;
                                rhombuses[selected_index].B.x = origRhombus.B.x + dx;
                                rhombuses[selected_index].B.y = origRhombus.B.y + dy;
                                rhombuses[selected_index].C.x = origRhombus.C.x + dx;
                                rhombuses[selected_index].C.y = origRhombus.C.y + dy;
                                rhombuses[selected_index].D.x = origRhombus.D.x + dx;
                                rhombuses[selected_index].D.y = origRhombus.D.y + dy;
                            }
                            break;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
        }
        break;
    case WM_LBUTTONUP:
        {
            // 如果 possibleToggle 标志仍然设置并且没有发生拖动，则关闭选择
            if(possibleToggle && !isDragging){
                // 在没有拖动的情况下单击了鼠标：如果是同一个图元，则关闭选择
                selected_type = -1; selected_index = -1; possibleToggle = false; InvalidateRect(hWnd, NULL, TRUE);
            }
            // 结束拖动
            isDragging = false; transformMode = TM_NONE; possibleToggle = false;
        }
        break;
    case WM_RBUTTONDOWN:
        {
            // 如果已选择，则开始旋转
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if(selected_type!=-1 && selected_index>=0){
                // 设置旋转：存储原始几何图形并将轴心设置在图元中心
                switch(selected_type){
                    case 0: origLine = lines[selected_index]; break;
                    case 1: origCircle = circles[selected_index]; break;
                    case 2: origRect = rects[selected_index]; break;
                    case 3: origCurve = curves[selected_index]; break;
                    case 4: origPoly = polylines[selected_index]; break;
                    case 11: origRhombus = rhombuses[selected_index]; break;
                }
                // 确定旋转的轴心(中心)
                POINT center = {0,0};
                switch(selected_type){
                    case 0: center.x = (lines[selected_index].s.x + lines[selected_index].e.x)/2; center.y = (lines[selected_index].s.y + lines[selected_index].e.y)/2; break;
                    case 1: center = circles[selected_index].O; break;
                    case 2: center.x = (rects[selected_index].A.x + rects[selected_index].B.x)/2; center.y = (rects[selected_index].A.y + rects[selected_index].B.y)/2; break;
                    case 3: center.x = (curves[selected_index].A.x + curves[selected_index].B.x + curves[selected_index].C.x)/3; center.y = (curves[selected_index].A.y + curves[selected_index].B.y + curves[selected_index].C.y)/3; break;
                    case 4: {
                        long sx=0, sy=0; for(auto &p:polylines[selected_index].p){ sx+=p.x; sy+=p.y; } size_t n=polylines[selected_index].p.size(); center.x = sx/n; center.y = sy/n; break; }
                    case 11: {
                        long sx=0, sy=0; 
                        sx = rhombuses[selected_index].A.x + rhombuses[selected_index].B.x + rhombuses[selected_index].C.x + rhombuses[selected_index].D.x;
                        sy = rhombuses[selected_index].A.y + rhombuses[selected_index].B.y + rhombuses[selected_index].C.y + rhombuses[selected_index].D.y;
                        center.x = sx/4; center.y = sy/4; break; }
                }
                rotate_center = center;
                rotate_init_angle = atan2((double)(pt.y - center.y), (double)(pt.x - center.x));
                transformMode = TM_ROTATE;
                mouse_down_pt = pt;
                isDragging = true;
            }
        }
        break;
    case WM_RBUTTONUP:
        {
            transformMode = TM_NONE; isDragging = false;
        }
        break;
    case WM_MOUSEWHEEL:
        {
            if(selected_type!=-1 && selected_index>=0){
                short delta = GET_WHEEL_DELTA_WPARAM(wParam);
                double factor = (delta>0) ? 1.1 : 0.9;
                // 相对于图元中心进行缩放
                POINT center = {0,0};
                switch(selected_type){
                    case 0: center.x = (lines[selected_index].s.x + lines[selected_index].e.x)/2; center.y = (lines[selected_index].s.y + lines[selected_index].e.y)/2; break;
                    case 1: center = circles[selected_index].O; break;
                    case 2: center.x = (rects[selected_index].A.x + rects[selected_index].B.x)/2; center.y = (rects[selected_index].A.y + rects[selected_index].B.y)/2; break;
                    case 3: center.x = (curves[selected_index].A.x + curves[selected_index].B.x + curves[selected_index].C.x)/3; center.y = (curves[selected_index].A.y + curves[selected_index].B.y + curves[selected_index].C.y)/3; break;
                    case 4: {
                        long sx=0, sy=0; for(auto &p:polylines[selected_index].p){ sx+=p.x; sy+=p.y; } size_t n=polylines[selected_index].p.size(); center.x = sx/n; center.y = sy/n; break; }
                    case 11: {
                        long sx=0, sy=0; 
                        sx = rhombuses[selected_index].A.x + rhombuses[selected_index].B.x + rhombuses[selected_index].C.x + rhombuses[selected_index].D.x;
                        sy = rhombuses[selected_index].A.y + rhombuses[selected_index].B.y + rhombuses[selected_index].C.y + rhombuses[selected_index].D.y;
                        center.x = sx/4; center.y = sy/4; break; }
                }
                // 从中心应用缩放
                switch(selected_type){
                    case 0: {
                        auto &L = lines[selected_index];
                        L.s.x = center.x + (int)round((L.s.x - center.x)*factor);
                        L.s.y = center.y + (int)round((L.s.y - center.y)*factor);
                        L.e.x = center.x + (int)round((L.e.x - center.x)*factor);
                        L.e.y = center.y + (int)round((L.e.y - center.y)*factor);
                        L.left=min(L.s.x,L.e.x); L.right=max(L.s.x,L.e.x); L.button=min(L.s.y,L.e.y); L.top=max(L.s.y,L.e.y);
                    } break;
                    case 1: {
                        auto &C = circles[selected_index];
                        C.r = max(1, (int)round(C.r * factor));
                    } break;
                    case 2: {
                        auto &R = rects[selected_index];
                        R.A.x = center.x + (int)round((R.A.x - center.x)*factor);
                        R.A.y = center.y + (int)round((R.A.y - center.y)*factor);
                        R.B.x = center.x + (int)round((R.B.x - center.x)*factor);
                        R.B.y = center.y + (int)round((R.B.y - center.y)*factor);
                        R.left = min(R.A.x, R.B.x); R.right = max(R.A.x, R.B.x); R.bottom = min(R.A.y, R.B.y); R.top = max(R.A.y, R.B.y);
                    } break;
                    case 3: {
                        auto &C = curves[selected_index];
                        C.A.x = center.x + (int)round((C.A.x - center.x)*factor); C.A.y = center.y + (int)round((C.A.y - center.y)*factor);
                        C.B.x = center.x + (int)round((C.B.x - center.x)*factor); C.B.y = center.y + (int)round((C.B.y - center.y)*factor);
                        C.C.x = center.x + (int)round((C.C.x - center.x)*factor); C.C.y = center.y + (int)round((C.C.y - center.y)*factor);
                    } break;
                    case 4: {
                        auto &P = polylines[selected_index];
                        for(auto &pt:P.p){ pt.x = center.x + (int)round((pt.x - center.x)*factor); pt.y = center.y + (int)round((pt.y - center.y)*factor); }
                    } break;
                    case 11: {
                        auto &rh = rhombuses[selected_index];
                        rh.A.x = center.x + (int)round((rh.A.x - center.x)*factor);
                        rh.A.y = center.y + (int)round((rh.A.y - center.y)*factor);
                        rh.B.x = center.x + (int)round((rh.B.x - center.x)*factor);
                        rh.B.y = center.y + (int)round((rh.B.y - center.y)*factor);
                        rh.C.x = center.x + (int)round((rh.C.x - center.x)*factor);
                        rh.C.y = center.y + (int)round((rh.C.y - center.y)*factor);
                        rh.D.x = center.x + (int)round((rh.D.x - center.x)*factor);
                        rh.D.y = center.y + (int)round((rh.D.y - center.y)*factor);
                    } break;
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
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
