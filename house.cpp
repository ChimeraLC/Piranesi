/*
 * Recreation of the house described in Piranesi
 */

/* Library imports */
#include <iostream>
#include <stdlib.h>
#include <glad/glad.h>
#include <cmath>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Header files */
#include "shaders/shader_s.h"
#include "camera.h"
#include "perlin.h"
#include "shapes.h"

/* Namespace */
using namespace std;

/* Image file structure */
typedef struct
{
    int width;
    int height;
    unsigned char *data;
} Image;

/* Function Prototypes */
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double mouseX, double mouseY);
void handleInput(GLFWwindow *window, float delta);
int bind_texture(int height, int width, int glTexture);
Image readBMP(char *filename);
Image generate_texture(int height, int width);

/* Window Settings */
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

/* Camera Settings */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Camera instance
float prevMouseX = SCR_WIDTH / 2;           // Place mouse initially in center of screen
float prevMouseY = SCR_HEIGHT / 2;
bool mouseFound = false; // If mouse has been moved

/* Rendering settings */
const float FPS = 30.0f;
const int GRID_WIDTH = 4;

/* Main function */

int main()
{

    /* Initialize GLFW*/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Initialize window */
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Piranesi", NULL, NULL);
    /* Check for error during initialization */
    if (window == NULL)
    {
        std::cout << "Error when creating window" << std::endl;
        glfwTerminate();
        return -1;
    }
    /* Set window to current */
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
    Shader mainShader("shaders/houseshader.vs", "shaders/houseshader.fs");

    /* Enable vertex depth */
    glEnable(GL_DEPTH_TEST);

    /* Vertex array and buffer objects, alongside element buffer objects. */
    unsigned int VBO[2], VAO[2], EBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

    /* Binding VAO used for the ground */
    glBindVertexArray(VAO[0]);

    /* Configuring vertex and indice data */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

    // Getting position vectors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Getting texture vectors
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Getting normal vectors
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* Binding VAO used for cubes */
    glBindVertexArray(VAO[1]);

    /* Configuring vertex and indice data */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // Getting position vectors
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Getting texture vectors
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Getting normal vectors
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* Configuring shader textures */
    mainShader.use();

    /* Generate texture for ground */
    bind_texture(128, 128, GL_TEXTURE0);

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
            /* Track frame time */
            prevFrame = currentFrame;

            /* Process inputs */
            handleInput(window, delta);

            /* Clear gl data */
            glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Use main shader */
            mainShader.use();

            /* Bind VAO used for the ground*/
            glBindVertexArray(VAO[0]);

            /* Set active texture for ground */
            glUniform1i(glGetUniformLocation(mainShader.ID, "textureID"), 0);

            /* Always center ground right below camera */
            float cameraGridX, cameraGridZ; // World is split into grid_size x grid_size squares
            glm::vec3 cameraPos = camera.GetPosition();
            cameraGridX = floor(cameraPos.x / GRID_WIDTH);
            cameraGridZ = floor(cameraPos.z / GRID_WIDTH);

            /* Create transformations */
            glm::mat4 model = glm::mat4(1.0f);
            camera.SetCameraHeight(((int) cameraGridX * 17 + (int) cameraGridZ * 13) % 3);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            /* Store transformation data locations */
            unsigned int modelLoc = glGetUniformLocation(mainShader.ID, "model");
            unsigned int viewLoc = glGetUniformLocation(mainShader.ID, "view");
            unsigned int projLoc = glGetUniformLocation(mainShader.ID, "projection");

            /* View and projection values are constant */
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            cameraGridX = 0;
            cameraGridZ = 0;

            /* Generate transitions */
            for (int i = cameraGridX - 2; i <= cameraGridX + 2; i++)
            {
                for (int j = cameraGridZ - 2; j <= cameraGridZ + 2; j++) {

                    model = glm::mat4(1.0f);
                    /* Translate to be along gridlines */
                    model = glm::translate(model, glm::vec3(GRID_WIDTH / 2, -2.0f, GRID_WIDTH / 2));

                    /* Update height */
                    model = glm::translate(model, glm::vec3(0, (i * 17 + j * 13) % 3, 0));

                    /* Translate to player */
                    model = glm::translate(model, glm::vec3(GRID_WIDTH * i, 0.0f, GRID_WIDTH * j));

                    /* Resize */
                    model = glm::scale(model, glm::vec3(GRID_WIDTH, 1.0, GRID_WIDTH));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                    /* Draw triangle*/
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                }
            }

            /* Bind VAO used for the cube */
            glBindVertexArray(VAO[1]);
            model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            /* Swap buffers and poll events */
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    /* Deallocate resources */
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);

    /* Terminate glfw */
    glfwTerminate();

    /* Return without error */
    return 0;
}
/**
 * Requires:
 *      Window is a properly initialized GLFW window.
 *      Camera has been propertly initialized.
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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        move_inputs.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        move_inputs.x += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        move_inputs.y -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        move_inputs.x -= 1;
    }

    /* Normalize*/
    if (move_inputs.x != 0 || move_inputs.y != 0)
    {
        move_inputs = glm::normalize(move_inputs);
    }

    /* Sprinting */
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    {
        move_inputs *= 3;
    }

    /* Scale by time */
    move_inputs *= delta;

    /* Pass information to camera */
    camera.ProcessInputs(move_inputs);
}

