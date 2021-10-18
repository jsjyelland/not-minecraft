#ifndef PLAYER_H
#define PLAYER_H

#include <Entity/Entity.h>

#include <Inventory/Inventory.h>

#define PLAYER_HEIGHT 1.8f
#define PLAYER_SIZE 0.6f

class Player: public Entity {
public:
    Player(glm::vec3 playerPosition);

    Inventory hotbar;
};

#endif