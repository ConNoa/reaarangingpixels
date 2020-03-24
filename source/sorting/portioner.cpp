#include <vector>
#include "../point.hpp"

class Portioner{

  public:

    Portioner(int width, int height,  std::vector<Point_d> const& random_points);

    std::vector<std::vector<Point_d>> _point_buckets;
    std::vector<std::vector<std::vector<Point_d>>> _point_buckets_fast;
  //---DEBUG---- void Mems::find_samples_next_to_mirror_fast----  int max_bucket_collumn;

    void make_Portion();
    void make_Portion_fast();

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
    _X_Bucketsize(50),
    _Y_Bucketsize(50)
    {
      std::cout << "_width is now: " <<_width<< "  . " <<"\n";
      std::cout << "_height is now: " <<_height<< "  . " <<"\n";
      int bucket_ammount = int(_width*height)/(_X_Bucketsize*_Y_Bucketsize);
      std::cout<<"bucket ammount = "<< bucket_ammount<<"\n";
      std::vector<std::vector<Point_d>> bucket_vec_y(_height/_Y_Bucketsize);
      std::vector<std::vector<std::vector<Point_d>>> bucket_matrix(_width/_X_Bucketsize, bucket_vec_y);
      _point_buckets_fast = bucket_matrix;
      //max_bucket_collumn = 0;

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


void Portioner::make_Portion_fast()
{
  std::cout<<"make Portions fast "<<"\n";

  for(auto it = std::begin(_random_points); it!=std::end(_random_points); ++it){
    long minusvalue = (int)it->x%_X_Bucketsize;
    int bucket_collumn = (int)(it->x-minusvalue)/_X_Bucketsize;
    //std::cout<<"bucket column "<< bucket_collumn<<"\n";

    minusvalue = (int)it->y%_Y_Bucketsize;
    int bucket_row = (int)(it->y-minusvalue)/_Y_Bucketsize;
    //std::cout<<"bucket row "<< bucket_row<<"\n";

    _point_buckets_fast[bucket_collumn][bucket_row].push_back(*it);


  }
  std::cout<<"made Portions fast "<<"\n";

  return;
}

std::vector<std::vector<Point_d>>Portioner::get_bucket_cluster(Point_d const& point_in_bucket)
{
  std::vector<std::vector<Point_d>>point_cluster;

  long minusvalue = ((int)point_in_bucket.x%_X_Bucketsize);
  int bucket_collumn = (int)(point_in_bucket.x-minusvalue)/_X_Bucketsize;
/*
DEBUG
  std::cout<<"bucket column "<< bucket_collumn<<"\n";
  if (bucket_collumn > max_bucket_collumn){
    max_bucket_collumn = bucket_collumn;
  }
*/
  minusvalue = (int)point_in_bucket.y%_Y_Bucketsize;
  int bucket_row = (int)(point_in_bucket.y-minusvalue)/_Y_Bucketsize;

  bool  top = false;
  bool  bottom = false;
  bool  left = false;
  bool  right = false;

  int from_x, till_x, from_y, till_y;


  if(bucket_collumn == 0) {
    left = true;
    from_x = bucket_collumn;
  }
  else{
    from_x = bucket_collumn-1;
  }
  if(bucket_collumn == (_width/_X_Bucketsize-1)) {
    right = true;
    till_x = bucket_collumn;
  }
  else{
    till_x = bucket_collumn+1;
  }
  if(bucket_row == 0) {
    top = true;
    from_y = bucket_row;
  }
  else{
    from_y = bucket_row-1;
  }
  if(bucket_row == (_height/_Y_Bucketsize-1)) {
    bottom = true;
    till_y = bucket_row;

  }
  else{
    till_y = bucket_row+1;
  }
  //if point is in a bucket which is not on border of buckets
  if(top==false && bottom==false && left==false && right==false){
      for(int i = bucket_collumn-1; i<=bucket_collumn+1; i++){
        for(int j = bucket_row-1; j<= bucket_row+1; j++){
          point_cluster.push_back(_point_buckets_fast[i][j]);
        }
      }
    }

  else{ for(int i = from_x; i<=till_x; i++){
          for(int j = from_y; j<= till_y; j++){
            point_cluster.push_back(_point_buckets_fast[i][j]);
          }
        }
      }

  return point_cluster;
}
