#include "Shader.h"

Shader::Shader(){}
Shader::Shader(const char* vertexPath, const char* fragmentPath){
    generateShader(vertexPath, fragmentPath);
}
// compiles and links the vertex and fragment shaders
void Shader::generateShader(const char* vertexPath, const char* fragmentPath){
    int success;
    char infoLog[512];

    GLuint vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);

    // create program and link the two shaders
    id=glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    //link
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);// check for linking errors
    if(!success){
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::activate(){
    glUseProgram(id);
}
// creates a shader object , reads the shader source code from file, compiles the shader source code, and checks for compilation errors
GLuint Shader::compileShader(const char* filepath, GLenum source){
    int success;
    char infoLog[512];
    // build and compile vertex shader
    GLuint vertexShader = glCreateShader(source);
    glShaderSource(vertexShader, 1, &filepath, NULL);
    glCompileShader(vertexShader);

    // check for errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return vertexShader;
}

void Shader::setMat4(const std::string &name, glm::mat4 mat) const{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setBool(const std::string &name, bool value) const{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::set3Float(const std::string &name, glm::vec3 v){
    set3Float(name, v.x, v.y, v.z);
}
void Shader::set3Float(const std::string &name, float v1, float v2, float v3){
    glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set4Float(const std::string &name, float v1, float v2, float v3, float v4){
    glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}