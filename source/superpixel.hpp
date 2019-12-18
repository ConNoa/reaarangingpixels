#ifndef SUPERPIXEL_HPP
#define SUPERPIXEL_HPP

#include "pixel.hpp"
#include <array>

class Superpixel_3
{
public:
  Superpixel_3();

  //std::vector<Pixel_d> pixelpart;
  std::array <Pixel_d,9> pixelarray;
};

#endif
