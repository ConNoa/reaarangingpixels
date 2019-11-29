#include <stdio.h>
#include <opencv2/opencv.hpp>   //image operations
#include <time.h>               //time measuring & seed
#include <string.h>
#include "pixel.hpp"
#include <math.h>
#include <vector>

using namespace cv;
/*
txt_to_dist_draw:
this program reads and draws random pixels/samples from a txt like file to a picture of
its original solution. All the missing pixels are interpolated, according to their
proximity, to the -count- closest sampled pixels, with the influence of a power(linear, square, 3,..).

example:
./program_name (String)txt_file (int)number_of_influencing_points (int)power_of_distance_to_influence
########################
todo:
-best bucketamount function! X,Y=f(width,height,samples,number_of_influencing_points)
-allow bucketamounts, that are not integer dividents
  ->closest_dist() muss überarbeitet werden!
-a full error analysis
*/

//BUCKETS
  //chose X and Y as natural divider of the resolution!
int X=1;//160;//160;//320;//640; //von 540
int Y=1;//90;//45;//90;//180; //von 360
#define PI 3.14159265
//RESOLTUION
int SIZEX;
int SIZEY;

/*
calcs a pixel(x,y)'s bucket N
*/
int xy_to_N(int x, int y)
{
  int N;
  N=std::floor((double)x*((double)X/((double)SIZEX)))+X*std::floor(((double)y*((double)Y/((double)SIZEY))));
  return N;
}

/*
distance, between two pixels
*/
double dist(int x1, int x2, int y1, int y2)
{
  double distance;
  distance=sqrt(std::pow((double)x1-x2,2)+std::pow((double)y1-y2,2)); //pow: nur pythagoras!!!
  return distance;
}

/*
calculates the closest distance of a pixel, from a box n, to a new box n_new
*/
double closest_dist(int x, int y, int n, int n_new)
{
  double n_d;
  if(n%X==n_new%X)  //gleiche spalte
  {
    if(n>n_new) //drüber
    {
      n_d=std::abs((((n_new+X)/X)*((double)SIZEY/((double)Y)))-y);
    }else{      //drunter
      n_d=std::abs(((n_new/X)*((double)SIZEY/((double)Y)))-y);
    }
  }else if(n/X==n_new/X) //gleiche reihe
  {
    if(n<n_new) //rechts
    {
      n_d=std::abs((n_new%X)*((double)SIZEX/((double)X))-x);
    }else{      //links
      n_d=std::abs(x-(((n_new+1)%X)*((double)SIZEX/((double)X))));
    }
  }else if(n%X<n_new%X)//rechts liegend
  {
    if(n/X>n_new/X) //oberhlab
    {
      //closest-point: linksunten
      int x_n=(n_new%X)*(SIZEX/X);
      int y_n=((n_new/X)+1)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }else{          //unterhalb
      //closest-point: links oben
      int x_n=(n_new%X)*(SIZEX/X);
      int y_n=(n_new/X)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }
  }else{  //links liegend
    if(n/X>n_new/X) //oberhlab
    {
      //closest-point: rechtsunten:
      int x_n=((n_new%X)+1)*(SIZEX/X);
      int y_n=((n_new/X)+1)*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }else{          //unterhalb
      //closest-point: rechts oben
      int x_n=((n_new%X)+1)*(SIZEX/X);
      int y_n=((n_new/X))*(SIZEY/Y);
      n_d=dist(x,x_n,y, y_n);
    }
  }

  if(n_d>1){//for safety reasons...lieber nen bucket zu viel besuchen.
    n_d-=1; //nötig?
  }
  return n_d;
}

/*
check_bucket() sorts all count-closest points of v_in towards Pixel(x,y)
in v_out!
*/

