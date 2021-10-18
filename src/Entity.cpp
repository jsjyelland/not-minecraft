#include <Entity/Entity.h>

bool Entity::intersects(Entity &other) {
    glm::vec3 min = position + (size / 2.0f);
    glm::vec3 max = position + (size / 2.0f);
    glm::vec3 omin = other.position + (other.size / 2.0f);
    glm::vec3 omax = other.position + (other.size / 2.0f);

    return glm::all(glm::greaterThan(max, omin) && glm::lessThan(min, omax));
}

glm::vec3 Entity::getSpeed() {
    return speed;
}

void Entity::setSpeed(glm::vec3 aSpeed) {
    speed = aSpeed;
}

glm::vec3 Entity::nextPosition(float time, glm::vec3 proposedSpeed) {
    return position + proposedSpeed * time;
}

void Entity::move(float time) {
    position = nextPosition(time, speed);
}

void Entity::accelerate(glm::vec3 accel, float time) {
    speed += accel * time;
}

glm::vec3 Entity::getPosition() {
    return position;
}