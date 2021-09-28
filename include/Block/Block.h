#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include <vector>

#include <math.h>
#include <stdio.h>

// Note:
// North is positive x,
// East is positive z.

enum BlockType {
    grass,
    dirt,
    stone
};

class Block {
public:
    static std::vector<unsigned int> atlasPosXY(unsigned int atlasPos);
    static std::vector<unsigned int> atlasMap(BlockType type);
    static float* constructMesh(BlockType type, int chunkX, int chunkY, int chunkZ, size_t *size);
};

#endif