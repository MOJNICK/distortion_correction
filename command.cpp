#include "command.hpp"
#include "Logger.hpp"

Command::Command(){}
Command::Command(std::shared_ptr<Morphing<cv::Point2i>> morphing) : morphing{morphing} {}
CommandAdd::CommandAdd(std::shared_ptr<Morphing<cv::Point2i>> morphing) : Command{morphing} {}
CommandDel::CommandDel(std::shared_ptr<Morphing<cv::Point2i>> morphing) : Command{morphing} {}

void Command::execute(MouseEvent event) const
{
    logger.logError("Dummy Command. Initialize me.");
}

void Command::setGeneralCallback(std::function<void(void)>&& callback)
{
    generalCallback = callback;
}

void CommandAdd::execute(MouseEvent event) const
{
    switch (event.type)
    {
        case cv::EVENT_LBUTTONDOWN :
        {
            morphing->addSrcPoint(event.point);
            break;
        }

        case cv::EVENT_RBUTTONDOWN :
        {
            morphing->addDstPoint(event.point);
            break;
        }
        default :
        {
            logger.logDebug("Are you sure what you are doing?");
            return;
        }
    }

    try
    {
        generalCallback();
    }
    catch(const std::bad_function_call& e)
    {
        logger.logError(__FILE__, __LINE__, __func__, e.what());
    }
}

void CommandDel::execute(MouseEvent event) const
{
    switch (event.type)
    {
        case cv::EVENT_LBUTTONDOWN :
        {
            morphing->deleteNearest(event.point);
            break;
        }
        default :
        {
            logger.logDebug("Are you sure what you are doing?");
            return;
        }
    }

    try
    {
        generalCallback();
    }
    catch(const std::bad_function_call& e)
    {
        logger.logError(__FILE__, __LINE__, __func__, e.what());
    }
}
