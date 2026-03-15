#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>

#include "error.h"
#include "file.h"
#include "shader.h"






float cubeVertices[] = {
	// positions          // col			//SPECIFY ALPHA COORDINATE HERE
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.f,	
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,	1.f,

	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,	1.f,

	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.f,	1.f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.f,	1.f,

	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.f,	1.f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.f,	1.f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.f,	1.f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.f,	1.f,

	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.f,	1.f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.f,	1.f
};
float planeVertices[] = {
	// positions          // col			//SPECIFY ALPHA COORDINATE HERE
	 5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.f,
	-5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.f,
	-5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.f,

	 5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.f,	1.f,
	-5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.f,
	 5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.f,	1.f
};


float transparentVertices[] = {
	// positions				// RGB				//SPECIFY ALPHA COORDINATE HERE
	-.5f,  -0.5f,  0.0f,		1.0f,  0.0f, 0.0f,	0.2f,
	.0f, 0.5f,  0.0f,			1.0f,  0.0f, 0.0f,	0.2f,
	.5f, -0.5f,  0.0f,			1.0f,  0.0f, 0.0f,	0.2f
};


float camX = .0f;
float camZ = .0f;



void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}


void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camZ -= .01f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camZ += .01f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camX -= .01f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camX += .01f;
}


int main(int argc, char** argv)
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "Transparency", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	glEnable(GL_DEPTH_TEST);

	//ENABLE BLENDING HERE
	glEnable(GL_BLEND);
	//SET BLEND FUNCTION HERE
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int shaderProgram = CompileShader("triangle.vert", "triangle.frag");

	unsigned int VAO[3];
	glGenVertexArrays(3, VAO);
	unsigned int VBO[3];
	glGenBuffers(3, VBO);

	//INCLUDE ALPHA COORDINATE IN THE VAO HERE
	const GLsizei stride = 7 * sizeof(float);
	
	//cube
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//plane
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//transparent
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(shaderProgram);

	std::vector<glm::vec3> positions;
	positions.push_back(glm::vec3(0, 0, 2.f));
	positions.push_back(glm::vec3(0.5f, 0, 0.f));
	positions.push_back(glm::vec3(0, 0, 1.f));
	positions.push_back(glm::vec3(-.5f, 0, 0.5f));
	positions.push_back(glm::vec3(0, 0, 1.5f));


	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { .02f, .5f, .75f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		glm::mat4 view = glm::mat4(1.f);
		view = glm::translate(view, -glm::vec3(camX, 0.f, camZ));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)800 / (float)600, .001f, 10.f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model = glm::mat4(1.f);

		// cubes
		glBindVertexArray(VAO[0]);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(.5, .5, .5));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.0f));
		model = glm::rotate(model, (float)glfwGetTime() / -2.f, glm::vec3(0.0f, 1.0f, .0f));
		model = glm::scale(model, glm::vec3(.5, .5, .5));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// floor
		glBindVertexArray(VAO[1]);
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);


		//SORT THE TRANSPARENT TRIANGLE POSITIONS HERE
		std::map<float, glm::vec3> sorted_positions;

		for (int i = 0; i < positions.size(); i++)
		{
			// camera position in world space
			glm::vec3 camPos(camX, 0.0f, camZ);

			// vector from camera to triangle
			glm::vec3 v = camPos - positions[i];

			// distance from camera
			float distance = glm::length(v);

			// store in map (key = distance)
			sorted_positions[distance] = positions[i];
		}


		glBindVertexArray(VAO[2]);
	
		glDepthMask(GL_FALSE);
		for (auto it = sorted_positions.rbegin(); it != sorted_positions.rend(); ++it)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
				1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		glDepthMask(GL_TRUE);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
	}

	glDeleteVertexArrays(3, VAO);
	glDeleteBuffers(3, VBO);

	glfwTerminate();

	return 0;
}
