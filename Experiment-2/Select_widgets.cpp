#include "Select_delete.h"
#include "Select_save.h"
#include "Select_open.h"
#include "Select_center.h"
#include "Select_perp.h"
#include "Select_tangent.h"

//删除按钮
RECT Select_delete::rect = {540,0,600,30};
COLORREF Select_delete::color = RGB(200,30,30);
// 保存、打开按钮
RECT Select_save::rect = {600,0,660,30};
COLORREF Select_save::color = RGB(30,120,200);
RECT Select_open::rect = {660,0,720,30};
COLORREF Select_open::color = RGB(30,200,100);
// 圆心按钮
RECT Select_center::rect = {720,0,780,30};
COLORREF Select_center::color = RGB(200,140,30);
RECT Select_perp::rect = {780,0,840,30};
COLORREF Select_perp::color = RGB(120,30,200);
RECT Select_tangent::rect = {840,0,900,30};
COLORREF Select_tangent::color = RGB(200,60,120);
