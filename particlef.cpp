#include "particlef.h"
#include"calcfeatures.h"
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h>
#include <fstream>
#include <random>



using namespace cv;
using namespace std;

ParticleF::ParticleF()
{


}


void ParticleF::init_particles(particle* part_reg,MatND& hist_patch,Rect r, int p_n,float displacmentx,float displacmenty, double scale_obj)
{
    sd= displacmentx;
    sd= displacmenty;
    scale_sd= scale_obj;
    int cx=r.x+(r.width/2);
    int cy= r.y+(r.height/2);
//    cout<<"cx="<<cx<<"\n";
//    cout<<"cy="<<cy<<"\n";
    particle part;
    for (int i=0;i<p_n;i++)
    {

        part.x= part.xp= part.xo= cx;
        part.y= part.yp= part.yo= cy;
        part.vx=0;
        part.vy=0;
        part.width=r.width;
        part.hieght=r.height;
        part.w=0;
        part.scale= part.scalep= 1.0;
        part.p_rect=r;
        part.particle_hist= hist_patch;
        part_reg[i]=part;


    }
        cout<<"cx="<<cx<<"\n";
        cout<<"cy="<<cy<<"\n";

}

void ParticleF::updateParticles(Mat& c_frame,const Mat& patch,MatND& ref_hist,particle* part_reg, int p_n)
{
    int nx;
    int ny;
    float nscale;
    float totalW=0;
    RNG rand;
    int frame_w=c_frame.cols;
    int frame_h=c_frame.rows;
//    gsl_rng* rng;
    gsl_rng_env_setup();
    rng = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(rng, time(NULL));


     ofstream output,output2;
     output.open("output_result.txt");
     output2.open("output_result2.txt");
     if (!output.is_open()||!output2.is_open())
             {
           cout << "Unable to open file for writing";
           return;

     }
     else{
//         std::default_random_engine de(time(0)); //seed
//         std::normal_distribution<double> nd(0, sdx); //mean followed by stdiv
////         int rarrary [101]; // [0, 100]
//         for(int i = 0; i < p_n; ++i){
//             output<<nd(de)<<endl;

//         }


//    for (int i=0;i<p_n;i++){
//        double d= gsl_ran_gaussian(rng,sd);
//        cout<<"random ="<<d<<endl;
//        output<<d<<endl;
//    }
//    output.close();
     }




    for (int p=0; p<p_n; p++)
    {
//        int nx=2*(part_reg[p].x-part_reg[p].xp)+(part_reg[p].xp-part_reg[p].xo)+gsl_ran_gaussian(rng,sd)+part_reg[p].xo;
//        int ny=2*(part_reg[p].y-part_reg[p].yp)+(part_reg[p].yp-part_reg[p].yo)+gsl_ran_gaussian(rng,sd)+part_reg[p].yo;
//        cout<<"random= "<<gsl_ran_gaussian(rng,sd)<<endl;

//        nx=(part_reg[p].x+part_reg[p].xp+part_reg[p].xo)/3+part_reg[p].vx;
//        ny= (part_reg[p].y+part_reg[p].yp+part_reg[p].yo)/3+part_reg[p].vy;
        double d= gsl_ran_gaussian(rng,sd);
        double d2= gsl_ran_gaussian(rng, scale_sd);
        output<<d<<endl;
        output2<<d2<<endl;
        part_reg[p].vx=part_reg[p].vx+d;
        part_reg[p].vy=part_reg[p].vy+d;
        nx=part_reg[p].x+part_reg[p].vx;
        ny= part_reg[p].y+part_reg[p].vy;

//        int ny=part_reg[p].y+r.gaussian(sd);
//        cout<<"nx part"<<nx<<"\n";
//        cout<<"ny part"<<ny<<"\n";


        nx=max(5,min(nx,frame_w-5));

        ny=max(5,min(ny,frame_h-5));
        cout<<"nx part"<<nx<<"\n";
        cout<<"ny part"<<ny<<"\n";

        nscale=d2+part_reg[p].scale;
        nscale=abs(nscale);
        nscale=min(max(double(nscale),.00006),(double)3);
        cout<<"new scale part"<<nscale<<"\n";
//        part_reg[p].scale =nscale;
        cout<<"new scale ="<<part_reg[p].scale<<endl;

        part_reg[p].scale = ( (nx + (nscale*part_reg[p].width)/2) < (frame_w-1) && (nx - (nscale*part_reg[p].width)/2) > 0 )?nscale:0;
        part_reg[p].scale = ( (ny + (nscale*part_reg[p].hieght)/2) < (frame_h-1) && (ny - (nscale*part_reg[p].hieght)/2) > 0 )?nscale:0;


        if (part_reg[p].scale==0)
        {
//            cout<<"new scale2 part"<<part_reg[p].scale<<"\n";
            part_reg[p].scale=1.0;


            totalW+=part_reg[p].w;
//            continue;
        }
//        part_reg[p].scale=1.0;
//        part_reg[p].xo=part_reg[p].xp;
//        part_reg[p].yo=part_reg[p].yp;
//        part_reg[p].xp=part_reg[p].x;
//        part_reg[p].yp=part_reg[p].y;
        part_reg[p].xo=part_reg[p].xp;
        part_reg[p].xp=part_reg[p].x;

        part_reg[p].x=nx;
        part_reg[p].yo=part_reg[p].yp;
        part_reg[p].yp=part_reg[p].y;
        part_reg[p].y=ny;

        cout<<"part w= "<<part_reg[p].width<<endl;
        cout<<"part h= "<<part_reg[p].hieght<<endl;

        part_reg[p].width=part_reg[p].width*part_reg[p].scale;
        part_reg[p].hieght=part_reg[p].hieght*part_reg[p].scale;

        cout<<"part new w= "<<part_reg[p].width<<endl;
        cout<<"part new h= "<<part_reg[p].hieght<<endl;

//        cout<<"part w= "<<part_reg[p].width<<endl;
//        cout<<"part h= "<<part_reg[p].hieght<<endl;

        part_reg[p].p_rect.x= part_reg[p].x - (part_reg[p].width/2);
        cout<<"part new rect X= "<<part_reg[p].p_rect.x<<endl;
        part_reg[p].p_rect.y= part_reg[p].y - (part_reg[p].hieght/2);
        cout<<"part new rect Y= "<<part_reg[p].p_rect.y<<endl;

        if ((part_reg[p].p_rect.x+ part_reg[p].p_rect.width)>frame_w-1){
            part_reg[p].width=frame_w-part_reg[p].p_rect.x-1;

        }
        if ((part_reg[p].p_rect.y+ part_reg[p].p_rect.height)>frame_h-1){
            part_reg[p].hieght=frame_h-part_reg[p].p_rect.y-1;

        }
        part_reg[p].p_rect.x=max(1,min(part_reg[p].p_rect.x,frame_w-1));

        part_reg[p].p_rect.y=max(1,min(part_reg[p].p_rect.y,frame_h-1));
        part_reg[p].p_rect.width= max(2,min(part_reg[p].width,frame_w-1));
        cout<<"part new rect Width= "<<part_reg[p].p_rect.width<<endl;
        part_reg[p].p_rect.height= max(2,min(part_reg[p].hieght,frame_h-1));;
        cout<<"part new rect Height= "<<part_reg[p].p_rect.height<<endl;




        Mat particle_patch=c_frame(part_reg[p].p_rect).clone();
        Mat resized_patch;
        resize(particle_patch,resized_patch,Size(patch.cols,patch.rows),0,0,CV_INTER_LINEAR);
        Mat Particle_lbp;
        Particle_lbp= CalcFeatures::oLBP(particle_patch);
        part_reg[p].particle_hist= CalcFeatures::getHistogram(particle_patch, Particle_lbp);



//        double mse= getMSE(particle_patch,patch);
//        part_reg[p].w=mse;
        part_reg[p].w= compareHist(ref_hist,part_reg[p].particle_hist,CV_COMP_CHISQR);
//        cout<<"wieght ="<<p<<"= "<<part_reg[p].w<<"\n";
//        part_reg[p].w=(float) 120/exp( part_reg[p].w);
        Scalar color(255, 100, 255 );
        totalW+=part_reg[p].w;
        part_reg[p].scale=1.0;


//        rectangle(c_frame,part_reg[p].p_rect,color,2,5,0);
    }
//    imshow("test", c_frame);
//    waitKey(0);

    for(int p=0; p < p_n; p++) {
        cout<<"wieght 1= "<<part_reg[p].w<<"\n";
//            part_reg[p].w = part_reg[p].w/totalW;
             cout<<"wieght 2= "<<part_reg[p].w<<"\n";

        }
     output.close();


}



