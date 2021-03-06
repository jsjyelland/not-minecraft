#include <Chunk/Chunk.h>

Chunk::Chunk(glm::vec2 chunkPos, ChunkMap *chunkMap) {
    north = NULL;
    east = NULL;
    south = NULL;
    west = NULL;

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
    BlockType type;
    int height;
    bool tree, flower, grass;

    std::vector<glm::vec3> treePositions;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            glm::vec2 genPos = glm::vec2(pos.x * CHUNK_SIZE + x, pos.y * CHUNK_SIZE + z);
            height = heightGen(genPos);
            tree = treeGen(genPos) && height > 42;
            flower = flowerGen(genPos) && height > 42 && !tree;
            grass = grassGen(genPos) && height > 42 && !tree && !flower;

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y >= height && y < 40) {
                    type = BlockType::water;
                } else if (y < 42 && y > 30 && y < height && y > height - 4) {
                    type = BlockType::sand;
                } else if (y <= 30 || y < height - 2) {
                    type = BlockType::stone;
                } else if (y < height - 1) {
                    type = BlockType::dirt;
                } else if (y < height) {
                    type = BlockType::grass;
                } else if (y == height && flower) {
                    type = BlockType::flower;
                } else if (y == height && grass) {
                    type = BlockType::short_grass;
                } else if (tree && y <= height + 3) {
                    type = BlockType::wood;
                } else if (tree && y > height + 3 && y < height + 5) {
                    type = BlockType::air;
                    treePositions.emplace_back(genPos.x, y, genPos.y);
                } else {
                    type = BlockType::air;
                }

                if (type != BlockType::air) {
                    blockMap[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z] = type;
                }
            }
        }
    }

    for (std::vector<glm::vec3>::iterator i = treePositions.begin(); i != treePositions.end(); i++) {
        for (int x = -2; x <= 2; x++) {
            for (int z = -2; z <= 2; z++) {
                if (abs(z) == 2 && abs(x) == 2) continue;

                for (int y = 0; y <= 1; y++) {
                    setBlockType(*i + glm::vec3(x, y, z), BlockType::leaves, false, true);
                }
            }
        }

        for (int x = -1; x <= 1; x++) {
            for (int z = -1; z <= 1; z++) {
                for (int y = 2; y <= 3; y++) {
                    if (abs(z) == 1 && abs(x) == 1 && y == 3) continue;

                    setBlockType(*i + glm::vec3(x, y, z), BlockType::leaves, false, true);
                }
            }
        }
    }

    empty = false;
}

void Chunk::render() {
    if (empty) {
        return;
    }

    solidVerticesVector.reserve(8000);
    translucentVerticesVector.reserve(8000);

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

        if (Block::isSprite(type)) {
            Block::constructSpriteMesh(type, i, j, k, translucentVerticesVector);
        } else if (Block::isSolid(type)) {
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
                Block::constructMesh(type, 1.0f, i, j, k, directionMask, solidVerticesVector);
            }
        } else if (Block::isTranslucent(type)) {
            // Translucent mesh

            BlockType topBlock = getBlockTypeInternal(i, j + 1, k);

            if (type == BlockType::leaves) {
                directionMask = DIRECTION_ALL;
            } else {
                if (!Block::isBlock(getBlockTypeInternal(i - 1, j, k))) {
                    directionMask |= DIRECTION_SOUTH;
                }

                if (!Block::isBlock(getBlockTypeInternal(i + 1, j, k))) {
                    directionMask |= DIRECTION_NORTH;
                }

                if (!Block::isBlock(topBlock)) {
                    directionMask |= DIRECTION_TOP;
                }

                if (!Block::isBlock(getBlockTypeInternal(i, j - 1, k))) {
                    directionMask |= DIRECTION_BOTTOM;
                }

                if (!Block::isBlock(getBlockTypeInternal(i, j, k - 1))) {
                    directionMask |= DIRECTION_WEST;
                }

                if (!Block::isBlock(getBlockTypeInternal(i, j, k + 1))) {
                    directionMask |= DIRECTION_EAST;
                }
            }

            if (directionMask) {
                Block::constructMesh(type, topBlock == BlockType::air && type == BlockType::water ? 0.8f : 1.0f, i, j, k, directionMask, translucentVerticesVector);
            }
        }
    }

    // std::cout << "mesh: " << glfwGetTime() - lastTime << std::endl;
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
    glDisable(GL_CULL_FACE);

    // Scale down ever so slightly
    // glm::mat4 transModel = glm::mat4(1.0f);
    // transModel = glm::scale(transModel, glm::vec3(0.999f));
    // transModel = glm::translate(transModel, glm::vec3(pos.x * CHUNK_SIZE, 0, pos.y * CHUNK_SIZE));

    // shader.setMat4("model", transModel);

    glBindVertexArray(translucentVAO);
    glDrawArrays(GL_TRIANGLES, 0, numTranslucentVertices);

    // Re-enable
    glEnable(GL_CULL_FACE);

    glBindVertexArray(0);
}

