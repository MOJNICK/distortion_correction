#include "externs.hpp"
#include <opencv2/core/core.hpp>
#include <string>
#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <memory>
#include <opencv2/calib3d/calib3d.hpp>
#include "undistort.hpp"
#include "trackbars.hpp"

    void DoubleTrack::Range::transformRangeTo(const Range& destinyRange){
        value = value - min;// normalize min to 0 next transform to absolute value
        value = value  * (destinyRange.max - destinyRange.min)/(max-min) + destinyRange.min;//destination absolute value
        min = destinyRange.min; max = destinyRange.max;
    }

    DoubleTrack::Range DoubleTrack::Range::transformRangeTo(const Range& src, const Range& dst){
        Range srcCpy = src;
        srcCpy.transformRangeTo(dst);
        return srcCpy;
    }

    void DoubleTrack::trackbarCallback(int trackbarValue, void* trackbarUserData){
        DoubleTrack* pObject = static_cast<DoubleTrack*>(trackbarUserData);
        double doubleValue = Range::transformRangeTo(
            Range{0.0, (double)(pObject->trackbarSteps),
            (double)(trackbarValue)}, pObject->trackbarRange)
            .value;

        pObject->trackbars->callback( doubleValue, pObject->user_data);
    }

    DoubleTrack::DoubleTrack(ConstructorParameters parameters) :
     trackbarSteps{parameters.trackbarSteps},
     trackbarRange(parameters.trackbarRange),
     user_data{parameters.user_data},
     trackbars{parameters.trackbars}
    {
        initValue = std::lround(Range::transformRangeTo(trackbarRange, Range{0, (double)(trackbarSteps)}).value);
        cv::createTrackbar(parameters.trackbarName, parameters.windowName, &initValue, trackbarSteps, trackbarCallback, this);
    }



    void Trackbars::callback(double trackbarPosition, void* user_data){
            auto value = reinterpret_cast<double*>(user_data);
            *value = trackbarPosition;
            notifier(*this);
    }

    void Trackbars::createTrackbars()
    {
        for(int i = 0; i < trackbarNames.size(); ++i){
            DoubleTrack::ConstructorParameters parameters;
                parameters.trackbarName = trackbarNames[i];
                parameters.windowName = windowName;
                parameters.trackbarRange = trackbarRange;
                parameters.trackbarSteps = trackbarSteps;
                parameters.user_data = &values.at<double>(0, i);
                parameters.trackbars = this;

                wrappedCallbacks.push_back(std::make_unique<DoubleTrack>(parameters));
        }
    }

    Trackbars::Trackbars(const std::vector<std::string>& trackbarNames,
     const std::string& windowName,
     DoubleTrack::Range trackbarRange,
     int trackbarSteps,
     std::function<void(const Trackbars&)> notifier)
    :
     trackbarNames{ trackbarNames },
     values{ cv::Mat::zeros(1, trackbarNames.size(), CV_64F)},
     windowName{windowName},
     trackbarSteps{trackbarSteps},
     trackbarRange(trackbarRange),
     notifier{notifier}
    {
        cv::imshow( window1, cv::Mat(1,1,1) );
        createTrackbars();
    }
    const cv::Mat& Trackbars::getValues() const {return values;}


void redraw(const RedrawParameters& parameters)
{
    cv::Mat input(parameters.input);
    cv::Mat cameraMatrix(parameters.cameraMatrix);
    cv::Mat output(parameters.output);
    cv::Mat distCoeffs = parameters.trackbars.getValues();

    double alpha = 1.0;
    cv::Mat newCameraMatrix =
        cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs,
                                      cv::Size(input.cols, input.rows),
                                      alpha, cv::Size(), nullptr, true );
    custom_cv::undistort(input, output, cameraMatrix, distCoeffs, newCameraMatrix);

    cv::imshow( window1, output );
}
