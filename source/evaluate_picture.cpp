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

/*
pipe-todo:
-store sample txt (maybe later)
-float sampling/interpolation?
(interpolation methods in vector(lambda..))
-bigger reference images!?
-bigger screen to evaluate via eye!
-screen has to fit outputresolution

integrate:
(-HEXA     //)
-cw-ssim  //external tool
(error-distribution-map)
(splatting  //artur)
-splatting combi

-always different sampling ... cannot compare diffetrent inteprolations with
the momentary setting, but this is good to compare patterns!

Evaluation Pipeline:
  1.IMAGES                                                               +
  Is there a dependency towards the image?

  2.SAMPLE AMOUNTS
  Is there a dependency towards the Sample Amount?
      ->sampleamount should be << reference IMAGES

  3.SAMPLE TECHNIQUES
  Grid vs. Hexa vs. Rand. vs Halton

  4.INTERPRETATION TECHNIQUES
  Which interpreation technique is best?
      -> Voronoi is the common way to interpret an image
      -> But Splatting and Proximity might give more

  5.QUALITY ASSESMENT
  eye(features, characteristics), naive error, ssim, cw-ssim)

GOAL 1:   "We want to see, wether a random sample-pattern is better than
          the common grid sampling!"
          ->Is this the case for all images, sample amounts,
            interpreation techniques and evaluation techniques?
          RESULTS:
            -GRID needs a fitting sample-amount! Random can take any!

GOAL 2:   "Which Interpretation Technique is best? Is there a dependency
          to a certain kind of image?"
          ->loop technique?!

*/

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

int main(int argc, char** argv )
{
  int splat_radius=1;
std::cout<<".........................................\n";
std::cout<<"Preperation_Start:.......................\n";

//REFERENCE IMAGES:.............................................................
  std::cout<<"loading reference images\n";
  std::vector<std::pair<std::string,Mat> > ref_images;                          //stores double-Referenze Images
  std::string dir = argv[1];                                                    //Inputfolder-path
  std::vector<std::string> files = std::vector<std::string>();                  //stores path to Ref-Images
  getdir(dir,files);
  cv::Mat image_l;
  for (unsigned int i = 0;i < files.size();i++) {                               //load images.
      if(((std::string(".")).compare(files[i]) != 0) && ((std::string("..")).compare(files[i]) != 0) )
      {
        image_l = cv::imread(dir+"/"+files[i], 1 );
        if ( image_l.data )
        {
            Mat image_d;
            std::cout << dir+files[i] << std::endl;
            image_l.convertTo(image_d, CV_64FC3);                                 //we want double images!!
            ref_images.push_back(std::pair<std::string,Mat>(files[i],image_d));
        }
      }else{
        //std::cout<<"exlude!?:"<<files[i]<<"\n";
      }
  }
  std::cout<<"loading of reference images done\n";
////////////////////////////////////////////////////////////////////////////////

//SAMPLE AMOUNTS:...............................................................
  std::cout<<"preparing sample amounts\n";
  std::vector<int> sample_amounts;
  int ref_samples=ref_images[0].second.cols*ref_images[0].second.rows;
  /*ACHTUNG, wähle für das GRID-Sampling fair!*/
  std::cout<<"ACHTUNG: Das GRID-Image braucht eine faire Sample-Anzahl!\n";     //3000*2000 =100%

  //sample_amounts.push_back(ref_samples*0.5*0.5);                                //1500* 1000  =25%
  sample_amounts.push_back(ref_samples*0.5*0.5*0.5*0.5);                        //750*  500   =6.25%
  //sample_amounts.push_back(ref_samples*0.5*0.5*0.5*0.5*0.5*0.5);                //375*  250   =1.5625%
  std::cout<<"preparing sample amounts done\n";
////////////////////////////////////////////////////////////////////////////////

//ANALYSIS.txt..:...............................................................
std::cout<<"preparing analysis.txt\n";
std::string analysis_doc="analysis.txt";
//std::fstream file0(analysis_doc.c_str(), std::ios::out );
//file0<<"EVALUATION\n";
//file0.close();
std::cout<<"preparing analysis.txt done\n";
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


std::cout<<".........................................\n";
std::cout<<"Calculation_Start:.......................\n";
int img_id=0;
//CALCULATION:
/*
  std::cout<<"DELAUNAY\n";
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {
      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.delaunay();
        std::string name=std::to_string(img_id)+"delaunay"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }

  std::cout<<"DELAUNAY_S_SPLATT\n";
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    if(ref_samples*0.5*0.5==*sample_amount)
    {
      splat_radius=1;
    }else if(ref_samples*0.5*0.5*0.5*0.5==*sample_amount)
    {
      splat_radius=2;
    }else{
      splat_radius=3;
    }
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {

      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.delaunay_splat(splat_radius);
        std::string name=std::to_string(img_id)+"delaunay_ssplat"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }
  */
  std::cout<<"VORONOI\n";   //most completly correct pixel
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {
      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
/*
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.voronoi();
        std::string name=std::to_string(img_id)+"voronoi"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
        */
      }
    }
  }
