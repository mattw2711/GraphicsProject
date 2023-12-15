#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class VertexBuffer
{
public:
    unsigned int buffer_ID;

    VertexBuffer(){
        return;
    };

    VertexBuffer(int size, const void* data){
        glGenBuffers(1, &buffer_ID);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    };

    void Bind(){
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ID);
    }

    void Unbind(){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};
#endif