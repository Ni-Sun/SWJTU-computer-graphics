#pragma once
#include <vector>
#include <windows.h>
#include "Polygon.h"

std::vector<POINT> sutherlandHodgmanClip(const std::vector<POINT>& subjectPolygon, const RECT& clipWindow);
