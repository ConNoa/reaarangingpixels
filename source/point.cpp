#include "point.hpp"
#include <ostream>



Point_d::Point_d(double a, double b, double distance)
  : x(a),
    y(b),
    dis(distance)
{}

Point_d::Point_d(double a, double b, double distance, int ID)
  : x(a),
    y(b),
    dis(distance),
    id(ID)
{}


void Point_d::print(std::ostream& os) const
{
  os << "Point: " << (double)x << ',' << (double)y ;
}

std::ostream& operator<<(std::ostream& os, const Point_d& a)
{
  std::ostream::sentry cerberus (os);
  if (cerberus)
    a.print(os);
  return os;
}
