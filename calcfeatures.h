#ifndef CALCFEATURES_H
#define CALCFEATURES_H
#include<opencv2/opencv.hpp>


using namespace cv;


class CalcFeatures
{
public:
    CalcFeatures();
    static Mat oLBP(const Mat &i);
    static MatND getHistogram(const Mat &image, const Mat &lbp_image);
    static void IIR_F(Mat& I1, Mat& I2);
    static double getMSE(const Mat& I1, const Mat& I2);
};

#endif // CALCFEATURES_H
