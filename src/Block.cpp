#include <Block/Block.h>

/**
 * Vector order:
 * top, north, east, south, west, bottom
 */
std::vector<unsigned int> Block::atlasMap(BlockType type) {
    std::vector<unsigned int> vec;

    switch (type) {
        case BlockType::dirt:
            vec = {2, 2, 2, 2, 2, 2};
            break;

        case BlockType::grass:
            vec = {0, 1, 1, 1, 1, 2};
            break;

        case BlockType::stone:
            vec = {3, 3, 3, 3, 3, 3};
            break;

        case BlockType::water:
            vec = {4, 4, 4, 4, 4, 4};
            break;

        case BlockType::wood:
            vec = {6, 5, 5, 5, 5, 6};
            break;

        case BlockType::sand:
            vec = {7, 7, 7, 7, 7, 7};
            break;

        case BlockType::leaves:
            vec = {8, 8, 8, 8, 8, 8};
            break;

        case BlockType::flower:
            vec = {9};
            break;

        case BlockType::short_grass:
            vec = {10};
            break;

        case BlockType::wood_planks:
            vec = {11, 11, 11, 11, 11, 11};
            break;

        case BlockType::cobblestone:
            vec = {12, 12, 12, 12, 12, 12};
            break;

        case BlockType::glass:
            vec = {13, 13, 13, 13, 13, 13};
            break;
    }

    return vec;
}

/**
 * Return the atlas position offset in both x & y
 */
std::vector<unsigned int> Block::atlasPosXY(unsigned int atlasPos) {
    unsigned int x = atlasPos % 16;
    unsigned int y = 15 - atlasPos / 16;

    std::vector<unsigned int> ret(2);

    ret = {x, y};

    return ret;
}

glm::vec3 Block::blockFacePos(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 selectedBlockPos) {
    glm::vec3 diff = cameraPos - selectedBlockPos;
    glm::vec3 sign = glm::sign(diff);
    glm::vec3 signDir = glm::sign(cameraFront);
    glm::vec3 boundsMultiplier = sign;

    if (glm::abs(diff).x <= 0.5f) {
        boundsMultiplier.x = -signDir.x;
    }

    if (glm::abs(diff).y <= 0.5f) {
        boundsMultiplier.y = -signDir.y;
    }

    if (glm::abs(diff).z <= 0.5f) {
        boundsMultiplier.z = -signDir.z;
    }

    glm::vec3 bounds = selectedBlockPos + (0.5f * boundsMultiplier); // get the block

    glm::vec3 invD = 1.0f / cameraFront;

    glm::vec3 t0 = (bounds - cameraPos) * invD;

    if (t0.x > t0.y && t0.x > t0.z) {
        // x direction is largest

        if (sign.x > 0) {
            // north side
            return selectedBlockPos + glm::vec3(1, 0, 0);
        } else {
            // south side
            return selectedBlockPos + glm::vec3(-1, 0, 0);
        }
    } else if (t0.y > t0.x && t0.y > t0.z) {
        // y direction is largest

        if (sign.y > 0) {
            // top side
            return selectedBlockPos + glm::vec3(0, 1, 0);
        } else {
            // bottom side
            return selectedBlockPos + glm::vec3(0, -1, 0);
        }
    } else {
        // z direction is largest
    
        if (sign.z > 0) {
            // east side
            return selectedBlockPos + glm::vec3(0, 0, 1);
        } else {
            // west side
            return selectedBlockPos + glm::vec3(0, 0, -1);
        }
    }
}

bool Block::isBlock(BlockType type) {
    return type != BlockType::air;
}

bool Block::isSolid(BlockType type) {
    return type != BlockType::air && !isTranslucent(type);
}

bool Block::isTranslucent(BlockType type) {
    return type == BlockType::water || type == BlockType::leaves || isSprite(type) || type == BlockType::glass;
}

bool Block::isSprite(BlockType type) {
    return type == BlockType::flower || type == BlockType::short_grass;
}

/**
 * Construct the mesh data for a block.
 * Has the following construction:
 * 3x vertex point, 3x normal direction, 2x atlas coord
 * 
 * In order of faces:
 * top, north, east, south, west, bottom
 */
