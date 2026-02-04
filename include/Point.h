#ifndef POINT_H
#define POINT_H

#include <SFML/Graphics.hpp>
#define pi 3.141595426
#include <any>
#include <functional>
#include "OnUpdateContext.hpp"
#include "PointEngineExport.hpp"
#include "CollisionContext.hpp"

class PointEngine;

using namespace sf;
using namespace std;

typedef Vector2f vec2;

class PointEngine;

class POINTENGINE_API Point
{
    public:
        Point();
        Point(vec2 pos, float radius, bool isStatic, bool shouldCollide, float mass = 50.f);
        void setPos(vec2 pos, bool overrideStatic);
        void move(vec2 offset, bool overrideStatic);
        void setAcc(vec2 acc);
        void addAcc(vec2 offset);
        void setShouldCollide(bool shouldCollide);
        void setGravityScale(float scale);
        void setIsStatic(bool isStatic);
        void setOldPos(vec2 pos);
        void setRadius(float radius);
        void setColor(Color color);
        void setMass(float mass);
        Color getColor();
        vec2 getPos();
        vec2 getOldPos();
        float getRadius();
        float getGravityScale();
        float getMass();
        vec2 getAcc();
        bool getIsStatic();
        bool getShouldCollide();
        int getIndex();
        void setIndex(int index);
        void setCollisionCallback(std::function<void(CollisionContext ctx)>  func);

        vector<any> args;
        function<void(OnUpdateContext ctx)> onUpdate;
        function<void(CollisionContext ctx)> onCollision;
    protected:

    private:
        int index;
        bool isStatic;
        bool shouldCollide;
        vec2 pos;
        vec2 oldPos;
        vec2 acc;
        float radius;
        float gravityScale = 1.f;
        float mass = 50.f;
        Color displayColor;

};

#endif // POINT_H
