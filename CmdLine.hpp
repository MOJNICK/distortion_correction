#ifndef CMD_LINE
#define CMD_LINE
#include "mouse.hpp"
#include "morphing.hpp"

class CmdLine
{
protected:
    Mouse mouse;
    std::shared_ptr<Morphing<cv::Point2i>> morphing;
    std::string windowName;
public:
    CmdLine(std::string windowName, std::unique_ptr<ImgWarp_MLS> algo, const cv::Mat srcImage);
    virtual void startKeyHandling();
protected:
    virtual void invalidOperation(std::string type);
    virtual void diagnose();
    virtual void handleAdd();
    virtual void handleDel();
    virtual void handleLine(Orientation targetOrientation);
    virtual void handleMorphing();
    virtual void handleMov() = delete;
};

class CmdLineManyTransforms : public CmdLine
{
    using CmdLine::CmdLine;
    void handleMorphing() override;
};
#endif
