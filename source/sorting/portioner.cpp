#include <vector>
#include "../point.hpp"

class Portioner{

  public:
    int _width;
    int _height;
    int _X_Bucketsize;
    int _Y_Bucketsize;
    std::vector<Point_d> _random_points;

    Portioner(int width, int height,  std::vector<Point_d> const& random_points);

    std::vector<std::vector<Point_d>> make_Portion();

  private:
};

    Portioner::Portioner(int width, int height,  std::vector<Point_d> const& random_points)
    :_width(width),
    _height(height),
    _random_points(random_points),
    _X_Bucketsize(100),
    _Y_Bucketsize(50)
    {}



std::vector<std::vector<Point_d>> Portioner::make_Portion()
{
  std::vector<std::vector<Point_d>>Point_bucket;

  int ybmin = 0;
  int ybmax = _Y_Bucketsize;

  while(ybmax < _height+_Y_Bucketsize)
    {
      int xbmin = 0;
      int xbmax = _X_Bucketsize;
      while(xbmax <_width+_X_Bucketsize)
      {
        std::vector<Point_d>bucket;
        for(auto it = std::begin(_random_points); it!=std::end(_random_points); ++it){
          if(it->x < xbmax && it->x >= xbmin){
            if(it->y < ybmax && it->y >= ybmin){
              //noch ganz schön langsam da random_points nicht kleiner wird
              bucket.push_back(*it);
            }
          }
        }
        Point_bucket.push_back(bucket);
        xbmin = xbmin+_X_Bucketsize;
        xbmax = xbmax+_X_Bucketsize;
      }
      xbmin = 0;
      xbmax = 0 + _X_Bucketsize;
      ybmin = ybmin + _Y_Bucketsize;
      ybmax = ybmax + _Y_Bucketsize;
    }
    return Point_bucket;
}
