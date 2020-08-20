#include "sampling/sample.cpp"
#include "interpretation/interpret.cpp"
#include "evaluation/evaluate.cpp"
#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <string>
#include "pixel.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "./superpixel.hpp"
#include "./multipix.hpp"
#include <array>
#include <opencv2/core/matx.hpp>
#include <algorithm>
#include <math.h>       /* sin */
#include "point.hpp"
#include "librarys/delaunator.hpp"
#include <cstdio>
#include "sorting/portioner.cpp"
#include "mems.cpp"
//#include "mems.cpp"

//#include "./collmap.hpp"

#define PI 3.14159265

using namespace cv;

//Get files of input folder:
void getdir (std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
}


//usage: ./sample_example ./path_to_image_folder
//	-this script uses the sampler class. The path_to_image_folder should provide all
//  reference images.
//
//	-after loading, it samples them in a grid, halton and random pattern
//
// -all the samples are stored in seperate vectors of pixels (x,y,r,g,b)
// 	the seperate vectors are stored in the pattern vector
//
//	-to visualize the output, the interpreter is used

bool compareBy_xValue(const Point_d &a, const Point_d &b)
{
    return a.x < b.x;
}
bool compareBy_yValue(const Point_d &a, const Point_d &b)
{
    return a.y < b.y;
}


