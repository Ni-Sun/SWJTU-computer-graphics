#pragma once
#include <vector>
#include <windows.h>

// Weiler-Atherton 算法的裁剪结果可能是多个分离的多边形
// 因此返回一个多边形列表
std::vector<std::vector<POINT>> weilerAthertonClip(const std::vector<POINT>& subjectPolygon, const std::vector<POINT>& clipPolygon);
