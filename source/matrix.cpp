#include "matrix.hpp"

Matrix::Matrix():
  _w{512},
  _h{320},
  
{
	int d1, d2; //dimensions
	vector<vector<int>> p; // All values of matrix
public:
	//We are not using the default constructor
	matrix(int, int); //Constructor will only create the space here LLL
	~matrix();
	void set_value(int, int, int); // first two args are element position specifiers.
};

matrix::matrix(int d11, int d22)
{
	d1=d11;
	d2=d22;
	vector<vector<int>> p(d2, vector<int>(d1,0));
}

matrix::~matrix()
{
	//I believe that I do not need to write a destructor as there is no pointer involved.
}

void matrix::set_value(int i, int j, int value)
{
	p[j][i] = value;
}


Collmap::Collmap():
    _w{3000},
    _h{2000},
    _collisionmap{std::vector<std::vector<int>>(10, std::vector<int> (10, 0))}{}


Collmap::Collmap(int width, int heigth):
    _w{width},
    _h{heigth},
    _collisionmap{std::vector<std::vector<int>>(width, std::vector<int> (heigth, 0))}{}
