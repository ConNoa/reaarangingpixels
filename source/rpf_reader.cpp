#include <fstream>




class RPF_reader{

  public:

    std::vector<Mirror>load_rpf(std::string const& file_to_read);

  private:


  };

std::vector<Mirror> RPF_reader::load_rpf(std::string const& file_to_read){
  std::ifstream inFile;
  std::string line;
  std::vector<Mirror> loadedMirrors;
  std::vector<Point_d> sampled_positions;
  Point_d m_position;
  Point_d sample_position;


  inFile.open(file_to_read);
  if (!inFile) {
      std::cerr << "Unable to open file "<<file_to_read;
      exit(1);   // call system to stop
  }
  else{
    while(std::getline(inFile, line)){

      std::stringstream ss;
      std::string keyword;


      //------------------------------------------------------
      //------------------------------------------------------
/*
      int id;
      Point_d _position;
      std::vector<Point_d> _matching_samples;
      // value ammount of _matching_samples  not used???
      int _ammount_of_matching_samples = 0;
      Point_d _displayed_sample;
      bool has_sample = false;
      */
      //------------------------------------------------------
      //------------------------------------------------------

      ss << line;
      ss >> keyword;


      int id;

      if(keyword == "m_id"){
      std::cout <<"keyword: m_id " << '\n';
        ss >> id;

      }

      if(keyword == "m_pos"){
      std::cout <<"keyword: m_pos " << '\n';

        ss >> m_position.x;
        ss >> m_position.y;
        ss >> m_position.dis;

      }
      //std::cout <<"keyword: m_pos.y "<< m_position.y << '\n';


      Point_d displayed_sample;
      bool displayed = true;

      if(keyword == "dis_sample"){
        std::cout <<"keyword: displayed_sample " << '\n';

        ss >> displayed_sample.id;
        ss >> displayed_sample.x;
        ss >> displayed_sample.y;
        ss >> displayed_sample.dis;
        if(displayed_sample.dis == 99999){
          displayed = false;
        }
      }

      int sample_amount;

      if(keyword == "sample_amount"){
        std::cout <<"keyword: sample_ammount " << '\n';

        ss >> sample_amount;
      }

      if(keyword == "pos"){
        std::cout <<"keyword: pos " << '\n';


        ss >> sample_position.id;
        ss >> sample_position.x;
        ss >> sample_position.y;
        ss >> sample_position.dis;

        sampled_positions.push_back(sample_position);

      }

      if(keyword == "---"){
        std::cout <<"keyword: --- " << '\n';

        Mirror incomming_mirror;

        incomming_mirror.id = id;
        incomming_mirror._position = m_position;
        incomming_mirror._displayed_sample = displayed_sample;
        incomming_mirror._ammount_of_matching_samples = sample_amount;
        incomming_mirror._matching_samples = sampled_positions;
        incomming_mirror.has_sample = displayed;

        loadedMirrors.push_back(incomming_mirror);

      }
    }
  }
  return loadedMirrors;
}