void check_bucket(int x, int y, std::vector<Pixel> & v_in, std::list<std::vector<double> > & v_out, int& count)
{
  for(int p = 0; p < v_in.size(); ++p) {
    double distance=dist(v_in[p].x,x,v_in[p].y,y);  //distance of new pixel
    // x, y, dist, r, g, b
    std::list<std::vector<double> > nec_points;
    std::vector<double> to_store;
    to_store.push_back(v_in[p].x);
    to_store.push_back(v_in[p].y);
    to_store.push_back(distance);
    to_store.push_back(v_in[p].color[0]);
    to_store.push_back(v_in[p].color[1]);
    to_store.push_back(v_in[p].color[2]);

    if(v_out.size())  //if v_out has already samples:
    {
      bool inserted=0;  //for later check

      for(std::list<std::vector<double> >::iterator o = v_out.begin(); o != v_out.end(); ++o) {
        //achtung mehrdeutigkeit: wenn zwei pixel gleichweit entfernt sind.. welcher wird als n
        //nächster in betracht gezogen?!?
        bool doit=false;
        if(distance<=((*o)[2]))
        {
          if(distance==((*o)[2])) //achtun mehrdeutigkeit
          {
            if(((*o)[3])!=to_store[3]){  //r
              if(((*o)[3])>to_store[3])
              {
                doit=true;
              }
            }else if(((*o)[4])!=to_store[4]){
              if(((*o)[4])>to_store[4])
              {
                doit=true;
              }
            }else{  //same color is okay!
              if(((*o)[5])>to_store[5])
              {
                doit=true;
              }
            }


          }else{  //echt kleiner
            doit=true;
          }
          if(doit)
          {
            inserted=1;
            v_out.insert(o,to_store); //problem?  //////////////////////////77
            break;
          }
        }
      }

      if((inserted==0)&&(v_out.size()<count)) //not inserted, but still not enough smaples.
      {
        v_out.push_back(to_store);
      }
    }else{  //always store first sample!:
      v_out.push_back(to_store);
    }

    if(v_out.size()>count) //liste zu groß?
    {
      v_out.pop_back();
    }
  }
}

//////////////////////////////////////
/*
ermittelt für einen pixel x,y, die count-viele nächste sample-punkte(aus v_in)
und übergibt einen vektor per referenz mit deren entfernungen und farben.
(dank bucket-system ist das ganze etwas unübersichtlich, aber effizient.)
*/

void closest_points(std::vector<std::vector<Pixel> > & v_in, int x, int y, int n, int& count, std::list<std::vector<double> > & v_out)
{
  ;
  int first_box=n;                                  //to save the origin-box
  std::list<std::pair<double,int> > sorted_buckets; //to stores buckets sorted, that might to be noted_boxes
  std::vector<int> noted_boxes;                     //stores already noted_boxes

  //origin-box:
  sorted_buckets.push_back(std::pair<double,int>(0.0,n));
  noted_boxes.push_back(n);
  double farest=99999999;                           //should be changed to infinity?

  while((sorted_buckets.size()))  //-as long, as there are buckets, to be visited left
  {                               //-farest-sample is farrer than closest-bucket
                                  //-or if we haven't found enough samples in the previous
                                  //  buckets?
    if(((farest>=sorted_buckets.begin()->first)|(v_out.size()<count)))
    {
      n=sorted_buckets.begin()->second; //take closest bucket
      check_bucket(x, y, v_in[n], v_out, count);//only vec, no: n, v_in

      if(v_out.size())    //schon einen sample?
      {
        std::list<std::vector<double> >::iterator la=v_out.end();
        --la; //bessere lösung?
        farest=((*la)[2]);
      }


      sorted_buckets.pop_front(); //kick just visited bucket fronm list

      //find new buckets(4 are maximum possible):
      for(int i=0; i<4;i++) //
      {
        std::pair<double,int> box;//new bucket
        int new_box;              //new n
        double n_d;               //distance to n

        bool skip=false;          //check if box is necessary

        if(i==0){//bucket over last bucket
          new_box=n-X;
          if(new_box<0) //out of image
          {
            skip=true;
          }
        }else if(i==1)//bucket right-next to last bucket
        {
          new_box=n+1;
          if((new_box>=X*Y)|(((int) (n/X))!=((int) (new_box/X))))
          {
            skip=true;
          }
        }else if(i==2)//bucket under last bucket
        {
          new_box=n+X;
          if(new_box>=X*Y)
          {
            skip=true;
          }
        }else if(i==3)//bucket left-next to last bucket
        {
          new_box=n-1;
          if((new_box<0)|(((int) (n/X))!=((int) (new_box/X))))
          {
            skip=true;
          }
        }

        //is new bucket already noted?:
        for(std::vector<int>::iterator p = noted_boxes.begin(); p != noted_boxes.end(); ++p) {
          if(new_box==*p)
          {
            skip=true;
            break;
          }
        }

        if(!skip){//take new bucket:

          noted_boxes.push_back(new_box);           //note bucket
          n_d=closest_dist(x,y,first_box,new_box);  //calc closest distance from pixel(x,y) to bucket
          std::pair<double,int> box=std::pair<double,int>(n_d,new_box); //the new bucket

          bool inserted=0;
          if(sorted_buckets.size()) //sort bucket into list of to be visited buckets:..
          {
            for(std::list<std::pair<double,int> >::iterator o = sorted_buckets.begin(); o != sorted_buckets.end(); ++o)
            {
              if(o->first>box.first)
               {
                 inserted=1;
                 sorted_buckets.insert(o,box);
                 break;
               }
            }
            if(inserted==0)
            {
              sorted_buckets.push_back(box);
            }
          }else
          {
            sorted_buckets.push_back(box);
          }
        }
      }
    }else //we want to leave while-loop:
    {
      sorted_buckets.clear();
    }
  }
}

