#define _USE_MATH_DEFINES 

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "error.h"
#include "file.h"
#include "shader.h"


#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h);
}


#define DEG2RAD(n) ((n) * (M_PI / 180.0f))


float* CreateCircle(int num_segments, float radius)
{
	// 3 vertices per triangle, 3 floats per vertex
	int floats_per_triangle = 9;
	int total_floats = num_segments * floats_per_triangle;

	float* verts = (float*)malloc(sizeof(float) * total_floats);

	float offset = 360.0f / num_segments;
	float angle = 0.0f;

	int index = 0;

	for (int i = 0; i < num_segments; i++)
	{
		// v0 
		verts[index++] = 0.0f;
		verts[index++] = 0.0f;
		verts[index++] = 0.0f;

		// v1
		verts[index++] = radius * sin(DEG2RAD(angle));
		verts[index++] = radius * cos(DEG2RAD(angle));
		verts[index++] = 0.0f;

		// v2 
		verts[index++] = radius * sin(DEG2RAD(angle + offset));
		verts[index++] = radius * cos(DEG2RAD(angle + offset));
		verts[index++] = 0.0f;

		angle += offset;
	}

	return verts;
}

int main()
{
	glfwInit();

	//set up window
	GLFWwindow* window = glfwCreateWindow(640, 480, "2D modelling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();

	// debuging
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// compile shaders 
	GLuint program = CompileShader("triangle.vert", "triangle.frag");

	int num_segments = 128;
	float radius = 0.5;

	float* vertices = CreateCircle(num_segments, radius);

	int buffer_size = sizeof(float) * num_segments * 9;


	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], buffer_size, vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(
		0,                  // location
		3,                  // x, y, z
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(float),
		(void*)0
	);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, num_segments * 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free(vertices);

	glfwDestroyWindow(window);
	glfwTerminate();
}