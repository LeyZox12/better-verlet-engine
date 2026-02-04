#include "../include/PointEngine.h"
#include <algorithm>
#include <memory>
#include <utility>
#include "CollisionContext.hpp"

PointEngine::PointEngine() : threadPool(ThreadPool(THREADS)) {
  // ctor
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
  points[points.size()-1].setIndex(points.size()-1);
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


void PointEngine::addRectangle(Rect<int> rect, float friction) {
  rectangles.push_back(Rectangle(rect, friction));
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
  if (constraintDistance == 0) {
    vec2 diff = points[i1].getPos() - points[i2].getPos();
    constraintDistance = hypot(diff.x, diff.y);
  }
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
  for (auto &p : points) {
    vec2 currPos = p.getPos();
    vec2 currOldPos = p.getOldPos();
    vec2 currAcc = p.getAcc();
    p.setAcc({0, 999.8f * p.getGravityScale()});
    // ²p.setAcc({p.getAcc() * 0.99f});
    vec2 newPos = currPos * 2.0f - currOldPos + currAcc * dt * dt;
    p.setOldPos(currPos);
    p.setPos(newPos, false);
    p.onUpdate(OnUpdateContext(*this, mousepos, p.args, index));
    index++;
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
      for (int _ = 0; _ < substeps; _++) {
        for (int j = 0; j < points.size(); j++) {
          Point &p = points[j];
          for (int i = 0; i < points.size(); i++)
            if ((p.getShouldCollide() ||
                points[i].getShouldCollide()) && i != j &&
                count_if(constraints.begin(), constraints.end(),
                 [i, j](auto& c)
                 {return c.getIndexes() == std::make_pair(i, j) || c.getIndexes() == std::make_pair(j,i);}) == 0) {
              Point *p2 = &points[i];
              float r1 = p.getRadius();   // radius point 1
              float r2 = p2->getRadius(); // radius point 2
              float rc = r1 + r2;         // radius combined,
              vec2 dir = p.getPos() - p2->getPos();
              float dist = hypot(dir.x, dir.y);
              if (dist > 0.f && dist < rc) {
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
          static sf::CircleShape sprite;
          sprite.setPosition(p.getPos());
          sprite.setOrigin({p.getRadius(), p.getRadius()});
          sprite.setRadius(p.getRadius());
          for (int r = 0; r < rectangles.size(); r++) {
            int collision =
                circleRectCollision(rectangles[r].getRect(), sprite);
            if(collision > NONE)
              p.onCollision(CollisionContext(false, r));
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
          }
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

void PointEngine::display(sf::RenderWindow &window, sf::Color color) {
  const int VERTEX_CIRCLE = 10;
  float ratio = (2 * PI) / VERTEX_CIRCLE;
  sf::VertexArray batch(sf::PrimitiveType::Triangles,
                    points.size() * VERTEX_CIRCLE * 3);
  int pIndex = 0;
  for (auto &p : points) {
    for (int i = 0; i < VERTEX_CIRCLE; i++) {
      int index = pIndex * VERTEX_CIRCLE * 3 + i * 3;
      batch[index].position = p.getPos();
      batch[index + 1].position =
          p.getPos() + vec2(cos(ratio * index) * p.getRadius(),
                            sin(ratio * index) * p.getRadius());
      batch[index + 2].position =
          p.getPos() + vec2(cos(ratio * (index + 1)) * p.getRadius(),
                            sin(ratio * (index + 1)) * p.getRadius());
      batch[index].color = color;
      batch[index + 1].color = color;
      batch[index + 2].color = color;
    }
    pIndex++;
  }
  window.draw(batch);
  VertexArray line(PrimitiveType::LineStrip, 2);
  line[0].color = color;
  line[1].color = color;
  for (auto &c : constraints) {
    if (c.getVisible()) {
      Point p1 = points[c.getIndexes().first];
      Point p2 = points[c.getIndexes().second];
      line[0].position = p1.getPos();
      line[1].position = p2.getPos();
      window.draw(line);
    }
  }
  for (auto &r : rectangles) {
    r.setColor(color);
    r.draw(window);
  }
}

void PointEngine::displayAsRects(RenderWindow &window, Color color,
                                 float width) {
  RectangleShape sprite;
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
