#ifndef IMAGE_PARSER
#define IMAGE_PARSER
#include <opencv2/core/core.hpp>
#include <map>

class ImageParser{
public:
    static std::vector<std::vector<cv::Point2i>> imageToPoints(cv::Mat image);
    static cv::Mat pointsToImage(std::vector<std::vector<cv::Point2i>>);
};

#endif
