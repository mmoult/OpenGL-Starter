#include <glad/glad.h> // previously gl.h
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const int MAX_ERR_LENGTH = 1024;

bool installShader(int program, GLenum shaderType, const char* fileName)
{
    GLuint shader = glCreateShader(shaderType);
    {
        std::ifstream t(fileName);
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string contents = buffer.str();
        char* text = new char[contents.size() + 1];
        strncpy_s(text, contents.size() + 1, contents.c_str(), contents.size());
        glShaderSource(shader, 1, &text, NULL);
        delete[] text;
    }
    glCompileShader(shader);
    // Verify that compilation was successful
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLchar errLog[MAX_ERR_LENGTH] = { 0 };
        glGetShaderInfoLog(shader, MAX_ERR_LENGTH, NULL, errLog);
        std::cerr << "Error compiling shader (" << fileName << "): " << errLog << std::endl;
        return false;
    }

    // Now we will attach the shader and dispose of it
    glAttachShader(program, shader);
    glDeleteShader(shader);
    return true;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLuint vertex_buffer, vertex_shader, fragment_shader;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(); // previously had glfwGetProcAddress as arg
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint program = glCreateProgram();
    if (!installShader(program, GL_VERTEX_SHADER, "vertex.glsl") ||
        !installShader(program, GL_FRAGMENT_SHADER, "frag.glsl"))
        return -1;

    glLinkProgram(program);
    // Check for any errors:
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLchar errLog[MAX_ERR_LENGTH] = { 0 };
        glGetProgramInfoLog(program, MAX_ERR_LENGTH, NULL, errLog);
        std::cerr << "Error linking program: " << errLog << std::endl;
        return -1;
    }
    glUseProgram(program); // apply the program to the OpenGL state 

    GLint mvp_location = glGetUniformLocation(program, "MVP");
    GLint vpos_location = glGetAttribLocation(program, "vPos");
    GLint vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete the program after usage, which will automatically clean up any
    // attached shaders pending deletion.
    glDeleteProgram(program);

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}