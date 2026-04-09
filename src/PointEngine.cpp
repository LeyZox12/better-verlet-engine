#include "../include/PointEngine.h"
#include "LineCollision.hpp"
#include "OnUpdateContext.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"
#include <algorithm>
#include <memory>
#include <utility>
#define GLFW_DLL
#include "CollisionContext.hpp"

#include "tracy/Tracy.hpp"

#include "glad/glad.h"



PointEngine::PointEngine()
:threadPool(ThreadPool(THREADS))
{
  pointTexture.loadFromMemory(point_png, point_png_len);
}

int PointEngine::getPointIndexAtPos(vec2 point) {
  for (int i = 0; i < points.size(); i++) {
    vec2 diff = points[i].getPos() - point;
    float dist = hypot(diff.x, diff.y);
    if (dist < points[i].getRadius())
      return i;
  }
  return -1;
}

int PointEngine::getPointIndexFromPtr(Point* ptr)
{
  return ptr - points.data();
}

int PointEngine::getPointCount() { return points.size(); }

int PointEngine::getConstraintCount() { return constraints.size(); }
Point* PointEngine::addPoint(vec2 pos, bool isStatic, bool shouldCollide,
                           float radius, float mass)
{
  points.emplace_back(Point(pos, radius, isStatic, shouldCollide, mass));
  Point& p = points[points.size()-1];
  p.setIndex(points.size()-1);
  cellSize = std::max((int)ceil(radius*1.5f), cellSize);
  return &points[points.size()-1];
}

Point &PointEngine::getPoint(int index) { return points[index]; }

Rectangle &PointEngine::getRect(int index) { return rectangles[index]; }

void PointEngine::removePoint(int index) {
  if (index < points.size()) {
    vector<PhysicConstraint> buffer = constraints;

    for (int c = 0; c < constraints.size(); c++) {
      if (constraints[c].getIndexes().first > index)
        constraints[c].setIndexes(
            make_pair(constraints[c].getIndexes().first - 1,
                      constraints[c].getIndexes().second));
      if (constraints[c].getIndexes().second > index)
        constraints[c].setIndexes(
            make_pair(constraints[c].getIndexes().first,
                      constraints[c].getIndexes().second - 1));
      // cout << "1:" << constraints[c].getIndexes().first << ", 2:"<<
      // constraints[c].getIndexes().second << endl;
    }
    removeConstraints(index);
    points.erase(points.begin() + index);
  }
}

//TODO remove links from point object
void PointEngine::removeConstraints(int index) {
  vector<int> toRemove = {};
  for (int i = 0; i < constraints.size(); i++) {
    if (constraints[i].getIndexes().first == index ||
        constraints[i].getIndexes().second == index)
      toRemove.push_back(i);
  }
  sort(toRemove.begin(), toRemove.end(), greater<int>());
  for (auto &r : toRemove)
    constraints.erase(constraints.begin() + r);
}


void PointEngine::addRectangle(sf::Rect<int> rect, float friction) {
  rectangles.push_back(Rectangle(rect, friction));
  int start = getPointCount()-1;
  /*addPoint((vec2)rect.position, true, false, 10.f);
  addPoint((vec2)rect.position + vec2(0, rect.size.y), true, false, 10.f);
  addPoint((vec2)rect.position + vec2(rect.size.x, 0), true, false, 10.f);
  addPoint((vec2)rect.position + (vec2)rect.size, true, false, 0.f);
  addConstraint(start, start+1, SOLID_LINE, 0.f, false);
  addConstraint(start+1, start+2, SOLID_LINE, 0.f, false);
  addConstraint(start+2, start+3, SOLID_LINE, 0.f, false);
  addConstraint(start, start+3, SOLID_LINE, 0.f, false);*/
}

void PointEngine::addRectangle(Rect<int> rect, string texturePath, float friction) {
  rectangles.push_back(Rectangle(rect, texturePath, friction));
}

void PointEngine::removeRectangle(int index) {
  if (index < rectangles.size())
    rectangles.erase(rectangles.begin() + index);
}

