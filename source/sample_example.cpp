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
/*
bool compareBy_xValue(const Pixel_d &a, const Pixel_d &b)
{
    return a.x < b.x;
}
bool compareBy_yValue(const Pixel_d &a, const Pixel_d &b)
{
    return a.y < b.y;
}
*/
bool compareBy_xValue(const Point_d &a, const Point_d &b)
{
    return a.x < b.x;
}
bool compareBy_yValue(const Point_d &a, const Point_d &b)
{
    return a.y < b.y;
}

/*
void rpf(std::string const& name_of_rpf, Mat const& output){
    std::fstream f;
    f.open(name_of_rpf, std::ios::out);
    f << "name of picture" << " " << "original-picture_width"  << " " << "original-picture_height"  << " "  << " has color depth of 32bit for r+b+g+alpha (4* 8bit)" <<"\n";
    f << "RPF has"  << " "  << "Samples of original picture"  << " "  << "Multipixels have size of " << " " <<"\n";
    for(auto it = output.begin(); it != output.end(); ++it){
      std::cout<<" Px "<<_mems_mirrors.back()._position.x<<" Py "<<_mems_mirrors.back()._position.y<<" Ox "<<_mems_mirrors.back()._displayed_sample.x<<" Oy "<<_mems_mirrors.back()._displayed_sample.y<<"\n";
      f <<" Px "<<_mems_mirrors.back()._position.x<<" Py "<<_mems_mirrors.back()._position.y<<" Ox "<<_mems_mirrors.back()._displayed_sample.x<<" Oy "<<_mems_mirrors.back()._displayed_sample.y<<"\n";
    }
    f << "--- " << "\n";
    f.close();
    return;
  }
*/

// function to compute the alpha value of the pixel
// int compute_alpha(int size, int pos_x, int pos_y) {
//   // get the position in respect to the center pixel
//   int x = pos_x % size;
//   int y = pos_y % size;

//   // distance to the center pixel
//   double pos = std::sqrt(std::pow((double)x, 2) + std::pow((double)y, 2));


// }

// create a window filled with alpha value and has size of multipixel
std::vector<std::vector<int>> alpha_value(int size) {
  // create new alpha matrix
  std::vector<std::vector<int>> alpha_matrix;

  // preparing the step and evaluation value
  double step_x = 1.0 / (size - 1.0);
  double step_y = 1.0 / (size - 1.0);
  std::cout << "step_x: " << step_x << std::endl;

  double eval_x = -1.0 / 2.0;
  std::cout << "eval_x: " << eval_x << std::endl;

  double eval_y = -1.0 / 2.0;
  double cos_value = 0.0;
  int alpha = 0;

  for(int i = 0; i < size; i++) {
    // row of the alpha matrix
    std::vector<int> row;

    for(int j = 0; j < size; j++) {
      // evaluation distance to center pixel
      double dis = std::sqrt(std::pow(eval_x, 2) + std::pow(eval_y, 2));

      if(dis <= 0.5) {
        cos_value = std::pow(cos(dis * CV_PI / 2.0), 4);
      } else
      {
        cos_value = 0;
      }
      alpha = (int)(cos_value * 255.0);
      row.push_back(alpha);
      eval_y += step_y;
    }
    alpha_matrix.push_back(row);
    // reset eval_y and raise eval_x
    eval_y = -1.0 / 2.0;
    eval_x += step_x;
  }
  return alpha_matrix;
}

// alpha matrix with fade out effect
std::vector<std::vector<int>> alpha_value_gradient(int size) {
  // create new alpha matrix
  std::vector<std::vector<int>> alpha_matrix;

  // center_x = center_y
  int center_pos = (size - 1) / 2;

  for(int i = 0; i < size; ++i) {

    std::vector<int> row;
    for(int j = 0; j < size; ++j) {
      // numbered the pixel with their position to the center pixel
      int dis = std::max(std::abs(i - center_pos), std::abs(j - center_pos));

      // center pos + 1 to ignore the 0 value of the value
      double cos_value = std::pow(cos((double)dis / ((double)center_pos + 1.0) * CV_PI / 2.0), 4);
      int alpha = (int)(cos_value * 255.0);
      row.push_back(alpha);
    }
    alpha_matrix.push_back(row);

  }
  return alpha_matrix;
}

