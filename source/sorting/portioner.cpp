#include <vector>
#include "../point.hpp"

class Portioner{

  public:

    Portioner(int width, int height,  std::vector<Point_d> const& random_points);

    std::vector<std::vector<Point_d>> _point_buckets;

    void make_Portion();
    std::vector<std::vector<Point_d>> get_bucket_cluster(Point_d const& point_in_bucket);

  private:
    int _width;
    int _height;
    int _X_Bucketsize;
    int _Y_Bucketsize;
    std::vector<Point_d> _random_points;

};

    Portioner::Portioner(int width, int height,  std::vector<Point_d> const& random_points)
    :_width(width),
    _height(height),
    _random_points(random_points),
    _X_Bucketsize(150),
    _Y_Bucketsize(100)
    {
      std::cout << "_width is now: " <<_width<< "  . " <<"\n";
      std::cout << "_height is now: " <<_height<< "  . " <<"\n";
}



void Portioner::make_Portion()
{
  int bucketcounter = 0;
  int minbucketsize = 99999;
  int maxbucketsize = 0;
//  std::vector<std::vector<Point_d>>Point_bucket;

  int ybmin = 0;
  int ybmax = _Y_Bucketsize;
  //std::cout << "ybmin is now: " <<ybmin<< "  . " <<"\n";
  //std::cout << "ybmax is now" <<ybmax<< "  . " <<"\n";


  while(ybmax <= _height)
    {
      //std::cout << "outer while loop starts: ----------------------" <<"\n";
      int xbmin = 0;
      int xbmax = _X_Bucketsize;
      //std::cout << "xbmin is now: " <<xbmin<< "  . " <<"\n";
      //std::cout << "xbmax is now" <<xbmax<< "  . " <<"\n";
      while(xbmax <=_width)
      {
        //std::cout << "inner while loop:    %%%%%%%%%     " <<"\n";
        std::vector<Point_d>bucket;
        for(auto it = std::begin(_random_points); it!=std::end(_random_points); ++it){
          if(it->x < xbmax && it->x >= xbmin){
            if(it->y < ybmax && it->y >= ybmin){
              //noch ganz schön langsam da random_points nicht kleiner wird
              bucket.push_back(*it);
            }
          }
        }
        //std::cout << "Bucket ist mit  " <<bucket.size()<< " gefüllt. " <<"\n";
        if(minbucketsize > bucket.size())minbucketsize = bucket.size();
        if(maxbucketsize < bucket.size())maxbucketsize = bucket.size();

        _point_buckets.push_back(bucket);
        bucketcounter += 1;

        xbmin = xbmin+_X_Bucketsize;
        xbmax = xbmax+_X_Bucketsize;
        //std::cout << "xbmin is now: " <<xbmin<< "  . " <<"\n";
        //std::cout << "xbmax is now" <<xbmax<< "  . " <<"\n";

      }
      //std::cout << "left inner while loop:    -->>-<-<-<-<-<-<-<-<-<-<-<-     " <<"\n";

      xbmin = 0;
      xbmax = 0 + _X_Bucketsize;
      //std::cout << "xbmin is now: " <<xbmin<< "  . " <<"\n";
      //std::cout << "xbmax is now" <<xbmax<< "  . " <<"\n";
      ybmin = ybmin + _Y_Bucketsize;
      ybmax = ybmax + _Y_Bucketsize;
      //std::cout << "ybmin is now: " <<ybmin<< "  . " <<"\n";
      //std::cout << "ybmax is now" <<ybmax<< "  . " <<"\n";

    }
    std::cout << "es gibt " <<bucketcounter<< " Buckets. " <<"\n";
    std::cout << "Bucketmaxsize ist  " << maxbucketsize  <<"\n";
    std::cout << "Bucketminsize ist  " << minbucketsize  <<"\n";


    //return _point_buckets;
    return;
}

std::vector<std::vector<Point_d>>Portioner::get_bucket_cluster(Point_d const& point_in_bucket)
{
  std::vector<std::vector<Point_d>>point_cluster;
  double act_x = point_in_bucket.x;
  double act_y = point_in_bucket.y;

  for(auto it = std::begin(_point_buckets); it != std::end(_point_buckets); ++it){
    if( (it->begin()->x >= act_x-_X_Bucketsize*2 && it->begin()->x <=act_x+_X_Bucketsize*2)
      &&(it->begin()->y >= act_y-_Y_Bucketsize*2 && it->begin()->y <=act_y+_Y_Bucketsize*2)){
        point_cluster.push_back(*it);
      }
  }
  return point_cluster;
}
