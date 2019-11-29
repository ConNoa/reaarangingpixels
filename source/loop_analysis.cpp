#include "sampling/sample.cpp"
#include "interpretation/interpret.cpp"
#include "evaluation/evaluate.cpp"
#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <string>
#include "pixel.hpp"

using namespace cv;

int main(int argc, char** argv )
{
  /*
  Kann das mehrmalige anwenden der interpretation auf dem schrumpfenden datensatz
  auskunft über die qualität der interpretation geben?
  */
  //SAMPLING
  int samples_amount=/*2048000;/*/1024000; //25%

  Mat image;

  std::string input= argv[1];
  Mat input_image;
  input_image=imread( input,1);
  std::size_t pos = input.find(".");      // position of "live" in str
  std::string name = std::string(input.substr (0,pos));
  std::cout<<"ref_image: "<<name<<"\n";
  std::cout<<"samples: "<<samples_amount<<"\n";
  name+=samples_amount;
  if ( !input_image.data )
  {
      std::cout<<"found no image data \n";
      return -1;
  }

//01. SAMPLING
  std::cout<<"\n01.°°°°°°°°°°°°°°°°°°°°°°°°SAMPLING\n";
  Sampler sam_vor(samples_amount, input_image);
  Sampler sam_pro(samples_amount, input_image);
  Sampler sam_sha(samples_amount, input_image);
  //...
  /*
  Size frameSize(static_cast<int>(input_image.cols), static_cast<int>(input_image.rows));
  VideoWriter out_vor ("./output_vor.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
  VideoWriter out_pro ("./output_pro.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true);
  out_vor.write(input_image);
  out_pro.write(input_image);
  */
  Interpreter interpreter(input_image.cols,input_image.rows);


  Mat vor;
  Mat pro;
  Mat sha;
  input_image.convertTo(vor, CV_64FC3);
  input_image.convertTo(pro, CV_64FC3);
  input_image.convertTo(sha, CV_64FC3);
  imwrite("testasas.jpg",vor);
  std::vector<Pixel_d> pat;
  for(int i=0; i<100; i++)
  {
    std::cout<<i<<"run\n";
    std::string outstr;



    std::cout<<"Voronoi----\n";


    sam_vor.set_image(vor);
    pat=sam_vor.calc_rand_d();
    interpreter.set_pattern(pat);
    vor=interpreter.shadow_proximity(2); //smallest total error
    //out_vor.write(vor);
    outstr=std::string("sha_prox_2")+std::to_string(i)+".jpg";
    imwrite(outstr,vor);


    //imwrite("a"+std::string(methods[i])+std::string("eval_3d_")+".jpg",evaluator.evaluate_abs(output_images[i]));
    std::cout<<"Proximity----\n";

    sam_pro.set_image(pro);       //set img
    pat=sam_pro.calc_rand_d();      //calc pat
    interpreter.set_pattern(pat); //set pat
    pro=interpreter.area_only_proximity(0);  //area only
    //out_pro.write(pro);
    outstr=std::string("area_only")+std::to_string(i)+".jpg";
    imwrite(outstr,pro);


    std::cout<<"shadow----\n";

    sam_sha.set_image(sha);       //set img
    pat=sam_sha.calc_rand_d();      //calc pat
    interpreter.set_pattern(pat); //set pat
    sha=interpreter.area_and_proximity(0); //most less completly wron pixel
    //out_pro.write(pro);
    outstr=std::string("area_prox_0")+std::to_string(i)+".jpg";
    imwrite(outstr,sha);
  }

  std::cout<<"done\n";

}
