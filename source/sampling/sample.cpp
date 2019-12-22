#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../superpixel.hpp"

#include "../pixel.hpp"
#include "../multipix.hpp"
#include <time.h>             //time measuring & seed
#include <math.h>             //ceil, sqrt etc
#include "../collmap.hpp"

using namespace cv;

class Sampler{
public:
  //expects CV_8UC3 image!
  Sampler(int amount, Mat const& image, /*Interface & generalout,*/ std::vector<std::vector<int> > & _coll_mat):
      _Amount(amount),
      _Image(image)
      //_Out(generalout),
  //    _Overlapmap(brigthnesmap)
      {
        srand(time(NULL));  //SEED
        _X=_Image.cols;
        _Y=_Image.rows;
      }
      //      std::cout<<"-----------------cols  "<<_Image.cols<<"  -----rows  "<<_Image.rows<<"\n";

  std::vector<Pixel_d>  calc_grid(){
    std::cout<<"sampling grid\n";

    std::vector<Pixel_d> output_pattern;

    //SAMPLING
    cv::Size size = _Image.size();
    Pixel_d pix;
    double count = std::ceil(std::sqrt(_Amount));
    int y_spacing = (int)std::max(std::ceil(size.height/count),1.0);
    int x_spacing = (int)std::max(std::ceil(size.width/count),1.0);
    std::cout<<size.height<<std::endl;
    for(int x=0; x<size.width; x+=x_spacing)
    {
      for(int y=0; y<size.height; x+=x_spacing)
      {
        pix.x = x;
        pix.y = y;
        pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));
        output_pattern.push_back(pix);
      }
    }
    return output_pattern;
  }

  std::vector<Pixel_d>  calc_halton(){
    std::cout<<"sampling halton\n";
    std::vector<Pixel_d> output_pattern;

    //SAMPLING
    cv::Size size = _Image.size();
    int p = 2; int q = 3;
    Pixel_d pix;

    for (int i=1; i<_Amount+1; i++)
    {
      double fx = 1; double fy = 1;
      int ix = i; int iy = i;
      double rx = 0; double ry = 0;

      while (ix > 0) {
      	fx /= p;
      	rx += fx * (ix % p);
      	ix = ix / p;
      }

      while (iy > 0) {
      	fy /= q;
      	ry += fy * (iy % q);
      	iy = iy / q;
      }
      double x = rx * size.width;
      double y = ry * size.height;
      pix.x= x;
      pix.y= y;
      pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));

      output_pattern.push_back(pix);
    }
    return output_pattern;
  }

