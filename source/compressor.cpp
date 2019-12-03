#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../pixel.hpp"
#include <math.h>             //ceil, sqrt etc




class Compressor{
  public:
    Compressor(Mat const& image, int out_x, int out_y):
    _Image(image),
    _Output_x(out_x),
    _Output_y(out_y){}


  private:
    Mat _Image;
    int _Output_x;
    int _Output_y;
};
