#ifndef TRACKBARS_HPP
#define TRACKBARS_HPP

#include <opencv2/core/core.hpp>
#include <string>
#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <memory>
#include <opencv2/calib3d/calib3d.hpp>
#include "undistort.hpp"

class Trackbars;

class DoubleTrack{
public:
    struct Range{
        double min;
        double max;
        double value;

        void transformRangeTo(const Range& destinyRange);
        static Range transformRangeTo(const Range& src, const Range& dst);
    };

    struct ConstructorParameters{
        std::string trackbarName;
        std::string windowName;
        void* user_data;
        Range trackbarRange;
        int trackbarSteps;
        Trackbars* trackbars;
    };

    static void trackbarCallback(int trackbarValue, void* trackbarUserData);

    DoubleTrack(ConstructorParameters parameters);

private:
    int initValue;
    int trackbarSteps;
    Range trackbarRange;

    void(*user_callback)(Trackbars*, double, void*);
    void* user_data;
    Trackbars* trackbars;
};


class Trackbars
{
    const std::vector<std::string> trackbarNames;
    cv::Mat values;
    std::vector<std::unique_ptr<DoubleTrack>> wrappedCallbacks;

    const std::string& windowName;
    //const
    DoubleTrack::Range trackbarRange;
    //const
    int trackbarSteps;
    std::function<void(const Trackbars&)> notifier;


    void createTrackbars();
public:
    void callback(double trackbarPosition, void* user_data);
    Trackbars(const std::vector<std::string>& trackbarNames,
     const std::string& windowName,
     DoubleTrack::Range trackbarRange,
     int trackbarSteps,
     std::function<void(const Trackbars&)> notifier
     );
    const cv::Mat& getValues() const;
};

struct RedrawParameters{
    const Trackbars& trackbars;
    cv::Mat input;
    cv::Mat output;
    cv::Mat cameraMatrix;
};

void redraw(const RedrawParameters& parameters);

#endif
