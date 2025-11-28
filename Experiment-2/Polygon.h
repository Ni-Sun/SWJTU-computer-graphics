#pragma once
#include <windows.h>
#include <vector>

using namespace std;

struct MyPolygon {
    vector<POINT> p;
    static RECT rect;
    static COLORREF color;

    MyPolygon() {}

    MyPolygon(vector<POINT> pp) {
        p = pp;
    }
};
