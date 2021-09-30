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

        // Re-render chunks that are adjacent
        Chunk *chunk;

        // south
        chunk = getChunk(std::vector<int>{pos[0] - 1, pos[1]}, false);
        if (chunk) chunk->render();
        
        // north
        chunk = getChunk(std::vector<int>{pos[0] + 1, pos[1]}, false);
        if (chunk) chunk->render();

        // west
        chunk = getChunk(std::vector<int>{pos[0], pos[1] - 1}, false);
        if (chunk) chunk->render();
        
        // east
        chunk = getChunk(std::vector<int>{pos[0], pos[1] + 1}, false);
        if (chunk) chunk->render();
    }
    
    return chunk;
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