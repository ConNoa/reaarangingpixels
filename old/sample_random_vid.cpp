
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
#include <string>
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

  std::cout<<"(required)  String:   ./video_filepath\n";
  std::cout<<"(required)  int:      samples_amount\n\n";
  //std::cout<<"(optional)  String:   ./outputfilepath\n";
  std::cout<<"outpufiles:  output_0.txt,...,output_n.txt";
  std::cout<<"----------------------------------------------------------------\n";
}

void mat_to_rand_txt(Mat& image, int samples_amount,  std::string const& output_name)
{

  int SIZEX=image.cols;
  int SIZEY=image.rows;

  Pixel pix;
  Vec3b color;
  //std::string file=output_name;
  RsfWriter writer_rand(output_name, SIZEX, SIZEY);    //will be wrting our pixel

  //take samples:
  std::cout<<"now sampling..";
  const clock_t begin_time = clock();
  //std::list<std::pair<int, int> > not_sampled_yet;
  std::vector<std::pair<int, int> > not_sampled_yet;
  for(int x=0; x<SIZEX; x++)
  {
    for(int y=0; y<SIZEY; y++)
    {
      not_sampled_yet.push_back(std::pair<int,int>(x,y));
    }
  }

  for (int i=0; i<samples_amount; i++)
  {
    
    int n= rand()% not_sampled_yet.size();
    //std::cout<<n<<"\n";
    //std::list<std::pair<int,int> >::iterator it = not_sampled_yet.begin();
    /*
    std::advance(it, n);
    pix.x= it->first;
    pix.y= it->second;
    */
    pix.x= not_sampled_yet[n].first;
    pix.y= not_sampled_yet[n].second;
    pix.color = image.at<Vec3b>(Point(pix.x,pix.y));
    writer_rand.add(pix);
    //https://stackoverflow.com/questions/3487717/erasing-multiple-objects-from-a-stdvector
    //i am not sure, wether this is quite allright, but it is fast!:
    not_sampled_yet[n]=not_sampled_yet.back();
    not_sampled_yet.pop_back();
  //  not_sampled_yet.erase(not_sampled_yet.begin()+n);
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

    //  READ SAMPLES-AMOUNT:
		char *endptr;
		int samples_amount= strtol(argv[2], &endptr, 10);
		if (*endptr != '\0') {
			std::cout<<"second parameter was not an integer\n";
      usage();
			return -1;
		}

    //VIDEO:
    VideoCapture capture;
    int totalFrameNumber;
    std::cout<<"video\n";
    capture=VideoCapture(argv[1]);
    totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
    srand(time(NULL));  //SEED
    cv::Mat currentFrame;

    for (unsigned int i = 0;i <totalFrameNumber; i++) {
      capture>>currentFrame;
      std::cout<<"frame: "<<i<<" of "<<totalFrameNumber<<"\n";
      if(!currentFrame.empty()){
        //i need to fix my stringlibrary...
        std::stringstream number;
        number<<i;
        std::string num;
        number>>num;
        //stringlibrary..
        std::string output_name=std::string("output ")+ num+" .txt";

        mat_to_rand_txt(currentFrame, samples_amount, output_name);
      }
    }
	  return 0;
}
