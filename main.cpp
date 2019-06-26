#include "externs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#include <vector>
#include <unordered_map>
#include <algorithm>


#include "trackbars.hpp"
#include "image_parser.hpp"
#include "Logger.hpp"
#include "CmdLine.hpp"

#include <imgwarp_mls_similarity.h>
#include <imgwarp_mls_rigid.h>
#include <imgwarp_piecewiseaffine.h>

int undistort()
{
    cv::Mat image;
    image = cv::imread( inputFile, cv::IMREAD_COLOR);
    if(! image.data )                              // Check for invalid input
    {
        logger.logError("Could not open or find the image");
        return -1;
    }

    //resize(image, image, cv::Size(image.cols*0.5, image.rows*0.5), cv::INTER_NEAREST);


    cv::Mat output;
    cv::Mat cameraMatrix = cv::Mat::zeros(3,3, CV_32F);
    cameraMatrix.at<float>(0,0) = 1000; cameraMatrix.at<float>(1,1) = 1000; cameraMatrix.at<float>(2,2) = 1;
    cameraMatrix.at<float>(1,2) = (image.rows - 1) * 0.5; cameraMatrix.at<float>(0,2) = (image.cols-1) * 0.5;


    auto redrawFunction = [&image, &output, &cameraMatrix](const Trackbars& trackbars){
        RedrawParameters redrawParameters{trackbars, image, output, cameraMatrix};
        redraw(redrawParameters);
    };

//    auto trackbars = Trackbars(distCoefNames, window1, DoubleTrack::Range{-5.0, 5.0, 0.0}, 500, redrawFunction);
    logger.logDebug("Creating winwow:", window1);
    cv::imshow(window1, image);
    cv::waitKey(100);


    auto trans = std::make_unique<ImgWarp_MLS_Rigid>();
    trans->alpha = 1.0;
    trans->gridSize = 5.0;
    CmdLine cmdLine(window1, std::move(trans), image);

    return 0;
}

int imageToPoints()
{
    cv::Mat image;
    image = cv::imread("/mnt/hgfs/dsk/mro/3groups.png", cv::IMREAD_COLOR);   // Read the file
    auto output = ImageParser::imageToPoints(image);
    return 0;
}

int main( int argc, char** argv )
{
    return undistort();
    //return imageToPoints();
}
