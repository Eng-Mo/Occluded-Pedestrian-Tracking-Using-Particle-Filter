#include "tracker.h"
#include<opencv2/objdetect.hpp>

Tracker::Tracker()
{

}

void Tracker::initTracking(MatND hist_object, Rect rect, float disp_noisex,float disp_noisey, double scale_noise){
    pf_obj.init_particles(objectparticles,hist_object, rect, part_n, disp_noisex,disp_noisey, scale_noise);

}

void Tracker::updateTracking(Mat& c_frame,const Mat& patch,MatND& ref_hist){
    Scalar s1=Scalar(255,0,0);
    Scalar s2= Scalar(0,255,0);
    Scalar s3=Scalar(0,0,255);
    drawParticles(c_frame,s1);
    pf_obj.updateParticles(c_frame,patch,ref_hist, objectparticles,part_n);
    drawParticles(c_frame,s2);
    overlappedRect();
//    generateHeatMap();
    Rect min;
    Rect max;
    double minw=100;
    double maxw=0;
    for(int p=0; p<part_n;p++){
        if (objectparticles[p].w<minw){
            min=objectparticles[p].p_rect;
            minw=objectparticles[p].w;
        }
        if(objectparticles[p].w>maxw){
            max= objectparticles[p].p_rect;
            maxw= objectparticles[p].w;
        }
    }
    cout<<"minimum W= "<<minw<<endl;
    cout<<"maximum W= "<<maxw<<endl;
//    imshow("max",c_frame(max));
//    imshow("min",c_frame(min));
//    waitKey(0);

    best_p= pf_obj.resampleParticles(objectparticles,part_n);

//    drawParticles(c_frame,s3);



    if (best_p.w>.0000001){
        actual_p=best_p;
        for (int p=0; p<part_n;p++){
            objectparticles[p]=actual_p;


        }
        circle(c_frame,Point(actual_p.x, actual_p.y),5,s3,2,8,0);

    }


}

void Tracker::drawParticles(Mat& c_frame, Scalar s){
    for (int i=0;i<part_n ;i++)
    {
        circle(c_frame,Point(objectparticles[i].x, objectparticles[i].y),1,s,1,1,0);

    }
//    circle(c_frame,Point(actual_p.x, actual_p.y),5,Scalar(255,0,0),2,8,0);
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


