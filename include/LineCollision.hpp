#pragma once
#include <SFML/Graphics.hpp>

struct CircleLineCollision
{
    CircleLineCollision(bool collided, float dist, sf::Vector2f n)
    :collided(collided), dist(dist), n(n){}
    bool collided;
    float dist;
    sf::Vector2f n;
};


struct RectLineCollision
{
    RectLineCollision(bool collided, float dist1, float dist2, sf::Vector2f n)
    :collided(collided), dist1(dist1), dist2(dist2), n(n){}
    bool collided;
    float dist1, dist2;
    sf::Vector2f n;
};