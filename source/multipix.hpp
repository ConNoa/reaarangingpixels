#ifndef MULTIPIX_HPP
#define MULTIPIX_HPP

#include <iosfwd>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;

struct MultiPix {
   int  x1=0;
   int  y1=0;
   int  x2=0;
   int  y2=0;
  std::vector<Vec3d> colors{0,0,0};
  unsigned int  xc=0;
  unsigned int  yc=0;

};

#endif
