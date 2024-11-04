#include <windows.h>
#include <vector>
#include <utility>
using std::vector;
using std::pair;
typedef pair<double, double> pdd;

double F03(double t)
{
	return 1.0/6*(-t*t*t+3*t*t-3*t+1);
}
double F13(double t)
{
	return 1.0/6*(3*t*t*t-6*t*t+4);
}
double F23(double t)
{
	return 1.0/6*(-3*t*t*t+3*t*t+3*t+1);
}
double F33(double t)
{
	return 1.0/6*t*t*t;
}


// void ThreeOrderBSplineSmooth(vector<pdd> &pt)
// {
//     int Num=pt.size();
//     vector<pdd> temp(Num+2);
// 	for(int i=0;i<Num;i++)
// 		temp[i+1]=pt[i];
 
// 	temp[0].first=2*temp[1].first-temp[2].first;                  //  将折线延长线上两点加入作为首点和尾点
// 	temp[0].second=2*temp[1].second-temp[2].second;
 
// 	temp[Num+1].first=2*temp[Num].first-temp[Num-1].first;
// 	temp[Num+1].second=2*temp[Num].second-temp[Num-1].second;
 
// 	pdd NodePt1,NodePt2,NodePt3,NodePt4;
// 	double t;
// 	for(int i=0;i<Num-1;i++)
// 	{
// 		NodePt1=temp[i]; NodePt2=temp[i+1]; NodePt3=temp[i+2]; NodePt4=temp[i+3];
 
// 		if(i==Num-4)                          //  最后一段取t=0.5和t=1点
// 		{
// 			t=0;
// 			pt[i].first=F03(t)*NodePt1.first+F13(t)*NodePt2.first+F23(t)*NodePt3.first+F33(t)*NodePt4.first;
// 			pt[i].second=F03(t)*NodePt1.second+F13(t)*NodePt2.second+F23(t)*NodePt3.second+F33(t)*NodePt4.second;
// 			t=1;
// 			pt[i+1].first=F03(t)*NodePt1.first+F13(t)*NodePt2.first+F23(t)*NodePt3.first+F33(t)*NodePt4.first;
// 			pt[i+1].second=F03(t)*NodePt1.second+F13(t)*NodePt2.second+F23(t)*NodePt3.second+F33(t)*NodePt4.second;
// 		}
//         else                                      //  中间段取t=0.5点
// 		{
// 			t=0;
// 			pt[i].first=F03(t)*NodePt1.first+F13(t)*NodePt2.first+F23(t)*NodePt3.first+F33(t)*NodePt4.first;
// 			pt[i].second=F03(t)*NodePt1.second+F13(t)*NodePt2.second+F23(t)*NodePt3.second+F33(t)*NodePt4.second;
// 		}
// 	}
// }

// // 计算三次B样条曲线上的点
// POINT CalculateCubicBSplinePoint(std::vector<POINT>& controlPoints, double t) {
//     double t2 = t * t;
//     double t3 = t2 * t;

//     double b0 = (-t3 + 3*t2 - 3*t + 1) / 6.0;
//     double b1 = (3*t3 - 6*t2 + 4) / 6.0;
//     double b2 = (-3*t3 + 3*t2 + 3*t + 1) / 6.0;
//     double b3 = t3 / 6.0;

//     POINT result;
//     result.x = static_cast<int>(b0 * controlPoints[0].x + b1 * controlPoints[1].x + b2 * controlPoints[2].x + b3 * controlPoints[3].x);
//     result.y = static_cast<int>(b0 * controlPoints[0].y + b1 * controlPoints[1].y + b2 * controlPoints[2].y + b3 * controlPoints[3].y);

//     return result;
// }

// LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//     switch (message) {
//         case WM_PAINT: {
//             PAINTSTRUCT ps;
//             HDC hdc = BeginPaint(hWnd, &ps);
//             FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

//             // 控制点
//             std::vector<POINT> controlPoints = { {100, 100}, {200, 300}, {400, 200}, {500, 400} };

//             MoveToEx(hdc, controlPoints[0].x, controlPoints[0].y, NULL);

//             for(double t=0.0;t<=1.0;t+=0.01)
//             {
//                 POINT point = CalculateCubicBSplinePoint(controlPoints, t);
//                 LineTo(hdc, point.x, point.y);
//             }

//             ReleaseDC(hWnd, hdc);

//             EndPaint(hWnd, &ps);
//             return 0;
//         }
//         case WM_DESTROY:
//             PostQuitMessage(0);
//             return 0;
//     }
//     return DefWindowProc(hWnd, message, wParam, lParam);
// }

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//     WNDCLASS wc = {};
//     wc.lpfnWndProc = WndProc;
//     wc.hInstance = hInstance;
//     wc.lpszClassName = (LPSTR)"B-Spline Curve";
//     RegisterClass(&wc);