bool ParticleF::comp(const particle& sP, const particle& eP)
{
  return sP.w < eP.w;
}

particle ParticleF::resampleParticles(particle* pArr,int partn) {

    particle npArr[partn];
    int m=0;
    int np=partn;
    particle part_b;
    sort(pArr,pArr+partn,this->comp);
    for(int p=0; p < partn; p++) {
        cout<<"wieght1= "<<pArr[p].w<<"\n";

        }
    part_b=pArr[0];

    for(int p=0; p < partn; p++) {
        np=partn*.3;
        for(int j=0;j<np; j++)
        {
        npArr[m] = pArr[p];
        m++;
//        npArr[m++] = pArr[p];
        if (m >= partn)
        {
             break;
        }
        }
        if (m == partn)
        {
             break;
        }


   }
    while (m<partn){
        npArr[m] = pArr[0];
        m++;

    }


    for(int p=0;p<partn;p++) {
        pArr[p]=npArr[p];
//        cout<<"sampled x= "<< npArr[p].w<<"\n";
//        cout<<"sampled y= "<< pArr[p].w<<"\n";
    }

    for (int p=0; p<partn; p++)
    {
        cout<<"sampled weight= "<<pArr[p].w<<endl;
    }

//    pArr=npArr;
    return part_b;
}



