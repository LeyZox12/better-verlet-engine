#pragma once

#include "PointEngine.h"
#include "SFML/Graphics.hpp"
#include "PointEngineExport.hpp"
#include <vector>
#include <any>

using namespace std;
using namespace sf;

class PointEngine;
typedef Vector2f vec2;

struct POINTENGINE_API OnUpdateContext
{
    OnUpdateContext(PointEngine& pe, vec2 mousepos, vector<any>& args, int index)
    :pe(pe), mousepos(mousepos), args(args), index(index) {};
    PointEngine& pe;
    vec2 mousepos;
    int index;
    vector<any>& args;
};
