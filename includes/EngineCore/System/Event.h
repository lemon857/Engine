#pragma once

#include "EngineCore/System/Keys.h"

#include <array>
#include <functional>

enum class EventType
{
    WindowResize = 0,
    WindowClose,

    KeyPressed,
    KeyReleased,

    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,

    MaximizeWindow,
    MoveWindow,

    CharSet,

    EventsCount
};

struct BaseEvent
{
	virtual ~BaseEvent() = default;
	virtual EventType get_type() const = 0;
};

class EventDispatcher
{
public:
    template<typename EventType>
    void add_event_listener(std::function<void(EventType&)> callback)
    {
        auto baseCallback = [func = std::move(callback)](BaseEvent& e)
        {
            if (e.get_type() == EventType::type)
            {
                func(static_cast<EventType&>(e));
            }
        };
        m_eventCallbacks[static_cast<size_t>(EventType::type)] = std::move(baseCallback);
    }

    void dispatch(BaseEvent& event)
    {
        auto& callback = m_eventCallbacks[static_cast<size_t>(event.get_type())];
        if (callback)
        {
            callback(event);
        }
    }

private:
    std::array<std::function<void(BaseEvent&)>, static_cast<size_t>(EventType::EventsCount)> m_eventCallbacks;
};

struct EventMouseMoved : public BaseEvent
{
    EventMouseMoved(const double new_x, const double new_y)
        : x(new_x)
        , y(new_y)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    double x;
    double y;

    static const EventType type = EventType::MouseMoved;
};

struct EventWindowResize : public BaseEvent
{
    EventWindowResize(const unsigned int new_width, const unsigned int new_height)
        : width(new_width)
        , height(new_height)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    unsigned int width;
    unsigned int height;

    static const EventType type = EventType::WindowResize;
};
struct EventWindowClose : public BaseEvent
{
    virtual EventType get_type() const override
    {
        return type;
    }

    static const EventType type = EventType::WindowClose;
};

struct EventCharSet : public BaseEvent
{
    EventCharSet(const wchar_t key_char)
        : key_char(key_char)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    wchar_t key_char;

    static const EventType type = EventType::CharSet;
};


struct EventKeyPressed : public BaseEvent
{
    EventKeyPressed(const KeyCode key_code, const bool repeated)
        : key_code(key_code)
        , repeated(repeated)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    KeyCode key_code;
    bool repeated;

    static const EventType type = EventType::KeyPressed;
};

struct EventKeyReleased : public BaseEvent
{
    EventKeyReleased(const KeyCode key_code)
        : key_code(key_code)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    KeyCode key_code;

    static const EventType type = EventType::KeyReleased;
};

struct EventMouseButtonPressed : public BaseEvent
{
    EventMouseButtonPressed(const MouseButton mouse_button, const double x_pos, const double y_pos)
        : mouse_button(mouse_button)
        , x_pos(x_pos)
        , y_pos(y_pos)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    MouseButton mouse_button;
    double x_pos;
    double y_pos;

    static const EventType type = EventType::MouseButtonPressed;
};

struct EventMouseButtonReleased : public BaseEvent
{
    EventMouseButtonReleased(const MouseButton mouse_button, const double x_pos, const double y_pos)
        : mouse_button(mouse_button)
        , x_pos(x_pos)
        , y_pos(y_pos)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    MouseButton mouse_button;
    double x_pos;
    double y_pos;

    static const EventType type = EventType::MouseButtonReleased;
};

struct EventMaximizeWindow : public BaseEvent
{
    EventMaximizeWindow(const bool isMaximixedWindow)
        : isMaximized(isMaximixedWindow)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    bool isMaximized;

    static const EventType type = EventType::MaximizeWindow;
};
struct EventMoveWindow : public BaseEvent
{
    EventMoveWindow(const int xPos, const int yPos)
        : x_pos(xPos)
        , y_pos(yPos)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    int x_pos;
    int y_pos;

    static const EventType type = EventType::MoveWindow;
};

struct EventMouseScrolled : public BaseEvent
{
    EventMouseScrolled(const float x, const float y)
        : x_offset(x)
        , y_offset(y)
    {
    }

    virtual EventType get_type() const override
    {
        return type;
    }

    float x_offset;
    float y_offset;

    static const EventType type = EventType::MouseScrolled;
};