/*
  std::vector<Pixel_d>  calc_halton_compressed(){
      std::cout<<"sampling halton compressed\n";
      std::vector<Pixel_d> output_pattern;

      //SAMPLING
      cv::Size size = _Image.size();
      //------------newstuff-------------------
      double count_x = 0;
      double count_y = 0;

      //----------------------------------------
      int p = 2; int q = 3;
      Pixel_d pix;

      for (int i=1; i<_Amount+1; i++)
      {
        double fx = 1; double fy = 1;
        int ix = i; int iy = i;
        double rx = 0; double ry = 0;

        while (ix > 0) {
        	fx /= p;
        	rx += fx * (ix % p);
        	ix = ix / p;
        }

        while (iy > 0) {
        	fy /= q;
        	ry += fy * (iy % q);
        	iy = iy / q;
        }
        double x = rx * size.width;
        double y = ry * size.height;

        pix.x= x;
        pix.y= y;

        pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));

        pix.x= count_x;
        pix.y= count_y;

        output_pattern.push_back(pix);
        count_x = count_x+1;
        if (int(count_x)%int(_Output_x) == 0){
          count_x = 0;
          count_y = count_y+1;
        }

      }
      return output_pattern;
    }
*/

  //expects CV_8UC3 image!
  void set_image(Mat const& image){
    _Image=image;
  }

  std::vector<Pixel_d>  calc_rand_d(){
      std::cout<<"sampling random\n";
      std::vector<Pixel_d> output_pattern;

      //PREPARE:
      std::vector<std::pair<int, int> > not_sampled_yet;
      for(int x=0; x<_X; x++)
      {
        for(int y=0; y<_Y; y++)
        {
          not_sampled_yet.push_back(std::pair<int,int>(x,y));
        }
      }

      //SAMPLING
      Pixel_d pix;

      for (int i=0; i<_Amount; i++)
      {
        //std::cout<<i<<"\n";
        int n= rand()% not_sampled_yet.size();
        pix.x= not_sampled_yet[n].first;
        pix.y= not_sampled_yet[n].second;
        pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));
        not_sampled_yet[n]=not_sampled_yet.back();
        not_sampled_yet.pop_back();
        output_pattern.push_back(pix);
      }
      return output_pattern;
    }

  std::vector<Superpixel_3> random_superpixel(){
      std::cout<<"superpixel sampling \n";
      std::vector<Superpixel_3> output_pattern;
      int superpixel_width = 3;
      int border = superpixel_width-1;  // bordercondition

      //Preparation
      std::vector<std::pair<int, int> > not_sampled_yet;
      for(int x=0; x<_X-border; x++)
      {
        for(int y=0; y<_Y-border; y++)
        {
          not_sampled_yet.push_back(std::pair<int,int>(x,y));
        }
      }
      Pixel_d px_uplft;

  //    int counter2 = 0;
  //    int counter1 = 0;
      for (int i=0; i<_Amount; i++)
      {
        Superpixel_3 suppix;

        int n= rand()% not_sampled_yet.size();
        int picked_x = not_sampled_yet[n].first;
        int picked_y = not_sampled_yet[n].second;

  //      if(counter1 <2)std::cout<<"\n Pixel "<<" x:  "<<picked_x<<"   y:  "<<picked_y<<"   \n";

        px_uplft.x= picked_x;
        px_uplft.y= picked_y;

        for(int p = 0; p<9; p++){

          px_uplft.color = _Image.at<Vec3d>(Point(px_uplft.x,px_uplft.y));
          //suppix.pixelarray[i-1] = (px_uplft);
          //-------another try with std::vector
          suppix.pixelpart.push_back(px_uplft);
//debug          if(counter2 <2)std::cout<<"\n Pixel "<<i<<" vom superpixel "<<counter2<<" mit den Werten:  "<<"x:  "<<px_uplft.x<<"   y:  "<<px_uplft.y<<"   ";
          px_uplft.x = px_uplft.x+1;

          if(p==2||p==5){
//debug            if(counter2 <2)std::cout<<" pickedx vorm sprung"<< picked_x<<"\n";
            px_uplft.y = px_uplft.y+1;
            px_uplft.x = px_uplft.x-3;
          }

        }
//        if(counter2 <2)std::cout<<"\n";
//        counter2++;
//        counter1++;
        not_sampled_yet[n]=not_sampled_yet.back();
        not_sampled_yet.pop_back();
        output_pattern.push_back(suppix);
      }
      std::cout<<"all pixels sampled \n";
      return output_pattern;
    }

    void random_multipix(){
        std::cout<<"superpixel sampling \n";
        std::vector<Superpixel_3> output_pattern;
        int superpixel_width = 3;
        int border = superpixel_width-1;  // bordercondition

        //Preparation
        std::vector<std::pair<int, int> > not_sampled_yet;
        for(int x=0; x<_X-border; x++)
        {
          for(int y=0; y<_Y-border; y++)
          {
            not_sampled_yet.push_back(std::pair<int,int>(x,y));
          }
        }
        Pixel_d px_uplft;

    //    int counter2 = 0;
    //    int counter1 = 0;
        for (int i=0; i<_Amount; i++)
        {
          Superpixel_3 suppix;

          int n= rand()% not_sampled_yet.size();
          int picked_x = not_sampled_yet[n].first;
          int picked_y = not_sampled_yet[n].second;

    //      if(counter1 <2)std::cout<<"\n Pixel "<<" x:  "<<picked_x<<"   y:  "<<picked_y<<"   \n";

          px_uplft.x= picked_x;
          px_uplft.y= picked_y;

          for(int p = 0; p<9; p++){

            px_uplft.color = _Image.at<Vec3d>(Point(px_uplft.x,px_uplft.y));
            //suppix.pixelarray[i-1] = (px_uplft);
            //-------another try with std::vector
            suppix.pixelpart.push_back(px_uplft);
    //debug          if(counter2 <2)std::cout<<"\n Pixel "<<i<<" vom superpixel "<<counter2<<" mit den Werten:  "<<"x:  "<<px_uplft.x<<"   y:  "<<px_uplft.y<<"   ";
            px_uplft.x = px_uplft.x+1;

            if(p==2||p==5){
    //debug            if(counter2 <2)std::cout<<" pickedx vorm sprung"<< picked_x<<"\n";
              px_uplft.y = px_uplft.y+1;
              px_uplft.x = px_uplft.x-3;
            }

          }
    //        if(counter2 <2)std::cout<<"\n";
    //        counter2++;
    //        counter1++;
          not_sampled_yet[n]=not_sampled_yet.back();
          not_sampled_yet.pop_back();
          output_pattern.push_back(suppix);
        }
        std::cout<<"all pixels sampled \n";
      //  return output_pattern;
      return;
      }


/*
  std::vector<Pixel_d>  calc_rand_d_compressed(){
      std::cout<<"sampling random compressed\n";
      std::vector<Pixel_d> output_pattern;

      //PREPARE:
      std::vector<std::pair<int, int> > not_sampled_yet;
      for(int x=0; x<_X; x++)
        {
          for(int y=0; y<_Y; y++)
          {
            not_sampled_yet.push_back(std::pair<int,int>(x,y));
          }
        }
      double count_x = 0;
      double count_y = 0;
      //SAMPLING
      Pixel_d pix;

      for (int i=0; i<_Amount; i++)
      {
        std::cout<<i<<"\n";
        int n= rand()% not_sampled_yet.size();
        pix.x= not_sampled_yet[n].first;
        pix.y= not_sampled_yet[n].second;
        pix.color = _Image.at<Vec3d>(Point(pix.x,pix.y));

        not_sampled_yet[n]=not_sampled_yet.back();
        not_sampled_yet.pop_back();

        pix.x= count_x;
        pix.y= count_y;

        output_pattern.push_back(pix);
        count_x = count_x+1;
        if (int(count_x)%int(_Output_x) == 0){
          count_x = 0;
          count_y = count_y+1;
        }
        output_pattern.push_back(pix);

      }


      return output_pattern;
    }
*/


private:
  int _Amount;
  Mat _Image;
//  Interface _Out;
//  Collmap _Overlapmap;
  int _X;
  int _Y;

};

//using namespace cv;
//sample_video
//sample_picture
//file:
