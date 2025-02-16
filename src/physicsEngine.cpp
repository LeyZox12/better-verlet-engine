#include "physicsEngine.h"

PhysicsEngine::PhysicsEngine()
{

}

void PhysicsEngine::addPoint(vec2 pos, float radius, bool visible)
{
    Point point(pos, radius, visible);
    points.push_back(point);
}

void PhysicsEngine::link(int i1, int i2, int type, int param1, int param2)
{
    linkIndexes.push_back(make_pair(i1, i2));
    linkTypes.push_back(type);
    if(param1 == 0) //if no distance or angle defined, keep it the way it is right now
    {
        vec2 p1 = points[i1].getPos();
        vec2 p2 = points[i2].getPos();
        switch(type)
        {
            case(0):
                param1 = hypot(p2.x - p1.x, p2.y - p1.y);
                break;
            case(4):
                break;
        }

    }

    linkParams1.push_back(param1);
    linkParams2.push_back(param2);
}

void PhysicsEngine::setYLimit(int y)
{
    yLimit = y;
}

void PhysicsEngine::updatePoints(float dt)
{
    for(auto& p : points)
        p.updatePos(dt);
}

void PhysicsEngine::applyConstraints(float dt)
{
    for(int i = 0; i < linkIndexes.size(); i++)
    {
        vec2 p1 = points[linkIndexes[i].first].getPos();
        vec2 p2 = points[linkIndexes[i].second].getPos();
        vec2 dir = vec2(p1 - p2);
        float dist = hypot(dir.x, dir.y);
        dir = vec2(dir.x / dist, dir.y / dist);
        switch(linkTypes[i])
        {
            case(0):
            {
                float diff = dist - linkParams1[i];
                vec2 fixed = vec2(p1.x - dir.x  * (diff * 0.5),
                                  p1.y - dir.y * (diff * 0.5));
                points[linkIndexes[i].first].setPos(fixed);
                dir = vec2(dir.x * -1, dir.y * -1);
                fixed = vec2(p2.x - dir.x  * (diff * 0.5),
                             p2.y - dir.y * (diff * 0.5));
                points[linkIndexes[i].second].setPos(fixed);
                break;
            }
            case(4):
            {
                float angle = atan2(dir.y, dir.x);
                float aDiff = linkParams1[i] - (angle * DEG + 180);
                bool shouldOverlap = false;
                float minAngle = linkParams1[i];
                float maxAngle = linkParams2[i];
                vec2 fixed = p1;
                angle *= DEG;
                angle += 180;
                if(angle < minAngle)
                    fixed = vec2(p2.x + cos((minAngle - 180) * RAD) * dist,
                                  p2.y + sin((minAngle - 180) * RAD) * dist);
                else if(angle > maxAngle)
                    fixed = vec2(p2.x + cos((maxAngle - 180) * RAD) * dist,
                                 p2.y + sin((maxAngle - 180) * RAD) * dist);
                points[linkIndexes[i].first].setPos(fixed);
            }
        }
    }

    for(int i = 0; i < points.size(); i++)
    {
        if(points[i].getPos().y + points[i].getRad() >= yLimit)
        {
            points[i].applyFriction();
            points[i].setPos(vec2(points[i].getPos().x, yLimit - points[i].getRad()));
            points[i].setVel(vec2(points[i].getVel().x, 0));
        }
    }
}

int PhysicsEngine::getPointMouse(vec2 mousePos)
{
    int i = 0;
    for(auto& p : points)
    {
        float dist = hypot(mousePos.x - p.getPos().x,
                           mousePos.y - p.getPos().y);
        if(dist < p.getRad())
            return i;
        i++;
    }
    return -1;
}

void PhysicsEngine::setPointPos(int pointIndex, vec2 pos)
{
    points[pointIndex].setPos(pos);
}

void PhysicsEngine::display(RenderWindow& window, Color renderColor,bool renderLines)
{
    for(int i = 0; i < points.size(); i++)
    {
        CircleShape point;
        point.setRadius((points[i].getRad()));
        point.setOrigin(points[i].getRad(), points[i].getRad());
        point.setPosition(points[i].getPos());
        point.setFillColor(renderColor);
        window.draw(point);
    }

    if(renderLines)
    {
        VertexArray line(LineStrip, 2);
        line[0].color = renderColor;
        line[1].color = renderColor;
        for(int i = 0; i < linkIndexes.size(); i++)
        {
            line[0].position = points[linkIndexes[i].first].getPos();
            line[1].position = points[linkIndexes[i].second].getPos();
            window.draw(line);
        }
    }
}
