#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <map>
#include <vector>

#include <Block/Block.h>

#include <Chunk/Chunk.h>

// Forward declaration
class Chunk;

class ChunkMap {
public:
    ChunkMap();

    Chunk* getChunk(std::vector<int> pos, bool generate);
    
    BlockType getBlock(glm::vec3 blockPos);
    void setBlock(glm::vec3 blockPos, BlockType type);
    void genChunks(unsigned int max);

private:
    std::map<std::vector<int>, Chunk*> chunkMap;
    std::vector<Chunk*> genQueue;
};

#endif