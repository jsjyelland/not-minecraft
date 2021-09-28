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

#include <Block/Block.h>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

class ChunkMap {
public:
    ChunkMap();

    void render(int x, int z);
    unsigned int chunkVAO(int x, int z);
    unsigned int numVertices(int x, int z);

    static glm::mat4 chunkModelMat(int x, int z);

private:
    std::map<std::vector<int>, unsigned int> VAOmap;
    std::map<std::vector<int>, unsigned int> vertexNumMap;

    BlockType getBlockType(int x, int y, int z);
};

#endif