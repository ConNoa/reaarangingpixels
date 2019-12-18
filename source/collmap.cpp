#include "collmap.hpp"


Collmap::Collmap():
    _w{3000},
    _h{2000},
    _collisionmap{std::vector<std::vector<int>>(10, std::vector<int> (10, 0))}{}


Collmap::Collmap(int width, int heigth):
    _w{width},
    _h{heigth},
    _collisionmap{std::vector<std::vector<int>>(width, std::vector<int> (heigth, 0))}{}
