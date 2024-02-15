#include <iostream>
#include <stdlib.h>
#include <glad/glad.h> 
#include <cmath>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/shader_s.h"
#include "camera.h"

typedef struct {
    int width;
    int height;
    unsigned char* data;
} Image;

// Function headings
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void handleInput(GLFWwindow *window, float delta);
int bind_texture(char* textureFilename, int glTexture);
Image readBMP(char* filename);

//  Window Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// Camera values
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float prevMouseX = SCR_WIDTH / 2;
float prevMouseY = SCR_HEIGHT / 2;
bool mouseFound = false; // If the mouse has been moved yet

// Rendering settings
const float FPS = 30.0f;
const float GROUND_SCALE = 400.0f;
const int PILLAR_SPACING = 8.0f;
const int PILLAR_COUNT = 25;
const float PILLAR_HEIGHT = 20.0f;
const float PILLAR_WIDTH = 3.0f;
const glm::vec3 LIGHT_SOURCE = glm::vec3(50.0f, 400.0f, 0.0f);

using namespace std;

int main() {

    /* Initialize GLFW*/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Initialize window */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Piranesi", NULL, NULL);
    /* Check for error during initialization */
    if (window == NULL)
    {
        std::cout << "Error when creating window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    /* Binding callback functions. */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
  
    // Capture mouse inputs using GLFW
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error when initializing GLAD" << std::endl;
        return -1;
    } 

    /* Building and compiling shaders */
    Shader mainShader("shaders/shader.vs", "shaders/shader.fs");
    Shader lightShader("shaders/lightshader.vs", "shaders/lightshader.fs");

    /* Enable vertex depth */
    glEnable(GL_DEPTH_TEST);  

    /* Vertex data for cubes */
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
    };

    /* Vertex data for pillars */
    float pillarVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  PILLAR_WIDTH, 0.0f, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  PILLAR_WIDTH, PILLAR_HEIGHT, 0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, PILLAR_HEIGHT, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  PILLAR_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  PILLAR_WIDTH, PILLAR_HEIGHT, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, PILLAR_HEIGHT, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, PILLAR_HEIGHT, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  PILLAR_WIDTH, PILLAR_HEIGHT, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  PILLAR_WIDTH, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  0.0f, PILLAR_HEIGHT, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  PILLAR_WIDTH, PILLAR_HEIGHT, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  PILLAR_WIDTH, 1.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    };

    /* Indice data for cube */
    unsigned int cubeIndices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 1, 5,
        0, 4, 5,
        1, 2, 6,
        1, 5, 6,
        2, 3, 7,
        2, 6, 7,
        3, 0, 4,
        3, 7, 4,
        4, 5, 6,
        4, 6, 7,
    };

    /* Incide data for pillar */
    unsigned int pillarIndices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
    };

    /* Positions of all cubes */
    glm::vec3 pillarPositions[4];

    /* Vertex data for ground */
    float groundVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  GROUND_SCALE, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.5f,   GROUND_SCALE, GROUND_SCALE, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,   0.0f, GROUND_SCALE, 0.0f, 1.0f, 0.0f,
    };

    /* Indices for ground */
    unsigned int groundIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    /* Vertex array and buffer objects, alongside element buffer objects. */
    unsigned int VBO[3], VAO[3], EBO[3];
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(3, EBO);


    /* Binding VAO used for the ground */
    glBindVertexArray(VAO[0]);

    /* Configuring vertex and indice data */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

    // Getting position vectors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Getting texture vectors
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Getting normal vectors 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* Binding VAO used for the light source */
    glBindVertexArray(VAO[1]);

    /* Configuring vertex and indice data */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // Getting position vectors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* Binding VAO used for pillars */
    glBindVertexArray(VAO[2]);

    /* Configuring vertex and indice data */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pillarVertices), pillarVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pillarIndices), pillarIndices, GL_STATIC_DRAW);

    // Getting position vectors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Getting texture vectors
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Getting normal vectors 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* Generate texture for ground */
    bind_texture((char *)"textures/ground_texture.bmp", GL_TEXTURE0);

    /* Generate texture for the pillars */
    bind_texture((char *)"textures/wood_texture.bmp", GL_TEXTURE1);

    /* Timing of frames */
    float delta = 0.0f;
    float prevFrame = static_cast<float>(glfwGetTime());

    /*
     * Main rendering loop.
     */
    while (!glfwWindowShouldClose(window))
    {
        /* Calculating delta */
        float currentFrame = static_cast<float>(glfwGetTime());
        delta = currentFrame - prevFrame;

        /* Snap to framerate */
        if (delta > 1 / FPS)
        {
            prevFrame = currentFrame;
            /* Process inputs */
            handleInput(window, delta);

            /* Clear gl data */
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Create transformations */
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection    = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            /* Store transformation data locations */
            unsigned int modelLoc = glGetUniformLocation(mainShader.ID, "model");
            unsigned int viewLoc  = glGetUniformLocation(mainShader.ID, "view");
            unsigned int projLoc  = glGetUniformLocation(mainShader.ID, "projection");
            unsigned int lightIntensityLoc  = glGetUniformLocation(mainShader.ID, "lightIntensity");
            //unsigned int lightSourceLoc  = glGetUniformLocation(mainShader.ID, "lightSource");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1f(lightIntensityLoc, 1.0f);
            mainShader.setVec3("viewSource", camera.Position);
            mainShader.setVec3("lightSource", LIGHT_SOURCE); // TODO: why doesn't other way work?

            /* Use light shader */
            lightShader.use();

            /* Bind VAO used for the light source */
            glBindVertexArray(VAO[1]);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(6.25f, 50.0f, 0.0f));
            glm::vec3 cameraPosition = camera.GetPosition();
            cameraPosition.x *= 17.5f/20.0f;
            cameraPosition.z *= 17.5f/20.0f;
            cameraPosition.y = 0;
            model = glm::translate(model, cameraPosition);
            model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);
            lightShader.setMat4("model", model);

            /* Draw triangle*/
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            /* Use main shader */
            mainShader.use();

            /* Bind VAO used for the ground*/
            glBindVertexArray(VAO[0]);

            /* Set active texture for ground */
            glUniform1i(glGetUniformLocation(mainShader.ID, "textureID"), 0);

            /* Always center ground righ below camera */
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
            float cameraGridX;
            float cameraGridZ;
            /* Snap ground position to grid */
            cameraGridX = (int) camera.GetPosition().x;
            cameraGridZ = (int) camera.GetPosition().z;
            model = glm::translate(model, glm::vec3(cameraGridX, 0.0f, cameraGridZ));
            /* Resize */
            model = glm::scale(model, glm::vec3(GROUND_SCALE, 1.0, GROUND_SCALE));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            /* Draw triangle*/
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

            /* Calculate closest grid positions and place cubes there */
            cameraGridX -= remainder(cameraGridX, PILLAR_SPACING);
            cameraGridZ -= remainder(cameraGridZ, PILLAR_SPACING);

            /* Start from far corner*/
            cameraGridX -= PILLAR_SPACING;
            cameraGridZ -= PILLAR_SPACING;
            for (unsigned int i = 0; i < 2; i++) {
                for (unsigned int j = 0; j < 2; j++) {
                    pillarPositions[2 * i + j] = 
                        glm::vec3(cameraGridX + PILLAR_SPACING * i, -2.0f, cameraGridZ + PILLAR_SPACING * j);
                }
            }

            /* Bind VAO used for objects */
            glBindVertexArray(VAO[2]); 

            /* Set active texture for pillars */
            glUniform1i(glGetUniformLocation(mainShader.ID, "textureID"), 1);

            /* Draw each cube */
            for (unsigned int i = 0; i < sizeof(pillarPositions) / sizeof(pillarPositions[0]); i++) {
                /* Generate model transformations */
                glm::mat4 model = glm::mat4(1.0f);

                /* Translate to cube location*/
                model = glm::translate(model, pillarPositions[i]);
                model = glm::translate(model, glm::vec3(0.0f, PILLAR_HEIGHT / 2 - 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(PILLAR_WIDTH, PILLAR_HEIGHT, PILLAR_WIDTH));
                
                /* Pass model transformation to shaders. */
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                /* Draw triangle*/
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }

            /* Swap buffers and poll events */
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    /* Deallocate resources */
    glDeleteVertexArrays(3, VAO);
    glDeleteBuffers(3, VBO);
    glDeleteBuffers(3, EBO);
    
    /* Terminate glfw */
    glfwTerminate();
    return 0;
}

