
#include <QCoreApplication>

#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h>
#include"particlef.h"
#include"tracker.h"
#include"calcfeatures.h"
using namespace std;
using namespace cv;

//gsl_rng* rng;




float sdx=5;
float sdy=5;
double scale_sd = .005 ;
int partn=200;


Mat histEq(Mat &image){
    Mat ycrcb;

    cvtColor(image,ycrcb,CV_BGR2YCrCb);
    vector<Mat> ch;
    split(ycrcb,ch);
    equalizeHist(ch[0],ch[0]);
    merge(ch,ycrcb);
    cvtColor(ycrcb,image,CV_YCrCb2BGR);

    return image;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int frame_n=0;
    CalcFeatures feat;


    VideoCapture cap("../Downloads/ps6/input/pedestrians.avi");

    if (!cap.isOpened())
        {
        cout<<"no vide\n";
        return -1;
    }


    Mat edges;


    particle partArr[partn];
    particle new_partArr[partn];
//    namedWindow("edges",1);
    Mat frame;


    cap >> frame;
    int x=211;
    int y=36;
    int w=100;
    int h=293;
    Rect roi(x,y,w,h);

    MatND intial_hist;



    Mat original_patch;
    Mat lbp_original;
    particle best_particle;

    Mat Updated_patch;

    bool start_track=0;

    VideoWriter out ("not sampled out 2.avi",
                     cap.get(CV_CAP_PROP_FOURCC),
                     cap.get(CV_CAP_PROP_FPS),
                     Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
                     cap.get(CV_CAP_PROP_FRAME_HEIGHT)));; // intialise output file
    cout<<"size= "<< Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
                          cap.get(CV_CAP_PROP_FRAME_HEIGHT))<<endl;

    Tracker obj;
    Rect best_rect;
    obj.w_image= cap.get(CV_CAP_PROP_FRAME_WIDTH);
    obj.h_image= cap.get(CV_CAP_PROP_FRAME_HEIGHT);



    while(1)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        int t=frame.type();
//        imshow("frame", frame);
//        waitKey(0);

        cout<<"frame type= "<<t<<endl;

        if (frame.empty()==1)
            break;

        Scalar color(255, 100, 255 );

//        frame=histEq(frame);


        if (start_track==0)
        {
            original_patch= frame(roi).clone();

            lbp_original=feat.oLBP(original_patch);
            imshow("LBP", lbp_original);
            imshow("original", original_patch);
            waitKey(0);
            intial_hist=feat.getHistogram(original_patch, lbp_original);
            obj.initTracking(intial_hist,roi,sdx, sdy,scale_sd);

            start_track=1;
            continue;

        }
        Scalar s1= Scalar(255,0,0);
        Scalar s2= Scalar(0,255,0);
        Scalar s3= Scalar(0,0,255);


        Mat dImage=frame;
//        obj.drawParticles(dImage,s1);
        obj.updateTracking(frame,original_patch,intial_hist);
//        obj.drawParticles(dImage,s2);




        best_rect=obj.bestRect();

        rectangle(frame,best_rect,Scalar(0,0,255),2,5,0);
//        imshow("result", frame);
//        waitKey(0);




        frame_n+=1;
        cout<<frame_n<<"\n";
        cout<<"frame number ="<< frame_n<<endl;
//        if (frame_n==309){
//            int f=1;

//        }
        out.write(frame);

    }
    out.release();
    cap.release();


    cout<<"end of video"<<"\n";


    return 0;
}
