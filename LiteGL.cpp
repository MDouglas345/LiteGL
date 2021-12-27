#include "LiteGL.h"

GLuint lit::GLobject::getID(){
    return m_ID;
}



lit::VertexBufferObject::VertexBufferObject() : GLobject(){
                glGenBuffers(1, &m_ID);
            }

void lit::VertexBufferObject::AssignData(GLenum BufferType, size_t BufferSize,  GLenum DrawMode, void* Data){
            m_BufferType = BufferType;
            m_Data = Data;
            m_BufferSize = BufferSize;
            m_DrawMode = DrawMode;
            BindVBO();
            CHECKERROR
            glBufferData(m_BufferType, m_BufferSize, m_Data, m_DrawMode);
            CHECKERROR
           
        }

void lit::VertexBufferObject::BindVBO(){
            glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        }

size_t lit::VertexBufferObject::getStride(){
            return m_Stride;
        }

size_t lit::VertexBufferObject::getBufferSize(){
    return m_BufferSize;
}

unsigned int lit::VertexBufferObject::getDataType(){
            return m_DataType;
        }



lit::VertexArrayObject::VertexArrayObject() : GLobject(){
             m_AttribCount = 0;
             glGenVertexArrays(1, &m_ID);
            }

void lit::VertexArrayObject::AssignVBO(VertexBufferObject* Buffer){
            BindVAO();
            CHECKERROR
            Buffer->BindVBO();
            CHECKERROR
            glVertexAttribPointer(m_AttribCount, Buffer->getBufferSize(), Buffer->getDataType(), GL_FALSE, Buffer->getStride(), (void*)0);
            CHECKERROR
            glEnableVertexAttribArray(m_AttribCount);
            CHECKERROR
            m_AttribCount++;
        }

void lit::VertexArrayObject::BindVAO(){
            glBindVertexArray(m_ID);
        }

GLuint lit::VertexArrayObject::getCurrentBufferID(){
    return CurrentBuffer->getID();
}

void lit::VertexArrayObject::CreateVBO(GLenum BufferType, void* Data, size_t DataSize, GLenum AccessMode){
    BindVAO();
    this->m_VBOS.push_back(lit::VertexBufferObject());
    VertexBufferObject& VBO = this->m_VBOS[this->m_VBOS.size() - 1];
    VBO.AssignData(BufferType, DataSize, AccessMode, Data);

    CHECKERROR

    VertextAttributes VertAtt{
        VertAtt.BufferType = BufferType,
        VertAtt.BufferSize = DataSize,
    };

    this->m_Attribs.push_back(VertAtt);
    this->CurrentAttributes = &(this->m_Attribs[this->m_Attribs.size() - 1]);
    this->CurrentBuffer = &(this->m_VBOS[this->m_VBOS.size() - 1]);
}

void lit::VertexArrayObject::DescribeVBO(GLenum DataType, size_t ElemsPerVert, size_t stride, size_t Offset){
    CurrentAttributes->DataType = DataType;
    CurrentAttributes->Stride = stride;
    CurrentAttributes->Offset = Offset;
    CurrentAttributes->ElementsPerVertex = ElemsPerVert;

    glVertexAttribPointer(this->m_Attribs.size()-1, CurrentAttributes->ElementsPerVertex, CurrentAttributes->DataType, GL_FALSE, CurrentAttributes->Stride, (void*)CurrentAttributes->Offset);
    CHECKERROR
    glEnableVertexAttribArray(0);
}



