#include "morphing.hpp"
#include <algorithm>
#include "mouse.hpp"
#include "Logger.hpp"
#include <limits>

template<class PointType>
Morphing<PointType>::Morphing(std::unique_ptr<ImgWarp_MLS> algo, const cv::Mat srcImage) : warp{std::move(algo)}, original{srcImage}{}

template<class PointType>
void Morphing<PointType>::doMorphing()
{
    logger.logDebug(__FILE__, ":", __LINE__, __func__);
    std::vector<cv::Point2i> vSrcPoints;
    std::vector<cv::Point2i> vDstPoints;

    std::unique_lock<std::mutex>(transformsLock);
    std::for_each(std::begin(transforms), std::end(transforms), [&vSrcPoints, &vDstPoints](auto& transform)
    {
        auto& trSrc = transform.getSrcPoints();
        auto& trDst = transform.getDstPoints();
        auto trSrcBegin = std::begin(trSrc);
        auto trDstBegin = std::begin(trDst);

        auto transformRealSize = std::min(std::distance(trSrcBegin, std::end(trSrc)), std::distance(std::begin(trDst), std::end(trDst)));
        vSrcPoints.insert(std::end(vSrcPoints), trSrcBegin, trSrcBegin + transformRealSize);
        vDstPoints.insert(std::end(vDstPoints), trDstBegin, trDstBegin + transformRealSize);
    });

    morphed = warp->setAllAndGenerate(original, vSrcPoints, vDstPoints,
    original.cols, original.rows, 1);
}

template<class PointType>
void Morphing<PointType>::addSrcPoint(PointType point)
{
    std::unique_lock<std::mutex>(transformsLock);
    logger.logDebug(__FILE__, ":", __LINE__, __func__, "addSrcPoint", point);
    transforms.back().srcPushBack(point);
}

/*template<class PointType>
void Morphing<PointType>::addSrcVector(std::vector<PointType> vPoint)
{
    std::unique_lock<std::mutex>(transformsLock);
    auto& vSrc = transforms.back().srcPoints;
    vSrc.insert(std::end(vSrc), std::begin(vPoint), std::end(vPoint));
}*/

template<class PointType>
void Morphing<PointType>::addDstPoint(PointType point)
{
    std::unique_lock<std::mutex>(transformsLock);
    logger.logDebug(__FILE__, ":", __LINE__, __func__, "addDstPoint", point);
    transforms.back().dstPushBack(point);
}

/*
template<class PointType>
void Morphing<PointType>::addDstVector(std::vector<PointType> vPoint)
{
    std::unique_lock<std::mutex>(transformsLock);
    auto& vDst = transforms.back().dstPoints;
    vDst.insert(std::end(vDst), std::begin(vPoint), std::end(vPoint));
}
*/
template<class PointType>
void Morphing<PointType>::createTransform()
{
    logger.logDebug(__FILE__, ":", __LINE__, __func__);
    std::unique_lock<std::mutex>(transformsLock);

    deleteZeroSizedGroups();
    transforms.push_back(TransformPoints<PointType>());
}

template<class PointType>
void Morphing<PointType>::transformSrcAsDstLine(Orientation targetOrientation)
{
    std::unique_lock<std::mutex>(transformsLock);
    auto& transform = transforms.back();
    transform.dstClear();
    if(transform.srcSize() >= 2)
    {
        logger.logDebug("Fitting line");
        cv::Mat dstMat;
        cv::Mat srcMat = transform.getSrcMat();


        cv::fitLine(srcMat, dstMat, CV_DIST_L2 , 0.0, 0.01, 0.01);
        if (dstMat.isContinuous() && srcMat.type() == CV_32SC2 && dstMat.type() == CV_32F)
        {
            float vx = dstMat.at<cv::Vec<float, 2>>(0)[0];
            float vy = dstMat.at<cv::Vec<float, 2>>(0)[1];
            float x = dstMat.at<cv::Vec<float, 2>>(1)[0];
            float y = dstMat.at<cv::Vec<float, 2>>(1)[1];
            cv::Point_<double> Lpoint(x, y);
            cv::Point_<double> Fl(vx, vy);
            switch ( targetOrientation )
            {
                case Orientation::doNotTouch :
                {
                    break;
                }
                case Orientation::horizontal :
                {
                    Fl.x = 1.0; Fl.y = 0.0;
                    break;
                }
                case Orientation::vertical :
                {
                    Fl.x = 0.0; Fl.y = 1.0;
                    break;
                }
                default : break;
            }
            logger.logDebug("lpoint: ", Lpoint);

            for(int i = 0; i < srcMat.total(); ++i)
            {
                cv::Point_<double> Cpoint(srcMat.at<cv::Vec<int, 2>>(i)[0], srcMat.at<cv::Vec<int, 2>>(i)[1]);
                cv::Point_<double> Fc = Cpoint - Lpoint;
                cv::Point_<double> Bpoint = Lpoint + ((Fl.ddot(Fc)) * Fl);
                transform.dstPushBack(Bpoint);
            }
        }
        else
        {
            logger.logError("Data not continuous or wrong type. ch depth elemSize type");
            logger.logError(srcMat.channels(), srcMat.depth(), srcMat.elemSize(), srcMat.type());
            logger.logError(dstMat.channels(), dstMat.depth(), dstMat.elemSize(), dstMat.type());
            std::abort();
        }
    }
}

