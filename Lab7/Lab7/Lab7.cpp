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


glm::vec3 lightDirection = glm::vec3(0.1f, -.81f, -.61f);
glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);

SCamera Camera;


float vertices[] =
{
	//back face
	//pos					//col				//normal
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f,  0.0f, -1.0f,

	//front face
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.0f,  0.0f,  1.0f,

	//left face
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,

	//right face
	0.5f,  0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f, 	1.f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,

	//bottom face
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.0f, -1.0f,  0.0f,

	//top face
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.f, 1.0f,	0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.f, 1.0f,	0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f,	0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f,	0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f,	0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, 	1.0f, 1.f, 1.0f,	0.0f,  1.0f,  0.0f
};



#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768


void drawFloorAndCubes(unsigned int shaderProgram)
{
	glBindVertexArray(VAOs[0]);

	//floor
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0, -3, 0));
	model = glm::scale(model, glm::vec3(100, 0.1, 100));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//cubes
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				glm::mat4 model = glm::mat4(1.f);
				model = glm::translate(model, glm::vec3(float(i * 2), float(j * 2), float(k * 2)));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}

}



void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lightDirection = Camera.Front;
		lightPos = Camera.Position;
	}

	float x_off = 0.f;
	float y_off = 0.f;
	bool cam_changed = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		y_off = -1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		y_off = 1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		x_off = -1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		x_off = 1.f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam_dist -= 0.1f;
		cam_changed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		cam_dist += 0.1f;
		cam_changed = true;
	}

	if (cam_changed)
	{
		MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_off, y_off);
	}
}

void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}


int main(int argc, char** argv)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	GLuint program = CompileShader("phong.vert", "phong.frag");

	InitCamera(Camera);
	cam_dist = 5.f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);


	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(program);

		glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(program, "lightColour"), 0.f, 0.f, 1.f);
		glUniform3f(glGetUniformLocation(program, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
		glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		drawFloorAndCubes(program);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
