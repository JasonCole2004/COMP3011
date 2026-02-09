
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


GLfloat vertices[] =
{
	//pos			
	0.f,  0.5f,
	-0.5f, -0.5f,
	0.5f,  -0.5f
};

const char* vertexShaderSource =
"#version 450 core\n"
"layout(location = 0) in vec4 vaPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vaPos;\n"
"}\n";

const char* fragmentShaderSource =
"#version 450 core\n"
"layout(location = 0) out vec4 fColour;\n"
"void main()\n"
"{\n"
"	fColour = vec4(0.f, 0.f, 1.f, 1.0f);\n"
"}\n";


#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "A Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);

	gl3wInit();

	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);


	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(program);

	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}