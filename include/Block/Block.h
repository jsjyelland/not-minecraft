#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include <glm/glm.hpp>

#include <vector>
#include <iostream>

#include <math.h>
#include <stdio.h>

#include <Entity/Entity.h>

// Note:
// North is positive x,
// East is positive z.

#define DIRECTION_TOP 0b1
#define DIRECTION_NORTH 0b10
#define DIRECTION_EAST 0b100
#define DIRECTION_SOUTH 0b1000
#define DIRECTION_WEST 0b10000
#define DIRECTION_BOTTOM 0b100000
#define DIRECTION_ALL 0b111111

enum BlockType {
    air,
    grass,
    dirt,
    stone,
    water,
    wood,
    leaves,
    sand,
    flower,
    short_grass,
    wood_planks,
    cobblestone,
    glass,
};

class Block {
public:
    static std::vector<unsigned int> atlasPosXY(unsigned int atlasPos);
    static std::vector<unsigned int> atlasMap(BlockType type);

    static glm::vec3 blockFacePos(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 selectedBlockPos);

    static bool isBlock(BlockType type);
    static bool isSolid(BlockType type);
    static bool isTranslucent(BlockType type);
    static bool isSprite(BlockType type);
    static bool collidable(BlockType type);

    static void constructSpriteMesh(BlockType type, int chunkX, int chunkY, int chunkZ, std::vector<float> &data);
    static void constructMesh(BlockType type, float blockheight, int chunkX, int chunkY, int chunkZ, unsigned int directionMask, std::vector<float> &data);

    static Entity* createEntity(glm::vec3 pos);
};

#endif