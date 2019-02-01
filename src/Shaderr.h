#pragma once

#include "includes.h"

class Shader {
public:
    GLuint program;
	Shader();
    Shader(std::string vertSource, std::string fragSource);
    std::string readFile(std::string filename);
	GLuint compileFromStrings(std::string vsh, std::string fsh);
    GLuint makeVertexShader(const char* shaderSource);
    GLuint makeFragmentShader(const char* shaderSource);
    void makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID);
    GLint bindAttribute(const char* attribute_name);
    GLint bindUniform(const char* uniform_name);
    void saveProgramInfoLog(GLuint obj);
    void saveShaderInfoLog(GLuint obj);
    std::string log;
    
    
};

