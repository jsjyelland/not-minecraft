#include <ChunkMap/ChunkMap.h>

ChunkMap::ChunkMap() {

}

/**
 * Render a chunk at (x, y), creating a vertex array object and storing it in the map.
 */
void ChunkMap::render(int x, int y) {
    // Create VAO, VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 288 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT, NULL, GL_STATIC_DRAW);

    size_t verticesSize;
    float* vertices;

    unsigned int index = 0;

    // Iterate through the chunk
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int k = 0; k < CHUNK_SIZE; k++) {
            int height = (int)((perlin(i + x * CHUNK_SIZE, k + y * CHUNK_SIZE, 0.01, 4) + 0.2) * (float)CHUNK_HEIGHT);

            if (height > CHUNK_HEIGHT) {
                height = CHUNK_HEIGHT;
            }

            for (int j = 0; j < height; j++) {
                BlockType type;

                if (j < height - 6) {
                    type = BlockType::stone;
                } else if (j < height - 1) {
                    type = BlockType::dirt;
                } else {
                    type = BlockType::grass;
                }

                // Generate vertex data

                vertices = Block::constructMesh(type, i, j, k, &verticesSize);

                glBufferSubData(GL_ARRAY_BUFFER, index * verticesSize, verticesSize, vertices);

                index++;
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
    vect = {x, y};

    VAOmap.insert(std::pair<std::vector<int>, unsigned int>(vect, VAO));
    vertexNumMap.insert(std::pair<std::vector<int>, unsigned int>(vect, index * 36));
}

unsigned int ChunkMap::chunkVAO(int x, int y) {
    std::vector<int> vect;
    vect = {x, y};

    if (VAOmap.find(vect) == VAOmap.end()) {
        render(x, y);
    }

    unsigned int VAO = VAOmap[vect];

    return VAO;
}

unsigned int ChunkMap::numVertices(int x, int y) {
    std::vector<int> vect;
    vect = {x, y};

    if (vertexNumMap.find(vect) == vertexNumMap.end()) {
        render(x, y);
    }

    unsigned int num = vertexNumMap[vect];

    return num;
}

glm::mat4 ChunkMap::chunkModelMat(int x, int y) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x * CHUNK_SIZE, 0, y * CHUNK_SIZE));

    return model;
}