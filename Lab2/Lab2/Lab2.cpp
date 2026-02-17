
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "file.h"
#include "shader.h"
#include "error.h"


GLfloat vertices[] =
{
	//pos				
	0.f,  0.5f,			1.0f, 0.f, 0.f,
	-0.5f, -0.5f,		0.f, 1.f, 0.f,
	0.5f,  -0.5f,		0.f, 0.f, 1.f
};



#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

float x_offset = 0.f;
float y_offset = 0.f;

void ProcessKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}

	// Arrow keys (Lab uses these for interaction)
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  x_offset -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) x_offset += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    y_offset += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  y_offset -= 0.01f;
}

void SizeCallback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "A Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, 0);

	GLuint program = CompileShader("triangle.vert", "triangle.frag");


	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);
		glUniform1f(glGetUniformLocation(program, "x_offset"), x_offset);
		glUniform1f(glGetUniformLocation(program, "y_offset"), y_offset);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
		
		ProcessKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

}