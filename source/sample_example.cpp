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
      std::cout << "disp_pix_width = " << disp_pix_width<<'\n';
      std::cout << "disp_pix_height = " << disp_pix_height<<'\n';
      std::vector<std::vector<int> > coll_mat(disp_pix_height, std::vector<int>(disp_pix_width,0));
      std::cout << "Collisions Matrix created" << '\n';
      std::cout << "Ammount of Sampled Pixels is = " << fixed_mems_amount<<'\n';

      //std::cout << "Interface created" << '\n';

      std::map<std::pair<int, int>, Vec4b> _SampledPixels;
      std::vector<Point_d> _RandPixels;
      std::vector<double>randompixel_coords;



      /* -----------Old Sampler for Single Pixels--------------
            //    Sampler sampler(fixed_mems_amount,ref_image_img, mems_w, mems_h); // Hier wird ein sampler erstellt!
            Sampler sampler(sample_amount, ref_image_img); // Hier wird ein sampler erstellt!

            std::cout<<"\n\n#Sampling reference image ("+ref_image_name+") with "<<sample_amount<<" samples ("<<((sample_amount*100)/((float)ref_samples))<<" percent of reference image pixels).\n";
            std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns; //speichert die verschiedenen samples!
            //0:GRID
            //  patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
            //1:HEXA
            //patterns.push_back(sampler.calc_rand_d());
            //2:RAND
            patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
            //  patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d_compressed()));

            //4:HALT
            //patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton_compressed()));
            patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      		  //die verschiedenen verteilungen sind nun im vektor namens pattern verfügbar!
      		  std::cout<<"#Sampling done!\n";
      -------------------------------------------------------------*/


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
      std::vector<std::vector<Point_d>> portionen;
      Portioner portionierer(ref_image_img.cols, ref_image_img.rows, _RandPixels);
      portionierer.make_Portion();
      std::cout << "# Portionierung done " <<"\n";
      std::cout << "\n";

      Mems mems_device(mems_w, mems_h);
      mems_device.fill_mems_with_mirrors(ref_image_img.cols, ref_image_img.rows);



/*
      randompixel_coords = sampler.sample_rand_coords();
      std::cout<<"# Returned rand cords\n";

      delaunator::Delaunator d(randompixel_coords);
      std::cout<<"# Delaunay finished\n";
      for(std::size_t i = 0; i < d.triangles.size(); i+=3) {
    printf(
        "Triangle points: [[%f, %f], [%f, %f], [%f, %f]]\n",
        d.coords[2 * d.triangles[i]],        //tx0
        d.coords[1 * d.triangles[i]],        //tx0
        d.coords[2 * d.triangles[i] + 1],    //ty0
        d.coords[2 * d.triangles[i + 1]],    //tx1
        d.coords[2 * d.triangles[i + 1] + 1],//ty1
        d.coords[2 * d.triangles[i + 2]],    //tx2
        d.coords[2 * d.triangles[i + 2] + 1] //ty2
    );
}
*/


      //compute triangle arround all points
/*
      std::sort(_RandPixels.begin(), _RandPixels.end(), compareBy_yValue);
      std::cout<<"Pixelvec sorted by y val\n";
      double ymin = _RandPixels.begin()->y;
      std::cout<<"ymin = "<<ymin<<"\n";
      double ymax = _RandPixels.back().y;
      double ycenter = (ymax-ymin)/2;

      std::cout<<"ymax = "<<ymax<<"\n";

      std::sort(_RandPixels.begin(), _RandPixels.end(), compareBy_xValue);
      std::cout<<"Pixelvec sorted by x val\n";
      double xmin = _RandPixels.begin()->x;
      std::cout<<"xmin = "<<xmin<<"\n";
      double xmax = _RandPixels.back().x;
      std::cout<<"xmax = "<<xmax<<"\n";
      double xcenter = (xmax-xmin)/2;
      //1.05 for a bit bigger triangle

      double inner_radius = 1.05*sqrt(pow(ycenter, 2)+ pow(xcenter,2));
      std::cout<<"ir = "<<inner_radius<<"\n";

      double hyp= sin(90*PI/180)*inner_radius/sin (30*PI/180);
      std::cout<<"hyp = "<<hyp<<"\n";

      double b = sin(60*PI/180)*hyp/sin (90*PI/180);

      Point_d P_c;
      Point_d P_1(P_c.x, P_c.y-hyp);
      Point_d P_2(P_c.x-b, P_c.y+inner_radius);
      Point_d P_3(P_c.x+b, P_c.y+inner_radius*1.1);

*/


      //die verschiedenen verteilungen sind nun im vektor namens pattern verfügbar!

      //--------------------End of new Sampler --------------------------
      //      Interpreter interpreter(mems_w,mems_h);
      //      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);




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
      std::cout<<"#Visualizing done!\n";
      std::cout<<"\n";
      std::cout<<"\n";
      std::cout<<"\n";
    }
}
