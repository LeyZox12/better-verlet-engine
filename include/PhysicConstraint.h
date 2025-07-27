#ifndef PHYSICCONSTRAINT_H
#define PHYSICCONSTRAINT_H
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

class PhysicConstraint
{
    public:
        PhysicConstraint();
        PhysicConstraint(int i1, int i2, int type, float distance, bool visible);
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
    private:
        pair<int, int> indexes;
        int type = 0;
        float distance = 0;
        bool visible = true;
};

#endif // PHYSICCONSTRAINT_H
