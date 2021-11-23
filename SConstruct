import os

env = Environment()
env.Append(CPPPATH = 'include')
env.Append(LIBPATH = ['./libs/Debug'])
env.Append(LIBS = ['opengl32', 'glfw3', 'user32', 'gdi32', 'shell32', 'vcruntime', 'msvcrt'])
t = env.Program(target = 'main', source =['main.cpp'])
Default(t)
