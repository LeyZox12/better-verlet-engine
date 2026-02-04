#pragma once

#include <optional>

struct CollisionContext
{
    CollisionContext(bool isPoint, int index)
    {
        this->isPoint = isPoint;
        this->index = index;
    }
    bool isPoint;
    int index;
};