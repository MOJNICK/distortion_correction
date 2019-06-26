#ifndef MOUSE_HPP
#define MOUSE_HPP
#include <opencv2/core/types.hpp>
#include <atomic>
#include "command.hpp"

class Command;

struct MouseEvent
{
    int type;
    cv::Point2i point;
};

class Mouse
{
public:
    std::unique_ptr<Command> command;

    Mouse(std::string onMouseWindowName, std::unique_ptr<Command> command = std::make_unique<Command>());
    static void onMouse(int eventType, int x, int y, int flags, void *userdata);
    void setCommand(std::unique_ptr<Command> command);
};

std::ostream& operator<<(std::ostream& stream, cv::Point2i point);
/*
template<typename stdVector>
std::ostream& operator<<(std::ostream& stream, stdVector vector)
{
    std::for_each(vector.begin(), vector.end(), [](auto& element){
        stream << element;
    });
    return stream;
}

template std::ostream& operator<< < std::vector<MousePoint> >(std::ostream&, MousePoint);*/
#endif
