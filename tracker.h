#ifndef TRACKER_H
#define TRACKER_H

#include"particlef.h"
#include<opencv2/opencv.hpp>
#include<opencv2/objdetect.hpp>

using namespace cv;

class Tracker
{
    const static int part_n=500;
    particle objectparticles [part_n];
    bool lostTrack=0;
    ParticleF pf_obj;


public:
    particle best_p, actual_p;
    int w_image;
    int h_image;
    Tracker();


    void initTracking(MatND hist_object, Rect rect, float disp_noisex,float disp_noisey, double scale_noise);
    void updateTracking(Mat& c_frame,const Mat& patch,MatND& ref_hist);
    void drawParticles(Mat& c_frame);
    Rect bestRect();
    void overlappedRect();
    void generateHeatMap();


};

#endif // TRACKER_H
