

class Mirror{
  public:
    int id;
    Point_d _position;
    std::vector<Point_d> _matching_samples;
    // value ammount of _matching_samples  not used???
    int _ammount_of_matching_samples = 0;
    Point_d _displayed_sample;
    bool has_sample = false;


};
