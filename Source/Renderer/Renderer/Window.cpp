#include "Window.h"

#include <GLFW/glfw3.h>

#include <cassert>

namespace Novus
{
    bool Window::_glfwInitialized = false;

    Window::Window()
            : _window(nullptr)
    {

    }

    Window::~Window()
    {
        if (_window != nullptr)
        {
            glfwDestroyWindow(_window);
        }
    }

    void error_callback(i32 error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    bool Window::Init(u32 width, u32 height)
    {
        if (!_glfwInitialized)
        {
            if (!glfwInit())
            {
                assert(false);
                return false;
            }
            glfwSetErrorCallback(error_callback);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#if NC_DEBUG
        _window = glfwCreateWindow(width, height, "NovusEngine (DEBUG)", NULL, NULL);
#else
        _window = glfwCreateWindow(width, height, "NovusEngine", NULL, NULL);
#endif

        if (!_window)
        {
            assert(false);
            return false;
        }
        glfwSetWindowUserPointer(_window, this);

        return true;
    }

    bool Window::Update(f32 deltaTime)
    {
        glfwPollEvents();

        if (glfwWindowShouldClose(_window))
        {
            return false;
        }
        return true;
    }

    void Window::Present()
    {
        //glfwSwapBuffers(_window);
    }
}
