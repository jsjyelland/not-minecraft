#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <glad/glad.h>

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <vector>

#include <perlin.h>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 64

class ChunkMap {
public:
    ChunkMap();

    void render(int x, int y);
    unsigned int chunkVAO(int x, int y);

private:
    std::map<std::vector<int>, unsigned int> VAOmap;
};

#endif