#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <iosfwd>
#include <opencv2/opencv.hpp>
using namespace cv;

class Pixel
{
public :

  Pixel(unsigned int = 0, unsigned int = 0);
  void print(std::ostream&) const;

  unsigned int x;
  unsigned int y;
  Vec3b color;
};

std::ostream& operator<<(std::ostream&, const Pixel&);


class Pixel_d
{
public :

  Pixel_d(double = 0, double = 0);
  void print(std::ostream&) const;

  double x;
  double y;
  Vec3d color;
  long id;
};

std::ostream& operator<<(std::ostream&, const Pixel_d&);

#endif
