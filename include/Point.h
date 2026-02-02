#ifndef POINT_H
#define POINT_H

#include <SFML/Graphics.hpp>
#define pi 3.141595426
#include <any>
#include <functional>
#include "OnUpdateContext.hpp"
#include "PointEngineExport.hpp"

class PointEngine;

using namespace sf;
using namespace std;

typedef Vector2f vec2;

class PointEngine;

class POINTENGINE_API Point
{
    public:
        Point();
        Point(vec2 pos, float radius, bool isStatic, bool shouldCollide, float friction);
        Point(vec2 pos, float radius, bool isStatic, bool shouldCollide, float friction, function<vector<any>(OnUpdateContext ctx)> onUpdate);
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
        void setFriction(float fricton);
        Color getColor();
        vec2 getPos();
        vec2 getOldPos();
        float getRadius();
        float getFriction();
        float getGravityScale();
        vec2 getAcc();
        bool getIsStatic();
        bool getShouldCollide();

        vector<any> args;
        function<vector<any>(OnUpdateContext ctx)> onUpdate;
    protected:

    private:
        bool isStatic;
        bool shouldCollide;
        vec2 pos;
        vec2 oldPos;
        vec2 acc;
        float radius;
        float friction;
        float gravityScale;
        Color displayColor;

};

#endif // POINT_H
