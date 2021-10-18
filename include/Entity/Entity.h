#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>

class Entity {
public:
    Entity(glm::vec3 entityPosition, glm::vec3 entitySize) : position(entityPosition), size(entitySize) {
        speed = glm::vec3(0.0f);
    }

    bool intersects(Entity &other);

    glm::vec3 getSpeed();
    void setSpeed(glm::vec3 aSpeed);

    glm::vec3 nextPosition(float time, glm::vec3 proposedSpeed);
    void move(float time);
    void accelerate(glm::vec3 accel, float time);

    glm::vec3 getPosition();

private:
    glm::vec3 position;
    glm::vec3 size;

    glm::vec3 speed;
};

#endif