/**
 * Requires:
 *      window is a properly initialized GLFW window.
 * 
 * Effects:
 *  Handles the user input for the given window.
 */
void handleInput(GLFWwindow *window, float delta)
{
    /* If user presses escape, close window */
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec2 move_inputs;

    /* Movement inputs */
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        move_inputs.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        move_inputs.x += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        move_inputs.y -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        move_inputs.x -= 1;
    }

    /* Normalize*/
    if (move_inputs.x != 0 || move_inputs.y != 0) {
        move_inputs = glm::normalize(move_inputs);
    }

    /* Sprinting */
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        move_inputs *= 3;
    }
    camera.ProcessInputs(move_inputs);
}

/* 
 *  Requires:
 *      This function should be registered by glfwSetFramebufferSizeCallback.
 * 
 *  Effects:
 *      This function is called whenever the window is resized.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window; // Unusued parameter
    glViewport(0, 0, width, height);
}

/* 
 *  Requires:
 *      This function should be registered by glfwSetCursorPosCallback.
 * 
 *  Effects:
 *      This function is called whenever the cursor position is moved.
 */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    /* Unused window parameter */
    (void) window;

    /* Cast positions */
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    /* If initial cursor position is still unknown */
    if (!mouseFound)
    {
        prevMouseX = xpos;
        prevMouseY = ypos;
        mouseFound = true;
    }

    /* Calculate mouse movement */
    float xoffset = xpos - prevMouseX;
    float yoffset = prevMouseY - ypos;

    /* Update mouse position */
    prevMouseX = xpos;
    prevMouseY = ypos;

    /* Pass cursor movement to camera */
    camera.ProcessMouseMovement(xoffset, yoffset);
}

