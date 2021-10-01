#include <ChunkMap/ChunkMap.h>

ChunkMap::ChunkMap() {
    
}

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
            chunk->render();
        }
    }
}

BlockType ChunkMap::getBlock(glm::vec3 blockPos) {
    if (round(blockPos.y) < 0 || round(blockPos.y) >= CHUNK_HEIGHT) {
        return BlockType::air;
    }

    float chunkX = floor(round(blockPos.x) / CHUNK_SIZE);
    float chunkZ = floor(round(blockPos.z) / CHUNK_SIZE);

    std::vector<int> chunkPos{(int)chunkX, (int)chunkZ};

    std::map<std::vector<int>, Chunk*>::iterator it = chunkMap.find(chunkPos);

    if (it != chunkMap.end()) {
        return it->second->getBlockType(blockPos);
    } else {
        return BlockType::air;
    }
}

void ChunkMap::setBlock(glm::vec3 blockPos, BlockType type) {
    float chunkX = floor(round(blockPos.x) / CHUNK_SIZE);
    float chunkZ = floor(round(blockPos.z) / CHUNK_SIZE);

    std::vector<int> chunkPos{(int)chunkX, (int)chunkZ};

    std::map<std::vector<int>, Chunk*>::iterator it = chunkMap.find(chunkPos);

    if (it != chunkMap.end()) {
        it->second->setBlockType(blockPos, type);
    }
}