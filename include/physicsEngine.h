/*
HOW TO USE:

TYPES = 1: distance min max; 2: distance max 3: distance min 4: angle



*/
#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H
#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

typedef Vector2f vec2;

class PhysicsEngine
{
    public:
        PhysicsEngine();
        void addPoint(vec2 pos, float radius, bool visible);
        void link(int i1, int i2, int type, int param1, int param2);
        void updatePoints(float dt);
        void applyConstraints(float dt);
        void display(RenderWindow& window, Color renderColor,bool renderLines);
        void setYLimit(int y);
        void setPointPos(int pointIndex, vec2 pos);
        int getPointMouse(vec2 mousePos);
    private:
        class Point
        {
            public:
                Point(vec2 pos, float radius, bool visible)
                {
                    this -> pos = pos;
                    this -> oldPos = pos;
                    this -> radius = radius;
                    this -> radius = radius;
                }

                vec2 getPos()
                {
                    return pos;
                }

                void setPos(vec2 pos)
                {
                    this -> pos = pos;
                }

                void setOldPos(vec2 oldPos)
                {
                    this -> oldPos = oldPos;
                }

                void setVel(vec2 vel)
                {
                    this -> vel = vel;
                }

                vec2 getVel()
                {
                    return vel;
                }

                void applyFriction()
                {
                    vel.x = (oldPos.x - pos.x) * 250 * 1.1;
                }

                float getRad()
                {
                    return radius;
                }

                void updatePos(float dt)
                {
                    vel.y += 99.8 * dt;
                    vel.y * 0.998;
                    vec2 newPos = vec2(2 * pos.x - oldPos.x + vel.x * (dt * dt),
                                       2 * pos.y - oldPos.y + vel.y * (dt * dt));
                    oldPos = pos;
                    pos = newPos;
                }

            private:
                vec2 pos;
                float radius;
                bool visible;
                vec2 oldPos;
                vec2 vel;
        };

        const float PI = 3.141592624;
        const float RAD = PI / 180;
        const float DEG = 180 / PI;
        vector<Point> points;
        vector<pair<int, int>> linkIndexes;
        vector<int> linkTypes;
        vector<float> linkParams1;
        vector<float> linkParams2;
        int yLimit;
};

#endif // PHYSICSENGINE_H
