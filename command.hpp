#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "mouse.hpp"
#include "morphing.hpp"
#include <functional>

class MouseEvent;

class Command
{
protected:
    std::shared_ptr<Morphing<cv::Point2i>> morphing;
    std::function<void(void)> generalCallback;
public:
    Command();
    Command(std::shared_ptr<Morphing<cv::Point2i>> morphing);
    virtual void execute(MouseEvent event) const;
    void setGeneralCallback(std::function<void(void)>&& callback);
};

class CommandDel : public Command
{
public:
    CommandDel(std::shared_ptr<Morphing<cv::Point2i>> morphing);
    void execute(MouseEvent event) const override;
};

class CommandAdd : public Command
{
public:
    CommandAdd(std::shared_ptr<Morphing<cv::Point2i>> morphing);
    void execute(MouseEvent event) const override; //press L to add src, press R to add dst
};

class CommandMoveGroup : public Command
{
    void execute(MouseEvent event) const override;
};

#endif
