#include <ChunkMap/ChunkMap.h>

ChunkMap::ChunkMap() {

}

Chunk* ChunkMap::getChunk(std::vector<int> pos) {
    Chunk* chunk;

    if (!chunkMap.count(pos)) {
        chunk = new Chunk(glm::vec2(pos[0], pos[1]));
        chunkMap.insert(std::pair<std::vector<int>, Chunk*>(pos, chunk));
    } else {
        chunk = chunkMap[pos];
    }
    
    return chunk;
}