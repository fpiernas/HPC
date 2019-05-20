/**
    xfc.hpp
    Helix Pseudocolor Algorithm

    MIT License
    Copyright (c) 2019 Fran Piernas Diaz
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Created using OpenCV 4.0.0

    @author Fran Piernas Diaz (fran.piernas@gmail.com)
    @version 1.0


 >>>Usage to pseudocolor an image using a single threshold, call this function:

        Mat xfcs_fframe(Mat sample_i,
                        int saturation,
                        int threshold)

            where:
                sample is the image to color
                saturation is the color saturation from 0 to 140
                threshold is the desired threshold. Low thresholds remove transparent parts

 >>>Usage to pseudocolor an image with preview of the result, call this function:

        void fframe_single_threshold(string file,
                                     int saturation)

            where:
                file is the string of the path to the image
                saturation is the color saturation from 0 to 140

            Use the "+" and "-" keys (not those of the numpad) and press enter to select the threshold.

 >>>Usage to pseudocolor an image with a range of thresholds from 10 to 254, call this function:

        void threshold_range(string file,
                             int saturation)

        where:
            file is the string of the path to the image
            saturation is the color saturation from 0 to 140

*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <cmath>
#define K_PLUS 43
#define K_MINUS 45
#define K_ENTER 13
#define SCALE_FACTOR 0.1
using namespace std;
using namespace cv;
class pixv
{
    public:
        int r,g,b;
        pixv(void);
};

pixv::pixv(void)
{
    r=0;
    g=0;
    b=0;
}

pixv false_color(int value, int saturation)
{
    pixv temp_value, result;
    temp_value.r=saturation*sin(M_PI/255.0*value)*sin(2.0*M_PI/255*value);
    temp_value.g=value*sqrt(3.0);
    temp_value.b=saturation*sin(M_PI/255.0*value)*cos(2.0*M_PI/255*value);

    result.r=-0.7071067811*temp_value.b+0.5773502691*temp_value.g+0.4082482904*temp_value.r;
    result.b=0.7071067811*temp_value.b+0.5773502691*temp_value.g+0.4082482904*temp_value.r;
    result.g=0.5773502691*temp_value.g+0.8164965809*temp_value.r;

    if(result.r>255)result.r=255;
    if(result.g>255)result.g=255;
    if(result.b>255)result.b=255;
    if(result.r<0)result.r=0;
    if(result.g<0)result.g=0;
    if(result.b<0)result.b=0;

    return result;
}


Mat xfcs_fframe(Mat sample_i, int saturation, int threshold)
{
    Mat sample;
    sample=sample_i.clone();
    unsigned int i,j;
    unsigned long int pixvl;
    pixv color_;
    Vec3b punto;
    for(i=0;i<sample.size().height;i++) //convert to B/W
    {
        for(j=0;j<sample.size().width;j++)
        {
            pixvl=sample.at<Vec3b>(i,j)[0]+sample.at<Vec3b>(i,j)[1]+sample.at<Vec3b>(i,j)[2];
            punto[0]=pixvl/3.0;
            punto[1]=pixvl/3.0;
            punto[2]=pixvl/3.0;
            sample.at<Vec3b>(i,j)=punto;
        }
    }
    for(i=0;i<sample.size().height;i++) //Apply threshold rescaling
    {
        for(j=0;j<sample.size().width;j++)
        {

            if(sample.at<Vec3b>(i,j)[0]>threshold)
            {

                sample.at<Vec3b>(i,j)[0]=255;
                sample.at<Vec3b>(i,j)[1]=255;
                sample.at<Vec3b>(i,j)[2]=255;
            }

            else
            {

                sample.at<Vec3b>(i,j)[0]=sample.at<Vec3b>(i,j)[0]*255.0/threshold;
                sample.at<Vec3b>(i,j)[1]=sample.at<Vec3b>(i,j)[1]*255.0/threshold;
                sample.at<Vec3b>(i,j)[2]=sample.at<Vec3b>(i,j)[2]*255.0/threshold;
            }
        }
    }

    for(i=0;i<sample.size().height;i++) //Apply false color algorithm
    {
        for(j=0;j<sample.size().width;j++)
        {
            color_=false_color(sample.at<Vec3b>(i,j)[0],saturation);
            sample.at<Vec3b>(i,j)[0]=color_.b;
            sample.at<Vec3b>(i,j)[1]=color_.g;
            sample.at<Vec3b>(i,j)[2]=color_.r;
        }
    }

    return sample;
}

unsigned long int choose_threshold(Mat sample, int saturation)
{
    Mat tempmat, displayframe;
    tempmat=sample.clone();
    displayframe=sample.clone();
    resize(displayframe,displayframe,Size(),SCALE_FACTOR,SCALE_FACTOR,INTER_LINEAR);
    namedWindow("Choose threshold.",WINDOW_NORMAL);
    int key_pressed=0;
    unsigned long int threshold=10;
    do
    {
        tempmat=xfcs_fframe(displayframe,saturation,threshold);
        imshow("Choose threshold.",tempmat);
        key_pressed=waitKey(0); // -=173, +=171
        if(key_pressed==K_PLUS&&threshold<255) threshold++;
        if(key_pressed==K_MINUS&&threshold>10) threshold--;
        if(key_pressed==K_ENTER) break;

        cout<<"Key: "<<key_pressed<<", threshold: "<<threshold<<endl;
    }while(true);
    return threshold;
}




void fframe_single_threshold(string file, int saturation)
{
    Mat image;
    image=imread(file.c_str(),IMREAD_ANYDEPTH|IMREAD_COLOR);
    imwrite("out.png",xfcs_fframe(image,saturation,choose_threshold(image,saturation)));
    return;
}

void threshold_range(string file, int saturation)
{
    Mat image;
    int threshold;
    stringstream converter;
    string path;
    image=imread(file.c_str(),IMREAD_ANYDEPTH|IMREAD_COLOR);
    for(threshold=1;threshold<255;threshold++)
    {
        cout<<threshold<<" / "<<255<<", saving ";
        path="./output_set/";
        converter<<threshold;
        path+=converter.str();
        path+=".png";
        imwrite(path.c_str(),xfcs_fframe(image,saturation,threshold));
        converter.str("");
        cout<<path<<endl;
    }
}
