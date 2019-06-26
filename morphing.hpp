#ifndef MORPHING_HPP
#define MORPHING_HPP
#include "imgwarp_mls.h"
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/types_c.h>
#include <mutex>

template<class PointT>
struct TransformPoints
{
    using PointType = PointT;
    TransformPoints() : srcPoints(), dstPoints() {}
    void deleteSrcPoint(int index){auto vi = std::begin(srcPoints); std::advance(vi, index); srcPoints.erase(vi);}
    void deleteDstPoint(int index){auto vi = std::begin(dstPoints); std::advance(vi, index); dstPoints.erase(vi);}
    auto srcPushBack(PointType point){return srcPoints.push_back(point);}
    auto srcPopBack(){return srcPoints.pop_back();}
    auto dstPushBack(PointType point){return dstPoints.push_back(point);}
    auto dstPopBack(){return dstPoints.pop_back();}
    PointType& getSrcPoint(int index){return srcPoints[index];}
    PointType& getDstPoint(int index){return dstPoints[index];}
    auto srcSize(){return srcPoints.size();}
    auto dstSize(){return dstPoints.size();}
    auto dstClear(){return dstPoints.clear();}
    auto getSrcMat(){return cv::Mat(srcPoints);}
    const auto& getSrcPoints() const {return srcPoints;}
    const auto& getDstPoints() const {return dstPoints;}

private:
    std::vector<PointType> srcPoints;
    std::vector<PointType> dstPoints;
};

enum class Orientation
{
    doNotTouch,
    horizontal,
    vertical
};

template<class PointType>
class Morphing
{
public:
    std::unique_ptr<ImgWarp_MLS> warp;
    std::mutex transformsLock;
public:
    std::vector<TransformPoints<PointType>> transforms = std::vector<TransformPoints<PointType>>(1);
    cv::Mat original;
    cv::Mat morphed;

    Morphing(std::unique_ptr<ImgWarp_MLS> algo, const cv::Mat srcImage);

    void setMorphingAlgorithm(std::unique_ptr<ImgWarp_MLS> algo){warp = std::move(algo);}

    void doMorphing();
    void addSrcPoint(PointType point);
    void addSrcVector(std::vector<PointType> point);
    void addDstPoint(PointType point);
    void addDstVector(std::vector<PointType> point);
    void createTransform();
    void transformSrcAsDstLine(Orientation targetOrientation);
    void deleteNearest(PointType point);

private:
    auto getNearest(const PointType point);
    void deleteZeroSizedGroups();
};
#endif
