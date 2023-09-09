
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <cstdlib>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define useNormals false

using namespace glm;

struct objectData {
    std::vector<vec3> vert;
    std::vector<ivec3> face;
    //std::vector<float> normal;
    //std::vector<int> normalIndex;
};

objectData readObj(const std::string& filepath) {
    objectData data;
    std::ifstream stream(filepath);
    std::string line;
    std::string x, y, z;
    while (getline(stream, line)) {
        std::stringstream ss{ line };
        char ch;
        ss >> ch;
        ss >> x >> y >> z;
        if (ch == 'v') {
            vec3 vertex;
            vertex.x = std::stof(x);
            vertex.y = std::stof(y);
            vertex.z = std::stof(z);
            data.vert.push_back(vertex);
        }
        else if (ch == 'f') {
            vec3 face;
            face.x = std::stof(x);
            face.y = std::stof(y);
            face.z = std::stof(z);
            data.face.push_back(face);
        }
    }
    return data;
}

//objectData readObjWithNormals(const std::string& filepath) {
//    objectData data;
//    std::ifstream stream(filepath);
//    std::string line;
//    std::string x, y, z;
//    while (getline(stream, line)) {
//        std::stringstream ss{ line };
//        std::string ch;
//        ss >> ch;
//        ss >> x >> y >> z;
//        if (ch == "v") {
//            data.vert.push_back(std::stof(x));
//            data.vert.push_back(std::stof(y));
//            data.vert.push_back(std::stof(z));
//        }
//        else if (ch == "f") {
//            data.face.push_back(std::stoi(x));
//            data.normalIndex.push_back(std::stoi(y));
//            data.face.push_back(std::stoi(z));
//            ss >> x >> y >> z;
//            data.normalIndex.push_back(std::stoi(x));
//            data.face.push_back(std::stoi(y));
//            data.normalIndex.push_back(std::stoi(z));
//        }
//        else if (ch == "vn") {
//            data.normal.push_back(std::stof(x));
//            data.normal.push_back(std::stof(y));
//            data.normal.push_back(std::stof(z));
//        }
//    }
//    return data;
//}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static std::string ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        ss << line << "\n";
    }
    return ss.str();
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;

}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main(void) {

    

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    GLFWwindow* window;
    const int height = 1080;
    const int width = 1080;

    if (!glfwInit())
        return -1;

    //fullscreen
    //window = glfwCreateWindow(width, height, "Hello World", glfwGetPrimaryMonitor(), NULL);

    window = glfwCreateWindow(width, height, "yee", monitor, NULL);
    
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error";
    }

    float positions[] = {
        -1.0, -1.0, 0.0, 0.0, 
        1.0, -1.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        -1.0, 1.0, 0.0, 1.0
    };
    
    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    //stbi_set_flip_vertically_on_load(true);
    //int texWidth, texHeight, nrChannels;
    //unsigned int texture;
    //glGenTextures(1, &texture);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texture);
    //// set the texture wrapping/filtering options (on the currently bound texture object)
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //unsigned char* stbData = stbi_load("neuron.jpeg", &texWidth, &texHeight, &nrChannels, 0);
    //if (stbData)
    //{
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, stbData);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else
    //{
    //    std::cout << "Failed to load texture" << std::endl;
    //}
    //stbi_image_free(stbData);



    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);


    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

    std::string vertexShader = ParseShader("shaders/vertex.shader");
    std::string fragmentShader = ParseShader("shaders/fragment.shader");

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    glUniform1i(glGetUniformLocation(shader, "useNormals"), useNormals);
    
    objectData d;

    if (useNormals) {
        //d = readObjWithNormals("models/knightNormal.txt");

        /*std::cout << d.vert.size() << " " << d.face.size() << " " <<d.normal.size()<<" "<<d.normalIndex.size()<<" ";

        glUniform3fv(glGetUniformLocation(shader, "vertices"), d.vert.size()/3, &d.vert[0]);
        glUniform1i(glGetUniformLocation(shader, "verticesSize"), d.vert.size());

        glUniform3iv(glGetUniformLocation(shader, "faces"), d.face.size()/3, &d.face[0]);
        glUniform1i(glGetUniformLocation(shader, "facesSize"), d.face.size());

        glUniform3fv(glGetUniformLocation(shader, "normals"), d.normal.size()/3, &d.normal[0]);
        glUniform1i(glGetUniformLocation(shader, "normalsSize"), d.normal.size());

        glUniform3iv(glGetUniformLocation(shader, "normalIndices"), d.normalIndex.size()/3, &d.normalIndex[0]);
        glUniform1i(glGetUniformLocation(shader, "normalIndicesSize"), d.normalIndex.size());*/
    }
    else {
        d = readObj("models/sus.txt");

        std::cout << d.vert.size() << " " << d.face.size() << " ";

        glUniform3fv(glGetUniformLocation(shader, "vertices"), d.vert.size()/3, &d.vert[0].x);
        glUniform1i(glGetUniformLocation(shader, "verticesSize"), d.vert.size());

        glUniform3iv(glGetUniformLocation(shader, "faces"), d.face.size()/3, &d.face[0].x);
        glUniform1i(glGetUniformLocation(shader, "facesSize"), d.face.size());
    }
    int maxVertUniformsVect;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformsVect);
    std::cout <<"    "<<maxVertUniformsVect;

    ivec4 vertData[968];
    //std::vector<ivec3> vertData;
    int vertDataIndex = 0;
    for (int i = 0; i < 968; i++) {
        vertData[vertDataIndex] = ivec4(d.face.at(i),0);
        vertDataIndex++;
    }

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertData), &vertData, GL_STATIC_DRAW);
    
    GLuint bindingPoint = 0;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



    /*
        GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * flatVertices.size() + sizeof(unsigned int) * flatFaces.size(), nullptr, GL_STATIC_DRAW);

    // Map and copy data to the buffer
    GLvoid* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    std::memcpy(ptr, flatVertices.data(), sizeof(glm::vec3) * flatVertices.size());
    std::memcpy((char*)ptr + sizeof(glm::vec3) * flatVertices.size(), flatFaces.data(), sizeof(unsigned int) * flatFaces.size());
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointIndex, ssbo);

    */

    //GLuint ubo;
    //glGenBuffers(1, &ubo);
    //glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    //glBufferData(GL_UNIFORM_BUFFER, d.vert.size(), nullptr, GL_DYNAMIC_DRAW);

    //GLuint bindingPoint = 0;
    //glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
    //GLuint blockIndex = glGetUniformBlockIndex(shader, "MyUBO");
    //glUniformBlockBinding(shader, blockIndex, bindingPoint);

    //glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    //glBufferSubData(GL_UNIFORM_BUFFER, 0, d.vert.size()*4, d.vert.data());

    float r = 0.01f;
    float increment = 1.0f;

    float previousTime = glfwGetTime();
    int fps = 0;
    int frameNum = 0;
   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);


    while (!glfwWindowShouldClose(window)) {

        processInput(window);
        fps++;
        float currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1.0) {
            glfwSetWindowTitle(window, std::to_string(fps).c_str());
            std::cout << fps << " fps, " << 1000 / fps << " ms" << std::endl;
            fps = 0;
            previousTime = currentTime;
        }

        glUniform1f(glGetUniformLocation(shader, "sphereVertical"), r);


        glUniform3f(glGetUniformLocation(shader, "camPos"), r / 6, 0.0, 0.0);
        if (r < -10.0f || r>20.0f) {
            increment *= -1;
        }
        r += increment;


        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glUniform1i(glGetUniformLocation(shader, "directOutPass"), 0);
        //if (frameNum % 20 != 0) {
            glUniform1i(glGetUniformLocation(shader, "rotateDeg"), (frameNum - (frameNum % 20)) * 25);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, screenTexture);
        //}
        frameNum++;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUniform1i(glGetUniformLocation(shader, "directOutPass"), 1);
        glUniform1i(glGetUniformLocation(shader, "frameNumber"), frameNum);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

       /* if (frameNum % 20 == 1) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }*/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteProgram(shader);


    glfwTerminate();
    return 0;
}


