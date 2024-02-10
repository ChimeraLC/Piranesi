/* Header file for OpenGL Shader */

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    /* Constructor that compiles the shader
     * vertexPath - path to the vertex shader
     * fragmentPath - path to the fragment shader
     */
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        /* Load and read vertex and fragment shaders */
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        /* Track any errors while reading streams */
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            /* Attempt to open files */
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            /* Read file contents */
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            /* Close files */
            vShaderFile.close();
            fShaderFile.close();

            /* Convert data streams to strings */
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        /* If there was an error reading the shaders */
        catch (std::ifstream::failure& e)
        {
            std::cout << "Error when reading shader files: " << e.what() << std::endl;
        }

        /* Get pointers to shader code */
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();

        /* Compiling shaders */
        unsigned int vertex, fragment;

        /* Vertex shader */
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        /* Fragment shader */
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        /* Shader program */
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        /* Delete attached shaders */
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    /* 
    *  Effects:
    *      Tells gl to use this shader.
    */
    void use() 
    { 
        glUseProgram(ID); 
    }

    /* 
    *  Effects:
    *      Sets a uniform with the given name to a boolean value
    */
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a integer value
    */
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a float value
    */
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a vec2 value
    */
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a vec3 value
    */
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a vec4 value
    */
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a mat2 value
    */
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a mat3 value
    */
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    /* 
    *  Effects:
    *      Sets a uniform with the given name to a mat4 value
    */
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    
    /* 
    *  Effects:
    *      Checks for any compile errors during compilation.
    */
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "Error when compiling shader of type: " << type << "\n" << infoLog << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "Error when compiling shaders of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};
#endif