void Block::constructMesh(BlockType type, float blockheight, int chunkX, int chunkY, int chunkZ, unsigned int directionMask, std::vector<float> &data) {
    std::vector<unsigned int> atlas = atlasMap(type);

    float topOffsetX = (float)atlasPosXY(atlas[0])[0];
    float topOffsetY = (float)atlasPosXY(atlas[0])[1];
    float northOffsetX = (float)atlasPosXY(atlas[1])[0];
    float northOffsetY = (float)atlasPosXY(atlas[1])[1];
    float eastOffsetX = (float)atlasPosXY(atlas[2])[0];
    float eastOffsetY = (float)atlasPosXY(atlas[2])[1];
    float southOffsetX = (float)atlasPosXY(atlas[3])[0];
    float southOffsetY = (float)atlasPosXY(atlas[3])[1];
    float westOffsetX = (float)atlasPosXY(atlas[4])[0];
    float westOffsetY = (float)atlasPosXY(atlas[4])[1];
    float bottomOffsetX = (float)atlasPosXY(atlas[5])[0];
    float bottomOffsetY = (float)atlasPosXY(atlas[5])[1];

    float x = (float)chunkX;
    float y = (float)chunkY;
    float z = (float)chunkZ;

    if (directionMask & DIRECTION_TOP) {
        float top[] = {
            -0.5f+x, -0.5f+blockheight+y, -0.5f+z,  0.0f,  1.0f,  0.0f, 0.0f + topOffsetX, 0.0f + topOffsetY,
             0.5f+x, -0.5f+blockheight+y,  0.5f+z,  0.0f,  1.0f,  0.0f, 1.0f + topOffsetX, 1.0f + topOffsetY,
             0.5f+x, -0.5f+blockheight+y, -0.5f+z,  0.0f,  1.0f,  0.0f, 1.0f + topOffsetX, 0.0f + topOffsetY,
             0.5f+x, -0.5f+blockheight+y,  0.5f+z,  0.0f,  1.0f,  0.0f, 1.0f + topOffsetX, 1.0f + topOffsetY,
            -0.5f+x, -0.5f+blockheight+y, -0.5f+z,  0.0f,  1.0f,  0.0f, 0.0f + topOffsetX, 0.0f + topOffsetY,
            -0.5f+x, -0.5f+blockheight+y,  0.5f+z,  0.0f,  1.0f,  0.0f, 0.0f + topOffsetX, 1.0f + topOffsetY,
        };
        
        data.insert(data.end(), top, top + 48);
    }
    
    if (directionMask & DIRECTION_NORTH) {
        float north[] = {
            0.5f+x, -0.5f+blockheight+y,  0.5f+z,  1.0f,  0.0f,  0.0f, 0.0f + northOffsetX, blockheight + northOffsetY,
            0.5f+x, -0.5f+y, -0.5f+z,  1.0f,  0.0f,  0.0f, 1.0f + northOffsetX, 0.0f + northOffsetY,
            0.5f+x, -0.5f+blockheight+y, -0.5f+z,  1.0f,  0.0f,  0.0f, 1.0f + northOffsetX, blockheight + northOffsetY,
            0.5f+x, -0.5f+y, -0.5f+z,  1.0f,  0.0f,  0.0f, 1.0f + northOffsetX, 0.0f + northOffsetY,
            0.5f+x, -0.5f+blockheight+y,  0.5f+z,  1.0f,  0.0f,  0.0f, 0.0f + northOffsetX, blockheight + northOffsetY,
            0.5f+x, -0.5f+y,  0.5f+z,  1.0f,  0.0f,  0.0f, 0.0f + northOffsetX, 0.0f + northOffsetY,
        };

        data.insert(data.end(), north, north + 48);
    }

    if (directionMask & DIRECTION_EAST) {
        float east[] = {
            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f,  0.0f, 1.0f, 0.0f + eastOffsetX, 0.0f + eastOffsetY,
             0.5f+x, -0.5f+y,  0.5f+z,  0.0f,  0.0f, 1.0f, 1.0f + eastOffsetX, 0.0f + eastOffsetY,
             0.5f+x, -0.5f+blockheight+y,  0.5f+z,  0.0f,  0.0f, 1.0f, 1.0f + eastOffsetX, blockheight + eastOffsetY,
             0.5f+x, -0.5f+blockheight+y,  0.5f+z,  0.0f,  0.0f, 1.0f, 1.0f + eastOffsetX, blockheight + eastOffsetY,
            -0.5f+x, -0.5f+blockheight+y,  0.5f+z,  0.0f,  0.0f, 1.0f, 0.0f + eastOffsetX, blockheight + eastOffsetY,
            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f,  0.0f, 1.0f, 0.0f + eastOffsetX, 0.0f + eastOffsetY,
        };

        data.insert(data.end(), east, east + 48);
    }

    if (directionMask & DIRECTION_SOUTH) {
        float south[] = {
            -0.5f+x, -0.5f+blockheight+y,  0.5f+z, -1.0f,  0.0f,  0.0f, 0.0f + southOffsetX, blockheight + southOffsetY,
            -0.5f+x, -0.5f+blockheight+y, -0.5f+z, -1.0f,  0.0f,  0.0f, 1.0f + southOffsetX, blockheight + southOffsetY,
            -0.5f+x, -0.5f+y, -0.5f+z, -1.0f,  0.0f,  0.0f, 1.0f + southOffsetX, 0.0f + southOffsetY,
            -0.5f+x, -0.5f+y, -0.5f+z, -1.0f,  0.0f,  0.0f, 1.0f + southOffsetX, 0.0f + southOffsetY,
            -0.5f+x, -0.5f+y,  0.5f+z, -1.0f,  0.0f,  0.0f, 0.0f + southOffsetX, 0.0f + southOffsetY,
            -0.5f+x, -0.5f+blockheight+y,  0.5f+z, -1.0f,  0.0f,  0.0f, 0.0f + southOffsetX, blockheight + southOffsetY,
        };

        data.insert(data.end(), south, south + 48);
    }

    if (directionMask & DIRECTION_WEST) { 
        float west[] = {
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f,  0.0f, -1.0f, 0.0f + westOffsetX, 0.0f + westOffsetY,
             0.5f+x, -0.5f+blockheight+y, -0.5f+z,  0.0f,  0.0f, -1.0f, 1.0f + westOffsetX, blockheight + westOffsetY,
             0.5f+x, -0.5f+y, -0.5f+z,  0.0f,  0.0f, -1.0f, 1.0f + westOffsetX, 0.0f + westOffsetY,
             0.5f+x, -0.5f+blockheight+y, -0.5f+z,  0.0f,  0.0f, -1.0f, 1.0f + westOffsetX, blockheight + westOffsetY,
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f,  0.0f, -1.0f, 0.0f + westOffsetX, 0.0f + westOffsetY,
            -0.5f+x, -0.5f+blockheight+y, -0.5f+z,  0.0f,  0.0f, -1.0f, 0.0f + westOffsetX, blockheight + westOffsetY,
        };

        data.insert(data.end(), west, west + 48);
    }

    if (directionMask & DIRECTION_BOTTOM) {
        float bottom[] = {
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, -1.0f,  0.0f, 0.0f + bottomOffsetX, 0.0f + bottomOffsetY,
             0.5f+x, -0.5f+y, -0.5f+z,  0.0f, -1.0f,  0.0f, 1.0f + bottomOffsetX, 0.0f + bottomOffsetY,
             0.5f+x, -0.5f+y,  0.5f+z,  0.0f, -1.0f,  0.0f, 1.0f + bottomOffsetX, 1.0f + bottomOffsetY,
             0.5f+x, -0.5f+y,  0.5f+z,  0.0f, -1.0f,  0.0f, 1.0f + bottomOffsetX, 1.0f + bottomOffsetY,
            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f, -1.0f,  0.0f, 0.0f + bottomOffsetX, 1.0f + bottomOffsetY,
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, -1.0f,  0.0f, 0.0f + bottomOffsetX, 0.0f + bottomOffsetY,
        };

        data.insert(data.end(), bottom, bottom + 48);
    }
}

