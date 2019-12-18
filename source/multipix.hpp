#ifndef MULTIPIX_HPP
#define MULTIPIX_HPP

#include <iosfwd>
#include <opencv2/opencv.hpp>
#include <string>

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

struct Interface {
  std::array<std::array<MultiPix, 512>, 320> px;

};
/*
struct Collisonmap{
        int const rows = 20;
        int const collums = 30;
  std::vector<std::vector<int>,2000>cmap(std::vector<int>(3000, 0));

};
*/


#endif
