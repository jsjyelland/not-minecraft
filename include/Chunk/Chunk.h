#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <array>
#include <vector>

#include <Block/Block.h>

#include <Shader/Shader.h>

#include <WorldGen/HeightMap.h>

#include <ChunkMap/ChunkMap.h>

#include <GLFW/glfw3.h>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT

// Forward declaration
class ChunkMap;

class Chunk {
public:
    Chunk(glm::vec2 chunkPos, ChunkMap *chunkMap);

    void setNorth(Chunk *chunk);
    void setEast(Chunk *chunk);
    void setSouth(Chunk *chunk);
    void setWest(Chunk *chunk);

    void generate();
    void render();
    void draw(Shader &shader);

    BlockType getBlockType(glm::vec3 blockPos);
    bool setBlockType(glm::vec3 blockPos, BlockType type, bool doRender, bool addToGen);

    glm::vec2 getPos();

private:
    bool empty = true;

    Chunk *north;
    Chunk *east;
    Chunk *south;
    Chunk *west;

    ChunkMap *map;

    std::vector<float> solidVerticesVector;
    std::vector<float> translucentVerticesVector;

    unsigned int solidVAO, translucentVAO;
    unsigned int solidVBO, translucentVBO;

    BlockType blockMap[CHUNK_VOLUME] = {BlockType::air};

    glm::vec2 pos;
    glm::mat4 model;
    
    bool inChunk(glm::vec3 blockPos);
    BlockType getBlockTypeInternal(int chunkX, int chunkY, int chunkZ);

    unsigned int numSolidVertices, numTranslucentVertices;
};

#endif