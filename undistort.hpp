#include <opencv2/calib3d.hpp> //cv namespace and INTER_LINEAR
#include <opencv2/imgproc.hpp>
#ifndef CUSTOM_UNDISTORT
#define CUSTOM_UNDISTORT
namespace custom_cv{
    using namespace cv;
void undistort( InputArray _src, OutputArray _dst, InputArray _cameraMatrix,
                    InputArray _distCoeffs, InputArray _newCameraMatrix=noArray(), int interpolation = INTER_LINEAR );
}
#endif
