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

#define PI 3.14159265

using namespace cv;
using namespace std;
// create a window filled with alpha value and has size of multipixel
vector<vector<int>> alpha_value(int size) {
  // create new alpha matrix
  vector<vector<int>> alpha_matrix;

  // preparing the step and evaluation value
  double step_x = 1.0 / (size - 1.0);
  double step_y = 1.0 / (size - 1.0);
  cout << "step_x: " << step_x << endl;

  double eval_x = -1.0 / 2.0;
  cout << "eval_x: " << eval_x << endl;

  double eval_y = -1.0 / 2.0;
  double cos_value = 0.0;
  int alpha = 0;

  for(int i = 0; i < size; i++) {
    // row of the alpha matrix
    vector<int> row;

    for(int j = 0; j < size; j++) {
      // evaluation distance to center pixel
      double dis = sqrt(pow(eval_x, 2) + pow(eval_y, 2));

      if(dis <= 0.5) {
        cos_value = pow(cos(dis * CV_PI / 2.0), 4);
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

//Get files of input folder:
void getdir (string dir, vector<string> &files){
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
}


int main(int argc, char** argv )
{
  //--- initialize members
  vector<pair<string,Mat>> ref_images;  //stores double-reference Images
  int size_of_multiPic;
  string dir;
  int pixelPercent = 10;              // default percent of the pixel that need tobe resampled
  int size_of_window = 5;
  string sampler_name = "";
  int base_x = 2;
  int base_y = 3;

  //--- commandline input
    if(argc <= 4){
      cout << "not enough arguments. check please. " << endl;
    }
    if(argc > 4){
      size_of_multiPic = stoi(argv[2]);
      cout << "Size of multipixel is: \n" << argv[2] << endl;
      sampler_name = argv[3];
      cout << "Use of " << argv[2]<< " sampling. \n" << endl;

      if(sampler_name == "halton") {
          base_x = stoi(argv[3]);
          base_y = stoi(argv[4]);
          pixelPercent = stoi(argv[5]);
          size_of_window = stoi(argv[6]);
      }
      if(sampler_name == "random") {
          pixelPercent = stoi(argv[3]);
          size_of_window = stoi(argv[4]);
      }
      if(sampler_name == "corjitt") {
          pixelPercent = stoi(argv[3]);
          size_of_window = stoi(argv[4]);
      }
      if(size_of_window % 2 == 0) {
        cout << "Size has to be odd!"<< endl;
        return 0;
      }
    }

  //--- define behaviour

  // create new alpha_matrix with size of multipixel
  vector<vector<int>> alpha_matrix = alpha_value(size_of_multiPic);


  vector<string> files = vector<string>();                  //stores path to Ref-Images
  getdir(dir,files);
  cv::Mat image_l;
  for (unsigned int i = 0;i < files.size();i++) {                               //load images.
      if(((string(".")).compare(files[i]) != 0) && ((string("..")).compare(files[i]) != 0))
      {
        image_l = cv::imread(dir+"/"+files[i], 1 );
        if ( image_l.data ){
            cv::Mat image_d;
            cout << dir+files[i] << endl;
            image_l.convertTo(image_d, CV_64FC3);                                 //we want double images!!
            ref_images.push_back(pair<string,Mat>(files[i],image_d));
        }
      }
  }
  {
    cout<<"loading of reference images done!\n\n";}

  //SAMPLING //////////////////////////////////////////////////////////////////////////////
  /*
  for optimal usage of beamer and mems resolution we compute how many (micro/standard) pixels the beamer can display
  and the ammount of mems pixels( on which we can display superpixel)

  */

	int sample_amount = ref_images[0].second.cols *ref_images[0].second.rows * 0.5*0.5*0.5; // How many samples? !achtung grid images brauchen passenden wert!
	int ref_samples = ref_images[0].second.cols*ref_images[0].second.rows; //reference image

  for(vector<pair<string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image) //für alle ref IMAGES
    {
      cout << "---------------------------- new file ----------------------------------------- \n";

      string ref_image_name=(*ref_image).first;
      Mat ref_image_img =(*ref_image).second;
      int const mems_w = 500;
      int const mems_h = 320;
      int fixed_mems_amount = mems_w*mems_h;
      int const disp_pix_width = 1920;
      int const disp_pix_height = 1080;
  //    int const disp_pix_width = ref_image_img.cols-(ref_image_img.cols%mems_w);
  //    int const disp_pix_height = ref_image_img.rows-(ref_image_img.rows%mems_h);
      cout << " width " << ref_image_img.cols<<" height "<<ref_image_img.rows<<'\n';

      cout << "disp_pix_width = " << disp_pix_width<<'\n';
      cout << "disp_pix_height = " << disp_pix_height<<'\n';
      vector<vector<int> > coll_mat(disp_pix_height, vector<int>(disp_pix_width,0));
      cout << "Collisions Matrix created" << '\n';
      cout << "Ammount of Sampled Pixels is = " << fixed_mems_amount<<'\n';

      //cout << "Interface created" << '\n';

      map<pair<int, int>, Vec4b> _SampledPixels;
      vector<Point_d> _RandPixels;
      vector<double>randompixel_coords;


      Sampler sampler(fixed_mems_amount, ref_image_img); // Hier wird ein sampler erstellt!
      //  _SampledPixels = sampler.create_random_multipix_map();
      _RandPixels = sampler.sample_rand_points();
      cout<<"#Sampling done!\n";

      cout << "\n";
      cout << "portioning pixels \n";
    //vector<vector<Point_d>> portionen;
      Portioner portionierer(ref_image_img.cols, ref_image_img.rows, _RandPixels);
//      portionierer.make_Portion();
      portionierer.make_Portion_fast();
      cout << "# Portionierung done " <<"\n";
      cout << "\n";


      Mems mems_device(mems_w, mems_h);
      mems_device.fill_mems_with_mirrors(ref_image_img.cols, ref_image_img.rows);

//      mems_device.find_samples_next_to_mirror_fast(portionierer);

      string dataname = "mirrorswithsamples.dat";
      string input2 = "mirrorswithsamples_weiterverarbeitet.dat";
      //      mems_device.save_mirrors_with_samples(dataname);
      //      mems_device.read_mirrors_with_samples(dataname);
      //      mems_device.print_informations();
      //      mems_device.give_every_mirror_a_sample_slow();
      //      mems_device.save_mirrors_with_samples_2(input2);
      cout << "# Mirrrorsampling done " <<"\n";
      mems_device.read_mirrors_with_samples_2(input2);
      mems_device.compare_by_id();

      //    mems_device.print_informations();

      for(auto it = begin(mems_device._mems_mirrors); it!= end(mems_device._mems_mirrors);++it){
        it->_position.x = (it->_position.x-3)/2;
        it->_position.y = (it->_position.y-3)/2;
      }

      mems_device.create_multipix(size_of_multiPic);
      string rpf_name = "file1.rpf";

      //mems_device.write_rpf(rpf_name);
      //mems_device.print_informations();
      //  exit(1);
      //die verschiedenen verteilungen sind nun im vektor namens pattern verfügbar!

      //--------------------End of new Sampler --------------------------
      //Interpreter interpreter(mems_w,mems_h);
      //Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);

      //    Mat output(mems_h*3, mems_w*3, CV_64FC(6), Scalar(0,0,0,255));

      Mat output(mems_h*3, mems_w*3, CV_64FC(6));

      for(auto it = begin(mems_device._mems_mirrors_multi); it != end(mems_device._mems_mirrors_multi); ++it){
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


      fstream f;
      f.open(rpf_name, ios::out);
      f << "name of picture" << " " << "original-picture_width"  << " " << "original-picture_height"  << " "  << " has color depth of 32bit for r+b+g+alpha (4* 8bit)" <<"\n";
      f << "RPF has"  << " "  << "Samples of original picture"  << " "  << "Multipixels have size of " << " " <<"\n";
      for(int i = 0; i < output.rows; i++)
      {
          const Vec6d* Mi = output.ptr<Vec6d>(i);
          for(int j = 0; j < output.cols; j++){
        //  cout<<" i "<<i<<"  j "<<j<<"   DSy "<<Mi[j][5]<<"   DSx "<<Mi[j][4]<<" R "<<Mi[j][0]<<"   G "<<Mi[j][1]<<" B "<<Mi[j][2]<<"   A "<<Mi[j][3]<<"\n";
          f<<" i "<<i<<"  j "<<j<<" DSy "<<Mi[j][5]<<" DSx "<<Mi[j][4]<<" R "<<Mi[j][0]<<" G "<<Mi[j][1]<<" B "<<Mi[j][2]<<" A "<<Mi[j][3]<<"\n";
        }
      }
      f << "--- " << "\n";
      f.close();

      fstream f_bit;
      f_bit.open("rpf_bit", ios::out);

      // output2 stores data in a matrix of original picture_width & picture_height
      // sample pixels are stored at original positions
      // function for correct alpha value is not yet implemented as extra function in multipixels
      Mat output2(2000, 3000, CV_64FC4, Scalar(0,0,0,0));
      // print to test the alpha matrix
      for(int i = 0; i <  size_of_multiPic; ++i) {
        for(int j = 0; j < size_of_multiPic; ++j) {
          cout<< alpha_matrix[i][j]<< "      ";
        }
        cout<< endl;
      }

      for(auto it = begin(mems_device._mems_mirrors_multi); it != end(mems_device._mems_mirrors_multi); ++it) {

        // sampling position
        int samp_x = it->_displayed_sample.x;
        int samp_y = it->_displayed_sample.y;
        Point samp_pos = Point(samp_x, samp_y);

        output2.at<Vec4d>(samp_pos)[0] = ref_image_img.at<Vec3d>(samp_pos)[0];
        output2.at<Vec4d>(samp_pos)[1] = ref_image_img.at<Vec3d>(samp_pos)[1];
        output2.at<Vec4d>(samp_pos)[2] = ref_image_img.at<Vec3d>(samp_pos)[2];

        // cout<< "samp_x: "<< it->_displayed_sample.x << "      samp_y: "<< it->_displayed_sample.y << endl;
        // cout<< "pos_x: "<< it->_position.x << "      pos_y: "<< it->_position.y<< endl;

        // ...position in alpha matrix (position to center pixel of the multipixel)
        int pos_x = (int)it->_position.x % size_of_multiPic;
        int pos_y = (int)it->_position.y % size_of_multiPic;

        // compute the alpha value for the pixel
        output2.at<Vec4d>(samp_pos)[3] = alpha_matrix[pos_x][pos_y];

      }

      // output3 stores data in a compressed way in matrix
      // sample pixels are stored without space between
      // function for correct alpha value is not yet implemented as extra function in multipixels
      Mat output3(mems_h*3, mems_w*3, CV_64FC4, Scalar(0,0,0,255));
      for(auto it = begin(mems_device._mems_mirrors_multi); it != end(mems_device._mems_mirrors_multi); ++it){
        output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[0]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[0];
        output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[1]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[1];
        output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[2]= ref_image_img.at<Vec3d>(Point(it->_displayed_sample.x, it->_displayed_sample.y))[2];
        if((int)it->_position.x % 3 == 0||(int)it->_position.x % 3 == 2||(int)it->_position.y % 3 == 0||(int)it->_position.y % 3 == 2){
          output3.at<Vec6d>(Point(it->_position.x,it->_position.y))[3]= 122;
        }
      }


      string name="basic_samples"+to_string(sample_amount)+ref_image_name;
      imwrite("result_1"+name+".png",output3);
      imwrite("result_2"+name+".png",output2);

      cout<<"#Visualizing done!\n";
      cout<<"\n";
      cout<<"\n";
      cout<<"\n";
    }
}
