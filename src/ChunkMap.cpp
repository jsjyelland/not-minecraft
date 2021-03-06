#include <ChunkMap/ChunkMap.h>

Chunk* ChunkMap::getChunk(std::vector<int> pos, bool generate) {
    Chunk* chunk = NULL;

    if (chunkMap.count(pos)) {
        chunk = chunkMap[pos];
    } else if (generate) {
        chunk = new Chunk(glm::vec2(pos[0], pos[1]), this);

        chunk->generate();

        chunkMap.insert(std::pair<std::vector<int>, Chunk*>(pos, chunk));

        addToRenderQueue(pos);

        std::vector<int> nextPos;
        Chunk *nextChunk;

        // Re-render chunks that are adjacent
        // south
        nextPos = std::vector<int>{pos[0] - 1, pos[1]};
        chunk->setSouth(getChunk(nextPos, false));
        addToRenderQueue(nextPos);
        
        // north
        nextPos = std::vector<int>{pos[0] + 1, pos[1]};
        chunk->setNorth(getChunk(nextPos, false));
        addToRenderQueue(nextPos);

        // west
        nextPos = std::vector<int>{pos[0], pos[1] - 1};
        chunk->setWest(getChunk(nextPos, false));
        addToRenderQueue(nextPos);
        
        // east
        nextPos = std::vector<int>{pos[0], pos[1] + 1};
        chunk->setEast(getChunk(nextPos, false));
        addToRenderQueue(nextPos);
    }
    
    return chunk;
}

void ChunkMap::addToRenderQueue(std::vector<int> pos) {
    if (std::find(renderQueue.begin(), renderQueue.end(), pos) == renderQueue.end()) {
        renderQueue.insert(renderQueue.begin(), pos);
    }
}

void ChunkMap::renderChunks(unsigned int max) {
    unsigned int count = 0;

    while (!renderQueue.empty()) {
        if (count++ > max) {
            return;
        }

        std::vector<int> pos = renderQueue.front();
        renderQueue.erase(renderQueue.begin());

        Chunk *chunk = getChunk(pos, false);

        if (chunk) {
            for (auto it = blocksToGen.cbegin(); it != blocksToGen.cend(); ) {
                std::vector<int> vec = it->first;
                if (chunk->setBlockType(glm::vec3(vec[0], vec[1], vec[2]), it->second, false, false)) {
                    blocksToGen.erase(it++);
                } else {
                    ++it;
                }
            }

            chunk->render();
        }
    }
}

BlockType ChunkMap::getBlock(glm::vec3 blockPos) {
    blockPos = glm::round(blockPos);

    if (blockPos.y < 0 || blockPos.y >= CHUNK_HEIGHT) {
        return BlockType::air;
    }

    float chunkX = floor(blockPos.x / CHUNK_SIZE);
    float chunkZ = floor(blockPos.z / CHUNK_SIZE);

    std::vector<int> chunkPos{(int)chunkX, (int)chunkZ};

    std::map<std::vector<int>, Chunk*>::iterator it = chunkMap.find(chunkPos);

    if (it != chunkMap.end()) {
        return it->second->getBlockType(blockPos);
    } else {
        return BlockType::air;
    }
}

bool ChunkMap::setBlock(glm::vec3 blockPos, BlockType type) {
    float chunkX = floor(blockPos.x / CHUNK_SIZE);
    float chunkZ = floor(blockPos.z / CHUNK_SIZE);

    std::vector<int> chunkPos{(int)chunkX, (int)chunkZ};

    std::map<std::vector<int>, Chunk*>::iterator it = chunkMap.find(chunkPos);

    if (it != chunkMap.end()) {
        it->second->setBlockType(blockPos, type, true, false);
        return true;
    }

    return false;
}

void ChunkMap::addToBlockGen(glm::vec3 blockPos, BlockType type) {
    blocksToGen.insert(std::pair<std::vector<int>, BlockType>(std::vector<int>{(int)blockPos.x, (int)blockPos.y, (int)blockPos.z}, type));
}

std::vector<Entity*>* ChunkMap::collidableNeighborBlocks(glm::vec3 pos) {
    pos = glm::round(pos);

    std::vector<Entity*>* ret = new std::vector<Entity*>();

    std::vector<glm::vec3> checkPositions = std::vector<glm::vec3>{
        pos,
        pos + glm::vec3(1.0f, 0.0f, 0.0f),
        pos + glm::vec3(1.0f, 0.0f, -1.0f),
        pos + glm::vec3(1.0f, 0.0f, 1.0f),
        pos + glm::vec3(-1.0f, 0.0f, 0.0f),
        pos + glm::vec3(-1.0f, 0.0f, -1.0f),
        pos + glm::vec3(-1.0f, 0.0f, 1.0f),
        pos + glm::vec3(0.0f, 0.0f, -1.0f),
        pos + glm::vec3(0.0f, 0.0f, 1.0f),

        pos + glm::vec3(1.0f, 1.0f, 0.0f),
        pos + glm::vec3(1.0f, 1.0f, -1.0f),
        pos + glm::vec3(1.0f, 1.0f, 1.0f),
        pos + glm::vec3(-1.0f, 1.0f, 0.0f),
        pos + glm::vec3(-1.0f, 1.0f, -1.0f),
        pos + glm::vec3(-1.0f, 1.0f, 1.0f),
        pos + glm::vec3(0.0f, 1.0f, 0.0f),
        pos + glm::vec3(0.0f, 1.0f, -1.0f),
        pos + glm::vec3(0.0f, 1.0f, 1.0f),

        pos + glm::vec3(1.0f, -1.0f, 0.0f),
        pos + glm::vec3(1.0f, -1.0f, -1.0f),
        pos + glm::vec3(1.0f, -1.0f, 1.0f),
        pos + glm::vec3(-1.0f, -1.0f, 0.0f),
        pos + glm::vec3(-1.0f, -1.0f, -1.0f),
        pos + glm::vec3(-1.0f, -1.0f, 1.0f),
        pos + glm::vec3(0.0f, -1.0f, 0.0f),
        pos + glm::vec3(0.0f, -1.0f, -1.0f),
        pos + glm::vec3(0.0f, -1.0f, 1.0f),
    };

    for (glm::vec3 v : checkPositions) {
        BlockType type = getBlock(v);
        
        if (Block::collidable(type)) {
            ret->insert(ret->begin(), Block::createEntity(v));
        }
    }
    
    return ret;
}   