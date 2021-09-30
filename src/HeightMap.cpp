#include <WorldGen/HeightMap.h>

unsigned int heightGen(glm::vec2 pos) {
    float noise = (glm::simplex(pos / 128.0f) + 1.2) / 4;
    unsigned int height = (unsigned int)(noise * CHUNK_HEIGHT);

    return height;
}