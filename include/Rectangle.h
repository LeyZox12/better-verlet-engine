#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace sf;

class Rectangle
{
    public:
        Rectangle();
        Rectangle(Rect<int> rect);
        Rectangle(Rect<int> rect, string texturePath);
        void setRect(Rect<int> rect);
        void setTexture(string path);
        void draw(RenderWindow& window);
        Texture* getTexture();
        bool getHasTexture();
        Rect<int> getRect();
    protected:

    private:
        Texture texture;
        bool hasTexture;
        Rect<int> rectangle;
        RectangleShape sprite;
};

#endif // RECTANGLE_H
