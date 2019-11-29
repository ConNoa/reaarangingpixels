//simply reads txt-like pixeldata and draws a picture without interpretation
/*
This script reads pixel data in our ppm-like format:
width height
x1 y1 r1 g1 b1
x2 y2 r2 g2 b2

the script draws each pixel. for each undrawn pixel it interpolates its
color using the (linear,squared,..)distance to the n closest points.
*/

#include <stdio.h>
#include <opencv2/opencv.hpp>   //image operations
#include <time.h>               //time measuring & seed
#include <string.h>
#include "pixel.hpp"
#include <math.h>

using namespace cv;

/*
calculates the distance between two points:
*/
double dist(int x1, int x2, int y1, int y2)
{
  double distance=sqrt(std::pow((double)x1-x2,2)+std::pow((double)y1-y2,2)); //pow: nur pythagoras!!!
  return distance;
}
/*
checks, wether a Pixel(x,y) is part of a vector v_all.
so it returns true or false.
if the return says false:
  -the scipts wants to find the n(=number) closest pixel.
  -it stores them in the v_out vector
  -you can read the sum of these pixels distaces to the pixel(x,y)
  -to define, wether you want the linear, squared or power of k-th distance, you use rel
  -later, you get a faktor

todo:
  -BB programmieren(muss eig nicht ins format, weil N-Schritte reichen)
  -zwei BB-vektoren übergeben.
    1.random
    2.missing-pixel
*/
//bool is_sample(std::vector<Pixel> & v_all, int x, int y, double& sum,  std::vector<Pixel> & v_out, int& number, double& faktor, int rel)
bool is_sample(std::vector<Pixel> & v_all, int x, int y, int& number, std::vector<Pixel> & v_out, double& sum, int rel, double& faktor)
{
  //run through all pixel, to check, wether x,y are new:
  for(std::vector<Pixel>::iterator p = v_all.begin(); p != v_all.end(); ++p) {    //  N-steps
    if(p->x==x&&p->y==y)
    {
      return true;
    }
    double distance=dist(p->x,x,p->y,y);
    if(v_out.size())
    {
      bool inserted=0;

      for(std::vector<Pixel>::iterator o = v_out.begin(); o != v_out.end(); ++o) {
        if(distance<dist(o->x,x,o->y,y))
        {
          inserted=1;
          v_out.insert(o,*p); //problem?
          break;
        }
      }
      if(inserted==0)
      {
        v_out.push_back(*p);
      }

    }else{
      v_out.push_back(*p);
    }

    if(v_out.size()>number)
    {
      v_out.erase (v_out.begin()+number,v_out.end());
    }
  }
  //vector fertig.. number times closest points!:
  for(std::vector<Pixel>::iterator o = v_out.begin(); o != v_out.end(); ++o) {
    sum+=dist(x,o->x,y,o->y);
  }

  for(std::vector<Pixel>::iterator p = v_out.begin(); p != v_out.end(); ++p) {
    double distance=sqrt(std::pow((double)p->x-x,2)+std::pow((double)p->y-y,2));//pow: nur pythagoras!!!
    faktor+=std::pow((sum-distance),rel);
  }
  return false;
}

int main(int argc, char** argv )
{
//CHECK_INPUT:
    //CHECK LENGTH
    if ( argc != 2 )//|| argc != 4)
    {
        return -1;
    }

		std::string input= argv[1];


//MAIN:

    std::string line;
    std::ifstream myfile(argv[1]);
    int SIZEX;
		int SIZEY;
    std::vector<Pixel> samples;
    if (myfile.is_open())
    {
        std::cout <<"open file" <<"\n";
        getline(myfile,line);
        std::stringstream meta;
        meta<<line;
        meta>>SIZEX;
        meta>>SIZEY;

        Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));


        while (getline(myfile,line))
        {
          int x;
          int y;
          int r;
          int g;
          int b;
            std::stringstream ss;
            ss<<line;                   //First Line in
            std::string firstWord;
            ss>>x;              //First Word in
            ss>>y;
            ss>>r;
            ss>>g;
            ss>>b;
            Pixel pix;
            pix.x=x;
            pix.y=y;
            pix.color[0]=r;
            pix.color[1]=g;
            pix.color[2]=b;
            samples.push_back(pix);
            output.at<Vec3b>(Point(x,y))[0]=r;
            output.at<Vec3b>(Point(x,y))[1]=g;
            output.at<Vec3b>(Point(x,y))[2]=b;
        }
        myfile.close();
        std::cout<<"reading done\n";
        for(int x=0; x<SIZEX; x++)
        {
          for(int y=0; y<SIZEY; y++)
          {
            std::cout<<"x "<<x<<" y "<<y<<"\n";
            double sum=0;
            double qsum=0;
            int number=3;
            double faktor=0;
            int rel=3;
            std::vector<Pixel> collection;

            if(!is_sample(samples,x,y,number,collection, sum,rel,faktor))
            {
              std::cout<<"----------newPix: "<<x<<","<<y<<"\n";
              double r=0;
              double g=0;
              double b=0;
              double check=0;
              for(std::vector<Pixel>::iterator p = collection.begin(); p != collection.end(); ++p) {
                double dist= sqrt(std::pow((double)p->x-x,2)+std::pow((double)p->y-y,2));
                std::cout<<dist<<"dist\n";
                //double faktor=(2*std::pow(sum-dist,2)/(std::pow(sum,2)*(number+1)-2*(std::pow(sum,2)-qsum)));
                //faktor=std::pow(sum-dist,2)/(faktor);

                check+=std::pow((sum-dist),rel)/faktor;
                //std::cout<<1/(faktor)<<"\n";
                r+=p->color[0]*std::pow((sum-dist),rel)/(faktor);
                g+=p->color[1]*std::pow((sum-dist),rel)/(faktor);
                b+=p->color[2]*std::pow((sum-dist),rel)/(faktor);
              }
              std::cout<<"cdvds "<<number/faktor<<"\n";
              std::cout<<"chc: "<<check<<"\n";
              output.at<Vec3b>(Point(x,y))[0]=floor(r+0.5);
              output.at<Vec3b>(Point(x,y))[1]=floor(g+0.5);
              output.at<Vec3b>(Point(x,y))[2]=floor(b+0.5);
              std::cout<<"is not a sample\n";
            }

          }
        }
        imwrite( "output_dist_interpretation.jpg", output );

    std::cout <<"done\n";
    return 0;
  }
}
