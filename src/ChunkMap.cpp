#include <ChunkMap/ChunkMap.h>

void ChunkMap::render(int x, int y) {
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int k = 0; k < CHUNK_SIZE; k++) {
            float height = (perlin(i / 64.0f, k / 64.0f) + 0.4) * (float)CHUNK_HEIGHT;

            for (int j = 0; j < CHUNK_HEIGHT; j++) {
            }
        }
    }
}

unsigned int ChunkMap::chunkVAO(int x, int y) {
    std::vector<int> vect(x, y);

    if (VAOmap.find(vect) == VAOmap.end()) {
        render(x, y);
    }

    return VAOmap[vect];
}