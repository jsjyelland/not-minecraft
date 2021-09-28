#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <vector>

#include <Block/Block.h>

#include <Shader/Shader.h>

#include <WorldGen/HeightMap.h>

#include <ChunkMap/ChunkMap.h>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT

// Forward declaration
class ChunkMap;

class Chunk {
public:
    Chunk(glm::vec2 chunkPos, ChunkMap *chunkMap);

    void render();
    void draw(Shader &shader);

    BlockType getBlockType(glm::vec3 blockPos);

    glm::vec2 getPos();

private:
    ChunkMap *map;

    unsigned int VAO;

    std::array<BlockType, CHUNK_VOLUME> blockMap;

    glm::vec2 pos;
    glm::mat4 model;

    void generate();
    
    bool inChunk(glm::vec3 blockPos);

    unsigned int numVertices;
};

#endif