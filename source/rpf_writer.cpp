#include <fstream>




class RPF_writer{

  public:

    void write_rpf(std::string const& name_of_rpf);

  private:


  };

void RPF_writer::write_rpf(std::string const& name_of_rpf){
  std::fstream f;
  f.open(name_of_rpf, std::ios::out);
  f << "name of picture" << " " << "original-picture_width"  << " " << "original-picture_height"  << " "  << " has color depth of 32bit for r+b+g+alpha (4* 8bit)" <<"\n";
  f << "RPF has"  << " "  << "Samples of original picture"  << " "  << "Multipixels have size of " << " " <<
  for(auto mi = _mems_mirrors.begin(); mi!= _mems_mirrors.end(); ++mi){
    f << "m_id " << mi->id<< ";\n";
    f << "m_pos " << mi->_position.x<< " "<< mi->_position.y<< " "<< mi->_position.dis<< ";\n";
    f << "dis_sample " << mi->_displayed_sample.id<< " "<< mi->_displayed_sample.x<< " "<< mi->_displayed_sample.y<< " "<< mi->_displayed_sample.dis<< ";\n";
    f << "sample_amount " << mi->_ammount_of_matching_samples<< ";\n";
    for(auto sam = mi->_matching_samples.begin(); sam!=mi->_matching_samples.end(); ++sam){
      f << "s_pos " <<sam->x<< " "<< sam->y<< " "<< sam->dis<< ";\n";
    }
    f << "--- " << ";\n";
  }
  f.close();

  }
