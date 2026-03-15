#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"

float vertices[] =
{
    //pos                    //col
    -0.5f, -0.5f, -0.5f,     1.f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,     1.f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,     1.f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,     1.f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,     1.f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,     1.f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,

     0.5f,  0.5f,  0.5f,     1.f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,     1.f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,     1.f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,     1.f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,     1.f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,     1.f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,     1.f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,     1.f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,     1.f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,     1.f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,     1.f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,     1.f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,     0.0f, 1.f, 1.0f,
     0.5f,  0.5f, -0.5f,     0.0f, 1.f, 1.0f,
     0.5f,  0.5f,  0.5f,     0.0f, 1.f, 1.0f,
     0.5f,  0.5f,  0.5f,     0.0f, 1.f, 1.0f,
    -0.5f,  0.5f,  0.5f,     0.0f, 1.f, 1.0f,
    -0.5f,  0.5f, -0.5f,     0.0f, 1.f, 1.0f,
};

SCamera Camera;

#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 640
#define HEIGHT 480

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float x_offset = 0.f;   // yaw
    float y_offset = 0.f;   // pitch
    bool cam_changed = false;

    // YAW
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        x_offset = 1.f;
        cam_changed = true;
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        x_offset = -1.f;
        cam_changed = true;
    }

    // PITCH
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        y_offset = -1.f;
        cam_changed = true;
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        y_offset = 1.f;
        cam_changed = true;
    }

    // Distance controls (if still required)
    if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cam_dist -= 0.1f;
        cam_changed = true;
    }

    if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cam_dist += 0.1f;
        cam_changed = true;
    }

    if (cam_changed)
    {
        MoveAndOrientCamera(
            Camera,
            glm::vec3(0.f, 0.f, 0.f),
            cam_dist,
            x_offset,
            y_offset
        );
    }
}

void SizeCallback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv)
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Camera", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, SizeCallback);
    glfwSetKeyCallback(window, KeyCallback); // ✅ register callback

    gl3wInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, 0);

    GLuint program = CompileShader("triangle.vert", "triangle.frag");

    InitCamera(Camera);
    MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, 0.f, 0.f);

    glCreateBuffers(NUM_BUFFERS, Buffers);
    glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);

    glGenVertexArrays(NUM_VAOS, VAOs);
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (6 * sizeof(float)), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (6 * sizeof(float)), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
        glClearBufferfv(GL_COLOR, 0, bgd);
        glClear(GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(program);

        glm::mat4 model = glm::mat4(1.f);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection =
            glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, 0.1f, 10.f);
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents(); // ✅ callback runs during event processing
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}