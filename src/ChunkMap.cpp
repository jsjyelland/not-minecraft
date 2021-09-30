#include <ChunkMap/ChunkMap.h>

ChunkMap::ChunkMap() {
    
}

Chunk* ChunkMap::getChunk(std::vector<int> pos, bool generate) {
    Chunk* chunk = NULL;

    if (chunkMap.count(pos)) {
        chunk = chunkMap[pos];
    } else if (generate) {
        chunk = new Chunk(glm::vec2(pos[0], pos[1]), this);
        chunkMap.insert(std::pair<std::vector<int>, Chunk*>(pos, chunk));
        genQueue.insert(genQueue.begin(), chunk);
    }
    
    return chunk;
}

void ChunkMap::genChunks(unsigned int max) {
    unsigned int count = 0;

    while (!genQueue.empty()) {
        if (count++ > max) {
            return;
        }

        double lastTime = glfwGetTime();

        Chunk *chunk = genQueue.front();
        genQueue.erase(genQueue.begin());

        chunk->generate();
        chunk->render();

        std::vector<int> pos{(int)chunk->getPos().x, (int)chunk->getPos().y};

        // Re-render chunks that are adjacent
        Chunk *nextChunk;

        // south
        nextChunk = getChunk(std::vector<int>{pos[0] - 1, pos[1]}, false);
        if (nextChunk) nextChunk->render();
        
        // north
        nextChunk = getChunk(std::vector<int>{pos[0] + 1, pos[1]}, false);
        if (nextChunk) nextChunk->render();

        // west
        nextChunk = getChunk(std::vector<int>{pos[0], pos[1] - 1}, false);
        if (nextChunk) nextChunk->render();
        
        // east
        nextChunk = getChunk(std::vector<int>{pos[0], pos[1] + 1}, false);
        if (nextChunk) nextChunk->render();

        std::cout << "chunkgen: " << glfwGetTime() - lastTime << std::endl;
    }
}

BlockType ChunkMap::getBlock(glm::vec3 blockPos) {
    float chunkX = floor(round(blockPos.x) / CHUNK_SIZE);
    float chunkZ = floor(round(blockPos.z) / CHUNK_SIZE);

    std::vector<int> chunkPos{(int)chunkX, (int)chunkZ};

    if (chunkMap.count(chunkPos)) {
        return chunkMap[chunkPos]->getBlockType(blockPos);
    } else {
        return BlockType::air;
    }
}

void ChunkMap::setBlock(glm::vec3 blockPos, BlockType type) {
    float chunkX = floor(round(blockPos.x) / CHUNK_SIZE);
    float chunkZ = floor(round(blockPos.z) / CHUNK_SIZE);

    std::vector<int> chunkPos{(int)chunkX, (int)chunkZ};

    if (chunkMap.count(chunkPos)) {
        chunkMap[chunkPos]->setBlockType(blockPos, type);
    }
}