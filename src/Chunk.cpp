#include <Chunk/Chunk.h>

Chunk::Chunk(glm::vec2 chunkPos, ChunkMap *chunkMap) {
    numSolidVertices = 0;
    numTranslucentVertices = 0;

    map = chunkMap;
    pos = chunkPos;
    
    // Generate VAO
    glGenVertexArrays(1, &solidVAO);
    glGenVertexArrays(1, &translucentVAO);

    // VBO
    glGenBuffers(1, &solidVBO);
    glGenBuffers(1, &translucentVBO);

    // Model transform
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x * CHUNK_SIZE, 0, pos.y * CHUNK_SIZE));
}

void Chunk::generate() {
    double lastTime = glfwGetTime();

    BlockType type;
    int height;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            height = heightGen(glm::vec2(pos.x * CHUNK_SIZE + x, pos.y * CHUNK_SIZE + z));

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y >= height && y < 40) {
                    type = BlockType::water;
                } else if (y < 32 || y < height - 2) {
                    type = BlockType::stone;
                } else if (y < height - 1) {
                    type = BlockType::dirt;
                } else if (y < height) {
                    type = BlockType::grass;
                } else {
                    type = BlockType::air;
                }

                if (type != BlockType::air) {
                    blockMap[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z] = type;
                }
            }
        }
    }

    empty = false;

    std::cout << "generate: " << glfwGetTime() - lastTime << std::endl;
}

void Chunk::render() {
    if (empty) {
        return;
    }

    double lastTime = glfwGetTime();
    // Reset vertex count
    numSolidVertices = 0;
    numTranslucentVertices = 0;

    BlockType type;
    glm::vec3 blockPos;
    unsigned int directionMask = 0;

    int i, j, k;

    // Loop through each block in the chunk to generate the mesh
    for (int q = 0; q < CHUNK_VOLUME; q++) {
        type = blockMap[q];

        if (type == BlockType::air) {
            continue;
        }

        i = q / (CHUNK_SIZE * CHUNK_HEIGHT);
        j = (q / CHUNK_SIZE) % CHUNK_HEIGHT;
        k = q % CHUNK_SIZE;

        // Generate vertex data
        directionMask = 0;
        
        if (Block::isSolid(type)) {
            // Solid mesh

            if (!Block::isSolid(getBlockTypeInternal(i - 1, j, k))) {
                directionMask |= DIRECTION_SOUTH;
            }

            if (!Block::isSolid(getBlockTypeInternal(i + 1, j, k))) {
                directionMask |= DIRECTION_NORTH;
            }

            if (!Block::isSolid(getBlockTypeInternal(i, j + 1, k))) {
                directionMask |= DIRECTION_TOP;
            }

            if (!Block::isSolid(getBlockTypeInternal(i, j - 1, k))) {
                directionMask |= DIRECTION_BOTTOM;
            }

            if (!Block::isSolid(getBlockTypeInternal(i, j, k - 1))) {
                directionMask |= DIRECTION_WEST;
            }

            if (!Block::isSolid(getBlockTypeInternal(i, j, k + 1))) {
                directionMask |= DIRECTION_EAST;
            }

            if (directionMask) {
                Block::constructMesh(type, i, j, k, directionMask, solidVerticesVector);
            }
        } else if (Block::isTranslucent(type)) {
            // Translucent mesh

            if (!Block::isTranslucent(getBlockTypeInternal(i - 1, j, k))) {
                directionMask |= DIRECTION_SOUTH;
            }

            if (!Block::isTranslucent(getBlockTypeInternal(i + 1, j, k))) {
                directionMask |= DIRECTION_NORTH;
            }

            if (!Block::isTranslucent(getBlockTypeInternal(i, j + 1, k))) {
                directionMask |= DIRECTION_TOP;
            }

            if (!Block::isTranslucent(getBlockTypeInternal(i, j - 1, k))) {
                directionMask |= DIRECTION_BOTTOM;
            }

            if (!Block::isTranslucent(getBlockTypeInternal(i, j, k - 1))) {
                directionMask |= DIRECTION_WEST;
            }

            if (!Block::isTranslucent(getBlockTypeInternal(i, j, k + 1))) {
                directionMask |= DIRECTION_EAST;
            }

            if (directionMask) {
                Block::constructMesh(type, i, j, k, directionMask, translucentVerticesVector);
            }
        }
    }

    std::cout << "mesh: " << glfwGetTime() - lastTime << std::endl;
    numSolidVertices = solidVerticesVector.size() / 8;
    numTranslucentVertices = translucentVerticesVector.size() / 8;

    // Solid
    glBindVertexArray(solidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * numSolidVertices, solidVerticesVector.data(), GL_STATIC_DRAW);

    solidVerticesVector.clear();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Translucent
    glBindVertexArray(translucentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, translucentVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * numTranslucentVertices, translucentVerticesVector.data(), GL_STATIC_DRAW);

    translucentVerticesVector.clear();

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
    if (numSolidVertices + numTranslucentVertices == 0) {
        return;
    }

    shader.setMat4("model", model);

    glBindVertexArray(solidVAO);
    glDrawArrays(GL_TRIANGLES, 0, numSolidVertices);

    // Disable back-face culling
    // glDisable(GL_CULL_FACE);

    // Scale down ever so slightly
    // glm::mat4 transModel = glm::mat4(1.0f);
    // transModel = glm::scale(transModel, glm::vec3(0.999f));
    // transModel = glm::translate(transModel, glm::vec3(pos.x * CHUNK_SIZE, 0, pos.y * CHUNK_SIZE));

    // shader.setMat4("model", transModel);

    glBindVertexArray(translucentVAO);
    glDrawArrays(GL_TRIANGLES, 0, numTranslucentVertices);

    // Re-enable
    // glEnable(GL_CULL_FACE);

    glBindVertexArray(0);
}

