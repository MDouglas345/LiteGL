import os

env = Environment(CPPFLAGS ='-std=c++17')
env.Append(LIBS = ['opengl32'])
t = env.Program(target = 'main', source =['main.cpp'])
Default(t)