void Block::constructSpriteMesh(BlockType type, int chunkX, int chunkY, int chunkZ, std::vector<float> &data) {
    std::vector<unsigned int> atlas = atlasMap(type);

    float offsetX = (float)atlasPosXY(atlas[0])[0];
    float offsetY = (float)atlasPosXY(atlas[0])[1];

    float x = (float)chunkX;
    float y = (float)chunkY;
    float z = (float)chunkZ;

    float north[] = {
         0.5f+x,  0.9142135624f+y,  0.5f+z,  1.0f,  0.0f,  -1.0f, 0.0f + offsetX, 1.0f + offsetY,
        -0.5f+x, -0.5f+y, -0.5f+z,  1.0f,  0.0f,  -1.0f, 1.0f + offsetX, 0.0f + offsetY,
        -0.5f+x,  0.9142135624f+y, -0.5f+z,  1.0f,  0.0f,  -1.0f, 1.0f + offsetX, 1.0f + offsetY,
        -0.5f+x, -0.5f+y, -0.5f+z,  1.0f,  0.0f,  -1.0f, 1.0f + offsetX, 0.0f + offsetY,
         0.5f+x,  0.9142135624f+y,  0.5f+z,  1.0f,  0.0f,  -1.0f, 0.0f + offsetX, 1.0f + offsetY,
         0.5f+x, -0.5f+y,  0.5f+z,  1.0f,  0.0f,  -1.0f, 0.0f + offsetX, 0.0f + offsetY,
    };

    data.insert(data.end(), north, north + 48);

    float east[] = {
        -0.5f+x, -0.5f+y,  0.5f+z, -1.0f, 0.0f, -1.0f, 0.0f + offsetX, 0.0f + offsetY,
         0.5f+x, -0.5f+y, -0.5f+z, -1.0f, 0.0f, -1.0f, 1.0f + offsetX, 0.0f + offsetY,
         0.5f+x,  0.9142135624f+y, -0.5f+z, -1.0f, 0.0f, -1.0f, 1.0f + offsetX, 1.0f + offsetY,
         0.5f+x,  0.9142135624f+y, -0.5f+z, -1.0f, 0.0f, -1.0f, 1.0f + offsetX, 1.0f + offsetY,
        -0.5f+x,  0.9142135624f+y,  0.5f+z, -1.0f, 0.0f, -1.0f, 0.0f + offsetX, 1.0f + offsetY,
        -0.5f+x, -0.5f+y,  0.5f+z, -1.0f, 0.0f, -1.0f, 0.0f + offsetX, 0.0f + offsetY,
    };

    data.insert(data.end(), east, east + 48);
}