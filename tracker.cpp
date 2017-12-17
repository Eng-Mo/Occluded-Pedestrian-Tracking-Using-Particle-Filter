#include "tracker.h"
#include<opencv2/objdetect.hpp>

Tracker::Tracker()
{

}

void Tracker::initTracking(MatND hist_object, Rect rect, float disp_noisex,float disp_noisey, double scale_noise){
    pf_obj.init_particles(objectparticles,hist_object, rect, part_n, disp_noisex,disp_noisey, scale_noise);

}

void Tracker::updateTracking(Mat& c_frame,const Mat& patch,MatND& ref_hist){
    pf_obj.updateParticles(c_frame,patch,ref_hist, objectparticles,part_n);
    overlappedRect();
//    generateHeatMap();

    best_p= pf_obj.resampleParticles(objectparticles,part_n);

    for (int p=0;p<part_n;p++)
    {

        cout<<"Particle weight= "<<objectparticles[p].w<<"\n";
    }
    cout<<"best weight= "<<objectparticles[0].w<<"\n";
    if (best_p.w<12){
        actual_p=best_p;
    }
    else {
        double difx, diffy;
        double dist, temdist;
        double mindist=0;
        difx=actual_p.vx-objectparticles[0].vx;
        diffy= actual_p.vy- objectparticles[0].vy;
        mindist=pow(difx,2)+pow(diffy,2);
        for (int p=0;p<part_n;p++)
        {

            difx=actual_p.vx-objectparticles[p].vx;
            diffy= actual_p.vy- objectparticles[p].vy;
            dist=pow(difx,2)+pow(diffy,2);
            if (mindist>dist)
            {

                actual_p.x=actual_p.x+objectparticles[p].vx;
                actual_p.y=actual_p.y+objectparticles[p].vy;
                mindist=dist;

                actual_p.p_rect.x= actual_p.x-(actual_p.width/2);
                actual_p.p_rect.y= actual_p.y-(actual_p.hieght/2);

            }
            objectparticles[p]=actual_p;

        }

    }



}

void Tracker::drawParticles(Mat& c_frame){
    for (int i=0;i<part_n ;i++)
    {
        circle(c_frame,Point(objectparticles[i].x, objectparticles[i].y),1,Scalar(0,0,255),1,8,0);

    }
    circle(c_frame,Point(actual_p.x, actual_p.y),5,Scalar(255,0,0),2,8,0);
//    imshow("particles", c_frame);
//    waitKey(0);
}

Rect Tracker::bestRect(){
    return actual_p.p_rect;
}

void Tracker::overlappedRect(){
    vector<Rect> rectG;
    for (int i=0; i<part_n; i++){
        rectG.push_back(objectparticles[i].p_rect);
//        cout<< "before x= "<<rectG[i].x<<endl;

    }
    cv::groupRectangles(rectG,10,.9);
    for (int i=0; i<part_n; i++){

//        cout<< "x= "<<rectG[i].x<<endl;

    }
    int c=0;

}

void Tracker::generateHeatMap(){

    Mat heatmap(h_image,w_image,CV_8U,cv::Scalar(0));
    vector<Rect> particles_rects;
    for (int p=0; p<part_n; p++)
    {
        particles_rects.push_back(objectparticles[p].p_rect);
    }
    for (int p=0; p<part_n; p++){
        Mat temp(h_image,w_image,CV_8U,Scalar(0));
        particles_rects[p].x=max(5,min(particles_rects[p].x,w_image-5));

        particles_rects[p].y=max(5,min(particles_rects[p].y,h_image-5));
        rectangle(temp,particles_rects[p],Scalar(100),-1);
        heatmap+=temp;

    }


        cv::imshow("Heatmap",heatmap);
        cv::waitKey();

}