/*
 *  Requires:
 *      This function should be registered by glfwSetFramebufferSizeCallback.
 *
 *  Effects:
 *      This function is called whenever the window is resized.
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window; // Unusued parameter
    glViewport(0, 0, width, height);
}

/*
 *  Requires:
 *      This function should be registered by glfwSetCursorPosCallback.
 *
 *  Effects:
 *      This function is called whenever the cursor position is moved.
 */
void mouse_callback(GLFWwindow *window, double mouseX, double mouseY)
{
    /* Unused window parameter */
    (void)window;

    /* Cast positions */
    float xpos = static_cast<float>(mouseX);
    float ypos = static_cast<float>(mouseY);

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
 *      The width and height should be positive values and glTexture an integer
 *      corresponding to a gl texture.
 *
 *  Effects:
 *      Reads the bmp file binds it to the given gl texture.
 */
int bind_texture(int height, int width, int glTexture)
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
    Image image = generate_texture(height, width);
    unsigned char *data = image.data;

    std::cout << "Reading texture with width " << width << " and height " << height << std::endl;

    /* If succesfully loaded */
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Error when loading texture data" << std::endl;
        return -1;
    }
    delete[] data;
    return 0;
}

/*
 *  Requires:
 *      The width and height should be positive values.
 *
 *  Effects:
 *      Generates a random texture with the given width and height
 * */
Image generate_texture(int height, int width)
{
    /* Create corresponding image size*/
    unsigned char *data = (unsigned char *)calloc(3 * width * height, sizeof(unsigned char));

    /* Create perlin noise */
    Perlin perlin = Perlin(height, width);

    /* Marble Colors*/
    int marble_dark[3] = {180, 190, 195};
    // int marble_dark[3] = {0, 0, 0};
    int marble_light[3] = {205, 224, 227};

    /* Generate internals */
    for (int row = 0; row < height / 2; row++)
    {
        for (int col = 0; col < width / 2; col++)
        {
            /* Calculate noise */
            double noise = perlin.Perlin_Marble((double)col, (double)row);

            /* Expand dark and light values */
            double light_noise = noise,
                   dark_noise = 1 - noise;

            /* Blue */
            data[3 * (row * width + col)] =
                (unsigned char)(dark_noise * marble_dark[0] + light_noise * marble_light[0]);
            data[3 * ((height - 1 - row) * width + col)] =
                data[3 * (row * width + col)];
            data[3 * (row * width + (width - 1 - col))] =
                data[3 * (row * width + col)];
            data[3 * ((height - 1 - row) * width + (width - 1 - col))] =
                data[3 * (row * width + col)];
            /* Green */
            data[3 * (row * width + col) + 1] =
                (unsigned char)(dark_noise * marble_dark[1] + light_noise * marble_light[1]);
            data[3 * ((height - 1 - row) * width + col) + 1] =
                data[3 * (row * width + col) + 1];
            data[3 * (row * width + (width - 1 - col)) + 1] =
                data[3 * (row * width + col) + 1];
            data[3 * ((height - 1 - row) * width + (width - 1 - col)) + 1] =
                data[3 * (row * width + col) + 1];
            /* Red */
            data[3 * (row * width + col) + 2] =
                (unsigned char)(dark_noise * marble_dark[2] + light_noise * marble_light[2]);
            data[3 * ((height - 1 - row) * width + col) + 2] =
                data[3 * (row * width + col) + 2];
            data[3 * (row * width + (width - 1 - col)) + 2] =
                data[3 * (row * width + col) + 2];
            data[3 * ((height - 1 - row) * width + (width - 1 - col)) + 2] =
                data[3 * (row * width + col) + 2];
        }
    }

    /* Return Image data */
    Image image;
    image.width = width;
    image.height = height;
    image.data = data;

    return image;
}
