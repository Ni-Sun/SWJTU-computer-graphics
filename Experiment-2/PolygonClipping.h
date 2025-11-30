#pragma once
#include <vector>
#include <windows.h>
#include "Polygon.h"

// Clips a polygon using the Sutherland-Hodgman algorithm against a rectangular window.
// Takes a vector of points representing the subject polygon and a RECT for the clip window.
// Returns a new vector of points for the clipped polygon.
std::vector<POINT> sutherlandHodgmanClip(const std::vector<POINT>& subjectPolygon, const RECT& clipWindow);
