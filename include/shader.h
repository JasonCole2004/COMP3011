#pragma once

GLuint CompileShader(const char* vsFilename, const char* fsFilename)
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* vertexShaderSource = read_file(vsFilename);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* fragmentShaderSource = read_file(fsFilename);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    free(fragmentShaderSource);
    free(vertexShaderSource);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

