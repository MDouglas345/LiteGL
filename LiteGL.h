/*
    My 2nd(?) take at a rendering engine (API?) using GLFW. Goal of LiteGL : Create an easy to use interface
    that allows for the creation of basic features of OpenGL. This includes Textures, Shaders, etc.
    This should be used with a higher level class that will call functions provided here to make something grand.

    Texture class holds all information about a texture (and perhaps some common variants?)
    LiteGL should store the unique ID for each Texture in a vector for quick lookup and allocation into shader programs.
*/

#ifndef LITEGL_H
#define LITEGL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace lit{

     inline void ExitGracefully(const char* text){
        std::cout << text << std::endl;
        glfwTerminate();
    }


     class Texture{
        private:
            GLuint m_ID;
            int m_Width, m_Height,nrChannels;
            unsigned char* m_Data;
            std::string Name;
        public:
        Texture(std::string FileLoc, std::string localname) : Name(localname){
            glGenTextures(1, &m_ID);
            glBindTexture(GL_TEXTURE_2D, m_ID);

            /*
                Need to add texture customizability, either through functions or inheritance
            */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            m_Data = stbi_load(FileLoc.c_str(), &m_Width, &m_Height, &nrChannels, 0);

            if (m_Data){
                /*
                    Lots can be worked on for customizability!!!
                */
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_Data);
            }
            else{
                std::string exit = "Failed to load Texture ";
                exit += localname;
                ExitGracefully(exit.c_str());
            }
        }
    };

    class Shader{
        protected:
            GLuint m_ID;
            std::string Name;
            std::string ShaderCode;
        public:
        Shader(char* fileloc, std::string localname) : Name(localname){
            ReadFile(fileloc);
            CompileShader();
            VerifyShader();
        }
        void ReadFile(char* file){
            std::ifstream ShaderStream(file, std::ios::in);
            if(ShaderStream.is_open()){
                std::stringstream sstr;
                sstr << ShaderStream.rdbuf();
                ShaderCode = sstr.str();
                ShaderStream.close();
	        }else{
                printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", file);
                getchar();
		        return ;
	        }

        }

        void VerifyShader(){
            GLint Result = GL_FALSE;
            int InfoLogLength;

            glGetShaderiv(m_ID, GL_COMPILE_STATUS, &Result);
	        glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	        if ( InfoLogLength > 0 ){
		        std::vector<char> ShaderErrorMessage(InfoLogLength+1);
		        glGetShaderInfoLog(m_ID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		        printf("%s\n", &ShaderErrorMessage[0]);
	        }
        }

        virtual void CompileShader () = 0;
    };

    class VertexShader : public Shader{
        public:
        VertexShader(char* fileloc, std::string localname) : Shader(fileloc, localname){

        }
        void CompileShader() override{
            m_ID = glCreateShader(GL_VERTEX_SHADER);
            char const * ShaderSourcePointer = ShaderCode.c_str();
            glShaderSource(m_ID, 1, &ShaderSourcePointer, NULL);
        }
    };

    class FragmentShader : public Shader{
        public:
        FragmentShader(char* fileloc, std::string localname) : Shader(fileloc, localname){
            
        }
         void CompileShader() override{
             m_ID = glCreateShader(GL_FRAGMENT_SHADER);
            char const * ShaderSourcePointer = ShaderCode.c_str();
            glShaderSource(m_ID, 1, &ShaderSourcePointer, NULL);
        }
    };

    class ComputeShader : public Shader{
        public:
        ComputeShader(char* fileloc, std::string localname) : Shader(fileloc, localname){
            
        }
         void CompileShader() override{
            m_ID = glCreateShader(GL_COMPUTE_SHADER);
            char const * ShaderSourcePointer = ShaderCode.c_str();
            glShaderSource(m_ID, 1, &ShaderSourcePointer, NULL);
        }
    };

    class ShaderProgram{
        private:
        GLuint m_ID;

        public:
        ShaderProgram(GLuint VertexShader, GLuint FragmentShader){

        }
    };

   

    class LiteGL{
    private:

    /*
        WINDOW VARIABLES
    */
    GLFWwindow* m_Window;
    std::string m_WindowName;
    int m_WindowHeight;
    int m_WindowLength;

    /*
        TEXTURE VARIABLES
    */
    std::vector<Texture> Textures;
   


    public:
    LiteGL(std::string name, size_t wl, size_t wh) : m_WindowHeight(wh), m_WindowLength(wl){
        if (!glfwInit()){
            std::cout << "Failed to init GLFW, nothing will continue./n";
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

         m_Window = glfwCreateWindow(wl, wh, name.c_str(), NULL, NULL);

         if (!m_Window){
             ExitGracefully("Failed to get Window running!");
         }

         glfwMakeContextCurrent(m_Window);
    }

    bool ShouldWindowClose(){
        return glfwWindowShouldClose(m_Window);
    }

    void SwapFrameBuffers(){
        glfwSwapBuffers(m_Window);
    }

    void PollEvents(){
         glfwPollEvents();
    }

    void ClearScreen(){
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void EndRenderer(){
        glfwTerminate();
    }

    };

   



}
#endif