//     HWND hWnd = CreateWindow((LPSTR)"B-Spline Curve", (LPSTR)"B-Spline Curve", WS_OVERLAPPEDWINDOW,
//         CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

//     ShowWindow(hWnd, nCmdShow);

//     MSG msg;
//     while (GetMessage(&msg, NULL, 0, 0)) {
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }

//     return 0;
// }





























// #include <Windows.h>
// #include <vector>

struct CPosition {
    double x;
    double y;
};

// 函数功能： 三次B样条拟合,在节点之间均匀插入指定个数点
// 输入参数： *pt ：给定点序列，执行完成后，会被替换成新的数据点
//            Num：节点点个数
//            *InsertNum: 节点之间需要插入的点个数指针 
// 返回值：   无返回值
// void ThreeOrderBSplineInterpolatePt(CPosition *&pt, int &Num, int *InsertNum);
void ThreeOrderBSplineInterpolatePt(CPosition *&pt,int &Num,int *InsertNum)
{
	if(pt==NULL || InsertNum==NULL) return;
 
	int InsertNumSum=0;                               //  计算需要插入的点总数
	for(int i=0;i<Num-1;i++)  InsertNumSum+=InsertNum[i];
 
	CPosition *temp=new CPosition[Num+2];
	for(int i=0;i<Num;i++)
		temp[i+1]=pt[i];
 
	temp[0].x=2*temp[1].x-temp[2].x;                  //  将折线延长线上两点加入作为首点和尾点
	temp[0].y=2*temp[1].y-temp[2].y;
 
	temp[Num+1].x=2*temp[Num].x-temp[Num-1].x;
	temp[Num+1].y=2*temp[Num].y-temp[Num-1].y;
 
	CPosition NodePt1,NodePt2,NodePt3,NodePt4;
	double t;
 
	delete []pt;                                      //  点数由原来的Num个增加到Num+InsertNumSum个，删除旧的存储空间，开辟新的存储空间
 
	pt=new CPosition[Num+InsertNumSum];              
 
	int totalnum=0;
	for(int i=0;i<Num-1;i++)                          //  每条线段均匀插入点
	{
		NodePt1=temp[i]; NodePt2=temp[i+1]; NodePt3=temp[i+2]; NodePt4=temp[i+3];
		double dt=1.0/(InsertNum[i]+1);
 
		for(int j=0;j<InsertNum[i]+1;j++)
		{
			t=dt*j;
			pt[totalnum].x=F03(t)*NodePt1.x+F13(t)*NodePt2.x+F23(t)*NodePt3.x+F33(t)*NodePt4.x;
			pt[totalnum].y=F03(t)*NodePt1.y+F13(t)*NodePt2.y+F23(t)*NodePt3.y+F33(t)*NodePt4.y;
			totalnum++;
		}
 
		if(i==Num-2){              //  最后一个尾点
			t=1;
			pt[totalnum].x=F03(t)*NodePt1.x+F13(t)*NodePt2.x+F23(t)*NodePt3.x+F33(t)*NodePt4.x;
			pt[totalnum].y=F03(t)*NodePt1.y+F13(t)*NodePt2.y+F23(t)*NodePt3.y+F33(t)*NodePt4.y;
			totalnum++;
		}
	}
	delete []temp;
	Num=Num+InsertNumSum;
}


// 绘制 B 样条曲线
void DrawBSplineCurve(HDC hdc, const std::vector<CPosition>& curvePoints) {
    MoveToEx(hdc, static_cast<int>(curvePoints[0].x), static_cast<int>(curvePoints[0].y), NULL);
    for (size_t i = 1; i < curvePoints.size(); i++) {
        LineTo(hdc, static_cast<int>(curvePoints[i].x), static_cast<int>(curvePoints[i].y));
    }
}

int main() {
    // 创建窗口
    HWND hwnd = GetConsoleWindow();
    HDC hdc = GetDC(hwnd);

    // 定义控制点
    int Num = 4;
    CPosition* controlPoints = new CPosition[Num];
    controlPoints[0] = {100, 100};
    controlPoints[1] = {200, 300};
    controlPoints[2] = {400, 200};
    controlPoints[3] = {500, 400};

    int InsertNum[3] = {5, 5, 5}; // 每个节点之间插入的点个数

    // 生成拟合曲线上的点
    ThreeOrderBSplineInterpolatePt(controlPoints, Num, InsertNum);

    // 绘制 B 样条曲线
    std::vector<CPosition> curvePoints(controlPoints, controlPoints + Num);
    DrawBSplineCurve(hdc, curvePoints);

    // 释放内存
    delete[] controlPoints;

    // 释放设备上下文
    ReleaseDC(hwnd, hdc);

    return 0;
}
