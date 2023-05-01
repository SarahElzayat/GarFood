#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: (Req 2) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            
            elementCount=elements.size();
            int verticesCount=vertices.size();
            //Vertex buffers are used for storing vertices data.
            //generate a buffer object and store its ID in the variable VBO.
            //The parameter '1' represents that we need only 1 buffer
            //VBO is unsigned integer that will store the ID of the returned buffer
            glGenBuffers(1,&VBO); 
            //binds the vertex buffer object (VBO) to the context's array buffer target.
            //OpenGl usually Binds objects before using it
            //So any coming instructions will be about VBO until it is unbound or another VBO is bound.
            //any subsequent vertex attribute data that is passed to OpenGL will be stored in the VBO.
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            //allocate memory for vertex data in the GPU.
            //Send data to that buffer
            //First parameter is the buffer type(the buffer will be used for vertex data. )
            //Second parameter is the size of the buffer in bytes.
            //verticesCount is the number of vertices 
            //sizeof(Vertex) is the size of a single vertex structure.
            //Third parameter is a pointer to the actual vertex data, vertices.data()
            //(void*) is for casting it to void pointer
            //Fourth parameter defines the usage of the data
            //GL_STATIC_DRAW says that the data will not be changed (static) and it will be used for drawing
            glBufferData(GL_ARRAY_BUFFER,verticesCount*sizeof(Vertex),(void*)(vertices.data()),GL_STATIC_DRAW);
            
            //object that defines how the data in the vertex buffer is interpreted and sent to the vertex shader as attributes
            //generate a vertex array object and store its ID in the variable VAO.
            //The parameter '1' represents that we need only 1 array
            //VAO is unsigned integer that will store the ID of the returned array
            //Vertex_Array is the responsible for understanding how to read the data from Vertex buffer.
            //It reads data from Vertex_Buffer and sends it to Vertex_Shader.
            glGenVertexArrays(1, &VAO);
            //binds the vertex array object (VAO) to the current OpenGL context.
            //OpenGl usually Binds objects before using it
            //So any coming instructions will be about VAO until it is unbound or another VAO is bound.
            //any subsequent calls to draw commands will use the vertex data stored in this VAO.
            glBindVertexArray(VAO);

            //enables the vertex attribute array for the position attribute to recieve the data that will be sent (else: it will always recieve 0).
            //the position data for each vertex will be read from a buffer object and passed to the shader program
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            //That will defines how to read the bits from the buffer
            //setting up a vertex attribute pointer for the location attribute of a vertex. 
            //First parameter is the location that the data will be sent to (location of the attribute in the shader program).
            //Second parameters is the number of components in the location attribute(three components to the location attribute (x,y,z))
            //Third parameter is the type of each component
            //Fourth parameter defines whether it is normalized or not
            // false, specifies that the data should not be normalized before being used. 
            //Fifth parameter is the stride:it defines how many bytes it will pass after reading the 3 floats of location data to go to the location data of the next vertex.
            //The Sixth argument specifies the offset from the beginning of the vertex structure to the start of the position data.
            glVertexAttribPointer(ATTRIB_LOC_POSITION,3,GL_FLOAT,false,sizeof(Vertex),(void*)offsetof(Vertex,position));
            
            //enables the vertex attribute array for the position attribute to recieve the data that will be sent (else: it will always recieve 0).
            //the position data for each vertex will be read from a buffer object and passed to the shader program
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            //That will defines how to read the bits from the buffer
            //setting up a vertex attribute pointer for the color attribute of a vertex. 
            //First parameter is the location that the data will be sent to (location of the attribute in the shader program).
            //Second parameters is the number of components in the color attribute(four components to the color attribute (red, green, blue, and alpha))
            //Third parameter is the type of each component
            //Fourth parameter defines whether it is normalized or not
            //true, specifies that the data should be normalized (scaled to a range between 0 and 1) before being passed to the shader. 
            //Without nsetting it to true ==> all colors except the black will be white(values greater than 1 will be dealt as it is 1)
            //Fifth parameter is the stride:it defines how many bytes it will pass after reading the 4 bytes of color data to go to the color data of the next vertex.
            //The Sixth argument specifies the offset from the beginning of the vertex structure to the start of the color data.
            glVertexAttribPointer(ATTRIB_LOC_COLOR,4,GL_UNSIGNED_BYTE,true,sizeof(Vertex),(void*)offsetof(Vertex,color));
            
            //enables the vertex attribute array for the texture coordinates attribute to recieve the data that will be sent (else: it will always recieve 0).
            //the texture coordinates data for each vertex will be read from a buffer object and passed to the shader program
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            //That will defines how to read the bits from the buffer
            //setting up a vertex attribute pointer for the Tex_Coordinates attribute of a vertex. 
            //First parameter is the location that the data will be sent to (location of the attribute in the shader program).
            //Second parameters is the number of components in the Tex_Coordinates attribute(two components to the Tex_Coordinates attribute (x,y))
            //Third parameter is the type of each component
            //Fourth parameter defines whether it is normalized or not
            // false, specifies that the data should not be normalized before being used. 
            //Fifth parameter is the stride:it defines how many bytes it will pass after reading the 2 floats of Tex_Coordinates data to go to the Tex_Coordinates data of the next vertex.
            //The Sixth argument specifies the offset from the beginning of the vertex structure to the start of the texture coordinates data.
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD,2,GL_FLOAT,false,sizeof(Vertex),(void*)offsetof(Vertex,tex_coord));
            
            //enables the vertex attribute array for the normal attribute to recieve the data that will be sent (else: it will always recieve 0).
            //the normal data for each vertex will be read from a buffer object and passed to the shader program
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            //That will defines how to read the bits from the buffer
            //setting up a vertex attribute pointer for the Normal attribute of a vertex. 
            //First parameter is the location that the data will be sent to (location of the attribute in the shader program).
            //Second parameter is the number of components in the normal attribute(three components to the normal attribute)
            //Third parameter is the type of each component
            //Fourth parameter defines whether it is normalized or not
            //false, specifies that the data should not be normalized before being used. 
            //Fifth parameter is the stride:it defines how many bytes it will pass after reading the 3 floats of normal data to go to the normal data of the next vertex.
            //The Sixth argument specifies the offset from the beginning of the vertex structure to the start of the normal data.
            glVertexAttribPointer(ATTRIB_LOC_NORMAL,3,GL_FLOAT,false,sizeof(Vertex),(void*)offsetof(Vertex,normal));
            
            //s a buffer that contains data that defines the indices of the 3 vertices needed to draw each triangle.
            //With EBOs, the indices of the vertices that make up each triangle are stored in a buffer object.
            //When rendering objects, these indices are used to specify the order in which vertices should be drawn. 
            //generate a buffer object and store its ID in the variable EBO.
            //The parameter '1' represents that we need only 1 buffer
            //EBO is unsigned integer that will store the ID of the returned buffer
            glGenBuffers(1,&EBO); 
            //binds the element buffer object (EBO) to the context's element buffer target.
            //specify which buffer object to use for storing indices
            //OpenGl usually Binds objects before using it
            //So any coming instructions will be about EBO until it is unbound or another VAO is bound.
            //any subsequent operations on the element array buffer will affect the buffer object with ID "EBO".
            //Specifies for the VA how to read the elements
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
            //allocate and initialize a buffer object for storing element indices
            //The first parameter specifies the type of target buffer object
            //The second parameter specifies the size of the data to be stored in bytes,
            //The third parameter is a pointer to the data that will be copied into the buffer object.
            //data is in [elements] array
            //Fourth parameter defines the usage of the data
            //GL_STATIC_DRAW says that the data will not be changed (static) and it will be used for drawing
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,elementCount*sizeof(unsigned int),(void*)(elements.data()),GL_STATIC_DRAW);//GL_STATIC_DRAW==I won't change it and I will use it in drawing
        }

        // this function should render the mesh
        void draw() 
        {
            //binds the vertex array object (VAO) to the current OpenGL context.
            //OpenGl usually Binds objects before using it
            //So any coming instructions will be about VAO until it is unbound or another VAO is bound.
            //any subsequent vertex attribute data that is passed to OpenGL will be stored in the VAO.
            glBindVertexArray(VAO);
            //This function draws a set of triangles specified by an array of indices.
            //The first parameter specifies the type of primitive to be drawn, in this case triangles.
            //The second parameter specifies the number of elements (indices) in the index array.
            //The third parameter specifies the data type of the elements
            //The fourth parameter represents a pointer to the start of the index array.
            //(void*)0 start from the beginning of the element buffer
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void*)0);
            //TODO: (Req 2) Write this function
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){

            //1-Unbind all arrays to ensure that no objects are left bound when starting a new pass or when switching to another rendering context.
            
            //unbinds the currently bound vertex array object.
            glBindVertexArray(0);
            //unbinds the currently bound element array buffer.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            //unbinds the currently bound array buffer. 
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            //2-Delete all arrays
            
            //delete the vertex buffer object (VBO), element buffer object (EBO)
            //the first parameter is the number of buffers to be deleted
            //the second parameter is a pointer to the buffer to be deleted.
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            //delete vertex array object (VAO)
            //the first parameter specifies the number of vertex arrays to be deleted
            //the second parameter is a pointer to the array to be deleted.
            glDeleteVertexArrays(1, &VAO);
        }
 
        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}