/*
    My 2nd(?) take at a rendering engine (API?) using GLFW. Goal of LiteGL : Create an easy to use interface
    that allows for the creation of basic features of OpenGL. This includes Textures, Shaders, etc.
    This should be used with a higher level class that will call functions provided here to make something grand.

    Texture class holds all information about a texture (and perhaps some common variants?)
    LiteGL should store the unique ID for each Texture in a vector for quick lookup and allocation into shader programs.
*/

/*
    Note about VAOs, VBOs, and Shaders :
    I designed this workflow where the mesh/object dictates what the shader should have.
    Meaning that a Mesh will contain vertex types (positions, normals, uvs, etc) and the VBOs will be created accordingly.
    The VAOs will follow suite, meaning that there is an implicit compatibility check between the VAO and the current shader to be used.

    APPENDUM:
    Obviously, the mesh dictates the shader! It will most likely have a matrial object that dictates the textures and shader to use!

    APPENDUM 2:
    Ok so, some changes to the philosophy. The VAO takes care of everything VBO related since the VAO is essentially a descriptor, it might as well
    control the VBO entirely.
    The core is still the same, however; The Mesh dictates the Material, the material stores Shader, Texture and VAO setup. How to include these unique things into the Mesh data?
    That is for future Michael to figure out.
*/

//TODO : Convert this whole summa into a .h and .cpp file. For Static Libary purposes. UPDATE : Done? When including the LiteGL library, inclde 
// #define STB_IMAGE_IMPLEMENTATION
// in the main application .cpp file


/*
    New structure plan! Abstract away the VBO! Only interface through the VAO in order to create VBOs and describe them.
*/

/*
    Prepare for a massive restructure when you want to use Vulkan *dark moon*
*/

/*
    Theres lots of redundant data/functions due to the trial and error of how these objects are structured! Look into the VBO since VertexAttributes exists. Lots of wasted memory
*/

/*
    NICE TO HAVE ! (In the plans for the future) Look into glBufferSubData! Great for modifying data on the GPU on the fly. Requires a buffer to be GL_DYNAMIC_*
*/

#ifndef LITEGL_H
#define LITEGL_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "stb_image.h"
#include <glm/glm.hpp>

//#define DEBUG

#ifdef DEBUG
#define CHECKERROR CheckError(__FILE__ , __LINE__);
#else 
#define CHECKERROR 
#endif


namespace lit{

     inline void ExitGracefully(const char* text){
        std::cout << text << std::endl;
        glfwTerminate();
    }

