#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <glad/glad.h>

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <map>
#include <vector>

#include <Block/Block.h>

#include <Chunk/Chunk.h>

class ChunkMap {
public:
    ChunkMap();

    Chunk* getChunk(std::vector<int> pos);

private:
    std::map<std::vector<int>, Chunk*> chunkMap;
};

#endif