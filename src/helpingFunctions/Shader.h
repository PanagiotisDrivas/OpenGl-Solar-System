#ifndef SHADER_H
#define SHADER_H

#include "../glad.h" // include glad to get all the required OpenGL headers

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
  
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader{
public:
    unsigned int id;// the program ID
  
    // constructor reads and builds the shader
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    void activate();// use/activate the shader
    void generateShader(const char* vertexPath, const char* fragmentPath);
    // utility functions
    GLuint compileShader(const char* shader, GLenum source);
    
    // uniform functions
    void setMat4(const std::string &name, glm::mat4 mat) const;
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void set3Float(const std::string &name, glm::vec3 v);
    void set3Float(const std::string &name, float v1, float v2, float v3);
    void set4Float(const std::string &name, glm::mat4 v);
    void set4Float(const std::string &name, float v1, float v2, float v3, float v4);
};
  
#endif