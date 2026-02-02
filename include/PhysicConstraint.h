#ifndef PHYSICCONSTRAINT_H
#define PHYSICCONSTRAINT_H
#include "SFML/Graphics/Rect.hpp"
#include "PointEngineExport.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <iostream>
#include <algorithm>
#include <optional>
#include <vector>

using namespace std;

struct POINTENGINE_API ConstraintSprite
{
    ConstraintSprite(string path, sf::Vector2f size)
    :size(size)
    {
        texture.loadFromFile(path);
    }
    sf::Texture texture;
    sf::Vector2f size;
};


class POINTENGINE_API PhysicConstraint
{
    public:
        PhysicConstraint();
        PhysicConstraint(int i1, int i2, int type, float distance, bool visible);
        PhysicConstraint(int i1, int i2, int type, float distance, bool visible, std::string path, sf::Vector2f size);
        void setIndex1(int index);
        void setIndex2(int index);
        void setType(int type);
        void setDistance(float dist);
        void setVisible(bool visible);
        void setIndexes(pair<int, int> indexes);
        pair<int, int> getIndexes();
        int getType();
        float getDist();
        bool getVisible();
        optional<ConstraintSprite>& getSprite();
    private:
        pair<int, int> indexes;
        int type = 0;
        float distance = 0;
        optional<ConstraintSprite> sprite;
        bool visible = true;
};

#endif // PHYSICCONSTRAINT_H
