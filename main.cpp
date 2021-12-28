#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

#include "LiteGL.h"

using namespace lit;

glm::vec3 Mesh[] = {
    glm::vec3(-0.5, -0.5, 0),
    glm::vec3(0, 0.5, 0),
    glm::vec3(0.5, -0.5,0)
};

glm::vec3 Colors[] = {
    glm::vec3(0.9,0.2,0.2),
    glm::vec3(0.4,0.8,0.9),
    glm::vec3(0.2,0.15,0.6)
};


void init(lit::LiteGL& renderer){
    /*
        This is a series of tests to ensure that the LiteGL framework is working and all is well in the world
    */

   /*
        Creating a VAO
   */
    lit::VertexArrayObject& VAO = renderer.CreateVAO();
    std::cout << "VAO Created with ID : " << VAO.getID() << "\n";
    CHECKERROR
    /*
        Creating a VBO
    */
   VAO.CreateVBO(GL_ARRAY_BUFFER, Mesh, sizeof(Mesh), GL_STATIC_DRAW);
   std::cout << "VBO Created with ID : " << VAO.getCurrentBufferID() << "\n";
   CHECKERROR
    /*
        Describe VBO
    */
    VAO.DescribeVBO(GL_FLOAT, 3, 0, 0);
    std::cout << "Described the current VBO to the VAO\n";
    CHECKERROR


    VAO.CreateVBO(GL_ARRAY_BUFFER, Colors, sizeof(Colors), GL_STATIC_DRAW);
    CHECKERROR

    VAO.DescribeVBO(GL_FLOAT, 3, 0, 0);
    CHECKERROR

    /*
        Compile Vertex Shader
    */
    lit::VertexShader& vert = renderer.CreateVertexShader("Shaders/basicvert.vs");
    std::cout << "Vertex Shader created with ID : " << vert.getID() << "\n";
    CHECKERROR
   /*
        Compile Fragment Shader
   */
    lit::FragmentShader& frag = renderer.CreateFragmentShader("Shaders/basicfrag.fs");
    std::cout << "Fragment Shader created with ID : " << frag.getID() << "\n";
    CHECKERROR
    /*
        Create Shader Program
    */
   lit::ShaderProgram& shaderprog = renderer.CreateShaderProgram(vert, frag);
   std::cout << "Shader program created with ID : " << shaderprog.getID() << "\n";
   CHECKERROR

    /*
        Set created shader program as the one to be used
    */
   shaderprog.UseProgram();
   CHECKERROR
}


int main(void)
{
    
    lit::LiteGL Renderer("Test Window", 640, 480);
    CHECKERROR

    init(Renderer);
    CHECKERROR

    /* Loop until the user closes the window */
    while (!Renderer.ShouldWindowClose())
    {
        /* Render here */
        Renderer.ClearScreen();


        glDrawArrays(GL_TRIANGLES, 0, 3);
        CHECKERROR

        /* Swap front and back buffers */
        Renderer.SwapFrameBuffers();

        /* Poll for and process events */
        Renderer.PollEvents();
    }

    Renderer.EndRenderer();

    return 0;
}
