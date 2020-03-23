#include <vector>
#include "point.hpp"
#include "mirror.cpp"
//#include "sorting/portioner.cpp"

class Mems{

  public:
    Mems(int width, int height);

    std::vector<Mirror> _mems_mirrors;
    int _width;
    int _height;

    void fill_mems_with_mirrors(int picture_width, int picture_height);
    void find_samples_next_to_mirror(std::vector<Point_d> const& sampled_points);
    void find_samples_next_to_mirror_fast(Portioner buckets);
//    bool compare_by_ammount_of_matching_samples(const Mirror &a, const Mirror &b);
    void give_every_mirror_a_sample();

  private:



};

  Mems::Mems(int width, int height)
  :_width(width),
  _height(height)
  {
    std::cout << "Mems created with width  " <<_width<<" and height "<<_height<< "  . " <<"\n";
    std::cout <<"\n";

  }

void Mems::fill_mems_with_mirrors(int picture_width, int picture_height){
  //pixelammounts for every mirror
  int counter = 0;
  double pixel_ammount_width = (double)(picture_width/_width);
  double pixel_ammount_height = (double)(picture_height/_height);
  for(int j = 0; j < _height; j++){
    for(int i = 0; i < _width; i++){
      Mirror act_mirror;
      act_mirror.id = counter;
      //std::cout << "counter is now: " <<counter<< "  . " <<"\n";
      act_mirror._position.x =  pixel_ammount_width/2+i*pixel_ammount_width;
      act_mirror._position.y =  pixel_ammount_height/2+j*pixel_ammount_height;
      //std::cout << "x: " <<act_mirror._position.x<< "  . " <<"\n";
      //std::cout << "y: " <<act_mirror._position.y<< "  . " <<"\n";
      _mems_mirrors.push_back(act_mirror);
      counter += 1;
    }
  }
}

void Mems::find_samples_next_to_mirror(std::vector<Point_d> const& sampled_points){
  int minfittingsamples = 999999;
  int x_y_distance = 7;
  int not_matching_mirror_ammount = 0;

  for(auto it = std::begin(_mems_mirrors); it!=std::end(_mems_mirrors); ++it){

    if(it->id % 1000 == 0)std::cout << "Find samples for Mirror  " <<it->id <<"\n";
    for(auto act_p = std::begin(sampled_points); act_p!=std::end(sampled_points); ++act_p)
    {
      if(((it->_position.x-act_p->x)*(it->_position.x-act_p->x)+(it->_position.y-act_p->y)*(it->_position.y-act_p->y))<=(x_y_distance*x_y_distance*2))
      {
        it->_matching_samples.push_back(*act_p);
      }
      if(minfittingsamples > it->_matching_samples.size())
      {
        minfittingsamples = it->_matching_samples.size();
        std::cout << "Minimal fitting samples sind jetzt " <<minfittingsamples<<"\n";
      }
      if(minfittingsamples == 0 && it->_matching_samples.size()==0)
      {
        not_matching_mirror_ammount += 1;
        std::cout << "not matching mirrorammount =  " <<not_matching_mirror_ammount<<"\n";

      }
    }
  }
}

void Mems::find_samples_next_to_mirror_fast(Portioner buckets){
  int minfittingsamples = 999999;
  int x_y_distance = 40;
  int not_matching_mirror_ammount = 0;

  //iterate selected_mirror all buckets
  for(auto selected_mirror = std::begin(_mems_mirrors); selected_mirror!=std::end(_mems_mirrors); ++selected_mirror){
    //fill vec with all relevant samplepoints with samples from mirrorbucket and buckets arround
    std::vector<std::vector<Point_d>>point_cluster = buckets.get_bucket_cluster(selected_mirror->_position);

        for(auto cluster_part = std::begin(point_cluster); cluster_part!=std::end(point_cluster); ++cluster_part)
        {
          for(auto point_to_check = std::begin(*cluster_part); point_to_check != std::end(*cluster_part); ++point_to_check)
          {
            double sample_dis = pow(selected_mirror->_position.x-point_to_check->x , 2)+pow(selected_mirror->_position.y-point_to_check->y , 2);

            if(sample_dis<=x_y_distance*x_y_distance*2)
            {
              point_to_check->dis = sample_dis;
              selected_mirror->_matching_samples.push_back(*point_to_check);
            }
          }
        }
        if(selected_mirror->id % 1000 == 0)std::cout << "Find samples for Mirror  " <<selected_mirror->id <<"\n";
        if(minfittingsamples > selected_mirror->_matching_samples.size())
        {
          minfittingsamples = selected_mirror->_matching_samples.size();
          std::cout << "Minimal fitting samples sind jetzt " <<minfittingsamples<<"\n";
        }
        if(minfittingsamples == 0 && selected_mirror->_matching_samples.size()==0)
        {
          not_matching_mirror_ammount += 1;
          std::cout << "not matching mirrorammount =  " <<not_matching_mirror_ammount<<"\n";
        }
      }
      std::cout << "bucket colloumn max = "<< buckets.max_bucket_collumn<< '\n';
  }

  bool compare_by_ammount_of_matching_samples(const Mirror &a, const Mirror &b)
    {
        return a._matching_samples.size() < b._matching_samples.size();
    }
  bool compare_by_distance(const Point_d &a, const Point_d &b)
    {
        return a.dis < b.dis;
    }

void Mems::give_every_mirror_a_sample(){
  //sorting mems for sample availability
  std::sort(_mems_mirrors.begin(), _mems_mirrors.end(), compare_by_ammount_of_matching_samples);
  std::cout << "Mirror with smallest ammount of samples has  " <<_mems_mirrors.begin()->_matching_samples.size()<<" samples "<<_height<< "  . " <<"\n";


  for(auto mirror = std::begin(_mems_mirrors); mirror != std::end(_mems_mirrors); ++mirror){
    std::sort(mirror->_matching_samples.begin(), mirror->_matching_samples.end(), compare_by_distance);
      for(auto sample = std::begin(mirror->_matching_samples); sample != std::end(mirror->_matching_samples); ++sample){
        std::cout << "Sample_dis  " <<sample->dis <<"\n";
        }

  }

}
