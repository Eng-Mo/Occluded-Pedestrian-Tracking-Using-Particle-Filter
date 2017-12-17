#include "calcfeatures.h"
#include<opencv2/opencv.hpp>



using namespace cv;
using namespace std;



CalcFeatures::CalcFeatures()
{


}


Mat CalcFeatures::oLBP(const Mat &i)
{
    Mat gray;
    cvtColor(i,gray,CV_BGR2GRAY);

    int t=gray.type();
    Mat dst;
    dst=Mat::zeros(Size(gray.cols,gray.rows),CV_8UC1);
    for (int r=1; r<gray.rows-1; r++)
    {
        for (int c=1; c<gray.cols-1; c++)
        {
            unsigned char center= gray.at<unsigned char>(r, c);
            unsigned char result=0;
            result |= (gray.at<unsigned char>(r-1,c-1) > center) << 7;
            result |= (gray.at<unsigned char>(r-1,c) > center) << 6;
            result |= (gray.at<unsigned char>(r-1,c+1) > center) << 5;
            result |= (gray.at<unsigned char>(r,c+1) > center) << 4;
            result |= (gray.at<unsigned char>(r+1,c+1) > center) << 3;
            result |= (gray.at<unsigned char>(r+1,c) > center) << 2;
            result |= (gray.at<unsigned char>(r+1,c-1) > center) << 1;
            result |= (gray.at<unsigned char>(r,c-1) > center) << 0;
            dst.at<unsigned char>(r-1,c-1) = result;

        }
    }


    return dst;

}

MatND CalcFeatures::getHistogram(const Mat &image, const Mat &lbp_image)
{

    Mat hsvImg;
//    cvtColor(image,hsvImg,CV_BGR2HSV);

    int bbins = 8, gbins = 8,rbins= 8, lbins=64;
    int histSize[] = {bbins,gbins, rbins, lbins};
    // hue varies from 0 to 179, see cvtColor
    float branges[] = { 0, 255 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float granges[] = { 0, 255 };
    float rranges[] = { 0, 255 };
    float lranges[] ={0, 255};
    const float* ranges[] = { branges,granges,rranges, lranges };
    MatND hist;
    Mat srcs[]={image, lbp_image};
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1, 2, 3};
    calcHist( srcs, sizeof(srcs), channels, Mat(), // do not use mask
             hist, 3, histSize, ranges,
             true, // the histogram is uniform
             false );
    normalize(hist,hist,1.0,NORM_L1);

    return hist;
}

void CalcFeatures::IIR_F(Mat& I1, Mat& I2)
{
//    imshow("I1",I1);
//    imshow("I2",I2);
//    waitKey(0);
    float gamma=.5;
    I2=gamma*I1+(1-gamma)*I2;
}
double CalcFeatures::getMSE(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    Scalar s = sum(s1);        // sum elements per channel
    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse  = sse / (double)(I1.channels() * I1.total());

        return mse;
    }
}

