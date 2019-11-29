


//////////////////////////////////////////////////////////////
/*
info:
  random_sampling_storing.cpp simply resamples a given input picture with a wished
  number of samples.  each sample will be taken random. Using our rsf_writer, we
  store the pixels(x,y;r,g,b) in a .txt like file:

  example use: ./rand_sample_store "./spuele.jpg" 20
  -> output is ./output.txt

todo:
  |necessary:
  -...?
  |optional:
  -tag-like parameter reading?
  -we might want to transmit more meta data to the rsf_writer?
  -a desired outputfilename could be integrated
  -a better calculation for the running time..
  -..?
*/
#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <time.h>             //time measuring & seed
#include <string.h>
#include "rsf_writer.hpp"
#include "pixel.hpp"
using namespace cv;

void usage(){
  std::cout<<"----------------------------------------------------------------\n";
  std::cout<<"usage: ./rand_sample_store ";
  std::cout<<"\"./picture_filepath\" ";
  std::cout<<"samples_amount ";
  //std::cout<<"\"./outputfilepath\"\n\n";
  std::cout<<"\n\n";

  std::cout<<"(required)  String:   ./picture_filepath\n";
  std::cout<<"(required)  int:      samples_amount\n\n";
  //std::cout<<"(optional)  String:   ./outputfilepath\n";
  std::cout<<"outpufile:  output.txt";
  std::cout<<"----------------------------------------------------------------\n";
}

void mat_to_rand_txt(Mat& image, int samples_amount,  std::string output_name)
{
  srand(time(NULL));  //SEED
  int SIZEX=image.cols;
  int SIZEY=image.rows;

  Pixel pix;
  Vec3b color;
  RsfWriter writer_rand(output_name, SIZEX, SIZEY);    //will be wrting our pixel

  //take samples:
  std::cout<<"now sampling..";
  const clock_t begin_time = clock();
  for (int i=0; i<samples_amount; i++)
  {
    pix.x= rand() % SIZEX;
    pix.y= rand() % SIZEY;
    pix.color = image.at<Vec3b>(Point(pix.x,pix.y));
    if(!writer_rand.exists(pix))  //no doubles please!
    {
      writer_rand.add(pix);
    }else{
      i--;
    }
  }

  std::cout<<"done\nnow storing..";
  writer_rand.save();
  std::cout <<"done\ntook:"<<(float( clock () - begin_time )/  CLOCKS_PER_SEC)<<"seconds in total\n";

}

int main(int argc, char** argv )
{
//CHECK_INPUT:
    //CHECK LENGTH
    if ( argc != 3 )//|| argc != 4)
    {
        usage();
        return -1;
    }

    //  READ IMAGE_INPUT:
    Mat image;

		std::string input= argv[1];
    image = imread( input, 1 );
    if ( !image.data )
    {
        std::cout<<"found no image data \n";
        usage();
        return -1;
    }

    //  READ SAMPLES-AMOUNT:
		char *endptr;
		int samples_amount= strtol(argv[2], &endptr, 10);
		if (*endptr != '\0') {
			std::cout<<"second parameter was not an integer\n";
      usage();
			return -1;
		}

    std::string output_name="output_name.txt";
    mat_to_rand_txt(image, samples_amount, output_name);
//MAIN:
		return 0;
}
