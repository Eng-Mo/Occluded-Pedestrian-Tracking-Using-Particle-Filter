#ifndef PARTICLEF_H
#define PARTICLEF_H
#include <iostream>
#include<opencv2/opencv.hpp>
#include <random>
#include<opencv2/imgproc.hpp>
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h>
#include"calcfeatures.h"




using namespace cv;
using namespace std;


struct particle
{
    float x;
    float y;
    double vx;
    double vy;
    float xp;
    float yp;
    float xo;
    float yo;
    double w;
    double scale;
    float scalep;
    int hieght;
    int width;
    Rect p_rect;
    MatND particle_hist;
    particle() {}
};

class ParticleF
{
private:
    double sd ;
    double scale_sd;
    gsl_rng* rng;



public:
    ParticleF();

    void init_particles(particle* part_reg,MatND& hist_patch,Rect r, int p_n, float displacmentx,float displacmenty, double scale_obj);
    void updateParticles(Mat& c_frame,const Mat& patch,MatND& ref_hist,particle* part_reg, int p_n);
    static bool comp(const particle& sP, const particle& eP);
    particle resampleParticles(particle* pArr,int partn);


};



#endif // PARTICLEF_H
