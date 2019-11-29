#include <stdio.h>
#include <opencv2/opencv.hpp>   //image operations
#include <time.h>               //time measuring & seed
#include <string.h>
#include "pixel.hpp"
#include <math.h>
#include <vector>
//#include <pair>

//allow any function
using namespace cv;
double dist(int x1, int x2, int y1, int y2)
{
  double distance=sqrt(std::pow((double)x1-x2,2)+std::pow((double)y1-y2,2)); //pow: nur pythagoras!!!
  return distance;
}

/////////////////////////////////////////////////////////////!!
//: pair benutzen und nur zwischenspeichern, was wirkich wichtig ist!
// in der höheren ebene drüberusmieren!
void closest_points(std::vector<Pixel> & v_in, int x, int y, int& count, std::vector<std::pair<double,Vec3b> > & v_out)
{
  //kann mit einer liste vielleicht schöner/effizienter sein? oder einem cleveren vector sort..?
  for(std::vector<Pixel>::iterator p = v_in.begin(); p != v_in.end(); ++p) {    //  N-steps
    double distance=dist(p->x,x,p->y,y);  //distance of new pixel
    std::pair<double,Vec3b> to_store(distance,p->color);
    if(v_out.size())
    {//der vektor hat schon inhalt:
      bool inserted=0;
      //lieber normale for-schleife? und dann checken, ob kleiner als count ist..
      for(std::vector<std::pair<double,Vec3b> >::iterator o = v_out.begin(); o != v_out.end(); ++o) {
        if(distance<o->first) //sobald kleiner, inserten!
        {
          inserted=1;
          v_out.insert(o,to_store); //problem?  //////////////////////////77
          break;
        }
      }
      //folgendes ist recht dämlich, hier sollte ein längen chek rein..
      if(inserted==0)
      {
        v_out.push_back(to_store);
      }

    }else{
      //der vektor ist noch leer:
      v_out.push_back(to_store);
    }

    if(v_out.size()>count) //das kürzen könnte man durhc vorherige clevernheit weglassen
    {
      v_out.erase(v_out.begin()+count,v_out.end());
    }
  }
  if(v_out.size()!=1)
  {
    std::cout<<"size?: "<<v_out.size()<<"\n";
  }
}

int main(int argc, char** argv )
{
//CHECK_INPUT:
    //CHECK LENGTH
    if ( argc != 4 )//|| argc != 4)
    {
      std::cout<<"fnumber of param wrongr\n";
        return -1;

    }

		std::string input= argv[1]; //file input
    char *endptr1;
    int count=strtol(argv[2], &endptr1, 10);//per parameter oben definieren und experimentieren!
    if (*endptr1 != '\0') {
      std::cout<<"second parameter was not an integer\n";
      return -1;
    }
    char *endptr2;
    int power=strtol(argv[3], &endptr2, 10);//per parameter oben definieren und experimentieren!
    if (*endptr2 != '\0') {
      std::cout<<"third parameter was not an integer\n";
      return -1;
    }

//MAIN:

    std::string line;
    std::ifstream myfile(argv[1]);
    int SIZEX;
		int SIZEY;
    std::vector<Pixel> samples; //vector with all samples
    //std::vector<Pixel> not_sampled;
    if (myfile.is_open())
    {
      //READ:
      //-...........................................
        std::cout <<"open file" <<"\n";
        getline(myfile,line);
        std::stringstream meta;
        meta<<line;
        //RESOLUTION:
        meta>>SIZEX;
        meta>>SIZEY;
          //-> nun könnte man ein 2D array anlegen und nachträglich alle bekannten
          //pixel rausschmeißen..

        Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
        Mat check_pic(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
        while (getline(myfile,line))  //read whole input data:
        {
          std::stringstream ss;
          ss<<line;                   //First Line in
          std::string firstWord;

          //Coordinat/Pixel:
          int x;
          int y;
          ss>>x;              //First Word in
          ss>>y;

          //Color:
          int r;
          int g;
          int b;
          ss>>r;
          ss>>g;
          ss>>b;

          Pixel pix;
          pix.x=x;
          pix.y=y;
          pix.color[0]=r;
          pix.color[1]=g;
          pix.color[2]=b;

          //store:  //for later use
          samples.push_back(pix);
          //draw color to ouput:
          output.at<Vec3b>(Point(x,y))[0]=r;
          output.at<Vec3b>(Point(x,y))[1]=g;
          output.at<Vec3b>(Point(x,y))[2]=b;
          //for later check:
          check_pic.at<Vec3b>(Point(x,y))[0]=100; //already sampled
        }
        myfile.close();
        std::cout<<"samples are ready!\n";
//-...........................................

//PROCESS DATA:
//-...........................................

//"

        //vlt. kann man folgende schleife irgendwie in die erste rienziehen um durchläufe zu sparen?
        for(int x=0; x<SIZEX; x++)
        {
          for(int y=0; y<SIZEY; y++)
          {
            if(!check_pic.at<Vec3b>(Point(x,y))[0])
            {//not sampled!
              std::cout<<"x "<<x<<" y "<<y<<"\n";

              int sum=0;
              double faktor=0.0;
              double check=0;
              std::vector<std::pair<double,Vec3b> >  nec_points; //distance,color
              //calc sum and get <count> closest points:
              closest_points(samples, x, y, count, nec_points);
              if(count==1)  //we dont want to divide by zero!
              {
                output.at<Vec3b>(Point(x,y))=nec_points[0].second;//floor(r+0.5);
              }else{
                //afterwards, calc sum!
                for(std::vector<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  sum+=p->first;
                }

                //calculate faktor:
                for(std::vector<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  double dist= p->first;
                  faktor+=std::pow((sum-dist),power);
                }

                //calculate color using pixels, distances, and faktor
                float r=0;
                float g=0;
                float b=0;

                for(std::vector<std::pair<double,Vec3b> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  check+=std::pow((sum-p->first),power)/faktor;

                  r+=p->second[0]*std::pow((sum-p->first),power)/(faktor);
                  g+=p->second[1]*std::pow((sum-p->first),power)/(faktor);
                  b+=p->second[2]*std::pow((sum-p->first),power)/(faktor);
                }
                std::cout<<"faktor: "<<faktor<<"\n";
                std::cout<<"check:=1?=  "<<check<<"\n";
                output.at<Vec3b>(Point(x,y))[0]=r;//floor(r+0.5);
                output.at<Vec3b>(Point(x,y))[1]=g;//floor(g+0.5);
                output.at<Vec3b>(Point(x,y))[2]=b;//floor(b+0.5);
              }
            }
          }
        }
        if(samples.size()<count)
        {
          std::cout<<"samples_count<influencin points\n";
        }
        imwrite( "output_dist_interpretation.jpg", output );
        std::cout <<"done\n";
        return 0;
  }
}