template<class PointType>
void Morphing<PointType>::deleteNearest(const PointType point)
{
    logger.logDebug(__FILE__, ":", __LINE__, __func__);
    std::unique_lock<std::mutex> lock(transformsLock);
    auto nearest = getNearest(point);
    auto* transform = std::get<0>(nearest);

    int srcSize = transform->srcSize();
    int dstSize = transform->dstSize();
    if(0 < srcSize && 0 < dstSize)
    {
        if(srcSize == dstSize)
        {
            logger.logInfo("Delete both.");
            transform->deleteSrcPoint(std::get<2>(nearest));
            transform->deleteDstPoint(std::get<2>(nearest));
        }
        else if (srcSize > dstSize)
        {
            logger.logInfo("Delete src end.");
            transform->srcPopBack();
        }
        else
        {
            logger.logInfo("Delete dst end.");
            transform->dstPopBack();
        }
    }
    else
    {
        logger.logInfo("Nothing to delete.");
    }
    deleteZeroSizedGroups();
}

template<class PointType>
auto Morphing<PointType>::getNearest(const PointType point)
{
    int nearestPointIdx;
    auto* nearestGroup = &transforms[0];
    auto* nearestSubGroup = &transforms[0].getSrcPoints();
    int lastMinDistance = std::numeric_limits<int>::max();

    std::for_each(std::begin(transforms), std::end(transforms), [point, &lastMinDistance, &nearestSubGroup, &nearestGroup, &nearestPointIdx](auto& transform)
    {
        const std::vector<PointType>& vSrcPoints = transform.getSrcPoints();
        const std::vector<PointType>& vDstPoints = transform.getDstPoints();
        int srcSize = vSrcPoints.size();
        int dstSize = vDstPoints.size();

        auto distance = [&point](PointType vecPoint)
        {
            return (point.x - vecPoint.x) * (point.x - vecPoint.x) + (point.y - vecPoint.y) * (point.y - vecPoint.y);
        };

        int minSrc = std::numeric_limits<int>::max();
        int srcIdx;
        for(int i = 0; i < vSrcPoints.size(); ++i){
            int dist = distance(vSrcPoints[i]);
            if(dist < minSrc){
                srcIdx = i;
                minSrc = dist;
            }
        }

        int minDst = std::numeric_limits<int>::max();
        int dstIdx;
        for(int i = 0; i < vDstPoints.size(); ++i){
            int dist = distance(vDstPoints[i]);
            if(dist < minDst){
                dstIdx = i;
                minDst = dist;
            }
        }

        if(minSrc < minDst)
        {
            int minimumDistance = minSrc;
            if(minimumDistance < lastMinDistance)
            {
                logger.logDebug(__FILE__, ":", __LINE__, __func__, "src:", transform.srcSize());
                lastMinDistance = minimumDistance;
                nearestGroup = &transform;
                nearestSubGroup = &vSrcPoints;
                nearestPointIdx = srcIdx;
            }
        }
        else
        {
            int minimumDistance = minDst;
            if(minimumDistance < lastMinDistance)
            {
                logger.logDebug(__FILE__, ":", __LINE__, __func__, "dst", transform.dstSize());
                lastMinDistance = minimumDistance;
                nearestGroup = &transform;
                nearestSubGroup = &vDstPoints;
                nearestPointIdx = dstIdx;
            }
        }
    });
    return std::make_tuple(nearestGroup,  nearestSubGroup, nearestPointIdx);
}

template<class PointType>
void Morphing<PointType>::deleteZeroSizedGroups()
{
    transforms.erase(std::remove_if(transforms.begin(), transforms.end(), [](auto& transform)
    {
        auto p = transform.srcSize() == 0 && transform.dstSize() == 0;
        return p;
    }), transforms.end());
}

template class Morphing<cv::Point_<int>>;
