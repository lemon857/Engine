#include "EngineCore/Engine/Window.h"

#include "EngineCore/Editor/UI/UImodule.h"
#include "EngineCore/System/Log.h"
#include "EngineCore/Renderer/Renderer.h"
#include "EngineCore/Resources/ResourceManager.h"
#include "EngineCore/System/ImageLoader.h"

#include <GLFW/glfw3.h>

Window::Window(std::string title, std::string path_icon_png, glm::ivec2& window_position, glm::ivec2& window_size, bool maximized, bool fullscreen)
    : m_data({ std::move(title), window_size, window_position, maximized, fullscreen, nullptr })
    , m_path_icon_png(path_icon_png)
{
	init();
    //UImodule::on_window_create(m_pWindow);
}

Window::~Window()
{
	shuitdown();
}

void Window::on_update()
{
    glfwSwapBuffers(m_pWindow);
    glfwPollEvents();
}

void Window::maximize()
{
    glfwMaximizeWindow(m_pWindow);
}

void Window::set_pos(glm::ivec2& pos)
{
    glfwSetWindowPos(m_pWindow, pos.x, pos.y);
}

void Window::set_cursor_visible(bool isVisible)
{
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, isVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Window::set_fullscreen(bool isFullscreen)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(m_pWindow, isFullscreen ? monitor : nullptr, 0, 0, mode->width, mode->height, mode->refreshRate);
    if (!isFullscreen)
    {
        glfwSetWindowPos(m_pWindow, m_data.window_position.x, m_data.window_position.y);
    }
}

bool Window::is_fullscreen()
{
    return glfwGetWindowMonitor(m_pWindow) != nullptr;
}

glm::vec2 Window::get_current_cursor_position() const
{
    double x_pos, y_pos;
    glfwGetCursorPos(m_pWindow, &x_pos, &y_pos);
    return glm::vec2(x_pos, y_pos);
}

void Window::set_event_callback(const EventCallback& callback)
{
    m_data.event_callback = callback;
}

int Window::init()
{
    LOG_INFO("Creating window \"{0}\" size {1}x{2}", m_data.title, m_data.window_size.x, m_data.window_size.y);

    if (!RenderEngine::Renderer::init(m_pWindow))
    {
        LOG_CRIT("Fail init OpenGL renderer");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_pWindow = glfwCreateWindow(m_data.window_size.x, m_data.window_size.y,
        m_data.title.c_str(), nullptr, nullptr);
   
    if (!m_pWindow)
    {
        LOG_CRIT("Generate window failed");
        shuitdown();
        return -1;
    }

    glfwMakeContextCurrent(m_pWindow);
    set_pos(m_data.window_position);
    if (m_data.maximized) maximize();
    if (m_data.fullscreen) set_fullscreen(true);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_CRIT("Glad load failed");
        return -1;
    }

    GLFWimage icon;
    int channels = 0;
    icon.pixels = load_image_png(m_path_icon_png.c_str(), &icon.width, &icon.height, &channels, false);

    if (!icon.pixels) {
        LOG_WARN("Error loading icon window");
    }
    else {
        glfwSetWindowIcon(m_pWindow, 1, &icon);
        clear_image(icon.pixels);
    }


//#define OFF_CALLBACKS

#ifndef OFF_CALLBACKS
    glfwSetWindowUserPointer(m_pWindow, &m_data);

    glfwSetErrorCallback( 
        [](int error_code, const char* description)
        {
            LOG_CRIT("GLFW Error: {0}: {1}", error_code, description);
        });

    glfwSetMouseButtonCallback(m_pWindow, 
        [](GLFWwindow* pWindow, int key, int action, int mods)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
            double x_pos = 0, y_pos = 0;
            glfwGetCursorPos(pWindow, &x_pos, &y_pos);
            switch (action)
            {
            case GLFW_PRESS:
            {
                EventMouseButtonPressed e(static_cast<MouseButton>(key), x_pos, y_pos);
                data.event_callback(e);
                break;
            }
            case GLFW_RELEASE:
            {
                EventMouseButtonReleased e(static_cast<MouseButton>(key), x_pos, y_pos);
                data.event_callback(e);
                break;
            }
            }
        });

    glfwSetKeyCallback(m_pWindow,
        [](GLFWwindow* pWindow, int key, int scancode, int action, int mods)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
            switch (action)
            {
            case GLFW_PRESS:
            {
                EventKeyPressed e(static_cast<KeyCode>(key), false);
                data.event_callback(e);
                break;
            }
            case GLFW_REPEAT:
            {
                EventKeyPressed e(static_cast<KeyCode>(key), true);
                data.event_callback(e);
                break;
            }
            case GLFW_RELEASE:
            {
                EventKeyReleased e(static_cast<KeyCode>(key));
                data.event_callback(e);
                break;
            }
            }
            //LOG_INFO("Key: {0} - {1} - {2} - {3}", key, scancode, action, mods);
        });

    glfwSetCharCallback(m_pWindow, [](GLFWwindow* pWindow, unsigned int codepoint) {
        WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
        EventCharSet e(static_cast<char>(codepoint));
        data.event_callback(e);
        });

    glfwSetWindowSizeCallback(m_pWindow, 
        [](GLFWwindow* pWindow, int width, int height)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
            data.window_size = glm::ivec2(width, height);
            EventWindowResize e(width, height);
            data.event_callback(e);
        });

    glfwSetCursorPosCallback(m_pWindow,
        [](GLFWwindow* pWindow, double x, double y)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

            EventMouseMoved e(x, y);
            data.event_callback(e);
        });
    glfwSetScrollCallback(m_pWindow,
        [](GLFWwindow* pWindow, double x, double y)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

            EventMouseScrolled e(x, y);
            data.event_callback(e);
        });

    glfwSetWindowCloseCallback(m_pWindow,
        [](GLFWwindow* pWindow)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

            EventWindowClose e = EventWindowClose();
            data.event_callback(e);
        });

    glfwSetWindowMaximizeCallback(m_pWindow, 
        [](GLFWwindow* pWindow, int maximized)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

            EventMaximizeWindow e = EventMaximizeWindow(maximized == 1 ? true : false);
            data.event_callback(e);
        });
    glfwSetWindowPosCallback(m_pWindow,
        [](GLFWwindow* pWindow, int x_pos, int y_pos)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));

            EventMoveWindow e = EventMoveWindow(x_pos, y_pos);
            data.event_callback(e);
        });

#endif // !OFF_CALLBACKS

    RenderEngine::Renderer::setClearColor(0, 0, 0, 1);
    RenderEngine::Renderer::setDepthTest(true);

	return 0;
}

void Window::shuitdown()
{
    //UImodule::on_window_close();
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}