glm::vec2 Chunk::getPos() {
    return pos;
}

#define MOD(x, y) ((x % y) + y) % y

BlockType Chunk::getBlockType(glm::vec3 blockPos) {
    if (inChunk(blockPos)) {
        int blockX = MOD((int)round(blockPos.x), CHUNK_SIZE);
        int blockZ = MOD((int)round(blockPos.z), CHUNK_SIZE);
        int blockY = (int)round(blockPos.y);

        int index = blockX * CHUNK_HEIGHT * CHUNK_SIZE + blockY * CHUNK_SIZE + blockZ;

        return blockMap[index];
    }
    
    return BlockType::air;
}

BlockType Chunk::getBlockTypeInternal(int chunkX, int chunkY, int chunkZ) {
    if (chunkY < 0 || chunkY >= CHUNK_HEIGHT) {
        return BlockType::air;
    }

    if (chunkX < 0) {
        if (south) {
            return south->getBlockTypeInternal(MOD(chunkX, CHUNK_SIZE), chunkY, MOD(chunkZ, CHUNK_SIZE));
        } else {
            return BlockType::air;
        }
    } else if (chunkZ < 0) {
        if (west) {
            return west->getBlockTypeInternal(MOD(chunkX, CHUNK_SIZE), chunkY, MOD(chunkZ, CHUNK_SIZE));
        } else {
            return BlockType::air;
        }
    } else if(chunkX > 15) {
        if (north) {
            return north->getBlockTypeInternal(MOD(chunkX, CHUNK_SIZE), chunkY, MOD(chunkZ, CHUNK_SIZE));
        } else {
            return BlockType::air;
        }
    } else if (chunkZ > 15) {
        if (east) {
            return east->getBlockTypeInternal(MOD(chunkX, CHUNK_SIZE), chunkY, MOD(chunkZ, CHUNK_SIZE));
        } else {
            return BlockType::air;
        }
    }

    return blockMap[chunkX * CHUNK_HEIGHT * CHUNK_SIZE + chunkY * CHUNK_SIZE + chunkZ];
}

bool Chunk::setBlockType(glm::vec3 blockPos, BlockType type, bool doRender, bool addToGen) {
    if (!inChunk(blockPos)) {
        if (addToGen) {
            map->addToBlockGen(blockPos, type);
        }
        
        return false;
    }

    int blockX = MOD((int)round(blockPos.x), CHUNK_SIZE);
    int blockZ = MOD((int)round(blockPos.z), CHUNK_SIZE);
    int blockY = (int)round(blockPos.y);

    int index = blockX * CHUNK_HEIGHT * CHUNK_SIZE + blockY * CHUNK_SIZE + blockZ;

    blockMap[index] = type;

    if (!doRender) {
        return true;
    }

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

    return true;
}

bool Chunk::inChunk(glm::vec3 blockPos) {
    float chunkX = floor(round(blockPos.x) / CHUNK_SIZE);
    float chunkZ = floor(round(blockPos.z) / CHUNK_SIZE);

    return chunkX == pos.x && chunkZ == pos.y && blockPos.y >= 0 && blockPos.y < CHUNK_HEIGHT;
}

void Chunk::setNorth(Chunk *chunk) {
    if (chunk == NULL) {
        return;
    }

    north = chunk;
    chunk->south = this;
}

void Chunk::setEast(Chunk *chunk) {
    if (chunk == NULL) {
        return;
    }

    east = chunk;
    chunk->west = this;
}

void Chunk::setSouth(Chunk *chunk) {
    if (chunk == NULL) {
        return;
    }

    south = chunk;
    chunk->north = this;
}

void Chunk::setWest(Chunk *chunk) {
    if (chunk == NULL) {
        return;
    }

    west = chunk;
    chunk->east = this;
}