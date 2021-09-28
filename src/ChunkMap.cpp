#include <ChunkMap/ChunkMap.h>

ChunkMap::ChunkMap() {

}

/**
 * Render a chunk at (x, z), creating a vertex array object and storing it in the map.
 */
void ChunkMap::render(int x, int z) {
    // Create VAO, VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 288 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT, NULL, GL_STATIC_DRAW);

    size_t verticesSize;
    float* vertices;

    unsigned int numVertices = 0;

    int chunkOffsetX = x * CHUNK_SIZE;
    int chunkOffsetZ = z * CHUNK_SIZE;

    // Iterate through the chunk
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int k = 0; k < CHUNK_SIZE; k++) {
            for (int j = 0; j < CHUNK_HEIGHT; j++) {
                BlockType type = getBlockType(i + chunkOffsetX, j, k + chunkOffsetZ);

                if (type == BlockType::air) {
                    continue;
                }

                // Generate vertex data

                unsigned int directionMask = 0;

                if (getBlockType(i - 1 + chunkOffsetX, j, k + chunkOffsetZ) == BlockType::air) {
                    directionMask |= DIRECTION_SOUTH;
                }

                if (getBlockType(i + 1 + chunkOffsetX, j, k + chunkOffsetZ) == BlockType::air) {
                    directionMask |= DIRECTION_NORTH;
                }

                if (getBlockType(i + chunkOffsetX, j + 1, k + chunkOffsetZ) == BlockType::air) {
                    directionMask |= DIRECTION_TOP;
                }

                if (getBlockType(i + chunkOffsetX, j - 1, k + chunkOffsetZ) == BlockType::air) {
                    directionMask |= DIRECTION_BOTTOM;
                }

                if (getBlockType(i + chunkOffsetX, j, k - 1 + chunkOffsetZ) == BlockType::air) {
                    directionMask |= DIRECTION_WEST;
                }

                if (getBlockType(i + chunkOffsetX, j, k + 1 + chunkOffsetZ) == BlockType::air) {
                    directionMask |= DIRECTION_EAST;
                }

                if (directionMask) {
                    vertices = Block::constructMesh(type, i, j, k, directionMask, &verticesSize);

                    glBufferSubData(GL_ARRAY_BUFFER, numVertices * 8 * sizeof(float), verticesSize, vertices);

                    numVertices += verticesSize / (8 * sizeof(float));
                }
            }
        }
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::vector<int> vect;
    vect = {x, z};

    VAOmap.insert(std::pair<std::vector<int>, unsigned int>(vect, VAO));
    vertexNumMap.insert(std::pair<std::vector<int>, unsigned int>(vect, numVertices));
}

unsigned int ChunkMap::chunkVAO(int x, int z) {
    std::vector<int> vect;
    vect = {x, z};

    if (VAOmap.find(vect) == VAOmap.end()) {
        render(x, z);
    }

    unsigned int VAO = VAOmap[vect];

    return VAO;
}

unsigned int ChunkMap::numVertices(int x, int z) {
    std::vector<int> vect;
    vect = {x, z};

    if (vertexNumMap.find(vect) == vertexNumMap.end()) {
        render(x, z);
    }

    unsigned int num = vertexNumMap[vect];

    return num;
}

glm::mat4 ChunkMap::chunkModelMat(int x, int z) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x * CHUNK_SIZE, 0, z * CHUNK_SIZE));

    return model;
}

BlockType ChunkMap::getBlockType(int x, int y, int z) {
    int height = (int)((perlin(x, z, 0.01, 2) - 0.2) * (float)CHUNK_HEIGHT);

    BlockType type;

    if (y > height || y < 0) {
        type = BlockType::air;
    } else if (y < 32) {
        type = BlockType::stone;
    } else if (y < height - 1) {
        type = BlockType::dirt;
    } else if (y < height) {
        type = BlockType::grass;
    } else {
        type = BlockType::air;
    }

    return type;
}