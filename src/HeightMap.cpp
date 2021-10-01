#include <WorldGen/HeightMap.h>

unsigned int heightGen(glm::vec2 pos) {
    float noise = (glm::simplex(pos / 128.0f) + 1.2) / 4;
    unsigned int height = (unsigned int)(noise * CHUNK_HEIGHT);

    return height;
}

bool treeGen(glm::vec2 pos) {
    float noise = glm::simplex(pos / 2.0f) - 0.8f;
    return noise > 0;
}

bool flowerGen(glm::vec2 pos) {
    float noise = glm::simplex((pos + glm::vec2(2, 2)) / 1.2f) - 0.8f;
    return noise > 0;
}

bool grassGen(glm::vec2 pos) {
    float noise = glm::simplex((pos + glm::vec2(-1, 3)) / 4.0f) - 0.6f;
    return noise > 0;
}