/*
  std::cout<<"VORONOI_SPLATT\n";   //most completly correct pixel
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    if(ref_samples*0.5*0.5==*sample_amount)
    {
      splat_radius=1;
    }else if(ref_samples*0.5*0.5*0.5*0.5==*sample_amount)
    {
      splat_radius=2;
    }else{
      splat_radius=3;
    }
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {

      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.splat_over(interpreter.voronoi(),splat_radius);
        std::string name=std::to_string(img_id)+"voronoi_splatt"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }

  std::cout<<"S_PROXIMITY2\n";  // "smallest total error"
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {
      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.shadow_proximity(2);
        std::string name=std::to_string(img_id)+"shadow_pr"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }

  std::cout<<"S_PROXIMITY2_SPLAT\n";  // "smallest total error"
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    if(ref_samples*0.5*0.5==*sample_amount)
    {
      splat_radius=1;
    }else if(ref_samples*0.5*0.5*0.5*0.5==*sample_amount)
    {
      splat_radius=2;
    }else{
      splat_radius=3;
    }
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {
      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.splat_over(interpreter.shadow_proximity(2),splat_radius);

        std::string name=std::to_string(img_id)+"shadow_pr_splat"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }

  std::cout<<"AREA&PROX0\n";   //"smallest amount of completly wrong+small total error"
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {
      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.area_and_proximity(0);
        std::string name=std::to_string(img_id)+"area_prox"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }

  std::cout<<"AREA&PROX0_SPLAT\n";   //"smallest amount of completly wrong+small total error"
  for(std::vector<int>::iterator sample_amount = sample_amounts.begin(); sample_amount != sample_amounts.end(); ++sample_amount)
  {
    std::cout<<"sample_amount: "<<*sample_amount<<" of "<<ref_samples<<", "<<((*sample_amount)/((float)ref_samples*100))<<"perc\n";
    if(ref_samples*0.5*0.5==*sample_amount)
    {
      splat_radius=1;
    }else if(ref_samples*0.5*0.5*0.5*0.5==*sample_amount)
    {
      splat_radius=2;
    }else{
      splat_radius=3;
    }
    for(std::vector<std::pair<std::string, Mat> >::iterator ref_image = ref_images.begin(); ref_image != ref_images.end(); ++ref_image)
    {
      std::string ref_image_name=(*ref_image).first;
      Mat ref_image_img         =(*ref_image).second;
      std::cout<<"ref_image: "+ref_image_name+"\n";

      Sampler sampler(*sample_amount,ref_image_img);
      std::cout<<"taking samples patterns: ";
      std::vector<std::pair<std::string,std::vector<Pixel_d> > > patterns;
      //0:GRID
        std::cout<<"GRID, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Grid",sampler.calc_grid()));
      //1:HEXA
        //patterns.push_back(sampler.calc_rand_d());
      //2:RAND
        std::cout<<"RAND, ";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Rand",sampler.calc_rand_d()));
      //4:HALT
        std::cout<<"HALT\n";
        patterns.push_back(std::pair<std::string,std::vector<Pixel_d> >("Halt",sampler.calc_halton()));

      Interpreter interpreter(ref_image_img.cols,ref_image_img.rows);
      Evaluator evaluator(ref_image_img);
      Mat output;
      Mat eval_out;
      for(std::vector<std::pair<std::string,std::vector<Pixel_d> > >::iterator pattern= patterns.begin(); pattern != patterns.end(); ++pattern)
      {
        std::fstream file(analysis_doc.c_str(), std::ios::app);
        file <<img_id<<": ";
        std::cout<<"IMAGE_ID: "<<img_id<<"\n";
        interpreter.set_pattern((*pattern).second);
        output = interpreter.splat_over(interpreter.area_and_proximity(0),splat_radius);
        std::string name=std::to_string(img_id)+"area_prox_splat"+std::to_string(*sample_amount)+ref_image_name+(*pattern).first;
        imwrite("result_"+name+".png",output);
        eval_out=evaluator.evaluate_abs(output,file);
        imwrite("eval_"+name+".png",eval_out);
        img_id++;
        file <<"\t||";
        file <<name;
        file <<"\n";
        file.close();
      }
    }
  }
  */
}
