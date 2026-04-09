#ifndef POINTENGINE_H
#define POINTENGINE_H

#include <SFML/Graphics.hpp>
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"
#include "ThreadPool.h"
#include "Point.h"
#include "PhysicConstraint.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include "Rectangle.h"
#include "PointEngineExport.hpp"
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include "LineCollision.hpp"
#include "pointSprite.h"
#include <unordered_map>
#include <vecHash.hpp>
#define PI 3.141592654


using namespace std;
using namespace sf;

typedef Vector2f vec2;
class Point;





class POINTENGINE_API PointEngine
{

    public:
        PointEngine();
        int getPointIndexAtPos(vec2 point);
        int getPointIndexFromPtr(Point* ptr);
        int getPointCount();
        int getConstraintCount();
        Point* addPoint(vec2 pos, bool isStatic, bool shouldCollide,
                           float radius, float mass = 50.f);
        Point& getPoint(int index);
        Rectangle& getRect(int index);
        PhysicConstraint& getConstraint(int index);
        void removePoint(int index);
        void addConstraint(int i1, int i2, int constraintType, float constraintDistance);
        void addConstraint(int i1, int i2, int constraintType, float constraintDistance, bool isVisible);
        void addConstraint(int i1, int i2, int constraintType, float constraintDistance, bool isVisible, string texturePath, vec2 size);
        void updatePointPos(float dt, vec2 mousepos);
        void applyConstraints(int substeps, float dt);
        void applyCollisions(int substeps);
        void applyCollisionsPart(int substeps, int start, int end); 
        void removeConstraint(int index);
        void removeConstraints(int index);
        void addRectangle(Rect<int> rect, float friction = 425.f);
        void addRectangle(Rect<int> rect, string texturePath, float friction);
        void removeRectangle(int index);
        void display(RenderTarget* window, Color color);
        void displayAsRects(RenderWindow& window, Color color, float width);
        void displayAsRects(RenderTexture& window, Color color, float width);
        int getRectCount();
        static int circleRectCollision(Rect<int> rect, CircleShape circle);
        static const int DISTANCE_CONSTRAINT_MIN = 0;
        static const int DISTANCE_CONSTRAINT_MAX = 1;
        static const int DISTANCE_CONSTRAINT_MINMAX = 2;
        static const int SPRING_CONSTRAINT_MINMAX = 3;
        static const int SOLID_LINE = 4;
        int cellSize = 0;
        const int THREADS = 10;
    protected:

    private:
        Vector2i offsets[9] = {
            {0, 0},
            {1, 1},
            {1, 0},
            {1, -1},
            {-1, 1},
            {-1, 0},
            {-1, -1},
            {0, 1},
            {0, -1}
        };
        unordered_map<Vector2i, pair<vector<int>, int>, vec2Hash> grid;
        CircleLineCollision circleLineCollision(sf::CircleShape cir, vec2 lineStart, vec2 lineEnd);
        RectLineCollision rectLineCollision(Rect<int>, vec2 lineStart, vec2 lineEnd);
        vector<Point> points;
        vector<PhysicConstraint> constraints;
        vector<Rectangle> rectangles;
        ThreadPool threadPool;
        Texture pointTexture;
};

#endif // POINTENGINE_H