void PointEngine::addConstraint(int i1, int i2, int constraintType,
                                float constraintDistance) {
  //dont add if already exists
  if(count_if(constraints.begin(), constraints.end(), 
  [i1, i2](auto& c)
  {
    return (c.getIndexes().first == i1 && c.getIndexes().second == i2) ||
           (c.getIndexes().first == i2 && c.getIndexes().second == i1);
  }) != 0)
  return;
  if (constraintDistance == 0) {
    vec2 diff = points[i1].getPos() - points[i2].getPos();
    constraintDistance = hypot(diff.x, diff.y);
  }
  points[i1].links.emplace(i2);
  points[i2].links.emplace(i1);
  constraints.emplace_back(
      PhysicConstraint(i1, i2, constraintType, constraintDistance, true));
}

void PointEngine::addConstraint(int i1, int i2, int constraintType,
                                float constraintDistance, bool isVisible) {
  for (auto &c : constraints)
    if (c.getIndexes() == make_pair(i1, i2))
      return;

  if (constraintDistance == 0) {
    vec2 diff = points[i1].getPos() - points[i2].getPos();
    constraintDistance = hypot(diff.x, diff.y);
  }
  constraints.emplace_back(
      PhysicConstraint(i1, i2, constraintType, constraintDistance, isVisible));
}

void PointEngine::addConstraint(int i1, int i2, int constraintType, float constraintDistance, bool isVisible, std::string path, vec2 size)
{
  for (auto &c : constraints)
    if (c.getIndexes() == std::make_pair(i1, i2))
      return;

  if (constraintDistance == 0) {
    vec2 diff = points[i1].getPos() - points[i2].getPos();
    constraintDistance = hypot(diff.x, diff.y);
  }
  constraints.emplace_back(
      PhysicConstraint(i1, i2, constraintType, constraintDistance, isVisible, path, size));
}

void PointEngine::removeConstraint(int index) {
  /*if(index < constraints.size())
  {
      constraintDistance.erase(constraintDistance.begin() + index);
      constraintType.erase(constraintType.begin() + index);
      constraints.erase(constraints.begin() + index);
  }*/
}

PhysicConstraint& PointEngine::getConstraint(int index) {
  return constraints[index];
}



void PointEngine::updatePointPos(float dt, vec2 mousepos) {
  int index = 0;
  ZoneScoped;
  {
    ZoneScopedN("pointPosUpdateLoop");
  for (auto &p : points) {
    vec2 currPos = p.getPos();
    vec2 currOldPos = p.getOldPos();
    vec2 currAcc = p.getAcc();
    p.setAcc({0, 999.8f * p.getGravityScale()});
    vec2 newPos = currPos * 2.0f - currOldPos + currAcc * dt * dt;
    p.setOldPos(currPos);
    p.setPos(newPos, false);
    p.onUpdate(OnUpdateContext(*this, mousepos, p.args, index));
    auto& g = grid[p.gridPos(cellSize)];
    if(g.second >= g.first.size())
    {
      g.first.push_back(index);
    }
    else
      g.first[g.second] = index;
    g.second++;
    index++;
  }
  }
}
void PointEngine::applyConstraints(int substeps, float dt) {
  Point *p1, *p2;
  vec2 fixed1, fixed2, dir;
  PhysicConstraint currConstraint;
  float dist;
  float diff;
  for (int _ = 0; _ < substeps; _++)
    for (int c = 0; c < constraints.size(); c++) {
      currConstraint = constraints[c];
      p1 = &points[currConstraint.getIndexes().first];
      p2 = &points[currConstraint.getIndexes().second];
      dir = p1->getPos() - p2->getPos();
      dist = hypot(dir.x, dir.y);
      if (dist != 0)
        dir /= dist;
      diff = (dist - currConstraint.getDist());
      float staticRatio1 = 0.5;
      float staticRatio2 = 0.5;
      if (p1->getIsStatic())
        staticRatio2 += 0.5;
      else if(p2->getIsStatic())
        staticRatio1 += 0.5;
      else 
      {
        staticRatio2 = std::clamp(p1->getMass() / p2->getMass(), 0.f, 0.5f);
        staticRatio1 = std::clamp(p2->getMass() / p1->getMass(), 0.f, 0.5f);
      }


      switch (currConstraint.getType()) {
      case (0):
        if (dist > constraints[c].getDist())
          break;
        fixed1 = p1->getPos() - dir * diff * staticRatio1;
        fixed2 = p2->getPos() + dir * diff * staticRatio2;
        p1->setPos(fixed1, false);
        p2->setPos(fixed2, false);
        break;
      case (1):
        if (dist < constraints[c].getDist())
          break;
        fixed1 = p2->getPos() + dir * diff * -staticRatio1;
        fixed2 = p1->getPos() - dir * diff * staticRatio2;
        p1->setPos(fixed1, false);
        p2->setPos(fixed2, false);
        break;
      case (2):
      case (4):
        if (dist == constraints[c].getDist())
          break;
        fixed1 = p1->getPos() - dir * diff * staticRatio1;
        fixed2 = p2->getPos() + dir * diff * staticRatio2;
        p1->setPos(fixed1, false);
        p2->setPos(fixed2, false);
        break;
      case (3):
        fixed1 = p1->getPos() - dir * diff * 0.05f * dt;
        fixed2 = p2->getPos() + dir * diff * 0.05f * dt;
        p1->setPos(fixed1, false);
        p2->setPos(fixed2, false);
        break;
      }
    }
}

