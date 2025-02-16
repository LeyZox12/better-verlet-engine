#include <iostream>
#include <SFML/Graphics.hpp>
#include "include/physicsEngine.h"

using namespace std;
using namespace sf;

RenderWindow window(VideoMode(512, 512), "StickMan");
Event e;
PhysicsEngine engine;
Clock deltaClock;
float dt;
RectangleShape floorRect(vec2(512, 12));
bool holding = false;
void drag(vec2 mousepos);
vec2 mousePos;

void start()
{
    window.setFramerateLimit(60);
    floorRect.setFillColor(Color::Green);
    floorRect.setPosition(0, 512);
    engine.setYLimit(500);
    engine.addPoint(vec2(200, 100), 5, true); // left hand : 0
    engine.addPoint(vec2(250, 100), 5, true); // left elbow : 1
    engine.addPoint(vec2(350, 100), 5, true); // right elbow : 2
    engine.addPoint(vec2(400, 100), 5, true); // right hand : 3
    engine.addPoint(vec2(300, 50), 10, true); // neck : 4
    engine.addPoint(vec2(300, 150), 5, true); // pelvis : 5
    engine.addPoint(vec2(250, 200), 5, true); // left leg : 6
    engine.addPoint(vec2(250, 250), 5, true); // left foot : 7
    engine.addPoint(vec2(350, 200), 5, true); // right leg : 8
    engine.addPoint(vec2(350, 250), 5, true); // right foot : 9
    engine.link(0, 1, 0, 0, 0);
    engine.link(2, 3, 0, 0, 0);
    engine.link(1, 4, 0, 0, 0);
    engine.link(2, 4, 0, 0, 0);
    engine.link(4, 5, 0, 0, 0);
    engine.link(5, 6, 0, 0, 0);
    engine.link(5, 8, 0, 0, 0);
    engine.link(6, 7, 0, 0, 0);
    engine.link(8, 9, 0, 0, 0);
    engine.link(5, 6, 4, 90, 170);
}

int main()
{
    start();

    while(window.isOpen())
    {
        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();
            if(e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
                holding = true;
            else if(e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
                holding = false;
        }
        mousePos = vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
        drag(mousePos);
        dt = deltaClock.restart().asSeconds();
        engine.updatePoints(dt);
        for(int _ = 0; _ < 4; _++)
            engine.applyConstraints(dt);
        window.clear(Color::Black);
        engine.display(window, Color::White, true);
        window.draw(floorRect);
        window.display();
    }
    return 0;
}

void drag(vec2 mousepos)
{
    static int target = -1;
    if(!holding)
        target = -1;
    else if(target == -1)
        target = engine.getPointMouse(mousepos);
    else
    {
        engine.setPointPos(target, mousepos);
    }
}
