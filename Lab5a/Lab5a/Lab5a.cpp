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
	//pos					//col			
	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	1.0f,  1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	1.0f,  1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 0.0f,

	-1.0f, -1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	1.0f, -1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	-1.0f,  1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,

	-1.0f,  1.0f,  1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, -1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f,  1.0f,  	0.0f, 0.0f, 1.0f,

	1.0f,  1.0f,  1.0f,  	1.f, 1.0f, 0.0f,
	1.0f,  1.0f, -1.0f,  	1.f, 1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 	1.f, 1.0f, 0.0f,
	1.0f, -1.0f, -1.0f,  	1.f, 1.0f, 0.0f,
	1.0f, -1.0f,  1.0f,  	1.f, 1.0f, 0.0f,
	1.0f,  1.0f,  1.0f,  	1.f, 1.0f, 0.0f,

	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 1.0f,
	1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 1.0f,
	1.0f, -1.0f,  1.0f,  	1.f, 0.0f, 1.0f,
	1.0f, -1.0f,  1.0f,  	1.f, 0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,  	1.f, 0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 1.0f,

	-1.0f,  1.0f, -1.0f,  	0.0f, 1.f, 1.0f,
	1.0f,  1.0f, -1.0f,  	0.0f, 1.f, 1.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.f, 1.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.f, 1.0f,
	-1.0f,  1.0f,  1.0f,  	0.0f, 1.f, 1.0f,
	-1.0f,  1.0f, -1.0f, 	0.0f, 1.f, 1.0f,
};


void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void ResizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h);
}


int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 32);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Antialiasing", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();

	int max_samples = 0;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	printf("MSAA samples = %d\n", max_samples);

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



	glEnable(GL_DEPTH_TEST);



	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 0.f, 0.f, 0.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(shaderProgram);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::translate(view, -glm::vec3(0.0f, 0.f, 4.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), 640.f / 460.f, 1.f, 10.f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model = glm::mat4(1.f);
		model = glm::rotate(model, 0.4f, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}