enum Collisions { NONE, UP, DOWN, LEFT, RIGHT };

void PointEngine::applyCollisions(int substeps) {
  ZoneScoped;
  {
      for (int _ = 0; _ < substeps; _++)
        for (int j = 0; j < points.size(); j++) {
          Point &p = points[j];
          {
            ZoneScopedN("pointCollision");
          
          for(auto& o: offsets)
          {
          sf::Vector2i gPos = p.gridPos(cellSize);
          auto it = grid.find(gPos+o);
          if(it != grid.end())
            for(int index = 0; index < it->second.second; index++)
            {
              int i = it->second.first[index];
              if ((p.getShouldCollide() ||
                  points[i].getShouldCollide()) && i != j 
                  /*&&!p.links.contains(i)*/) {
                Point *p2 = &points[i];
                float r1 = p.getRadius();   // radius point 1
                float r2 = p2->getRadius(); // radius point 2
                float rc = r1 + r2;         // radius combined,
                vec2 dir = p.getPos() - p2->getPos();
                float dist = dir.x * dir.x + dir.y * dir.y;
                if (dist > 0.f && dist < rc*rc) {
                  p.onCollision(CollisionContext(true, i, *  this));
                  p2->onCollision(CollisionContext(true, j, *  this));
                  dist = sqrt(dist);
                  dir /= dist;
                  float difference = rc - dist;
                  float ratio1 = r2 / r1;
                  float ratio2 = r1 / r2;
                  vec2 fixed1, fixed2;
                  if (ratio1 < ratio2) {
                    fixed1 = p.getPos() - (rc - dist) * ratio1 * -dir;
                    fixed2 = p2->getPos() - (rc - dist) * (1.f - ratio1) * dir;
                  } else if (ratio2 < ratio1) {
                    fixed1 = p.getPos() - (rc - dist) * (1.f - ratio2) * -dir;
                    fixed2 = p2->getPos() - (rc - dist) * ratio2 * dir;
                  } else {

                    fixed1 = p.getPos() - (rc - dist) * 0.5f * -dir;
                    fixed2 = p2->getPos() - (rc - dist) * 0.5f * dir;
                  }
                  if (dist < rc) {
                    p.setPos(fixed1, false);
                    p2->setPos(fixed2, false);
                  }
                }
              }
            }
          }
          }
        {
          ZoneScopedN("sprite");
          static sf::CircleShape sprite;
          sprite.setPosition(p.getPos());
          sprite.setOrigin({p.getRadius(), p.getRadius()});
          sprite.setRadius(p.getRadius());
          if(p.getShouldCollide())
          for(auto c : constraints)
          {
            if(!(p.links.contains(c.getIndexes().first) || p.links.contains(c.getIndexes().second)))
            {
              Point& start = points[c.getIndexes().first];
              Point& end = points[c.getIndexes().second];
              CircleLineCollision col = circleLineCollision(sprite, points[c.getIndexes().first].getPos(), points[c.getIndexes().second].getPos());
              if(col.collided)
              {
                float ratio = 1/2.f;
                float correction = p.getRadius() - abs(col.dist);
                if(col.dist < 0.f)
                {
                  ratio *= -1;
                }
                p.move(col.n * correction * ratio, false);
                start.move(col.n * correction * -ratio, false);
                end.move(col.n * correction * -ratio, false);
              }
            }
          }
          
          for (int r = 0; r < rectangles.size(); r++) {
            int collision =
                circleRectCollision(rectangles[r].getRect(), sprite);
            if(collision > NONE && p.getShouldCollide())
              p.onCollision(CollisionContext(false, r, *this));
            if (collision == UP) {
              p.setPos({p.getPos().x,
                        rectangles[r].getRect().position.y - p.getRadius()},
                       false);
              float vel = p.getOldPos().x - p.getPos().x;
              p.setAcc({vel * rectangles[r].getFriction(), 0});
            } else if (collision == RIGHT) {
              p.setPos({rectangles[r].getRect().position.x +
                            rectangles[r].getRect().size.x + p.getRadius(),
                        p.getPos().y},
                       false);
              p.setAcc({0, p.getAcc().y});
            } else if (collision == LEFT) {
              p.setPos({rectangles[r].getRect().position.x - p.getRadius(),
                        p.getPos().y},
                       false);
              p.setAcc({0, p.getAcc().y});
            }
            else if(collision == DOWN) {
              p.setPos({p.getPos().x,
                        rectangles[r].getRect().position.y+ rectangles[r].getRect().size.y + p.getRadius()},
                       false);
            }
          }
        }
        }
      {
        ZoneScopedN("gridClear");
      for(auto& v: grid)
        v.second.second = 0;
      }
    }
  }

