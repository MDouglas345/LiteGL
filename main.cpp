#include <iostream>
#include "LiteGL.h"

int main(void)
{
    lit::LiteGL Renderer("Test Window", 1280, 720);
   

    /* Loop until the user closes the window */
    while (!Renderer.ShouldWindowClose())
    {
        /* Render here */
        Renderer.ClearScreen();

        /* Swap front and back buffers */
        Renderer.SwapFrameBuffers();

        /* Poll for and process events */
        Renderer.PollEvents();
    }

    Renderer.EndRenderer();

    return 0;
}
