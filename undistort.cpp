#include "undistort.hpp"
namespace custom_cv{
    using namespace cv;
    void undistort( InputArray _src, OutputArray _dst, InputArray _cameraMatrix,
                        InputArray _distCoeffs, InputArray _newCameraMatrix, int interpolation )
    {
        Mat src = _src.getMat(), cameraMatrix = _cameraMatrix.getMat();
        Mat distCoeffs = _distCoeffs.getMat(), newCameraMatrix = _newCameraMatrix.getMat();

        _dst.create( src.size(), src.type() );
        Mat dst = _dst.getMat();

        CV_Assert( dst.data != src.data );

        int stripe_size0 = std::min(std::max(1, (1 << 12) / std::max(src.cols, 1)), src.rows);
        Mat map1(stripe_size0, src.cols, CV_16SC2), map2(stripe_size0, src.cols, CV_16UC1);

        Mat_<double> A, Ar, I = Mat_<double>::eye(3,3);

        cameraMatrix.convertTo(A, CV_64F);
        if( !distCoeffs.empty() )
            distCoeffs = Mat_<double>(distCoeffs);
        else
        {
            distCoeffs.create(5, 1, CV_64F);
            distCoeffs = 0.;
        }

        if( !newCameraMatrix.empty() )
            newCameraMatrix.convertTo(Ar, CV_64F);
        else
            A.copyTo(Ar);

        double v0 = Ar(1, 2);
        for( int y = 0; y < src.rows; y += stripe_size0 )
        {
            int stripe_size = std::min( stripe_size0, src.rows - y );
            Ar(1, 2) = v0 - y;
            Mat map1_part = map1.rowRange(0, stripe_size),
                map2_part = map2.rowRange(0, stripe_size),
                dst_part = dst.rowRange(y, y + stripe_size);

            initUndistortRectifyMap( A, distCoeffs, I, Ar, Size(src.cols, stripe_size),
                                     map1_part.type(), map1_part, map2_part );
            remap( src, dst_part, map1_part, map2_part, interpolation, BORDER_CONSTANT );
        }
    }
}