void PointEngine::applyCollisionsPart(int substeps, int start, int end) {}

int PointEngine::circleRectCollision(sf::Rect<int> rect, sf::CircleShape circle) {
  float bx = circle.getPosition().x;
  float by = circle.getPosition().y;
  float br = circle.getRadius();
  float rx = rect.position.x;
  float ry = rect.position.y;
  float sx = rect.size.x;
  float sy = rect.size.y;
  int collision = NONE;
  std::vector<float> distances = {abs(by + br - ry), abs(by - br - (ry + sy)),
                             abs(bx + br - rx), abs(bx - br - (rx + sx))};
  int collisionIndex = distance(
      distances.begin(), min_element(distances.begin(), distances.end()));
  if (bx+br > rx && by+br > ry && bx-br < rx + sx && by-br < ry + sy) 
    collision = collisionIndex + 1;
  return collision;
}

static void clearPool(std::vector<std::thread> &pool) {
  for (auto &t : pool)
    t.join();
  pool.clear();
}

void PointEngine::display(sf::RenderTarget* window, sf::Color color) {
  int pIndex = 0;
  RectangleShape sprite;
  sprite.setTexture(&pointTexture);
  for (auto &p : points) {
    sprite.setSize({p.getRadius()* 2, p.getRadius()*2});
    sprite.setOrigin({p.getRadius(), p.getRadius()});
    sprite.setPosition(p.getPos());
    sprite.setFillColor(color);
    window->draw(sprite);
    pIndex++;
  }
  VertexArray line(PrimitiveType::LineStrip, 2);
  line[0].color = color;
  line[1].color = color;
  for (auto &c : constraints) {
    if (c.getVisible()) {
      Point p1 = points[c.getIndexes().first];
      Point p2 = points[c.getIndexes().second];
      line[0].position = p1.getPos();
      line[1].position = p2.getPos();
      window->draw(line);
    }
  }
  for (auto &r : rectangles) {
    r.setColor(color);
    r.draw(window);
  }
}

