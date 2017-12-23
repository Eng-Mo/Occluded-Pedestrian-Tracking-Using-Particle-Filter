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

        part.x=  cx;
        part.y=  cy;
        part.vx=0;
        part.vy=0;
        part.width=r.width;
        part.hieght=r.height;
        part.w=1.0;
        part.scale= part.scalep= 1.0;
        part.p_rect=r;
//        part.particle_hist= hist_patch;
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
    default_random_engine gen;
    normal_distribution<double> dist_x(0, sd);
    normal_distribution<double> dist_y(0,1);
    normal_distribution<double> d_scale(0, scale_sd);



     ofstream output,output2;
     output.open("output_result.txt");
     output2.open("output_result2.txt");





    for (int p=0; p<p_n; p++)
    {
        int nx=part_reg[p].x+dist_x(gen);
        int ny=part_reg[p].y+dist_y(gen);
        nx=max(5,min(nx,frame_w-5));

        ny=max(5,min(ny,frame_h-5));
        cout<<"nx part"<<nx<<"\n";
        cout<<"ny part"<<ny<<"\n";

        nscale=d_scale(gen)+part_reg[p].scale;
        nscale=abs(nscale);
        nscale=min(max(double(nscale),scale_sd),(double)3);
        cout<<"new scale part"<<nscale<<"\n";
        part_reg[p].scale =nscale;
        cout<<"new scale ="<<part_reg[p].scale<<endl;

        part_reg[p].scale = ( (nx + (nscale*part_reg[p].width)/2) < (frame_w-1) && (nx - (nscale*part_reg[p].width)/2) > 0 )?nscale:0;
        part_reg[p].scale = ( (ny + (nscale*part_reg[p].hieght)/2) < (frame_h-1) && (ny - (nscale*part_reg[p].hieght)/2) > 0 )?nscale:0;


        if (part_reg[p].scale==0)
        {
//            cout<<"new scale2 part"<<part_reg[p].scale<<"\n";
            part_reg[p].scale=1.0;


            totalW+=part_reg[p].w;
            continue;
        }
        part_reg[p].scale=nscale;
        part_reg[p].x=nx;
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
        part_reg[p].w*= compareHist(ref_hist,part_reg[p].particle_hist,CV_COMP_CHISQR);
        cout<<"weight= "<<part_reg[p].w<<endl;
        part_reg[p].w= exp(-part_reg[p].w)/(2*4);
        cout<<"weight normed= "<<part_reg[p].w<<endl;

        totalW+=part_reg[p].w;
        part_reg[p].scale=1.0;



    }


    for(int p=0; p < p_n; p++) {

            part_reg[p].w = part_reg[p].w/totalW;
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
    // TODO: Resample particles with replacement with probability proportional to their weight.
    // NOTE: You may find std::discrete_distribution helpful here.
    //   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
//	cout<<"start resample"<<endl;
    vector<double> weight_vector;
    vector<particle> temp_particles;
    for (int p=0; p<partn;p++){
        weight_vector.push_back(pArr[p].w);
    }
    default_random_engine gen;
    discrete_distribution<int> d(weight_vector.begin(),weight_vector.end());

//    int index= d(gen);

    double beta=0.0;

    double max_w= *max_element(weight_vector.begin(),weight_vector.end());
//	cout<<"end max w="<<max_w<<endl;
    uniform_real_distribution<double> unifor_v(0, max_w);
    for (int p=0; p< partn;p++){
        particle prtc=pArr[d(gen)];
        temp_particles.push_back(prtc);
    }

    double we=0;
    particle best_particle;

    for (int p=0; p<partn;p++){
        pArr[p]=temp_particles[p];
        if (pArr[p].w>we)
        {
            best_particle=pArr[p];
            we =pArr[p].w;

        }
    }

    cout<<"best wieght = "<<best_particle.w<<"\n";
    cout<<"max wieght1= "<<max_w<<"\n";


    return best_particle;
}



