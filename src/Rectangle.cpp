#include "../include/Rectangle.h"

Rectangle::Rectangle()
{

}

Rectangle::Rectangle(Rect<int> rect)
{
    this->rectangle = rect;
    this->hasTexture = false;
    sprite.setPosition({rect.position.x, rect.position.y});
    sprite.setSize({rect.size.x, rect.size.y});
}

Rectangle::Rectangle(Rect<int> rect, string texturePath)
{
    this->rectangle = rect;
    setTexture(texturePath);
    sprite.setPosition({rect.position.x, rect.position.y});
    sprite.setSize({rect.size.x, rect.size.y});
}

void Rectangle::setRect(Rect<int> rect)
{
    this->rectangle = rect;
    sprite.setPosition({rect.position.x, rect.position.y});
    sprite.setSize({rect.size.x, rect.size.y});
}

void Rectangle::setColor(Color col)
{
    sprite.setFillColor(col);
}

void Rectangle::draw(RenderWindow& window)
{
    sprite.setTexture(&texture);
    window.draw(sprite);
}

void Rectangle::setTexture(string path)
{
    if(!texture.loadFromFile(path))
    {
        cout << "Texture couldn't load properly\n";
        hasTexture = false;
    }
    else
    {
        cout << "texture set successfully\n";
        hasTexture = true;

    }
}

bool Rectangle::getHasTexture()
{
    return hasTexture;
}

Texture* Rectangle::getTexture()
{
    return &texture;
}

Rect<int> Rectangle::getRect()
{
    return rectangle;
}