int main(int argc, char** argv )
{
//INPUT-CHECK:
    const clock_t begin_time = clock(); //for time measurement
    if ( argc != 4 )//|| argc != 4)
    {
      std::cout<<"fnumber of param wrong\n";
      //USAGE?
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

//-...........................................
//MAIN:
    //Prepare Buckets
    std::vector<std::vector<Pixel> > samples; //buckets
    for(int x=0; x<X; x++)
    {
      for(int y=0; y<Y; y++)
      {
        std::vector<Pixel> vec;
        samples.push_back(vec);
      }
    }

//READ INPUT
    std::string line;
    std::ifstream myfile(argv[1]);
    if (myfile.is_open())
    {
        std::cout <<"open file" <<"\n";
        getline(myfile,line);
        std::stringstream meta;
        meta<<line;
        //RESOLUTION:
        meta>>SIZEX;
        meta>>SIZEY;

        Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));   //output-image
        Mat check_pic(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));//easy-trick, to remember, which pixel is sampled
                                                            //might be changed to a 2D array of booleans?

        while (getline(myfile,line))  //read whole input data:
        {
          std::stringstream ss;
          ss<<line;                   //First Line in
          std::string firstWord;

          int N;
          //Coordinat/Pixel:
          int x;
          int y;
          ss>>x;
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

          N=xy_to_N(x, y);  //get bucket
          samples[N].push_back(pix);            //store in bucket!

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
        for(int x=0; x<SIZEX; x++)
        {
          for(int y=0; y<SIZEY; y++)
          {
            if(!check_pic.at<Vec3b>(Point(x,y))[0]) // if not allready sampled:
            {
              std::cout<<"x "<<x<<" y "<<y<<"\n";
              int n=xy_to_N(x, y);

              //std::list<std::pair<double,Vec3b> >  nec_points;    //neccesseray points
              // x, y, dist, r, g, b
              std::list<std::vector<double> > nec_points;
              closest_points(samples, x, y,n, count, nec_points); //get closest_points


              std::vector<std::vector<int> > shadows;
              std::vector<int> indi;
              int ika=0;
              for(std::list<std::vector<double> >::iterator o = nec_points.begin(); o != nec_points.end(); ++o)
              {
                bool in_shadow=false;
                for(int p=0; p<shadows.size();p++ )
                {

                  //std::cout<<(shadows[p][0])<<"\n";
                  //*((*o)[0])+(shadows[p][1])*((*o)[1])<<">?"<<(shadows[p][2])<<"\n";
                  if((shadows[p][0])*((*o)[0])+(shadows[p][1])*((*o)[1])>(shadows[p][2]))
                  {

                    in_shadow=true;//

                    //std::cout<<"shadow!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    break;
                  }
                }

                if(in_shadow)
                {
                    indi.push_back(ika); //delete later!
                  //delete sample
                }else{
                  //add new shadow_space
                  //ortho:

                  double n_x=(*o)[0]-x;
                //  std::cout<<(*o)[0]<<"as\n";
                //  std::cout<<x<<"ass\n";
                  double n_y=(*o)[1]-y;
                  std::vector<int> new_shad;
                  new_shad.push_back(n_x);//x_fac;
                  new_shad.push_back(n_y);//y_fac;
                  new_shad.push_back((*o)[0]*n_x+(*o)[1]*n_y);//bordert_val;
                  shadows.push_back(new_shad);
                }
                ika++;
              }

              //delete shadow samples:

              for(int i=indi.size()-1; i>=0; i--)
              {
                std::list<std::vector<double> >::iterator la=nec_points.begin();
                for(int kr=0; kr<indi[i];kr++)
                {
                  la++;
                }
                nec_points.erase(la);
              }




              std::cout<<"count:"<<nec_points.size()<<"\n";
              power=1;//std::pow(nec_points.size()-1,2);
              if(nec_points.size()==1)  //specialcas: we dont want to divide by zero!
              {
                //only closest:
                output.at<Vec3b>(Point(x,y))[0]=(*nec_points.begin())[3];
                output.at<Vec3b>(Point(x,y))[1]=(*nec_points.begin())[4];
                output.at<Vec3b>(Point(x,y))[2]=(*nec_points.begin())[5];
                //floor(r+0.5);
              }else{

                //distance:
                double dist_fac=1;
                double angl_fac=1-dist_fac;
                int power_b=1;
                //sum of distances:
                int sum=0;  //now use max+min...
                for(std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  sum+=(*p)[2];
                }
                //std::list<std::vector<double> >::iterator las=nec_points.back();
                //las--;
                sum=(*nec_points.begin())[2]+nec_points.back()[2];


                //calc angle:
                std::vector<double> angle;
                for(std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  double s_x=(*p)[0]-x;
                  double s_y=(*p)[1]-y;
                  double pos_angl=400;
                  double neg_angl=400;
                  int vorz=1;
                  double clos=400;
                  //compare directional vector to others, to get angle:
                  for(std::list<std::vector<double> >::iterator q = nec_points.begin(); q != nec_points.end(); ++q) {
                    double s_x_q=(*q)[0]-x;
                    double s_y_q=(*q)[1]-y;
                    if((*p)[0]!=(*q)[0]||(*p)[1]!=(*q)[1])
                    {
                      double tmp_angl;
                      /*
                      if(s_x*s_x_q+s_y*s_y_q>=0)
                      {

                        tmp_angl=180*std::acos((s_x*s_x_q+s_y*s_y_q)/(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2))))/PI;

                      }else{
                          tmp_angl=-180*std::acos((s_x*s_x_q+s_y*s_y_q)/(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2))))/PI;
                      }
                      */
                      //std::cout<<tmp_angl<<"actmp\n";
                      tmp_angl=180*std::atan2(s_x*s_y_q-s_y*s_x_q,(s_x*s_x_q+s_y*s_y_q))/PI;//(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2))))/PI;
                      //std::cout<<tmp_angl<<"atmp\n";
                      //std::cout<<(s_x*s_x_q+s_y*s_y_q)/(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2)))<<"sad\n";
                      //std::cout<<tmp_angl*180/PI<<"aas\n";

                      if(std::abs(tmp_angl)<clos)
                      {
                        clos=std::abs(tmp_angl);
                        if(tmp_angl>=0)
                        {
                          vorz=1;
                        }else{
                          vorz=-1;
                        }
                      }
                    }else{
                    //  std::cout<<"sam\n";
                    }


                    //cos(phi)=A*B/|A|*|B|

                  }

                    pos_angl=clos;
                    clos=400;
                    for(std::list<std::vector<double> >::iterator q = nec_points.begin(); q != nec_points.end(); ++q) {
                      double s_x_q=(*q)[0]-x;
                      double s_y_q=(*q)[1]-y;
                      if((*p)[0]!=(*q)[0]||(*p)[1]!=(*q)[1])
                      {
                        double tmp_angl;
                        //if(s_x*s_x_q+s_y*s_y_q>=0)
                        //{

                          //tmp_angl=180*std::acos((s_x*s_x_q+s_y*s_y_q)/(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2))))/PI;
                          tmp_angl=180*std::atan2(s_x*s_y_q-s_y*s_x_q,(s_x*s_x_q+s_y*s_y_q))/PI;//(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2))))/PI;

                        //}else{
                            //tmp_angl=-180*std::acos((s_x*s_x_q+s_y*s_y_q)/(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2))))/PI;
                        //}
                        //std::cout<<(s_x*s_x_q+s_y*s_y_q)/(sqrt(std::pow(s_x,2)+std::pow(s_y,2))*sqrt(std::pow(s_x_q,2)+std::pow(s_y_q,2)))<<"sad\n";
                        //std::cout<<tmp_angl*180/PI<<"aas\n";
                      //  std::cout<<tmp_angl<<"tmp\n";
                        if(vorz>0)
                        {
                          if(tmp_angl>0)
                          {
                            tmp_angl=360-tmp_angl;
                          }
                        }else{
                          if(tmp_angl<0)
                          {
                            tmp_angl=360+tmp_angl;
                          }
                        }

                        if(std::abs(tmp_angl)<clos)
                        {
                          clos=std::abs(tmp_angl);
                        }
                      }


                      //cos(phi)=A*B/|A|*|B|

                    }


                    neg_angl=clos;

                  std::cout<<"pos. "<<pos_angl/2<<" neg.: "<<neg_angl/2<<"\n";

                  angle.push_back((pos_angl/2+neg_angl/2)); //or 2pi
                }

                //calculate faktor: //to ensure, that each pixel has 100% intensity

                double faktor=0.0;
                int i=0;
                double ang_sum=0.0;
                for(std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  double dist= (*p)[2];
                  double angl_val=angle[i];
                  ang_sum+=angl_val;
                  //dist:
                  //faktor+=(std::pow((sum-dist),power));//(angl_val);///std::pow((sum-dist),power)*std::pow(angl_val,power_b);
                  //area:
                  //faktor+=(std::pow((angl_val),power));
                  //b oth:
                  faktor+=std::pow((sum-dist),power)*(std::pow((angl_val),power));
                  i++;
                }
                std::cout<<"angsum:      "<<ang_sum<<"\n";

                //calc color:
                float r=0;
                float g=0;
                float b=0;
                i=0;
                for(std::list<std::vector<double> >::iterator p = nec_points.begin(); p != nec_points.end(); ++p) {
                  /*
                  output.at<Vec3b>(Point(x,y))[0]=nec_points.begin()[3];
                  output.at<Vec3b>(Point(x,y))[1]=nec_points.begin()[4];
                  output.at<Vec3b>(Point(x,y))[2]=nec_points.begin()[5];
                  */
                  //r+= p->second[0] * std::pow(  (sum-p->first)  , power ) / (faktor);
                  /*
                  r+=(*p)[3]*(std::pow(sum-(*p)[2],power)*std::pow(angle[i],power_b))/(faktor);
                  g+=(*p)[4]*(std::pow(sum-(*p)[2],power)*std::pow(angle[i],power_b))/(faktor);
                  b+=(*p)[5]*(std::pow(sum-(*p)[2],power)*std::pow(angle[i],power_b))/(faktor);
                  */
                  //dist: double influence=(std::pow(sum-(*p)[2],power))/faktor;///sum)*(*/angle[i]/360));///(faktor);
                  //area:
                  //double influence=(std::pow(angle[i],power))/faktor;///sum)*(*//360));///(faktor);
                  // both:
                  double influence=(std::pow(angle[i],power))*(std::pow(sum-(*p)[2],power))/faktor;//sum)*(
                  r+=(*p)[3]*influence;
                  g+=(*p)[4]*influence;
                  b+=(*p)[5]*influence;
                  //std::cout<<"fak"<<nec_points.begin())[3]<<"\n";
                  i++;
                }
                //std::cout<<"fak"<<nec_points.begin())[3]<<"\n";

                //commi
                output.at<Vec3b>(Point(x,y))[0]=r;
                output.at<Vec3b>(Point(x,y))[1]=g;
                output.at<Vec3b>(Point(x,y))[2]=b;
              }
            }
          }
        }
        imwrite( "output_dist_interpretation.jpg", output );
        std::cout <<"done\ntook:"<<(float( clock () - begin_time )/  CLOCKS_PER_SEC)<<"seconds in total\n";
        if(samples.size()<count)
        {
          std::cout<<"samples_count<influencin points\n";
        }
      }
      return 0;
}
