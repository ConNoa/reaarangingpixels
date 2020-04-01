#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../superpixel.hpp"
#include <cmath>
#include "../pixel.hpp"
#include "../multipix.hpp"
#include <time.h>             //time measuring & seed
#include <math.h>             //ceil, sqrt etc
#include "../collmap.hpp"
#include <opencv2/core/matx.hpp>
#include "../point.hpp"



using namespace cv;

class Sampler{
public:
  //expects CV_8UC3 image!
  Sampler(int amount, Mat const& image):
      _Amount(amount),
      _Image(image),
  //    _SampleMap_x(),
      _SampleMap_xy()
      //_Out(generalout),
  //    _Overlapmap(brigthnesmap)
      {
        srand(time(NULL));  //SEED
        _Beamer_Resolution_x = 1920;
        _Beamer_Resolution_y = 1080;
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


  //expects CV_8UC3 image!
  void set_image(Mat const& image){
    _Image=image;
  }

  void fill_GeneralOut(){}

  int compute_multipix_size(){
    int pow_seed = 1;
    int pixel_ammount_beamer = _Beamer_Resolution_x*_Beamer_Resolution_y;

    while((_Amount*pow(pow_seed, 2))< pixel_ammount_beamer){
      ++pow_seed;
    }
    --pow_seed;

    std::cout << "Multipix Size is set to "<< pow_seed << '\n';

    return pow_seed;
  }


    std::vector<Point_d> sample_rand_points(){
      std::cout<<"sampling random points\n";
      std::vector<Point_d> output_pattern;

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
      Point_d pix;
      for (int i=0; i<_Amount; i++)
      {
        //std::cout<<i<<"\n";
        int n= rand()% not_sampled_yet.size();
        pix.x= (double)not_sampled_yet[n].first;
        pix.y= (double)not_sampled_yet[n].second;
        pix.id = i;
        not_sampled_yet[n]=not_sampled_yet.back();
        not_sampled_yet.pop_back();
        output_pattern.push_back(pix);
      }
      return output_pattern;

    }

      std::vector<double> sample_rand_coords(){
        std::cout<<"sampling random points\n";
        std::vector<double> output_pattern;

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
        for (int i=0; i<_Amount; i++)
        {
          //std::cout<<i<<"\n";
          int n= rand()% not_sampled_yet.size();
          output_pattern.push_back((double)not_sampled_yet[n].first);
          output_pattern.push_back((double)not_sampled_yet[n].second);
          not_sampled_yet[n]=not_sampled_yet.back();
          not_sampled_yet.pop_back();
        }
        return output_pattern;

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
        pix.x= (double)not_sampled_yet[n].first;
        pix.y= (double)not_sampled_yet[n].second;
        pix.color = _Image.at<Vec3d>(Point((int)pix.x,(int)pix.y));
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

  std::vector<MultiPix> random_multipix(){
        std::cout<<"multipix sampling \n";
        int multipix_width = 3;
        int multipix_height = multipix_width;
        int mems_w = 512;
        int mems_h = 320;
        int pix_ammount = mems_h*mems_w;
        int border = multipix_width-1;  // bordercondition
        std::vector<MultiPix> mpoutput(pix_ammount);
        //Preparation
        std::vector<std::pair<int, int> > not_sampled_yet;
        for(int x=0; x<_X-border; x++){
  //        std::cout<< "x und y sind  "<< _X-border <<" und "<< _Y-border<<"\n";
          for(int y=0; y<_Y-border; y++){
            not_sampled_yet.push_back(std::pair<int,int>(x,y));
          }
        }
        //create map?



      //für sizeof mems werden multipxelerstellt zuerst das x1,y1, x2,y2 dann sollen die farben in den color vec geworfen werden, zum schluss müssen xc undyc noch  i und l  values bekommen
      //  bool print = true;
      /*
      for (int i=0; i<mems_h; i++)
      {
        if(i%2==0)  std::cout << "h is " << i <<'\n';

        for (int l=0; l<mems_w; l++)
        {
          //if(l%1==0)    std::cout << "w is " << l <<'\n';
          int n= rand()% not_sampled_yet.size();

          int x_act = not_sampled_yet[n].first;
          int y_act = not_sampled_yet[n].second;
        // std::cout<< "define xy1: \n";
          _GeneralOut[i][l].x1 = x_act;
          _GeneralOut[i][l].y1 = y_act;
        //  std::cout << "Pixeladded" << '\n';
        //  if(print==true) std::cout<<"x1: "<<_GeneralOut[i][l].x1<<"  and y1: "<<_GeneralOut[i][l].y1<<"\n";
        //  std::cout<< "define xy2: \n";
          _GeneralOut[i][l].x2 = x_act+2;
          _GeneralOut[i][l].y2 = y_act+2;
        //  if(l>5)print = false;
        //  std::cout<< "set color vec and  coll matrix";
          int counter = 0;
          for(int k=0; k<multipix_height; k++){
            for(int m=0; m<multipix_width; m++){
        //  std::cout<< k<<" . "<<m<<"\n";
        //std::cout << "colorcounter: "<<counter << '\n';
              _GeneralOut[i][l].colors[counter] = _Image.at<Vec3d>(Point(x_act+k,y_act+m));
          //    _CollMat[x_act+k][y_act+m]=   (_CollMat[x_act+k][y_act+m])+1;
                counter++;
          //  if(_CollMat[x_act+k][y_act+m]>3)std::cout<<_CollMat[x_act+k][y_act+m]<<"\n";
        // std::cout << "coloradded :" <<_Image.at<Vec3d>(Point(_GeneralOut[i][l].x1+k,_GeneralOut[i][l].y1+m)) <<'\n';
        // std::cout << "at:  :" <<Point(_GeneralOut[i][l].x1+k,_GeneralOut[i][l].y1+m) <<'\n';
            }
          }
        }
      }
      */
      int mpixcount = 0;
      for (int i=0; i<mems_h; i++)
      {
        if(i%2==0)  std::cout << "h is " << i <<'\n';

        for (int l=0; l<mems_w; l++)
        {
            if(l%1==0)    std::cout << "w is " << l <<'\n';
            int n= rand()% not_sampled_yet.size();
            MultiPix mpixel;
            int x_act = not_sampled_yet[n].first;
            int y_act = not_sampled_yet[n].second;
            std::cout<< "Size of not-sampled-yet: "<< not_sampled_yet.size()<<"\n";

            std::cout<< "define xy1: \n";
            mpixel.x1 = x_act;
            mpixel.y1 = y_act;
            mpixel.x2 = x_act+2;
            mpixel.y2 = y_act+2;

            int counter = 0;
            for(int k=0; k<multipix_height; k++){
              for(int m=0; m<multipix_width; m++){
              //  std::cout<< "counter = " <<counter<<" \n";

                mpixel.colors[counter] = _Image.at<Vec3d>(Point(x_act+k,y_act+m));
                counter++;
                mpixcount++;
              }
            }
            std::cout<< "break 1 \n";

            mpixel.xc = l;
            std::cout<< "break 2 \n";

            mpixel.yc = i;
            std::cout<< "break 3 \n";

            not_sampled_yet[n]=not_sampled_yet.back();
            not_sampled_yet.pop_back();
            std::cout<< "break 4 \n";

            mpoutput.push_back(mpixel);
            std::cout<< "break 5 \n";

        }


      }
        std::cout<< "multipixel written to refference: "<<mpixcount<<"\n";
        return mpoutput;
        /*
          MultiPix m_pix;

          int n= rand()% not_sampled_yet.size();
          m_pix.x1 = not_sampled_yet[n].first;
          m_pix.y1 = not_sampled_yet[n].second;
          m_pix.x2 = m_pix.x1+2;
          m_pix.y2 = m_pix.x1+2;

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
      //  return output_pattern;*/
      }
/*
  std::map<std::pair<int, int>, Vec4b> create_random_multipix_map(){
    //compute multipix size
    int multipixsize = compute_multipix_size();

    //evtl noch bessere morderconditions?
    //create Vector with all possible pix positions
    std::vector<std::pair<int, int> > not_sampled_yet;

    for(int x=0; x<_X-multipixsize; x++){
      for(int y=0; y<_Y-multipixsize; y++){
        not_sampled_yet.push_back(std::pair<int,int>(x,y));
      }
    }
    std::cout<< "Created Vec not-sampled-yet with size: "<< not_sampled_yet.size()<<"\n";
    int pos_pos = not_sampled_yet.size();
    //-----

    for(int i=0; i<_Amount; i++){
      int n= rand()% not_sampled_yet.size();
      auto pos_act = not_sampled_yet[n];
      int x_act = not_sampled_yet[n].first;
      int y_act = not_sampled_yet[n].second;
      not_sampled_yet[n]=not_sampled_yet.back();
      not_sampled_yet.pop_back();

//      std::cout<< "Size of sampled: "<< pos_pos-not_sampled_yet.size()<<"\n";
  //    std::cout<< "auto ptr_x_map = _SampleMap_x.find(pos_act.first); : =   "<<pos_act.first<< " / "<<pos_act.second<<"\n";

      for(int k=0; k<multipixsize; k++){
        for(int m=0; m<multipixsize; m++){
          auto pos_loop = pos_act;
          pos_loop.first = pos_loop.first + k;
          pos_loop.second = pos_loop.second + m;
          auto ptr_x_map = _SampleMap_xy.find(pos_loop);
        //  std::cout<< "auto ptr_x_map = _SampleMap_x.find(pos_act.first); : =   "<<<< "\n";
          if(ptr_x_map == _SampleMap_xy.end() ) {
              // x value not found
              Vec3d pointcolor =  _Image.at<Vec3d>(Point(x_act+k,y_act+m));
              Vec4d hit = Vec4d(pointcolor[0],pointcolor[1],pointcolor[2], 1);
              //std::cout<< "in the hit: "<< hit[0]<< "in the hit: "<< hit[1]<< "in the hit: "<< hit[2]<< "in the hit: "<< hit[3]<<"\n";
              _SampleMap_xy[pos_act] = hit;
              } else {
                Vec4d found_value = _SampleMap_xy[pos_act];
                found_value[3] = found_value[3]+1;
                _SampleMap_xy[pos_act] = found_value;
                std::cout<< "changed value"<<"\n";
                std::cout<< "in the found_value: "<< found_value[0]<< "in the found_value: "<< found_value[1]<< "in the found_value: "<< found_value[2]<< "in the found_value: "<< found_value[3]<<"\n";
            // found
          }
      }
    }
  }
}
*/

    //for i< ammount
      //get random pos xy
      //set actual xy,
      //delete random pos from vec

      //loop over width and heigth of multipix
      //for each xy-pos of multipix:
          //chek if in map x
            // 1 ->check if in map y
                //1 -> set layer value++;
                //0 -> get rgb, set layervalue 1
            // 0 ->create x,y and get rgb, set layervalue 1

      //return mapmap



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
  std::map<std::pair<int,int>,Vec4b> _SampleMap_xy;
//  std::map< int,std::map<int,Vec4b> > _SampleMap_x;
//  std::map<int, Vec4b> _SampleMap_y;

//  Interface _Out;
//  Collmap _Overlapmap;
  //FullHD Beamer Resolution
  int _Beamer_Resolution_x;
  int _Beamer_Resolution_y;


  int _X;
  int _Y;



};

//using namespace cv;
//sample_video
//sample_picture
//file:
