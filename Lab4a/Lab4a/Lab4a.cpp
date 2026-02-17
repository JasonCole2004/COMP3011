#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "error.h"
#include "file.h"
#include "shader.h"

#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

float vertices[] =
{
    // TL face: Top, B, A  (red)
     0.0f,  1.0f,  0.0f,   1.f, 0.f, 0.f,
    -1.0f,  0.0f, -1.0f,   1.f, 0.f, 0.f,
    -1.0f,  0.0f,  1.0f,   1.f, 0.f, 0.f,

    // TB face: Top, C, B  (green)
     0.0f,  1.0f,  0.0f,   0.f, 1.f, 0.f,
     1.0f,  0.0f, -1.0f,   0.f, 1.f, 0.f,
    -1.0f,  0.0f, -1.0f,   0.f, 1.f, 0.f,

    // TR face: Top, D, C  (blue)
     0.0f,  1.0f,  0.0f,   0.f, 0.f, 1.f,
     1.0f,  0.0f,  1.0f,   0.f, 0.f, 1.f,
     1.0f,  0.0f, -1.0f,   0.f, 0.f, 1.f,

     // TF face: Top, A, D  (yellow)
      0.0f,  1.0f,  0.0f,   1.f, 1.f, 0.f,
     -1.0f,  0.0f,  1.0f,   1.f, 1.f, 0.f,
      1.0f,  0.0f,  1.0f,   1.f, 1.f, 0.f,

      // BL face: Bottom, A, B  (magenta)
       0.0f, -1.0f,  0.0f,   1.f, 0.f, 1.f,
      -1.0f,  0.0f,  1.0f,   1.f, 0.f, 1.f,
      -1.0f,  0.0f, -1.0f,   1.f, 0.f, 1.f,

      // BB face: Bottom, B, C  (cyan)
       0.0f, -1.0f,  0.0f,   0.f, 1.f, 1.f,
      -1.0f,  0.0f, -1.0f,   0.f, 1.f, 1.f,
       1.0f,  0.0f, -1.0f,   0.f, 1.f, 1.f,

       // BR face: Bottom, C, D  (orange)
        0.0f, -1.0f,  0.0f,   1.f, 0.5f, 0.f,
        1.0f,  0.0f, -1.0f,   1.f, 0.5f, 0.f,
        1.0f,  0.0f,  1.0f,   1.f, 0.5f, 0.f,

        // BF face: Bottom, D, A  (purple-ish)
         0.0f, -1.0f,  0.0f,   0.6f, 0.f, 1.f,
         1.0f,  0.0f,  1.0f,   0.6f, 0.f, 1.f,
        -1.0f,  0.0f,  1.0f,   0.6f, 0.f, 1.f,
};

void ProcessKeyboard(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void ResizeCallback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

// Camera position in world coordinates
glm::vec3 oct_pos = glm::vec3(0.f, 0.f, -5.0f);
glm::vec3 cam_pos = glm::vec3(0.f, 0.5f, 0.f);


int main(int argc, char** argv)
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(640, 480, "3D modelling", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, ResizeCallback);

    gl3wInit();

    // Depth + clear colour MUST be after OpenGL init
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.f, 1.f, 1.f, 1.f);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, 0);

    unsigned int shaderProgram = CompileShader("triangle.vert", "triangle.frag");

    glCreateBuffers(NUM_BUFFERS, Buffers);
    glGenVertexArrays(NUM_VAOS, VAOs);

    glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(shaderProgram);

        // MODEL: translate to oct_pos then rotate around Y
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, oct_pos);
        model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.f, 1.f, 0.f));

        // VIEW: move camera up
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, -cam_pos);

        // PROJECTION: perspective frustum
        float aspect = 640.0f / 480.0f;
        float fovy = glm::radians(25.0f);   
        glm::mat4 projection = glm::perspective(fovy, aspect, 1.0f, 10.0f);


        // Upload uniforms
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        ProcessKeyboard(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
