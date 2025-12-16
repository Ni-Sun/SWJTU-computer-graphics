#pragma once
#include "Line.h"
#include <windows.h>

// Liang-Barsky algorithm for line clipping
// Returns false if the line is completely outside the clip rectangle, true otherwise.
// The line 'l' is modified to the clipped coordinates if it is partially or fully inside.
bool liangBarskyClip(Line& l, const RECT& clipRect);
