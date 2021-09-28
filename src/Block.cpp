#include <Block/Block.h>

/**
 * Vector order:
 * top, north, east, south, west, bottom
 */
std::vector<unsigned int> Block::atlasMap(BlockType type) {
    std::vector<unsigned int> vec;

    switch (type) {
        case BlockType::dirt:
            vec = {2, 2, 2, 2, 2, 2};
            break;

        case BlockType::grass:
            vec = {0, 1, 1, 1, 1, 2};
            break;

        case BlockType::stone:
            vec = {3, 3, 3, 3, 3, 3};
            break;
    }

    return vec;
}

/**
 * Return the atlas position offset in both x & y
 */
std::vector<unsigned int> Block::atlasPosXY(unsigned int atlasPos) {
    unsigned int x = atlasPos % 16;
    unsigned int y = 15 - atlasPos / 16;

    std::vector<unsigned int> ret(2);

    ret = {x, y};

    return ret;
}

/**
 * Construct the mesh data for a block.
 * Has the following construction:
 * 3x vertex point, 3x normal direction, 2x atlas coord
 * 
 * In order of faces:
 * top, north, east, south, west, bottom
 */
float* Block::constructMesh(BlockType type, int chunkX, int chunkY, size_t *size) {
    std::vector<unsigned int> atlas = atlasMap(type);

    float topOffsetX = (float)atlasPosXY(atlas[0])[0];
    float topOffsetY = (float)atlasPosXY(atlas[0])[1];
    float northOffsetX = (float)atlasPosXY(atlas[1])[0];
    float northOffsetY = (float)atlasPosXY(atlas[1])[1];
    float eastOffsetX = (float)atlasPosXY(atlas[2])[0];
    float eastOffsetY = (float)atlasPosXY(atlas[2])[1];
    float southOffsetX = (float)atlasPosXY(atlas[3])[0];
    float southOffsetY = (float)atlasPosXY(atlas[3])[1];
    float westOffsetX = (float)atlasPosXY(atlas[4])[0];
    float westOffsetY = (float)atlasPosXY(atlas[4])[1];
    float bottomOffsetX = (float)atlasPosXY(atlas[5])[0];
    float bottomOffsetY = (float)atlasPosXY(atlas[5])[1];

    float* vertices;
    vertices = new float[]{
        // Top
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f + topOffsetX, 0.0f + topOffsetY,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f + topOffsetX, 1.0f + topOffsetY,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f + topOffsetX, 0.0f + topOffsetY,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f + topOffsetX, 1.0f + topOffsetY,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f + topOffsetX, 0.0f + topOffsetY,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f + topOffsetX, 1.0f + topOffsetY,

        // North
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f + northOffsetX, 1.0f + northOffsetY,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f + northOffsetX, 0.0f + northOffsetY,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f + northOffsetX, 1.0f + northOffsetY,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f + northOffsetX, 0.0f + northOffsetY,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f + northOffsetX, 1.0f + northOffsetY,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f + northOffsetX, 0.0f + northOffsetY,

        // East
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f + eastOffsetX, 0.0f + eastOffsetY,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f + eastOffsetX, 0.0f + eastOffsetY,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f + eastOffsetX, 1.0f + eastOffsetY,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f + eastOffsetX, 1.0f + eastOffsetY,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f + eastOffsetX, 1.0f + eastOffsetY,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f + eastOffsetX, 0.0f + eastOffsetY,

        // South
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f + southOffsetX, 1.0f + southOffsetY,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f + southOffsetX, 1.0f + southOffsetY,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f + southOffsetX, 0.0f + southOffsetY,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f + southOffsetX, 0.0f + southOffsetY,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f + southOffsetX, 0.0f + southOffsetY,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f + southOffsetX, 1.0f + southOffsetY,

        // West
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f + westOffsetX, 0.0f + westOffsetY,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f + westOffsetX, 1.0f + westOffsetY,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f + westOffsetX, 0.0f + westOffsetY,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f + westOffsetX, 1.0f + westOffsetY,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f + westOffsetX, 0.0f + westOffsetY,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f + westOffsetX, 1.0f + westOffsetY,

        // Bottom
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f + bottomOffsetX, 0.0f + bottomOffsetY,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f + bottomOffsetX, 0.0f + bottomOffsetY,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f + bottomOffsetX, 1.0f + bottomOffsetY,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f + bottomOffsetX, 1.0f + bottomOffsetY,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f + bottomOffsetX, 1.0f + bottomOffsetY,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f + bottomOffsetX, 0.0f + bottomOffsetY,
    };

    *size = sizeof(vertices);

    return vertices;
}