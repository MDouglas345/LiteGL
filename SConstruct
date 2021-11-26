import os
from SCons.Defaults import SharedCheck, ProgScan
from SCons.Script.SConscript import SConsEnvironment
import platform

plat = platform.system()

env = Environment()


env.Append(CPPFLAGS = ['-std=c++17'])
if (plat == 'Darwin'):
    env['FRAMEWORKS'] = ['OpenGL']
    env.Append(CPPPATH = 'include')
    env.Append(LIBPATH = ['./libs/MACOSX'])
    env.Append(LIBS = ['libglfw.3.dylib','GLEW'])
else:
    env.Append(CPPPATH = 'include')
    env.Append(LIBPATH = ['./libs/Debug'])
    env.Append(LIBS = ['opengl32', 'glfw3', 'glew32sd', 'user32', 'gdi32', 'shell32', 'vcruntime', 'msvcrt'])

createStatic = ARGUMENTS.get('static', 0)
t = 0
if (createStatic):
    t = env.StaticLibrary('LiteGL', ['LiteGL.cpp', 'LiteGL.h'])
else:
    t = env.Program(target = 'main', source =['main.cpp'])
Default(t)
