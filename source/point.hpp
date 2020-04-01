#ifndef POINT_HPP
#define POINT_HPP

#include <iosfwd>
#include <opencv2/opencv.hpp>
using namespace cv;


class Point_d
{
public :

Point_d (double = 999999, double = 99999, double = 99999, int = 9999999);
  void print(std::ostream&) const;

  double x;
  double y;
  double dis;
  int id;
};



std::ostream& operator<<(std::ostream&, const Point_d&);

#endif
