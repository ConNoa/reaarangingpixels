/*
info:
  the rsf writer writes given pixel, x-,y-coordinat and color(r,g,b) to a .txt
  like file.

todo:
  |necessary:
  -...?
  |optional:
  -further header information?
  -better data reservation/calculation for vector..
  -later we could code the information cheaper
*/
#include "rsf_writer.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>


RsfWriter::RsfWriter(std::string const& file, int x, int y):
  file_(file),
  x_(x),
  y_(y),
  samples_(0),
  data_()
  {clean();}

void RsfWriter::add(Pixel const& p)
{
  samples_++;
  data_.push_back(p);
}

bool RsfWriter::exists(Pixel const& p)
{
  for ( std::vector<Pixel>::iterator i = data_.begin(); i != data_.end(); i++ )
  {
    if((i->x==p.x) &&(i->y==p.y))
    {
      return true;
    }
  }
  return false;
}

void RsfWriter::save()
{
  std::fstream file(file_.c_str(), std::ios::out);
  //header:?
  //file<<"samples: "<< samples_<<"\n";

  //data:
  file << x_<<" "<<y_<<"\n";
  for ( std::vector<Pixel>::iterator p = data_.begin(); p != data_.end(); p++ ) {
    file << p->x << " ";
    file << p->y << " ";
    file << (int)p->color[0] << " ";
    file << (int)p->color[1] << " ";
    file << (int)p->color[2] << "\n";
  }
  file.close();
}

void RsfWriter::clean()
{
  std::fstream file(file_.c_str(), std::ios::out);
  file.clear();
  file.close();
}