lit::Texture::Texture(std::string FileLoc, std::string localname) : GLobject(), Name(localname){
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



lit::Shader::Shader(char* fileloc, std::string localname) : GLobject(), Name(localname){
                ReadFile(fileloc);
            }

void lit::Shader::InternalShaderSetup(char* source){
    ShaderCode = source;
}

void lit::Shader::ReadFile(char* file){
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

void lit::Shader::VerifyShader(){
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



lit::VertexShader::VertexShader(char* fileloc, std::string localname) : Shader(fileloc, localname){
            CompileShader();
            VerifyShader();
        }

void lit::VertexShader::CompileShader(){
            m_ID = glCreateShader(GL_VERTEX_SHADER);
            char const * ShaderSourcePointer = ShaderCode.c_str();
            glShaderSource(m_ID, 1, &ShaderSourcePointer, NULL);
            glCompileShader(m_ID);
        }



lit::FragmentShader::FragmentShader(char* fileloc, std::string localname) : Shader(fileloc, localname){
            CompileShader();
            VerifyShader();  
        }

void lit::FragmentShader::CompileShader(){
             m_ID = glCreateShader(GL_FRAGMENT_SHADER);
            char const * ShaderSourcePointer = ShaderCode.c_str();
            glShaderSource(m_ID, 1, &ShaderSourcePointer, NULL);
            glCompileShader(m_ID);
        }



lit::ComputeShader::ComputeShader(char* fileloc, std::string localname) : Shader(fileloc, localname){
            CompileShader();
            VerifyShader();
        }

void lit::ComputeShader::CompileShader(){
            m_ID = glCreateShader(GL_COMPUTE_SHADER);
            char const * ShaderSourcePointer = ShaderCode.c_str();
            glShaderSource(m_ID, 1, &ShaderSourcePointer, NULL);
            glCompileShader(m_ID);
        }



lit::ShaderProgram::ShaderProgram(GLuint vert, GLuint frag) : GLobject(){
            m_ID = glCreateProgram();
            glAttachShader(m_ID, vert);
            glAttachShader(m_ID, frag);
            glLinkProgram(m_ID);
        }

lit::ShaderProgram::ShaderProgram(char* vertexshader, char* fragementshader) : GLobject(){
            m_ID = glCreateProgram();
            vertShader = new VertexShader(vertexshader, "vert");
            fragShader = new FragmentShader(fragementshader, "frag");

            vertShaderID = vertShader->getID();
            fragShaderID = fragShader->getID();
        }

int lit::ShaderProgram::GetUniformLocation(char* uni){
            return glGetUniformLocation(m_ID, uni);
        }

void lit::ShaderProgram::UseProgram(){
            glUseProgram(m_ID);
        }



lit::LiteGL:: LiteGL(std::string name, size_t wl, size_t wh) : m_WindowHeight(wh), m_WindowLength(wl){
        if (!glfwInit()){
            std::cout << "Failed to init GLFW, nothing will continue./n";
            return;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //This breaks on windows? Fix it.
        //WindowIcon.pixels = stbi_load("LiteGLIcon.png", &WindowIcon.width, &WindowIcon.height, 0, 4);
        //glfwSetWindowIcon(m_Window, 1, &WindowIcon);

         m_Window = glfwCreateWindow(wl, wh, name.c_str(), NULL, NULL);

         if (!m_Window){
             ExitGracefully("Failed to get Window running!");
         }

         glfwMakeContextCurrent(m_Window);

        glewExperimental = GL_TRUE;
        glewInit();
    }

bool lit::LiteGL::ShouldWindowClose(){
        return glfwWindowShouldClose(m_Window);
    }

void lit::LiteGL::SwapFrameBuffers(){
        glfwSwapBuffers(m_Window);
    }

void lit::LiteGL::PollEvents(){
         glfwPollEvents();
    }

void lit::LiteGL::ClearScreen(){
        glClear(GL_COLOR_BUFFER_BIT);
    }

void lit::LiteGL::EndRenderer(){
        glfwTerminate();
    }

lit::VertexArrayObject& lit::LiteGL::CreateVAO(){
    this->VAOs.push_back(lit::VertexArrayObject());
    return this->VAOs[this->VAOs.size()-1];
}

lit::VertexBufferObject& lit::LiteGL::CreateVBO(){
    this->VBOs.push_back(lit::VertexBufferObject());
    return this->VBOs[this->VBOs.size()-1];
}

lit::VertexShader& lit::LiteGL::CreateVertexShader(char* source){
    this->m_VertShaders.push_back(lit::VertexShader(source, "BasicVert"));
    return this->m_VertShaders[this->m_VertShaders.size()-1];
}

lit::FragmentShader& lit::LiteGL::CreateFragmentShader(char* source){
    this->m_FragShaders.push_back(lit::FragmentShader(source, "BasicFrag"));
    return this->m_FragShaders[this->m_FragShaders.size()-1];
}

lit::ComputeShader& lit::LiteGL::CreateComputeShader(char* source){
    this->m_ComputeShaders.push_back(lit::ComputeShader(source, "BasicFrag"));
    return this->m_ComputeShaders[this->m_ComputeShaders.size()-1];
}

lit::ShaderProgram& lit::LiteGL::CreateShaderProgram(lit::VertexShader& vert, lit::FragmentShader& frag){
    this->m_ShaderPrograms.push_back(lit::ShaderProgram(vert.getID(), frag.getID()));
    return this->m_ShaderPrograms[this->m_ShaderPrograms.size()-1];
}