glm::vec2 Chunk::getPos() {
    return pos;
}

#define MOD(x, y) ((x % y) + y) % y

BlockType Chunk::getBlockType(glm::vec3 blockPos) {
    if (blockPos.y < 0 || blockPos.y >= CHUNK_HEIGHT) {
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

BlockType Chunk::getBlockTypeInternal(int chunkX, int chunkY, int chunkZ) {
    if (chunkY < 0 || chunkY >= CHUNK_HEIGHT) {
        return BlockType::air;
    } 

    if (chunkX < 0 || chunkZ < 0 || chunkX > 15 || chunkZ > 15) {
        int chunkOffsetX = pos.x * CHUNK_SIZE;
        int chunkOffsetZ = pos.y * CHUNK_SIZE;

        return map->getBlock(glm::vec3(chunkX + chunkOffsetX, chunkY, chunkZ + chunkOffsetZ));
    }

    return blockMap[chunkX * CHUNK_HEIGHT * CHUNK_SIZE + chunkY * CHUNK_SIZE + chunkZ];
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

    if (blockX == 0) {
        // re-render south chunk

        Chunk *chunk = map->getChunk(std::vector<int>{(int)pos.x - 1, (int)pos.y}, true);
        chunk->render();
    } else if (blockX == 15) {
        // north

        Chunk *chunk = map->getChunk(std::vector<int>{(int)pos.x + 1, (int)pos.y}, true);
        chunk->render();
    }
    
    if (blockZ == 0) {
        // west

        Chunk *chunk = map->getChunk(std::vector<int>{(int)pos.x, (int)pos.y - 1}, true);
        chunk->render();
    } else if (blockZ == 15) {
        // east
        Chunk *chunk = map->getChunk(std::vector<int>{(int)pos.x, (int)pos.y + 1}, true);
        chunk->render();
    }
}

bool Chunk::inChunk(glm::vec3 blockPos) {
    float chunkX = floor(round(blockPos.x) / CHUNK_SIZE);
    float chunkZ = floor(round(blockPos.z) / CHUNK_SIZE);

    return chunkX == pos.x && chunkZ == pos.y;
}