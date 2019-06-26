#include "mouse.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Logger.hpp"
#include <thread>

Mouse::Mouse(std::string onMouseWindowName, std::unique_ptr<Command> command) : command{std::move(command)}
{
    logger.logDebug("SetMouseCallback for window: ", onMouseWindowName);
    cv::setMouseCallback( onMouseWindowName, onMouse, this);
}


void Mouse::onMouse(int eventType, int x, int y, int flags, void *userdata)
{
    static std::mutex onMouseMtx;
    std::unique_lock<std::mutex> lock(onMouseMtx);
    switch (eventType)
    {
        case cv::EVENT_LBUTTONDOWN :
    	{
            Mouse* mousePtr = reinterpret_cast<Mouse*>(userdata);
            MouseEvent local{eventType, cv::Point2i{x,y}};
            logger.logInfo("EVENT_LBUTTONDOWN", local.point);
            mousePtr->command->execute(local);
            break;
        }
        case cv::EVENT_RBUTTONDOWN :
        {
            Mouse* mousePtr = reinterpret_cast<Mouse*>(userdata);
            MouseEvent local{eventType, cv::Point2i{x,y}};
            logger.logInfo("EVENT_RBUTTONDOWN", local.point);
            mousePtr->command->execute(local);
            break;
        }
    }
}

void Mouse::setCommand(std::unique_ptr<Command> command)
{
    this->command = std::move(command);
}


std::ostream& operator<<(std::ostream& stream, cv::Point2i point)
{
    return stream << "Point: x" << point.x << "y:" << point.y;
}
