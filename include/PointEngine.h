#ifndef POINTENGINE_H
#define POINTENGINE_H

#include <SFML/Graphics.hpp>
#include "Point.h"
#include "PhysicConstraint.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include "OnUpdateContext.hpp"
#include "Rectangle.h"
#include <chrono>

#include <thread>

using namespace std;
using namespace sf;

typedef Vector2f vec2;
class Point;

class OnUpdateContext;

class PointEngine
{
    public:
        PointEngine();
        int getPointIndexAtPos(vec2 point);
        Point& getPoint(int index);
        void addPoint(vec2 pos, bool isStatic, bool shouldCollide, float radius, float friction);
        void addPoint(vec2 pos, bool isStatic, bool shouldCollide, float radius, float friction, Color displayColor);
        void addPoint(vec2 pos, bool isStatic, bool shouldCollide, float radius, float friction, function<vector<any>(OnUpdateContext ctx)> onUpdate);
        void removePoint(int index);
        void addConstraint(int i1, int i2, int constraintType, float constraintDistance);
        void addConstraint(int i1, int i2, int constraintType, float constraintDistance, bool isVisible);
        PhysicConstraint getConstraint(int index);
        void updatePointPos(float dt, vec2 mousepos);
        void applyConstraints(int substeps);
        void applyCollisions(int substeps);
        void removeConstraint(int index);
        void removeConstraints(int index);
        void addRectangle(Rect<int> rect);
        void addRectangle(Rect<int> rect, string texturePath);
        void removeRectangle(int index);
        int getPointCount();
        int getConstraintCount();
        void display(RenderWindow& window, Color color);
        void displayAsRects(RenderWindow& window, Color color, float width);
        static int circleRectCollision(Rect<int> rect, CircleShape circle);
        static const int DISTANCE_CONSTRAINT_MIN = 0;
        static const int DISTANCE_CONSTRAINT_MAX = 1;
        static const int DISTANCE_CONSTRAINT_MINMAX = 2;
    protected:

    private:
        vector<Point> points;
        vector<PhysicConstraint> constraints;
        vector<Rectangle> rectangles;
};

#endif // POINTENGINE_H
