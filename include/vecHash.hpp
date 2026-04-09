#pragma once
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>

struct vec2Hash
{
    size_t operator()(const sf::Vector2i& v)const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) * 2654435761u); 
    }
};