int main(int argc, char** argv )
{
//REFERENCE IMAGES:.............................................................
  std::cout<<"loading reference images\n";
  std::vector<std::pair<std::string,Mat> > ref_images;                          //stores double-Referenze Images
  std::string dir = argv[1];                                                    //Inputfolder-path
  int size_of_multiPic;
  if(argc > 2) {
    size_of_multiPic = std::stoi(argv[2]);
    std::cout << "Size of multipixel is: " << argv[2] << std::endl;
  } else
  {
    size_of_multiPic = 3;
    std::cout << "Using default size for multipixel: 3" << std::endl;
  }

  // create new alpha_matrix with size of multipixel
  std::vector<std::vector<int>> alpha_matrix = alpha_value(size_of_multiPic);
  
  
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

      //std::cout << "Interface created" << '\n';

      std::map<std::pair<int, int>, Vec4b> _SampledPixels;
      std::vector<Point_d> _RandPixels;
      std::vector<double>randompixel_coords;



      //-----------New Sampler for Superpixel_3/MultiPix Pixels--------------
      //    Sampler sampler(fixed_mems_amount,ref_image_img, mems_w, mems_h); // Hier wird ein sampler erstellt!
      //          Sampler sampler(fixed_mems_amount, ref_image_img, alloutput, collisionmap); // Hier wird ein sampler erstellt!
      Sampler sampler(fixed_mems_amount, ref_image_img); // Hier wird ein sampler erstellt!
        /*
            //Superpixelsampling with Superpixel_3
            std::cout<<"\n\n#Sampling reference image ("+ref_image_name+") with "<<fixed_mems_amount<<" samples of Superpixel_3   ("<<((fixed_mems_amount*9*100)/((float)ref_samples))<<" percent of reference image pixels).\n";
            std::vector<std::pair<std::string,std::vector<Superpixel_3>>> superpixelpattern; //speichert die verschiedenen samples!
            superpixelpattern.push_back(std::pair<std::string,std::vector<Superpixel_3> >("SRand",sampler.random_superpixel()));
                */

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

      //    mems_device.print_informations();

      for(auto it = std::begin(mems_device._mems_mirrors); it!= std::end(mems_device._mems_mirrors);++it){
        it->_position.x = (it->_position.x-3)/2;
        it->_position.y = (it->_position.y-3)/2;
      }

      mems_device.create_multipix(size_of_multiPic);
      std::string rpf_name = "file1.rpf";

      //mems_device.write_rpf(rpf_name);
      //mems_device.print_informations();
      //  exit(1);
      //die verschiedenen verteilungen sind nun im vektor namens pattern verfügbar!

      //--------------------End of new Sampler --------------------------
      //Interpreter interpreter(mems_w,mems_h);
      //Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);

      //    Mat output(mems_h*3, mems_w*3, CV_64FC(6), Scalar(0,0,0,255));

      Mat output(mems_h*3, mems_w*3, CV_64FC(6));

      for(auto it = std::begin(mems_device._mems_mirrors_multi); it != std::end(mems_device._mems_mirrors_multi); ++it){
        output.at<Vec6d>(Point(it->_position.x,it->_position.y))[0]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[0];
        output.at<Vec6d>(Point(it->_position.x,it->_position.y))[1]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[1];
        output.at<Vec6d>(Point(it->_position.x,it->_position.y))[2]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[2];
        if((int)it->_position.x % 3 == 0||(int)it->_position.x % 3 == 2||(int)it->_position.y % 3 == 0||(int)it->_position.y % 3 == 2){
          output.at<Vec6d>(Point(it->_position.x,it->_position.y))[3]= 122;
        }
        else{
          output.at<Vec6d>(Point(it->_position.x,it->_position.y))[3]= 255;
        }
        output.at<Vec6d>(Point(it->_position.x,it->_position.y))[4]= it->_displayed_sample.x;
        output.at<Vec6d>(Point(it->_position.x,it->_position.y))[5]= it->_displayed_sample.y;

      }


      std::fstream f;
      f.open(rpf_name, std::ios::out);
      f << "name of picture" << " " << "original-picture_width"  << " " << "original-picture_height"  << " "  << " has color depth of 32bit for r+b+g+alpha (4* 8bit)" <<"\n";
      f << "RPF has"  << " "  << "Samples of original picture"  << " "  << "Multipixels have size of " << " " <<"\n";
      for(int i = 0; i < output.rows; i++)
      {
          const Vec6d* Mi = output.ptr<Vec6d>(i);
          for(int j = 0; j < output.cols; j++){
        //  std::cout<<" i "<<i<<"  j "<<j<<"   DSy "<<Mi[j][5]<<"   DSx "<<Mi[j][4]<<" R "<<Mi[j][0]<<"   G "<<Mi[j][1]<<" B "<<Mi[j][2]<<"   A "<<Mi[j][3]<<"\n";
          f<<" i "<<i<<"  j "<<j<<" DSy "<<Mi[j][5]<<" DSx "<<Mi[j][4]<<" R "<<Mi[j][0]<<" G "<<Mi[j][1]<<" B "<<Mi[j][2]<<" A "<<Mi[j][3]<<"\n";
        }
      }
      f << "--- " << "\n";
      f.close();

      std::fstream f_bit;
      f_bit.open("rpf_bit", std::ios::out);
/*
      f_bit << "name of picture: " << ref_image_name <<"\n";
      f_bit << "Orig-pic_width"  << " 3000" << " Orig-pic_height"  << " 1930 "  << " has color depth of 32bit for r+b+g+alpha (4* 8bit)" <<"\n";
      f_bit << "RPF has"  << " 16000 "  << "Samples of original picture"  << " "  << "Multipixels have size of " << "3*3" <<"\n";
      f_bit << " RPF - width  "<< output.cols << " RPF - height "  << output.rows  << " "  <<"\n";
      f_bit <<"\n";
      {
          std::cout << "name of picture: " << ref_image_name <<"\n";
          std::cout << "Orig-pic_width"  << " 3000" << " Orig-pic_height"  << " 1930 "  << " has color depth of 32bit for r+b+g+alpha (4* 8bit)" <<"\n";
          std::cout << "RPF has"  << " 16000 "  << "Samples of original picture"  << " "  << "Multipixels have size of " << "3*3" <<"\n";
          std::cout << " RPF - width  "<< output.cols << " RPF - width  "<< output.cols.toHex() << " RPF - height "  << output.rows  << " "  <<"\n";
          std::cout <<"\n";
      }

      for(int i = 0; i < output.rows; i++)
      {
          const Vec6d* Mi = output.ptr<Vec6d>(i);
          for(int j = 0; j < output.cols; j++){
        //  std::cout<<" i "<<i<<"  j "<<j<<"   DSy "<<Mi[j][5]<<"   DSx "<<Mi[j][4]<<" R "<<Mi[j][0]<<"   G "<<Mi[j][1]<<" B "<<Mi[j][2]<<"   A "<<Mi[j][3]<<"\n";
          f_bit<<Mi[j][5].toHex()<<" "<<Mi[j][4]<<" "<<Mi[j][0]<<" "<<Mi[j][1]<<" "<<Mi[j][2]<<" "<<Mi[j][3]<<"\n";
        }
      }
      f_bit << "--- " << "\n";
      f_bit.close();
      */


      // ------------------------------------------------------------------------
      // ------------------------------------------------------------------------
      // ------------------------------------------------------------------------


      // output2 stores data in a matrix of original picture_width & picture_height
      // sample pixels are stored at original positions
      // function for correct alpha value is not yet implemented as extra function in multipixels
      Mat output2(2000, 3000, CV_64FC4, Scalar(0,0,0,0));
      // print to test the alpha matrix
      for(int i = 0; i <  size_of_multiPic; ++i) {
        for(int j = 0; j < size_of_multiPic; ++j) {
          std::cout<< alpha_matrix[i][j]<< "      ";
        }
        std::cout<< std::endl;
      }

      for(auto it = std::begin(mems_device._mems_mirrors_multi); it != std::end(mems_device._mems_mirrors_multi); ++it) {

        // sampling position
        int samp_x = it->_displayed_sample.x;
        int samp_y = it->_displayed_sample.y;
        Point samp_pos = Point(samp_x, samp_y);

        output2.at<Vec4d>(samp_pos)[0] = ref_image_img.at<Vec3d>(samp_pos)[0];
        output2.at<Vec4d>(samp_pos)[1] = ref_image_img.at<Vec3d>(samp_pos)[1];
        output2.at<Vec4d>(samp_pos)[2] = ref_image_img.at<Vec3d>(samp_pos)[2];
        
        // std::cout<< "samp_x: "<< it->_displayed_sample.x << "      samp_y: "<< it->_displayed_sample.y << std::endl;
        // std::cout<< "pos_x: "<< it->_position.x << "      pos_y: "<< it->_position.y<< std::endl;

        // ...position in alpha matrix (position to center pixel of the multipixel)
        int pos_x = (int)it->_position.x % size_of_multiPic;
        int pos_y = (int)it->_position.y % size_of_multiPic;

        // compute the alpha value for the pixel
        output2.at<Vec4d>(samp_pos)[3] = alpha_matrix[pos_x][pos_y];

        // if(pos_x % 3 == 0 || pos_x % 3 == 2 || pos_y % 3 == 0 || pos_y % 3 == 2) {
        // // output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[3]= output2.at<Vec4d>(Point(it->_displayed_sample.x,it->_displayed_sample.y))[3] + 100;
        //   output2.at<Vec4d>(samp_pos)[3] += 122;
        // }
        // else {
        //   output2.at<Vec4d>(samp_pos)[3] = 255;
        // }
      }

      // output3 stores data in a compressed way in matrix
      // sample pixels are stored without space between
      // function for correct alpha value is not yet implemented as extra function in multipixels
      Mat output3(mems_h*3, mems_w*3, CV_64FC4, Scalar(0,0,0,255));
      for(auto it = std::begin(mems_device._mems_mirrors_multi); it != std::end(mems_device._mems_mirrors_multi); ++it){
        output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[0]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[0];
        output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[1]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[1];
        output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[2]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[2];
        if((int)it->_position.x % 3 == 0||(int)it->_position.x % 3 == 2||(int)it->_position.y % 3 == 0||(int)it->_position.y % 3 == 2){
          output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[3]= 122;
        }
      }


    /*  for(std::vector<Pixel_d>::iterator i = _Pattern.begin(); i != _Pattern.end(); ++i) {
          output.at<Vec3d>(Point((*i).x,(*i).y))[0]=(*i).color[0];
          output.at<Vec3d>(Point((*i).x,(*i).y))[1]=(*i).color[1];
          output.at<Vec3d>(Point((*i).x,(*i).y))[2]=(*i).color[2];
        }

*/
      /*-----------------Old Interpreter, imwrite---------------------*/

/*
      Mat output;
      Mat eval_out;

      for(std::vector<std::pair<std::string,std::vector<Superpixel_3> > >::iterator pattern= superpixelpattern.begin(); pattern != superpixelpattern.end(); ++pattern)
      {
        std::cout<<"Interpreting pixel";
        interpreter.set_pattern_s((*pattern).second);
        output = interpreter.no_interpretation();
        std::string name="basic_samples"+std::to_string(sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
      }

*/
      std::string name="basic_samples"+std::to_string(sample_amount)+ref_image_name;
      imwrite("result_1"+name+".png",output3);
      imwrite("result_2"+name+".png",output2);

      std::cout<<"#Visualizing done!\n";
      std::cout<<"\n";
      std::cout<<"\n";
      std::cout<<"\n";
    }
}
