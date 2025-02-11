#pragma once

#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();

    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setVec3(const std::string &name, GLsizei count, glm::vec3 *value) const;
    void setVec2(const std::string &name, glm::vec2 value) const;

    unsigned int getProgram() const;

private:
    unsigned int ID;

    static void check_shader_compilation_errors(GLuint shader, std::string shader_type);
    static void check_program_link_error(GLuint program);


};

