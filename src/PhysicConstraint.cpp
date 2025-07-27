#include "../include/PhysicConstraint.h"

PhysicConstraint::PhysicConstraint()
{

}

PhysicConstraint::PhysicConstraint(int i1, int i2, int type, float distance, bool visible)
{
    indexes = make_pair(i1, i2);
    this -> type = type;
    this -> distance = distance;
    this -> visible = visible;
}

void PhysicConstraint::setIndex1(int index)
{
    indexes.first = index;
}

void PhysicConstraint::setIndex2(int index)
{
    indexes.second = index;
}

void PhysicConstraint::setType(int type)
{
    this -> type = type;
}

void PhysicConstraint::setDistance(float dist)
{
    distance = dist;
}

void PhysicConstraint::setVisible(bool visible)
{
    this -> visible = visible;
}

void PhysicConstraint::setIndexes(pair<int, int> indexes)
{
    this -> indexes = indexes;
}

pair<int, int> PhysicConstraint::getIndexes()
{
    return indexes;
}

int PhysicConstraint::getType()
{
    return type;
}

float PhysicConstraint::getDist()
{
    return distance;
}

bool PhysicConstraint::getVisible()
{
    return visible;
}