    inline void CheckError(char* file, int line){
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR){
            std::string error;
            switch(err){
                case GL_INVALID_ENUM: error = "GL_Invalid_Enum";break;
                case GL_INVALID_VALUE: error = "GL_Invalid_Value";break;
                case GL_INVALID_OPERATION: error = "GL_Invalid_Operation";break;
                case GL_STACK_OVERFLOW: error = "GL_Stack_Overflow";break;
                case GL_STACK_UNDERFLOW: error = "GL_Stack_Underflow";break;
                case GL_OUT_OF_MEMORY: error = "GL_Out_Of_Memory"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_Invalid_Framebuffer_Operation";break;
                case GL_CONTEXT_LOST: error = "GL_Context_Lost"; break;
                case GL_TABLE_TOO_LARGE: error = "GL_Table_Too_Large"; break;
            }
            std::cout << "GL Error : " << error << " in file : " << file << " at line : " << line << "\n";
        }
    }

    class GLobject{
        protected:
            GLuint m_ID;

        public:
            GLobject(){}
            GLuint getID();
    };

    class VertexBufferObject : public GLobject{
        private:
            GLenum m_DrawType;
            GLenum m_BufferType;
            void* m_Data;
            size_t m_Stride;
            unsigned int m_DrawMode;
            size_t m_BufferSize;
            size_t m_ElementCount;
            GLuint m_ElementType;
            GLenum m_DataType;

        public:

        VertexBufferObject();

        void AssignData(GLenum BufferType, size_t BufferSize,  GLenum DrawMode, void* Data);

        void BindVBO();

        size_t getStride();

        size_t getBufferSize();

        GLenum getDataType();
    };

    struct VertextAttributes{
        GLenum BufferType;
        size_t BufferSize;
        GLenum DataType;
        size_t ElementsPerVertex;
        size_t Stride;
        size_t Offset;
    };

    class VertexArrayObject : public GLobject{
        private:
            size_t m_AttribCount;
            std::vector<VertexBufferObject> m_VBOS;
            std::vector<VertextAttributes> m_Attribs;
            VertextAttributes* CurrentAttributes;
            VertexBufferObject* CurrentBuffer;
        public:
        
        VertexArrayObject();
        
        void AssignVBO(VertexBufferObject* Buffer);

        void DescribeVBO(GLenum DataType, size_t ElemsPerVert, size_t stride, size_t Offset);

        void CreateVBO(GLenum BufferType, void* Data, size_t DataSize, GLenum AccessMode);

        GLuint getCurrentBufferID();

        void BindVAO();
    };


     class Texture : public GLobject{
        private:
            int m_Width, m_Height,nrChannels;
            unsigned char* m_Data;
            std::string Name;
        public:
            Texture(std::string FileLoc, std::string localname);
    };
    
    namespace obj{

    }

    class Shader : public GLobject{
        protected:
            std::string Name;
            std::string ShaderCode;
        public:
            Shader(char* fileloc, std::string localname);

        void ReadFile(char* file);

        void InternalShaderSetup(char* source);

        void VerifyShader();

        virtual void CompileShader () = 0;

        virtual ~Shader(){}
    };

    /*
        Should make an automated uniform detection
    */
    class VertexShader : public Shader{
        public:

        VertexShader(char* fileloc, std::string localname);

        void CompileShader() override;
    };

    class FragmentShader : public Shader{
        public:

        FragmentShader(char* fileloc, std::string localname);

         void CompileShader() override;
    };

    class ComputeShader : public Shader{
        public:

        ComputeShader(char* fileloc, std::string localname);

         void CompileShader() override;
    };

    class ShaderProgram : public GLobject{
        private:
        GLuint vertShaderID, fragShaderID;
        VertexShader* vertShader;
        FragmentShader*  fragShader;

        public:

        ShaderProgram(GLuint vert, GLuint frag);

        ShaderProgram(char* vertexshader, char* fragementshader);

        virtual void Setup(){}

        int GetUniformLocation(char* uni);

        void UseProgram();
    };

   

    class LiteGL{
    private:


    //TODO: Use a Hash table (map) instead of vectors. These objects need to be identifiable to be interchangeable

    /*
        WINDOW VARIABLES
    */
    GLFWwindow* m_Window;
    std::string m_WindowName;
    int m_WindowHeight;
    int m_WindowLength;
    GLFWimage m_WindowIcon;

    /*
        TEXTURE VARIABLES
    */
    std::vector<Texture> m_Textures;

    /*
        SHADER VARIABLES
    */
   std::vector<VertexShader> m_VertShaders;
   std::vector<FragmentShader> m_FragShaders;
   std::vector<ComputeShader> m_ComputeShaders;
   std::vector<ShaderProgram> m_ShaderPrograms;

   /*
        VERTEX ARRAY OBJECT VARIABLES
   */
    std::vector<VertexArrayObject> VAOs;

    /*
        VERTEX BUFFER OBJECT VARIABLES
    */
    std::vector<VertexBufferObject> VBOs;
   


    public:

    LiteGL(std::string name, size_t wl, size_t wh);

    bool ShouldWindowClose();

    void SwapFrameBuffers();

    void PollEvents();

    void ClearScreen();

    void EndRenderer();


    lit::VertexArrayObject& CreateVAO();

    lit::VertexBufferObject& CreateVBO();

    lit::VertexShader& CreateVertexShader(char* source);

    lit::FragmentShader& CreateFragmentShader(char* source);

    lit::ComputeShader& CreateComputeShader(char* source);

    lit::ShaderProgram& CreateShaderProgram(lit::VertexShader& vert, lit::FragmentShader& frag);


    };

   



}
#endif