/* 
 *  Requires:
 *      This function should be provided a valid filename pointing to a bmp file.
 * 
 *  Effects:
 *      Reads the bmp file and returns the image data.
 */
Image readBMP(char* filename)
{
    /* Open file */
    FILE *f = fopen(filename, "rb");

    /* Reading header */
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, f);

    /* Get height and width info */
    int width = *(int*) &header[18];
    int height = *(int*) &header[22];

    /* Create corresponding image size*/
    unsigned char* data = (unsigned char*) calloc(3 * width * height, sizeof(unsigned char));

    /* Read remaining image data and close file */
    fread(data, sizeof(unsigned char), 3 * width * height, f);
    fclose(f);

    /* Return Image data */
    Image image;
    image.width = width;
    image.height = height;
    image.data = data;

    return image;
}

/* 
 *  Requires:
 *      This provided filename should be a valid bmp file and glTexture an integer
 *      corresponding to a gl texture.
 * 
 *  Effects:
 *      Reads the bmp file binds it to the given gl texture.
 */
int bind_texture(char* textureFilename, int glTexture)
{
    unsigned int texture_ground;
    glGenTextures(1, &texture_ground);
    glActiveTexture(glTexture);
    glBindTexture(GL_TEXTURE_2D, texture_ground);
    /* Set texture wrapping */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Load textures */
    Image image = readBMP(textureFilename);
    int width = image.width;
    int height = image.height;
    unsigned char *data = image.data;

    std::cout << "Reading texture with width " << width << " and height " << height << std::endl;

    /* If succesfully loaded */
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Error when loading texture data" << std::endl;
        return -1;
    }
    delete[] data;
    return 0;
}