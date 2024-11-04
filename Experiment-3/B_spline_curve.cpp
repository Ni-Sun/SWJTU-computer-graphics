#include "B_spline_curve.h"
COLORREF B_spline_curve::color=RGB(182,215,255);

void Draw_B_spline_curve(HWND hWnd)
{
    B_spline_curve bsc(arr);
    vector<POINT> curvePoints = bsc.points;
    HDC hdc = GetDC(hWnd);
    HPEN hPen = CreatePen(PS_SOLID, 2, B_spline_curve::color);
    SelectObject(hdc,hPen);

    MoveToEx(hdc,curvePoints[0].x,curvePoints[0].y,nullptr);
    for(auto [x,y]:curvePoints)
        LineTo(hdc,x,y);
    DeleteObject(hPen);
    ReleaseDC(hWnd,hdc);
    debug(str);
}

// 三次样条基函数
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


// 三次B样条拟合曲线
vector<Point> B_spline_curve::ComputeBSplineCurve(const vector<POINT> &control_points)
{
    if(control_points.size()==0)
        return vector<Point>();

    int num=control_points.size();
	int insert_num_sum=0;                               //  计算需要插入的点总数
    vector<int> insert_num(num-1);
    for(int i=0;i<num-1;i++)
    {
        insert_num[i]=(dis_point_to_point(control_points[i],control_points[i+1])/10.0);
        insert_num_sum+=insert_num[i];
    }

    vector<Point> temp(num+2);
	for(int i=0;i<num;i++)
		temp[i+1]=control_points[i];

	temp[0].x=2*temp[1].x-temp[2].x;                  //  将折线延长线上两点加入作为首点和尾点
	temp[0].y=2*temp[1].y-temp[2].y;

	temp[num+1].x=2*temp[num].x-temp[num-1].x;
	temp[num+1].y=2*temp[num].y-temp[num-1].y;

	double t;
	Point NodePt1,NodePt2,NodePt3,NodePt4;
    vector<Point> res(num+insert_num_sum);

	int totalnum=0;
	for(int i=0;i<num-1;i++)                          //  每条线段均匀插入点
	{
		NodePt1=temp[i]; NodePt2=temp[i+1]; NodePt3=temp[i+2]; NodePt4=temp[i+3];
		double dt=1.0/(insert_num[i]+1);

		for(int j=0;j<insert_num[i]+1;j++)
		{
			t=dt*j;
			res[totalnum].x=F03(t)*NodePt1.x+F13(t)*NodePt2.x+F23(t)*NodePt3.x+F33(t)*NodePt4.x;
			res[totalnum].y=F03(t)*NodePt1.y+F13(t)*NodePt2.y+F23(t)*NodePt3.y+F33(t)*NodePt4.y;
			totalnum++;
		}

		if(i==num-2){              //  最后一个尾点
			t=1;
			res[totalnum].x=F03(t)*NodePt1.x+F13(t)*NodePt2.x+F23(t)*NodePt3.x+F33(t)*NodePt4.x;
			res[totalnum].y=F03(t)*NodePt1.y+F13(t)*NodePt2.y+F23(t)*NodePt3.y+F33(t)*NodePt4.y;
			totalnum++;
		}
	}
    return res;
}
