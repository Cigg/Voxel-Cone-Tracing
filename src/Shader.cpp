#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <string.h>

#include "Shader.h"

GLuint loadShaders(const char* vert, const char* frag, const char* geom) {
    // Create the shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint geometryShader;
    if(geom) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    }
    
    // Read the Vertex Shader code from the file
    std::string vertexShaderCode;
    std::ifstream vertexShaderStream(vert, std::ios::in);
    if(vertexShaderStream.is_open()) {
        std::string Line = "";
        while(getline(vertexShaderStream, Line))
            vertexShaderCode += "\n" + Line;
        vertexShaderStream.close();
    } else {
        std::cout << "Couldn't open shader " << vert << "!" << std::endl;
        getchar();
        return 0;
    }
    
    // Read the Fragment Shader code from the file
    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(frag, std::ios::in);
    if(fragmentShaderStream.is_open()) {
        std::string Line = "";
        while(getline(fragmentShaderStream, Line))
            fragmentShaderCode += "\n" + Line;
        fragmentShaderStream.close();
    } else {
        std::cout << "Couldn't open shader " << frag << "!" << std::endl;
        getchar();
        return 0;
    }
    
    // Read the Geometry Shader code from the file
    std::string geometryShaderCode;
    if(geom) {
        std::ifstream geometryShaderStream(geom, std::ios::in);
        if(geometryShaderStream.is_open()){
            std::string Line = "";
            while(getline(geometryShaderStream, Line))
                geometryShaderCode += "\n" + Line;
            geometryShaderStream.close();
        } else {
            std::cout << "Couldn't open shader " << geom << "!" << std::endl;
            getchar();
            return 0;
        }
    }
    
    GLint Result = GL_FALSE;
    int infoLogLength;
    
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vert);
    char const * vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShader, 1, &vertexSourcePointer , NULL);
    glCompileShader(vertexShader);
    
    // Check Vertex Shader
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ){
        std::vector<char> errorMessage(infoLogLength+1);
        glGetShaderInfoLog(vertexShader, infoLogLength, NULL, &errorMessage[0]);
        printf("%s\n", &errorMessage[0]);
    }
    
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", frag);
    char const * fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSourcePointer , NULL);
    glCompileShader(fragmentShader);
    
    // Check Fragment Shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ) {
        std::vector<char> errorMessage(infoLogLength+1);
        glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, &errorMessage[0]);
        printf("%s\n", &errorMessage[0]);
    }
    
    // Compile Geometry Shader
    if(geom) {
        printf("Compiling shader : %s\n", geom);
        char const * geometrySourcePointer = geometryShaderCode.c_str();
        glShaderSource(geometryShader, 1, &geometrySourcePointer , NULL);
        glCompileShader(geometryShader);
        
        // Check Geometry Shader
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        if ( infoLogLength > 0 ) {
            std::vector<char> errorMessage(infoLogLength+1);
            glGetShaderInfoLog(geometryShader, infoLogLength, NULL, &errorMessage[0]);
            printf("%s\n", &errorMessage[0]);
        }
    }
    
    // Link the program
    printf("Linking program\n");
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if(geom) {
        glAttachShader(program, geometryShader);
    }
    glLinkProgram(program);
    
    // Check the program
    glGetProgramiv(program, GL_LINK_STATUS, &Result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(infoLogLength+1);
        glGetProgramInfoLog(program, infoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if(geom) {
        glDeleteShader(geometryShader);
    }
    
    return program;
}