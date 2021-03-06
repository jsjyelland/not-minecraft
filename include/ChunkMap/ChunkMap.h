#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <map>
#include <vector>
#include <algorithm>

#include <Block/Block.h>

#include <Chunk/Chunk.h>

// Forward declaration
class Chunk;

class ChunkMap {
public:
    ChunkMap() {}

    Chunk* getChunk(std::vector<int> pos, bool generate);

    void addToRenderQueue(std::vector<int> pos);
    void addToBlockGen(glm::vec3 blockPos, BlockType type);

    std::vector<Entity*>* collidableNeighborBlocks(glm::vec3 pos);
    
    BlockType getBlock(glm::vec3 blockPos);
    bool setBlock(glm::vec3 blockPos, BlockType type);
    void renderChunks(unsigned int max);

private:
    std::map<std::vector<int>, BlockType> blocksToGen;
    std::map<std::vector<int>, Chunk*> chunkMap;
    std::vector<std::vector<int>> renderQueue;
};

#endif