void PointEngine::displayAsRects(RenderWindow &window, Color color,
                                 float width) {
  static RectangleShape sprite;
  sprite.setFillColor(color);

  for (auto &c : constraints) {

    vec2 diff = points[c.getIndexes().second].getPos() -
                points[c.getIndexes().first].getPos();
    float dist = hypot(diff.x, diff.y);
    if(c.getSprite().has_value())
    {
      ConstraintSprite& cs = c.getSprite().value();
      float ratio = cs.size.y / cs.size.x;
      sprite.setSize({dist, ratio * dist});
      sprite.setTexture(&cs.texture);
      sprite.setOrigin({0.f, sprite.getSize().y / 2.f});
    }
    else
    {
      sprite.setSize({dist, width});
      sprite.setOrigin({0, width / 2.f});
    }
    sprite.setPosition(points[c.getIndexes().first].getPos());
    sprite.setRotation(radians(atan2(diff.y, diff.x)));
    window.draw(sprite);
  }
  static RectangleShape rect;
  rect.setFillColor(color);
  for (auto &r : rectangles) {
    rect.setPosition({r.getRect().position.x, r.getRect().position.y});
    rect.setSize({r.getRect().size.x, r.getRect().size.y});
    if (r.getHasTexture())
      rect.setTexture(r.getTexture());
    window.draw(rect);
  }
}

void PointEngine::displayAsRects(RenderTexture &window, Color color,
                                 float width) {
  RectangleShape sprite;
  sprite.setFillColor(color);

  for (auto &c : constraints) {

    vec2 diff = points[c.getIndexes().second].getPos() -
                points[c.getIndexes().first].getPos();
    float dist = hypot(diff.x, diff.y);
    sprite.setSize({width, dist});
    sprite.setOrigin({width / 2.0, 0});
    sprite.setPosition(points[c.getIndexes().first].getPos());
    sprite.setRotation(radians(atan2(diff.y, diff.x) - pi / 2.0));
    window.draw(sprite);
  }
  RectangleShape rect;
  rect.setFillColor(color);
  for (auto &r : rectangles) {
    rect.setPosition({r.getRect().position.x, r.getRect().position.y});
    rect.setSize({r.getRect().size.x, r.getRect().size.y});
    if (r.getHasTexture())
      rect.setTexture(r.getTexture());
    window.draw(rect);
  }
}

int PointEngine::getRectCount()
{
  return rectangles.size();
}

using namespace std;

bool checkOverlap(float a1, float a2, float b1, float b2)
{
  return max(min(a1, a2), min(b1, b2)) <=
         min(max(a1, a2), max(b1, b2));
}
    bool lineCollision(vec2 line1Start, vec2 line1End, vec2 line2Start, vec2 line2End, vec2& intersection)
    {
      vec2 d1, d2;
      d1 = line1End - line1Start;
      d2 = line2End - line2Start;
      float denominator = d1.cross(d2);
      if(denominator != 0.f)
      {
        float c1 = vec2(line2Start - line1Start).cross(d2) / denominator;
        float c2 = vec2(line2Start - line1Start).cross(d1) / denominator;
        intersection = line1Start + c1 * d1;
        return c1 >= 0 && c1 <= 1 && c2 >= 0 && c2 <=1;
      }/*
      else if(vec2(line2Start-line1Start).cross(d1) == 0)
      {
        intersection = line1Start;
        return 
        checkOverlap(line1Start.x, line1End.x, line2Start.x, line2End.x) &&
        checkOverlap(line1Start.y, line1End.y, line2Start.y, line2End.y);
      }*/
      return false;
    }

CircleLineCollision PointEngine::circleLineCollision(sf::CircleShape cir, vec2 lineStart, vec2 lineEnd)
{
  vec2 segment = lineEnd-lineStart;
  vec2 segment2 = lineStart - lineEnd;
  vec2 diffStart = cir.getPosition() - lineStart;
  vec2 diffEnd = cir.getPosition() - lineEnd;
  vec2 center = segment * .5f;
  vec2 n = vec2(-segment.y, segment.x).normalized();
  if(diffEnd.dot(segment2) >= 0 && diffStart.dot(segment) >= 0)
  {
    vec2 point;
    if(abs(segment.x) > 0.f)
      point = vec2(diffStart.x, segment.y / segment.x * diffStart.x);
    else
      point = vec2(0, diffStart.y);

    vec2 diff2 = point - diffStart;
    float dist = n.dot(center - diff2);
    if(abs(dist) < cir.getRadius()) return CircleLineCollision(true, dist, n);
  }
  return CircleLineCollision(false, 0.f, n);
}