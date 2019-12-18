#ifndef MULTIPIX_HPP
#define MULTIPIX_HPP

#include <iosfwd>
#include <opencv2/opencv.hpp>
using namespace cv;

struct MultiPix {
  unsigned int  x1=0,
                y1=0,
                x2=0,
                y2=0;
  std::vector<Vec3d> colors{0,0,0};
  unsigned int  xc=0,
                yc=0;

};


#endif
