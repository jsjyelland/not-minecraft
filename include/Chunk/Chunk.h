#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <vector>

#include <Block/Block.h>

#include <Shader/Shader.h>

#include <WorldGen/HeightMap.h>

#define CHUNK_SIZE 16
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_HEIGHT 128

class Chunk {
public:
    Chunk(glm::vec2 chunkPos);

    void render();
    void draw(Shader &shader);

    glm::vec2 getPos();

private:
    unsigned int VAO;

    std::array<unsigned int, CHUNK_AREA> heightMap;

    glm::vec2 pos;
    glm::mat4 model;

    unsigned int getHeight(glm::vec3 blockPos);
    BlockType getBlockType(glm::vec3 blockPos);
    
    bool inChunk(glm::vec3 blockPos);

    unsigned int numVertices;
};

#endif