#include "pixel.hpp"
#include <ostream>

Pixel::Pixel(unsigned int a, unsigned int b)
  : x(a),
    y(b),
    color()
{}

void Pixel::print(std::ostream& os) const
{
  os << "Pixel[" << (int)x << ',' << (int)y << "]("
     << color[0] << ','
     << color[1] << ','
     << color[2] << ')';
}

std::ostream& operator<<(std::ostream& os, const Pixel& a)
{
  std::ostream::sentry cerberus (os);
  if (cerberus)
    a.print(os);
  return os;
}

//////////////////////////
Pixel_d::Pixel_d(double a, double b)
  : x(a),
    y(b),
    color()
{}

void Pixel_d::print(std::ostream& os) const
{
  os << "Pixel[" << (double)x << ',' << (double)y << "]("
     << color[0] << ','
     << color[1] << ','
     << color[2] << ')';
}

std::ostream& operator<<(std::ostream& os, const Pixel_d& a)
{
  std::ostream::sentry cerberus (os);
  if (cerberus)
    a.print(os);
  return os;
}
