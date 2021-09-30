#include <stdio.h>
#include <iostream>
#include <math.h>

#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader/Shader.h>

#include <vector>

#include <Block/Block.h>
#include <ChunkMap/ChunkMap.h>

#define FLYING 1

#if FLYING
    #define CAMERA_SPEED 20.0f
#else
    #define CAMERA_SPEED 5.0f
#endif

#define CHUNK_BUFFER_SIZE CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT

#define RENDER_DISTANCE 6

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000

#define GRAVITY 30.0f
#define JUMP_SPEED 7.0f

glm::vec3 cameraPos = glm::vec3(0.0f, 130.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float vertSpeed = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float pitch = 0.0f;
float yaw = -90.f;

float lastX = WINDOW_WIDTH / 2;
float lastY = WINDOW_HEIGHT / 2;

float fov = 80.0f;

bool firstMouse = true;

bool wireframe = false;

ChunkMap chunkMap;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;

        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }

    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    }

    if (fov > 120.0f) {
        fov = 120.0f;
    }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mode) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Break block

        glm::vec3 direction = glm::normalize(cameraFront);
        // Raycast
        for (int i = 0; i < 100; i++) {
            glm::vec3 testPos = cameraPos + direction * (float)i * 0.05f;

            BlockType block = chunkMap.getBlock(testPos);
            
            if (block != BlockType::air) {
                chunkMap.setBlock(testPos, BlockType::air);
                break;
            }
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Place block
    }
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    const float cameraSpeed = CAMERA_SPEED * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraUp, glm::cross(cameraFront, cameraUp)));
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraUp, glm::cross(cameraFront, cameraUp)));
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    #if !FLYING
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (chunkMap.getBlock(cameraPos + glm::vec3(0, -2.1f, 0)) != BlockType::air) {
                vertSpeed = JUMP_SPEED;
                cameraPos += cameraUp * 0.1f;
            }
        }
    #else
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cameraPos += cameraUp * cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cameraPos -= cameraUp * cameraSpeed;
        }
    #endif

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        wireframe = !wireframe;

        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }
}

unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;

        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    } else {
        printf("Texture failed to load at path: %s", path);
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;

            if (nrComponents == 1) {
                format = GL_RED;
            } else if (nrComponents == 3) {
                format = GL_RGB;
            } else if (nrComponents == 4) {
                format = GL_RGBA;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            printf("Cubemap tex failed to load at path: %s", faces[i].c_str());
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}  

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Not Minecraft", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create window");

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialise GLAD");
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glfwSetScrollCallback(window, scrollCallback);

    Shader shader("shaders/shader.vs", "shaders/shader.fs");
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    Shader cursorShader("shaders/cursor.vs", "shaders/cursor.fs");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture

    stbi_set_flip_vertically_on_load(true);
    unsigned int atlas = loadTexture("textures/atlas.png");

    std::vector<std::string> faces{
        "textures/skybox/left.png",
        "textures/skybox/left.png",
        "textures/skybox/top.png",
        "textures/skybox/bottom.png",
        "textures/skybox/left.png",
        "textures/skybox/left.png"
    };

    stbi_set_flip_vertically_on_load(false);
    unsigned int cubemapTexture = loadCubemap(faces);

    // Skybox
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    float cursorVertices[] = {
        -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f 
    };

    unsigned int cursorVAO, cursorVBO;
    glGenVertexArrays(1, &cursorVAO);
    glGenBuffers(1, &cursorVBO);
    glBindVertexArray(cursorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cursorVertices), &cursorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)3);

    glLineWidth(10.0f);
    glEnable(GL_LINE_SMOOTH);

    // Render loop

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();

        float time = glm::mod<float>(currentFrame / 50.f, (2.0f * glm::pi<float>()));

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        float fps = 1 / deltaTime;

        #if !FLYING
            if (chunkMap.getBlock(cameraPos + glm::vec3(0, -2, 0)) != BlockType::air) {
                vertSpeed = 0.0f;
            } else {
                vertSpeed -= GRAVITY * deltaTime;
            }

            cameraPos += cameraUp * vertSpeed * deltaTime;

            if (cameraPos.y < -60.0f) {
                cameraPos.y = 128.0f;
            }
        #endif

        // std::cout << "FPS:" << fps << std::endl;

        processInput(window);

        glClearColor(0.3f, 0.6f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 500.0f);

        // Draw chunks

        shader.use();

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setInt("material.emission", 2);
        shader.setVec3("material.specular", glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setFloat("material.shininess", 0.0f);

        float lightMul = glm::cos(time) / 2.0f + 1.0f;

        // directional light
        shader.setVec3("dirLight.direction", glm::vec3(0, -cos(time), -sin(time)));
        shader.setVec3("dirLight.ambient", glm::vec3(0.2f) * lightMul + 0.01f);
        shader.setVec3("dirLight.diffuse", glm::vec3(0.4f) * lightMul);
        shader.setVec3("dirLight.specular", glm::vec3(0.3f) * lightMul);

        shader.setVec3("viewPos", cameraPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, atlas);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, atlas);

        int chunkX = cameraPos.x / 16;
        int chunkZ = cameraPos.z / 16;

        for (int i = -RENDER_DISTANCE; i < RENDER_DISTANCE; i++) {
            for (int k = -RENDER_DISTANCE; k < RENDER_DISTANCE * 2; k++) {
                if (sqrt(i * i + k * k) < RENDER_DISTANCE) {
                    chunkMap.getChunk(std::vector<int>{i + chunkX, k + chunkZ}, true)->draw(shader);
                }
            }
        }

        // Draw skybox

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix

        // Rotate according to time of day

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, time, glm::vec3(1.0f, 0.0f, 0.0f));

        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("model", model);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // draw cursor

        cursorShader.use();

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::scale(transform, glm::vec3(50.0f / (float)WINDOW_WIDTH, 50.0f / (float)WINDOW_HEIGHT, 0));
        cursorShader.setMat4("transform", transform);

        glBindVertexArray(cursorVAO);
        glDrawArrays(GL_LINES, 0, 4);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}