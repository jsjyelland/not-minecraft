#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include <vector>

#include <math.h>
#include <stdio.h>

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
    wood
};

class Block {
public:
    static std::vector<unsigned int> atlasPosXY(unsigned int atlasPos);
    static std::vector<unsigned int> atlasMap(BlockType type);

    static bool isSolid(BlockType type);
    static bool isTranslucent(BlockType type);

    static void constructMesh(BlockType type, float blockheight, int chunkX, int chunkY, int chunkZ, unsigned int directionMask, std::vector<float> &data);
};

#endif