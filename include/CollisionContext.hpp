#pragma once

#include <PointEngine.h>

struct CollisionContext
{
    CollisionContext(bool isPoint, int index, PointEngine& pe)
    :pe(pe)
    {
        this->isPoint = isPoint;
        this->index = index;
    }
    bool isPoint;
    int index;
    PointEngine& pe;
};