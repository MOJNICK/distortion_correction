#include "CmdLine.hpp"
#include "Logger.hpp"
#include <thread>
#include <memory>
#include <pthread.h>
#include <imgwarp_mls_similarity.h>
#include <imgwarp_mls_rigid.h>
#include <imgwarp_piecewiseaffine.h>

CmdLine::CmdLine(std::string windowName, std::unique_ptr<ImgWarp_MLS> algo, const cv::Mat srcImage) : mouse{windowName}, windowName{windowName}
{
    std::make_shared<cv::Point2i>(10,20);
    morphing = std::make_shared<Morphing<cv::Point2i>>(std::move(algo), srcImage);
    startKeyHandling();
}

void CmdLine::startKeyHandling()
{
    logger.logDebug("CmdLine::startKeyHandling");

    auto th = std::thread([](){
        while(true){
            cv::waitKey();
        }
    });
    pthread_setname_np(th.native_handle(), "Key wait thread");
    th.detach();

    std::string command;
    while(true)
    {
        logger.logInfo("You are in main loop.");
        std::cin >> command;
        if(command == "exit")
        {
            cv::destroyAllWindows();
            return;
        } else if("add"== command) //type add to enter points adding mode
        {
            handleAdd();
        } else if("del" == command) //type for deletion
        {
            handleDel();
        } else if ("line" == command)//automatically create straight line 
        {
            handleLine(Orientation::doNotTouch);
        } else if ("horizontal" == command) //horizontal line
        {
            handleLine(Orientation::horizontal);
        } else if ("vertical" == command)
        {
            handleLine(Orientation::vertical); //vertical line
        } else if ("redraw" == command)
        {
            //assert vector same size
            //doMorphing
            //replace morphing.image with morphing.morphed - so prevvious selected points will stay in place - no morphing applied
            //
            //transforms.src=transforms.dst
        } else if ("moveLine" == command)
        {
        } else if("morphing" == command)
        {
            handleMorphing();
        } else if ("original" == command)
        {
            cv::imshow( "original", morphing->original);
        } else if ("destroyall" == command)
        {
            cv::destroyAllWindows();
        } else if ("diagnose" == command)
        {
            diagnose();
        } else if ("algo1" == command)
        {
            auto trans = std::make_unique<ImgWarp_MLS_Rigid>();
            trans->alpha = 1.0;
            trans->gridSize = 5.0;
            morphing->setMorphingAlgorithm(std::move(trans));
        } else if ("algo2" == command)
        {
            auto trans = std::make_unique<ImgWarp_MLS_Similarity>();
            trans->alpha = 1.0;
            trans->gridSize = 5.0;
            morphing->setMorphingAlgorithm(std::move(trans));
        } else if ("algo3" == command)
        {
            auto trans = std::make_unique<ImgWarp_PieceWiseAffine>();
            trans->alpha = 1.0;
            trans->gridSize = 5.0;
            morphing->setMorphingAlgorithm(std::move(trans));
        }else
        {
            invalidOperation("2");
        }
    }
}

void CmdLine::invalidOperation(std::string type)
{
    logger.logError("invalid operation");
}

void CmdLine::diagnose() //use to draw result screen
{
    logger.logDebug(__FILE__, ":", __LINE__, __func__);
    cv::Mat withArrows = morphing->original.clone();
    auto& transforms = morphing->transforms;

    std::unique_lock<std::mutex>(transformsLock);
    std::for_each(std::begin(transforms), std::end(transforms), [this, &withArrows](TransformPoints<cv::Point2i>& transform)
    {
        auto srcSize = transform.srcSize();
        auto dstSize = transform.dstSize();

        logger.logDebug("srcSize=", srcSize, " dstSize=", dstSize);

        if( srcSize != dstSize )
        {
            logger.logError("Sizes not match.", "srcSize:", srcSize, " dstSize", dstSize);
        }

        static int red;
        red += 50;
        red %= 255;
        for(int i = 0; i < std::min(srcSize, dstSize); ++i)
        {
            cv::arrowedLine( withArrows, transform.getSrcPoint(i), transform.getDstPoint(i), cv::Scalar{100,255, (double)red,100}, 2);//, int line_type=8, int shift=0, double tipLength=0.1)
        }
    });
    logger.logDebug(__FILE__, ":", __LINE__, __func__, "Imshow with arrows.");
    cv::imshow(windowName, withArrows);
}

void CmdLine::handleAdd()
{
    morphing->createTransform();
    auto command = std::make_unique<CommandAdd>(morphing);
    command->setGeneralCallback([this](){diagnose();});
    mouse.setCommand(std::move(command));
}


void CmdLine::handleDel()
{
    auto command = std::make_unique<CommandDel>(morphing);
    command->setGeneralCallback([this](){diagnose();});
    mouse.setCommand(std::move(command));
}

void CmdLine::handleLine(Orientation targetOrientation)
{
    morphing->createTransform();
    auto command = std::make_unique<CommandAdd>(morphing);
    command->setGeneralCallback([this, targetOrientation]()
        {
            morphing->transformSrcAsDstLine(targetOrientation);
            diagnose();
            logger.logDebug(__FILE__, ":", __LINE__, __func__, "line generalCallback");
        });
    mouse.setCommand(std::move(command));
}

/*
void CmdLine::handleMorphing()
{
    morphing->doMorphing();
    cv::imshow( "morphed", morphing->morphed);
    logger.logDebug("imshow morphed");
}
*/

void CmdLine/*ManyTransforms*/::handleMorphing()
{
    morphing->doMorphing();
    cv::imshow( "morphed", morphing->morphed);
    logger.logDebug("imshow morphed manytrans");
    morphing->original = morphing->morphed.clone();
    morphing->transforms.clear();
    cv::imshow(windowName, morphing->original);
    logger.logDebug("imshow original on windowName:", windowName);
}
