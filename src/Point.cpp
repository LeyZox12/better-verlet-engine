#include "../include/Point.h"

Point::Point()
{

}

Point::Point(vec2 pos, float radius, bool isStatic, bool shouldCollide, float friction, float mass)
{
    this -> pos = pos;
    this -> oldPos = pos;
    this -> radius = radius;
    this -> isStatic = isStatic;
    this -> shouldCollide = shouldCollide;
    this -> friction = friction;
    onUpdate = [](OnUpdateContext ctx) { return vector<any>();};

}

Point::Point(vec2 pos, float radius, bool isStatic, bool shouldCollide, float friction, function<vector<any>(OnUpdateContext ctx)> onUpdate)
{
    this -> pos = pos;
    this -> oldPos = pos;
    this -> radius = radius;
    this -> isStatic = isStatic;
    this -> shouldCollide = shouldCollide;
    this -> friction = friction;
    this -> onUpdate = onUpdate;
    this -> index = index;
}


void Point::setPos(vec2 pos, bool overrideStatic)
{
    if(isStatic && !overrideStatic) return;
    this -> pos = pos;
}

void Point::move(vec2 offset, bool overrideStatic)
{
    if(isStatic && !overrideStatic) return;
    oldPos = pos;
    pos += offset;
}

void Point::setGravityScale(float scale)
{
    this->gravityScale = scale;
}

void Point::setAcc(vec2 acc)
{
    this -> acc = acc;
}

void Point::addAcc(vec2 offset)
{
    acc += offset;
}

void Point::setShouldCollide(bool shouldCollide)
{
    this -> shouldCollide = shouldCollide;
}

void Point::setIsStatic(bool isStatic)
{
    this -> isStatic = isStatic;
}

void Point::setFriction(float friction)
{
    this -> friction = friction;
}

void Point::setOldPos(vec2 pos)
{
    oldPos = pos;
}

void Point::setRadius(float radius)
{
    this -> radius = radius;
}

void Point::setColor(Color color)
{
    displayColor = color;
}

Color Point::getColor()
{
    return displayColor;
}

vec2 Point::getPos()
{
    return pos;
}

vec2 Point::getOldPos()
{
    return oldPos;
}

float Point::getRadius()
{
    return radius;
}

float Point::getFriction()
{
    return friction;
}

float Point::getGravityScale()
{
    return gravityScale;
}

float Point::getMass()
{
    return mass;
}

vec2 Point::getAcc()
{
    return acc;
}

bool Point::getIsStatic()
{
    return isStatic;
}

bool Point::getShouldCollide()
{
    return shouldCollide;
}

int Point::getIndex()
{
    return index;
}

void Point::setIndex(int index)
{
    this->index = index;
}

void Point::setMass(float mass)
{
    this->mass = mass;
}