int main(int argc, char** argv )
{
//REFERENCE IMAGES:.............................................................
  std::cout<<"loading reference images\n";
  std::vector<std::pair<std::string,Mat> > ref_images;                          //stores double-Referenze Images
  std::string dir = argv[1];                                                    //Inputfolder-path
  std::vector<std::string> files = std::vector<std::string>();                  //stores path to Ref-Images
  getdir(dir,files);

  cv::Mat image_l;
  for (unsigned int i = 0;i < files.size();i++) {                               //load images.
    String windowname = files[i];

      if(((std::string(".")).compare(files[i]) != 0) && ((std::string("..")).compare(files[i]) != 0) )
      {
        image_l = cv::imread(dir+"/"+files[i], 1 );
      //  imshow(windowname, image_l);
      //  waitKey(0);
      //  destroyWindow(windowname);
        if ( image_l.data )
        {
            cv::Mat image_d;
            std::cout << dir+files[i] << std::endl;
            image_l.convertTo(image_d, CV_64FC3);                                 //we want double images!!
            ref_images.push_back(std::pair<std::string,Mat>(files[i],image_d));
        }
      }else{
        //std::cout<<"exlude!?:"<<files[i]<<"\n";
      }
  }
  std::cout<<"loading of reference images done!\n\n";

//SAMPLING //////////////////////////////////////////////////////////////////////////////

	int sample_amount = ref_images[0].second.cols *ref_images[0].second.rows * 0.5*0.5*0.5; // How many samples? !achtung grid images brauchen passenden wert!
	int ref_samples = ref_images[0].second.cols*ref_images[0].second.rows; //reference image
    //std::cout<<"Sampling "<<sample_amount<<" samples, which are "<<((sample_amount)/((float)ref_samples*100))<<"percent of the reference image.\n";
    //std::cout<<ref_images[0].second.cols;
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image) //für alle ref IMAGES
    {
      std::cout << "---------------------------- new file ----------------------------------------- \n";

      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      int const mems_w = 500;
      int const mems_h = 320;
      int fixed_mems_amount = mems_w*mems_h;
      int const disp_pix_width = 1920;
      int const disp_pix_height = 1080;
  //    int const disp_pix_width = ref_image_img.cols-(ref_image_img.cols%mems_w);
  //    int const disp_pix_height = ref_image_img.rows-(ref_image_img.rows%mems_h);
      std::cout << " width " << ref_image_img.cols<<" height "<<ref_image_img.rows<<'\n';

      std::cout << "disp_pix_width = " << disp_pix_width<<'\n';
      std::cout << "disp_pix_height = " << disp_pix_height<<'\n';
      std::vector<std::vector<int> > coll_mat(disp_pix_height, std::vector<int>(disp_pix_width,0));
      std::cout << "Collisions Matrix created" << '\n';
      std::cout << "Ammount of Sampled Pixels is = " << fixed_mems_amount<<'\n';

      std::map<std::pair<int, int>, Vec4b> _SampledPixels;
      std::vector<Point_d> _RandPixels;
      std::vector<double>randompixel_coords;

      Sampler sampler(fixed_mems_amount, ref_image_img); // Hier wird ein sampler erstellt!
            //Superpixelsampling with Superpixel_3


                std::cout<<"\n\n#Sampling reference image ("+ref_image_name+") with "<<fixed_mems_amount<<" samples of Multipixel3 \n";
                std::cout<<"this is arround "<<(int)round(((fixed_mems_amount*9*100)/((float)ref_samples)))<<" % of reference image pixels.\n";
                std::cout<<"this is arround "<<(int)round(((fixed_mems_amount*9*100)/((float)disp_pix_width*disp_pix_height)))<<" % of beamer pixels.\n";
                std::cout<<"\n";

      //  _SampledPixels = sampler.create_random_multipix_map();
      _RandPixels = sampler.sample_rand_points();
      std::cout<<"#Sampling done!\n";

      std::cout << "\n";
      std::cout << "portioning pixels \n";
    //std::vector<std::vector<Point_d>> portionen;
      Portioner portionierer(ref_image_img.cols, ref_image_img.rows, _RandPixels);
//      portionierer.make_Portion();
      portionierer.make_Portion_fast();
      std::cout << "# Portionierung done " <<"\n";
      std::cout << "\n";


      Mems mems_device(mems_w, mems_h);
      mems_device.fill_mems_with_mirrors(ref_image_img.cols, ref_image_img.rows);

//      mems_device.find_samples_next_to_mirror_fast(portionierer);

      std::string dataname = "mirrorswithsamples.dat";
      std::string input2 = "mirrorswithsamples_weiterverarbeitet.dat";


//      mems_device.save_mirrors_with_samples(dataname);

//      mems_device.read_mirrors_with_samples(dataname);
//      mems_device.print_informations();
//      mems_device.give_every_mirror_a_sample_slow();
//      mems_device.save_mirrors_with_samples_2(input2);

      std::cout << "# Mirrrorsampling done " <<"\n";

      mems_device.read_mirrors_with_samples_2(input2);

      mems_device.compare_by_id();

      //mems_device.print_informations();

      for(auto it = std::begin(mems_device._mems_mirrors); it!= std::end(mems_device._mems_mirrors);++it){
        it->_position.x = (it->_position.x-3)/2;
        it->_position.y = (it->_position.y-3)/2;

      }

      mems_device.create_multipix();
      //mems_device.print_informations();

      //--------------------End of new Sampler --------------------------
      //Interpreter interpreter(mems_w,mems_h);
      //      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);

      Mat output(mems_h*3, mems_w*3, CV_64FC4, Scalar(0,0,0,255));

      for(auto it = std::begin(mems_device._mems_mirrors_multi); it != std::end(mems_device._mems_mirrors_multi); ++it){
        output.at<Vec4d>(Point(it->_position.x,it->_position.y))[0]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[0];
        output.at<Vec4d>(Point(it->_position.x,it->_position.y))[1]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[1];
        output.at<Vec4d>(Point(it->_position.x,it->_position.y))[2]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[2];
        if((int)it->_position.x % 3 == 0||(int)it->_position.x % 3 == 2||(int)it->_position.y % 3 == 0||(int)it->_position.y % 3 == 2){
          output.at<Vec4d>(Point(it->_position.x,it->_position.y))[3]= 122;

        }
      }



      Mat output2(2000, 3000, CV_64FC4, Scalar(0,0,0,0));

      for(auto it = std::begin(mems_device._mems_mirrors_multi); it != std::end(mems_device._mems_mirrors_multi); ++it){
        output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[0]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[0];
        output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[1]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[1];
        output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[2]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[2];
        if((int)it->_position.x % 3 == 0||(int)it->_position.x % 3 == 2||(int)it->_position.y % 3 == 0||(int)it->_position.y % 3 == 2){
        output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[3]= output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[3] + 100;
        }
        else{
          output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[3]= 255;
        }
      }

      std::string name="basic_samples"+std::to_string(sample_amount)+ref_image_name;
      imwrite("result_1"+name+".png",output);
      imwrite("result_2"+name+".png",output2);

      std::cout<<"#Visualizing done!\n";
      std::cout<<"\n";
      std::cout<<"\n";
      std::cout<<"\n";
    }
}
