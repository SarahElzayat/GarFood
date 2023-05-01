#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram {

    private:
        //Shader Program Handle (OpenGL object name)
        GLuint program;

    public:
        ShaderProgram()
        {
            //TODO: (Req 1) Create A Shader Program
            program = glCreateProgram();
        }
        ~ShaderProgram()
        {
            //TODO: (Req 1) Delete a shader program
            glDeleteProgram(program);
        }

        bool attach(const std::string& filename, GLenum type) const;

        bool link() const;

        void use()
        {
            glUseProgram(program);
        }

        GLuint getUniformLocation(const std::string& name)
        {
            //TODO: (Req 1) Return the location of the uniform with the give name
            return glGetUniformLocation(program, name.c_str());
        }

        //Here we set the the value of a UNIFORM VARIABLE for the current PROGRAM object in its location
        //whether it's to an int, float, vectors, matrix
        void set(const std::string& uniform, GLfloat value)
        {
            //TODO: (Req 1) Send the given float value to the given uniform
            glUniform1f(getUniformLocation(uniform), value);
        }

        void set(const std::string& uniform, GLuint value)
        {
            //TODO: (Req 1) Send the given unsigned integer value to the given uniform
            glUniform1ui(getUniformLocation(uniform), value);
        }

        void set(const std::string& uniform, GLint value)
        {
            //TODO: (Req 1) Send the given integer value to the given uniform
            glUniform1i(getUniformLocation(uniform), value);
        }

        void set(const std::string& uniform, glm::vec2 value)
        {
            //TODO: (Req 1) Send the given 2D vector value to the given uniform
            glUniform2f(getUniformLocation(uniform), value.x, value.y);
        }

        void set(const std::string& uniform, glm::vec3 value)
        {
            //TODO: (Req 1) Send the given 3D vector value to the given uniform
            glUniform3f(getUniformLocation(uniform), value.x, value.y, value.z);
        }

        void set(const std::string& uniform, glm::vec4 value)
        {
            //TODO: (Req 1) Send the given 4D vector value to the given uniform
            glUniform4f(getUniformLocation(uniform), value.x, value.y, value.z, value.w);
        }

        void set(const std::string& uniform, glm::mat4 matrix)
        {
            //TODO: (Req 1) Send the given matrix 4x4 value to the given uniform
            glUniformMatrix4fv(getUniformLocation(uniform), 1, GL_FALSE, glm::value_ptr(matrix));
        }

        //TODO: (Req 1) Delete the copy constructor and assignment operator.
        //Question: Why do we delete the copy constructor and assignment operator?
        //         What happens if we don't delete them?

        // Because we don't want to leave a copy the shader program, we want to use the SAME one.
        // if we don't delete it then we'll have 2 shaders. which isn't the case wanted
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
    };

}

#endif