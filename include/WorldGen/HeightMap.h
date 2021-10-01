#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <Chunk/Chunk.h>

unsigned int heightGen(glm::vec2 pos);
bool treeGen(glm::vec2 pos);
bool flowerGen(glm::vec2 pos);

#endif