#include <vector>
#include "point.hpp"
#include "mirror.cpp"
#include <fstream>
//#include "sorting/portioner.cpp"

class Mems{

  public:
    Mems(int width, int height);

    std::vector<Mirror> _mems_mirrors;
    std::vector<Mirror> _mems_mirrors_randomrasterized;
    int _width;
    int _height;
    double max_sample_dis;

    void fill_mems_with_mirrors(int picture_width, int picture_height);
    void find_samples_next_to_mirror(std::vector<Point_d> const& sampled_points);
    void find_samples_next_to_mirror_fast(Portioner buckets);
    void save_mirrors_with_samples();
//    bool compare_by_ammount_of_matching_samples(const Mirror &a, const Mirror &b);
    std::vector<Mirror> get_mirror_cluster(Point_d const& displayed_sample) ;

    void give_every_mirror_a_sample();
    void give_every_mirror_a_sample_slow();


  private:



};

  Mems::Mems(int width, int height)
  :_width(width),
  _height(height),
  max_sample_dis(18)
  {
    std::cout << "Mems created with width  " <<_width<<" and height "<<_height<< "  . " <<"\n";
    std::cout <<"\n";

  }

  bool compare_by_ammount_of_matching_samples(const Mirror &a, const Mirror &b){
          return a._matching_samples.size() < b._matching_samples.size();
      }

  bool compare_by_distance(const Point_d &a, const Point_d &b){
          return a.dis < b.dis;
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
/*
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
*/
void Mems::find_samples_next_to_mirror_fast(Portioner buckets){
  int minfittingsamples = 999999;           //DeBUG
  int x_y_distance;
  const int x_y_distance_max = 20;                //maximum Distance between Mirror and Sample for x and y
  int not_matching_mirror_ammount = 0;      //DebUG needed?

  bool enough_samples;                      // needed?
  int mirrornumber = 0;                     //ID

  const int max_samples = 5; //max Ammount of needed Samples
  const int min_samples = 2;


  //iterate selected_mirror all buckets
  for(auto selected_mirror = std::begin(_mems_mirrors); selected_mirror!=std::end(_mems_mirrors); ++selected_mirror){
    mirrornumber += 1;
    x_y_distance = 5;
    enough_samples = false;
    //fill vec with all relevant samplepoints with samples from mirrorbucket and buckets arround
    std::vector<std::vector<Point_d>>point_cluster = buckets.get_bucket_cluster(selected_mirror->_position);
  //  std::cout<<"id: "<<selected_mirror->id<<"\n";
  //  std::cout<<"size: \n";
/*
    int point_amm_in_cluster = 0;
    for(auto it = std::begin(point_cluster); it != std::end(point_cluster); it++){
      point_amm_in_cluster = point_amm_in_cluster+it->size();
    }

    std::cout<<"looking for samples for mirror "<< mirrornumber<<" with "<<point_amm_in_cluster<<" Points in MirrorCluster"<<"\n";
*/
while(x_y_distance<x_y_distance_max ){
//  while(x_y_distance<x_y_distance_max || (selected_mirror->_matching_samples.size() < min_samples)){
        selected_mirror->_matching_samples.clear();
        selected_mirror->_ammount_of_matching_samples = 0;
  //      std::cout<<" x_y_distance = "<<x_y_distance<<"\n";
        for(auto cluster_part = std::begin(point_cluster); cluster_part!=std::end(point_cluster); ++cluster_part)
        {
          for(auto point_to_check = std::begin(*cluster_part); point_to_check != std::end(*cluster_part); ++point_to_check)
          {
            //perform this actions for all Samples which are in bucket for mirror


  //----> nicht nötig          //check, if ammount of needed samples is reached
  //----> nicht nötig                     //not reached: suche mit der aktuellen sample_distance in den buckets nach reached samples
                          //  wenn distance <= (maxdis^2)*2
                                //  put distance to point struct
                                //  push back point to selected mirror
                                //  incremet ammountof matching samples im selected mirror
                                //  if ammount of matching samples = max_sample
                                      // break, go to next mems_mirror

            double sample_dis = pow(selected_mirror->_position.x-point_to_check->x , 2)+pow(selected_mirror->_position.y-point_to_check->y , 2);
            if(sample_dis<=x_y_distance*x_y_distance*2)
            {
              point_to_check->dis = sample_dis;
              selected_mirror->_matching_samples.push_back(*point_to_check);
              selected_mirror->_ammount_of_matching_samples = selected_mirror->_ammount_of_matching_samples +1;
//              std::cout<<selected_mirror->_matching_samples.size()<<" ";

              //std::cout << "break2 \n" ;
            //  break;

            }
        //  MAXSAMPLES:
          }
        }
        if(selected_mirror->_matching_samples.size() >= max_samples){
        //  goto MAXSAMPLES;
//                std::cout << "break1 \n" ;
        break;
        }
        x_y_distance += 2;
      }
//      std::cout << "\n" ;


        if(selected_mirror->id % 10000 == 0)
        {
          int progress = (int)((double)selected_mirror->id/(double)_mems_mirrors.size()*100) ;
          std::cout <<"Find samples for all Mems-Mirrors  "<< progress <<" % . \n";
        }

        if(minfittingsamples > selected_mirror->_matching_samples.size())
        {
          minfittingsamples = selected_mirror->_matching_samples.size();
          //std::cout << "Minimal fitting samples sind jetzt " <<minfittingsamples<<"\n \n";
        }
        if(minfittingsamples == 0 && selected_mirror->_matching_samples.size()==0)
        {
          not_matching_mirror_ammount += 1;
        }
      }
//      std::cout << "not matching mirrorammount =  " <<not_matching_mirror_ammount<<"\n";
      std::cout << "All samples for Mirrors found"<< '\n';

      for(auto mirror = std::begin(_mems_mirrors); mirror != std::end(_mems_mirrors); ++mirror)
      {
        std::sort(mirror->_matching_samples.begin(), mirror->_matching_samples.end(), compare_by_distance);
      }

  }

void Mems::save_mirrors_with_samples(){
  std::fstream f;
  f.open("mirrors_with_samples.dat", std::ios::out);
  f << "mirrors with samples" << "\n"<< "\n"<< "\n";
  for(auto mi = _mems_mirrors.begin(); mi!= _mems_mirrors.end(); ++mi){
    f << "id " << mi->id<< ";\n";
    f << "pos " << mi->_position.x<< " , "<< mi->_position.y<< " , "<< mi->_position.dis<< ";\n";
    f << "sample_ammount " << mi->_ammount_of_matching_samples<< ";\n";
    for(auto sam = mi->_matching_samples.begin(); sam!=mi->_matching_samples.end(); ++sam){
      f << "pos " <<sam->x<< " , "<< sam->y<< " , "<< sam->dis<< ";\n";
    }
    f << "--- " << ";\n";




  }
  f << "Dieser Text geht in die Datei2" << std::endl;
  f.close();
}


std::vector<Mirror> Mems::get_mirror_cluster(Point_d const& displayed_sample){

  std::vector<Mirror> m_cluster;
  for(auto it = std::begin(_mems_mirrors); it != std::end(_mems_mirrors); it++)
    {
      double dis_x = displayed_sample.x - it->_position.x;
      double dis_y = displayed_sample.y - it->_position.y;
      double sample_dis = pow(dis_x , 2)+pow(dis_y , 2);
              if(sample_dis<=max_sample_dis)
                {
                  m_cluster.push_back(*it);
                }
    }
    std::cout<<"created mirrorcluster for mirror with size: "<< m_cluster.size()<< "\n";
    return m_cluster;
  }

  void Mems::give_every_mirror_a_sample(){
    std::cout<<"\n";

    std::cout<<"give every mirror a sample"<< "\n"<< "\n";

    while(_mems_mirrors.size() > 0)
    {
      std::cout<<"Pixels left: "<<_mems_mirrors.size()<<"\n";
      //sorting mems for sample availability
      std::sort(_mems_mirrors.begin(), _mems_mirrors.end(), compare_by_ammount_of_matching_samples);

      std::cout << "Mirror with smallest ammount of samples has  " <<_mems_mirrors.begin()->_matching_samples.size()<<" samples. " <<"\n";
      //gehe durch alle spiegel und sortiere ihren sample vector der größe nach
      for(auto mirror = std::begin(_mems_mirrors); mirror != std::end(_mems_mirrors); ++mirror)
      {
        std::sort(mirror->_matching_samples.begin(), mirror->_matching_samples.end(), compare_by_distance);
      }
      // give first mirror(am wenigsten matches) sample_displayed which is at matching samples first position
      _mems_mirrors.begin()->_displayed_sample = _mems_mirrors.begin()->_matching_samples.front();
      std::cout<<"_mems_mirror.begin() on pos: x: "<<_mems_mirrors.begin()->_position.x<<" y: "<<_mems_mirrors.begin()->_position.y<<"\n";
      std::cout<<"_mems_mirror.begin() shows now: x: "<<_mems_mirrors.begin()->_displayed_sample.x<<" y: "<<_mems_mirrors.begin()->_displayed_sample.y<<"\n";

      //point aus allen umliegenden mirrors als available point löschen
      //
      std::vector<Mirror> mirrors_to_proof = get_mirror_cluster(_mems_mirrors.begin()->_displayed_sample) ;
      std::cout<<"Mirrorcluster to proof, if  x,y is although there inside is: "<<mirrors_to_proof.size() <<"\n";
      //-dafür mirror cluster aus umliegenden buckets durchiterieren und point  aus _matching_samples wenn existent löschen
      int mirrorcounter = 0;
    //  std::cout<<"There are "<<mirrors_to_proof.size()-mirrorcounter<<" mirrors to proof, if sample is although there. \n";

      for(auto it = std::begin(mirrors_to_proof); it != std::end(mirrors_to_proof); ++it){

        std::cout<<"Next mirror has "<< it->_matching_samples.size()<<" Samples \n";
          int counter = 0;
          for(auto sample = std::begin(it->_matching_samples); sample != std::end(it->_matching_samples); ++sample){
            std::cout<<"checking sample "<< counter<<"  \n";
            counter++;

            if(it->_displayed_sample.x == sample->x && it->_displayed_sample.y == sample->y){
              //mirror has sample!!!
              _mems_mirrors.begin()->_matching_samples.erase(_mems_mirrors.begin()->_matching_samples.begin()+counter);
              std::cout<<"sample erased. \n";
            //  std::cout<<"Pixels left: "<<_mems_mirrors.size()<<"\n";
              mirrorcounter += mirrorcounter;
              //-------------------------
              //-------------------------
              //sample erased
              break;
            }
            else{
            }
            _mems_mirrors.begin()->_matching_samples.shrink_to_fit();
          }
        }

        _mems_mirrors.begin()->_matching_samples.clear();
        _mems_mirrors_randomrasterized.push_back(*_mems_mirrors.begin());
        std::cout<<"new mirrorvector has size: "<<_mems_mirrors_randomrasterized.size()<<"\n";
        _mems_mirrors.erase(_mems_mirrors.begin());
    //    _mems_mirrors.front() = _mems_mirrors.back();
    //    _mems_mirrors.pop_back();
        std::sort(_mems_mirrors.begin(), _mems_mirrors.end(), compare_by_ammount_of_matching_samples);


        //
        //- update size() of manipulated vector;
      //_mems_mirrors.begin()->_matching_samples.clear();
      //  _mems_mirrors_randomrasterized.back()._displayed_sample;
      }
    }

    void Mems::give_every_mirror_a_sample_slow(){
      std::cout<<"\n";

      std::cout<<"give every mirror a sample"<< "\n"<< "\n";

      while(_mems_mirrors.size() > 0)
      {
        std::cout<<"Pixels left: "<<_mems_mirrors.size()<<"\n";
        //sorting mems for sample availability
        std::sort(_mems_mirrors.begin(), _mems_mirrors.end(), compare_by_ammount_of_matching_samples);

        std::cout << "Mirror with smallest ammount of samples has  " <<_mems_mirrors.begin()->_matching_samples.size()<<" samples. " <<"\n";
        //gehe durch alle spiegel und sortiere ihren sample vector der größe nach
        for(auto mirror = std::begin(_mems_mirrors); mirror != std::end(_mems_mirrors); ++mirror)
        {
          std::sort(mirror->_matching_samples.begin(), mirror->_matching_samples.end(), compare_by_distance);
        }
        // give first mirror(am wenigsten matches) sample_displayed which is at matching samples first position
        _mems_mirrors.begin()->_displayed_sample = _mems_mirrors.begin()->_matching_samples.front();
        std::cout<<"_mems_mirror.begin() on pos: x: "<<_mems_mirrors.begin()->_position.x<<" y: "<<_mems_mirrors.begin()->_position.y<<"\n";
        std::cout<<"_mems_mirror.begin() shows now: x: "<<_mems_mirrors.begin()->_displayed_sample.x<<" y: "<<_mems_mirrors.begin()->_displayed_sample.y<<"\n";

        //point aus allen umliegenden mirrors als available point löschen
        //
        std::vector<Mirror> mirrors_to_proof = get_mirror_cluster(_mems_mirrors.begin()->_displayed_sample) ;
        std::cout<<"Mirrorcluster to proof, if  x,y is although there inside is: "<<mirrors_to_proof.size() <<"\n";
        //-dafür mirror cluster aus umliegenden buckets durchiterieren und point  aus _matching_samples wenn existent löschen
        int mirrorcounter = 0;
      //  std::cout<<"There are "<<mirrors_to_proof.size()-mirrorcounter<<" mirrors to proof, if sample is although there. \n";

        for(auto it = std::begin(mirrors_to_proof); it != std::end(mirrors_to_proof); ++it){

          std::cout<<"Next mirror has "<< it->_matching_samples.size()<<" Samples \n";
            int counter = 0;
            for(auto sample = std::begin(it->_matching_samples); sample != std::end(it->_matching_samples); ++sample){
              std::cout<<"checking sample "<< counter<<"  \n";
              counter++;

              if(it->_displayed_sample.x == sample->x && it->_displayed_sample.y == sample->y){
                //mirror has sample!!!
                _mems_mirrors.begin()->_matching_samples.erase(_mems_mirrors.begin()->_matching_samples.begin()+counter);
                std::cout<<"sample erased. \n";
              //  std::cout<<"Pixels left: "<<_mems_mirrors.size()<<"\n";
                mirrorcounter += mirrorcounter;
                //-------------------------
                //-------------------------
                //sample erased
                break;
              }
              else{
              }
              _mems_mirrors.begin()->_matching_samples.shrink_to_fit();
            }
          }

          _mems_mirrors.begin()->_matching_samples.clear();
          _mems_mirrors_randomrasterized.push_back(*_mems_mirrors.begin());
          std::cout<<"new mirrorvector has size: "<<_mems_mirrors_randomrasterized.size()<<"\n";
          _mems_mirrors.erase(_mems_mirrors.begin());
      //    _mems_mirrors.front() = _mems_mirrors.back();
      //    _mems_mirrors.pop_back();
          std::sort(_mems_mirrors.begin(), _mems_mirrors.end(), compare_by_ammount_of_matching_samples);


          //
          //- update size() of manipulated vector;
        //_mems_mirrors.begin()->_matching_samples.clear();
        //  _mems_mirrors_randomrasterized.back()._displayed_sample;
        }
      }
