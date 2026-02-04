#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "PointEngineExport.hpp"

using namespace std;
using namespace sf;

class POINTENGINE_API Rectangle
{
    public:
        Rectangle();
        Rectangle(Rect<int> rect, float friction);
        Rectangle(Rect<int> rect, string texturePath, float friction);
        void setRect(Rect<int> rect);
        void setTexture(string path);
        void draw(RenderWindow& window);
        void setColor(Color col);
        Texture* getTexture();
        bool getHasTexture();
        Rect<int> getRect();
        float getFriction();
    protected:

    private:
        Texture texture;
        bool hasTexture;
        float friction = 425.f;
        Rect<int> rectangle;
        RectangleShape sprite;
};

#endif // RECTANGLE_H
