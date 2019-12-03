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

    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image) //für alle ref IMAGES
    {

      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;

      int pic_width = 512;
      int pic_height = 320;
      int fixed_mems_amount = pic_width*pic_height;


  //    Sampler sampler(fixed_mems_amount,ref_image_img, pic_width, pic_height); // Hier wird ein sampler erstellt!
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

	   //Hier wird eine Interpreter erstellt um die samples zu visualisieren:
	    std::cout<<"\n\n#Visualizing Image!\n";
//      Interpreter interpreter(pic_width,pic_height);
      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        interpreter.set_pattern((*pattern).second);
        output = interpreter.no_interpretation();
        std::string name="basic_samples"+std::to_string(sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
      }
		std::cout<<"#Visualizing done!\n";
    }
  }
