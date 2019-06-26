#include "image_parser.hpp"


namespace std {
template<>
struct less<cv::Point3_<uint8_t>>
{
   bool operator()(const cv::Point3_<uint8_t>& k1, const cv::Point3_<uint8_t>& k2) const
   {
      uint32_t wk1 = k1.x; wk1<<=8;
      wk1 += k1.y; wk1 <<= 8; wk1 += k1.z;
      uint32_t wk2 = k2.x; wk2<<=8;
      wk2 += k2.y; wk2 <<= 8; wk2 += k2.z;
      return wk1 < wk2;
   }
};
}

std::vector<std::vector<cv::Point2i>> ImageParser::imageToPoints(cv::Mat image){
    std::map<cv::Point3_<uint8_t>, std::vector<cv::Point2i>> pointGroups;//choosen because of easier operator< in map
    using Pixel = cv::Point3_<uint8_t>;

    for(int row = 0; row<image.rows; ++row)
        for(int col =0; col<image.cols; ++col){
            Pixel p = image.at<Pixel>(row, col);
            if(0 != p.x+p.y+p.z){
                pointGroups[p].push_back(cv::Point2i(row, col));
            }
        }

    std::vector<std::vector<cv::Point2i>> dest;

    std::transform(pointGroups.begin(), pointGroups.end(), std::back_inserter(dest),
        [](std::pair<cv::Point3_<uint8_t>, std::vector<cv::Point2i>>&& group)->std::vector<cv::Point2i>&&{
            return std::move(group.second);
        }
    );

    return dest;
}

