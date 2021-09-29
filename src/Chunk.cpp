#include <Chunk/Chunk.h>

Chunk::Chunk(glm::vec2 chunkPos, ChunkMap *chunkMap) {
    map = chunkMap;
    pos = chunkPos;

    generate();
    
    // Generate VAO
    glGenVertexArrays(1, &VAO);

    // Model transform
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x * CHUNK_SIZE, 0, pos.y * CHUNK_SIZE));

    render();
}

void Chunk::generate() {
    // Generate heightmap
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            BlockType type;
            unsigned int height = heightGen(glm::vec2(pos.x * CHUNK_SIZE + x, pos.y * CHUNK_SIZE + z));

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y > height || y < 0) {
                    type = BlockType::air;
                } else if (y < 32 || y < height - 3) {
                    type = BlockType::stone;
                } else if (y < height - 1) {
                    type = BlockType::dirt;
                } else if (y < height) {
                    type = BlockType::grass;
                } else {
                    type = BlockType::air;
                }

                blockMap[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z] = type;
            }            
        }
    }
}

void Chunk::render() {
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    size_t verticesSize;
    float* vertices;

    numVertices = 0;

    int chunkOffsetX = pos.x * CHUNK_SIZE;
    int chunkOffsetZ = pos.y * CHUNK_SIZE;

    std::vector<float> verticesVector;

    // Generate meshes
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int k = 0; k < CHUNK_SIZE; k++) {
            for (int j = 0; j < CHUNK_HEIGHT; j++) {
                glm::vec3 blockPos = glm::vec3(i + chunkOffsetX, j, k + chunkOffsetZ);

                BlockType type = getBlockType(blockPos);

                if (type == BlockType::air) {
                    continue;
                }

                // Generate vertex data

                unsigned int directionMask = 0;

                if (getBlockType(blockPos + glm::vec3(-1, 0, 0)) == BlockType::air) {
                    directionMask |= DIRECTION_SOUTH;
                }

                if (getBlockType(blockPos + glm::vec3(1, 0, 0)) == BlockType::air) {
                    directionMask |= DIRECTION_NORTH;
                }

                if (getBlockType(blockPos + glm::vec3(0, 1, 0)) == BlockType::air) {
                    directionMask |= DIRECTION_TOP;
                }

                if (getBlockType(blockPos + glm::vec3(0, -1, 0)) == BlockType::air) {
                    directionMask |= DIRECTION_BOTTOM;
                }

                if (getBlockType(blockPos + glm::vec3(0, 0, -1)) == BlockType::air) {
                    directionMask |= DIRECTION_WEST;
                }

                if (getBlockType(blockPos + glm::vec3(0, 0, 1)) == BlockType::air) {
                    directionMask |= DIRECTION_EAST;
                }

                if (directionMask) {
                    Block::constructMesh(type, i, j, k, directionMask, verticesVector);
                }
            }
        }
    }

    numVertices = verticesVector.size() / 8;
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * numVertices, verticesVector.data(), GL_STATIC_DRAW);

    verticesVector.clear();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chunk::draw(Shader &shader) {
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
}

glm::vec2 Chunk::getPos() {
    return pos;
}

#define MOD(x, y) ((x % y) + y) % y

BlockType Chunk::getBlockType(glm::vec3 blockPos) {
    if (blockPos.y < 0 || blockPos.y > 127.0) {
        return BlockType::air;
    }

    if (inChunk(blockPos)) {
        int blockX = MOD((int)round(blockPos.x), CHUNK_SIZE);
        int blockZ = MOD((int)round(blockPos.z), CHUNK_SIZE);
        int blockY = (int)round(blockPos.y);

        int index = blockX * CHUNK_HEIGHT * CHUNK_SIZE + blockY * CHUNK_SIZE + blockZ;

        return blockMap[index];
    } else {
        return map->getBlock(blockPos);
    }
}

void Chunk::setBlockType(glm::vec3 blockPos, BlockType type) {
    if (!inChunk(blockPos)) {
        return;
    }

    int blockX = MOD((int)round(blockPos.x), CHUNK_SIZE);
    int blockZ = MOD((int)round(blockPos.z), CHUNK_SIZE);
    int blockY = (int)round(blockPos.y);

    int index = blockX * CHUNK_HEIGHT * CHUNK_SIZE + blockY * CHUNK_SIZE + blockZ;

    blockMap[index] = type;

    render();
}

bool Chunk::inChunk(glm::vec3 blockPos) {
    float chunkX = floor(blockPos.x / CHUNK_SIZE);
    float chunkZ = floor(blockPos.z / CHUNK_SIZE);

    return chunkX == pos.x && chunkZ == pos.y;
}