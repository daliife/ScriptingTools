#include "Shader.h"
#include <vector>
#include <fstream>
#include <sstream>


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

Shader::Shader() {}

std::string Shader::readFile(std::string filename) {
	std::ifstream f(filename);
	std::stringstream buffer;
	buffer << f.rdbuf();
	std::string content = buffer.str();
	return content;
}

Shader::Shader(std::string vertSource, std::string fragSource) {
    
	std::string vertexShaderSourceCode=readFile(vertSource);
	std::string fragmentShaderSourceCode=readFile(fragSource);
    makeShaderProgram(makeVertexShader(vertexShaderSourceCode.c_str()), makeFragmentShader(fragmentShaderSourceCode.c_str()));
}

GLuint Shader::compileFromStrings(std::string vsh, std::string fsh) {
	makeShaderProgram(makeVertexShader(vsh.c_str()), makeFragmentShader(fsh.c_str()));
	return 1;
}

GLuint Shader::makeVertexShader(const char* shaderSource)
{
    GLuint vertexShaderID=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID,1,(const GLchar**)&shaderSource, NULL);
    glCompileShader(vertexShaderID);
    
    GLint compile=0;
    glGetShaderiv(vertexShaderID,GL_COMPILE_STATUS,&compile);
    
    //we want to see the compile log if we are in debug (to check warnings)
    if (!compile)
    {
        saveShaderInfoLog(vertexShaderID);
        std::cout << "Shader code:\n " << std::endl;
        std::string code = shaderSource;
        std::vector<std::string> lines = split( code, '\n' );
        for( size_t i = 0; i < lines.size(); ++i)
            std::cout << i << "  " << lines[i] << std::endl;
    
    }
    
    
    return vertexShaderID;
}
GLuint Shader::makeFragmentShader(const char* shaderSource)
{
    GLuint fragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID,1,(const GLchar**)&shaderSource, NULL);
    glCompileShader(fragmentShaderID);
    
    GLint compile=0;
    glGetShaderiv(fragmentShaderID,GL_COMPILE_STATUS,&compile);
    
    //we want to see the compile log if we are in debug (to check warnings)
    if (!compile)
    {
        saveShaderInfoLog(fragmentShaderID);
        std::cout << "Shader code:\n " << std::endl;
        std::string code = shaderSource;
        std::vector<std::string> lines = split( code, '\n' );
        for( size_t i = 0; i < lines.size(); ++i)
            std::cout << i << "  " << lines[i] << std::endl;
        
    }
    
    return fragmentShaderID;
}

void Shader::saveShaderInfoLog(GLuint obj)
{
    int len = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
    
    if (len > 0)
    {
        char* ptr = new char[len+1];
        GLsizei written=0;
        glGetShaderInfoLog(obj, len, &written, ptr);
        ptr[written-1]='\0';
        log.append(ptr);
        delete[] ptr;
        
        printf("LOG **********************************************\n%s\n",log.c_str());
    }
}

void Shader::saveProgramInfoLog(GLuint obj)
{
    int len = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
    
    if (len > 0)
    {
        char* ptr = new char[len+1];
        GLsizei written=0;
        glGetProgramInfoLog(obj, len, &written, ptr);
        ptr[written-1]='\0';
        log.append(ptr);
        delete[] ptr;
        
        printf("LOG **********************************************\n%s\n",log.c_str());
    }
}

void Shader::makeShaderProgram(GLuint vertexShaderID,GLuint fragmentShaderID)
{
    program=glCreateProgram();
    glAttachShader(program, vertexShaderID);
    glAttachShader(program,fragmentShaderID);
    
    glLinkProgram(program);
    GLint link_ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        fprintf(stderr, "glLinkProgram:");
        saveProgramInfoLog(program);
    
    }
}

GLint Shader::bindAttribute(const char* attribute_name) {
    GLint attribute_ID = glGetAttribLocation(program, attribute_name);
    if (attribute_ID == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return 0;
    }
    else return attribute_ID;
}

GLint Shader::bindUniform(const char* uniform_name) {
    GLint uniform_ID = glGetUniformLocation(program, uniform_name);
    if (uniform_ID == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }
    else return uniform_ID;
}
