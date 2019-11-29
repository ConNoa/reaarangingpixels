//simply reads txt-like pixeldata and draws a picture without interpretation

#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include <time.h>             //time measuring & seed
#include <string.h>

using namespace cv;

int main(int argc, char** argv )
{
//CHECK_INPUT:
    //CHECK LENGTH
    if ( argc != 2 )//|| argc != 4)
    {
        return -1;
    }

		std::string input= argv[1];


//MAIN:

    std::string line;
    std::ifstream myfile(argv[1]);
    int SIZEX;
		int SIZEY;
    if (myfile.is_open())
    {
        std::cout <<"open file" <<"\n";
        getline(myfile,line);
        std::stringstream meta;
        meta<<line;
        meta>>SIZEX;
        meta>>SIZEY;

        Mat output(SIZEY, SIZEX, CV_8UC3, Scalar(0,0,0));
        int x;
        int y;
        int r;
        int g;
        int b;

        while (getline(myfile,line))
        {
            std::stringstream ss;
            ss<<line;                   //First Line in
            std::string firstWord;
            ss>>x;              //First Word in
            ss>>y;
            ss>>r;
            ss>>g;
            ss>>b;
            output.at<Vec3b>(Point(x,y))[0]=r;
            output.at<Vec3b>(Point(x,y))[1]=g;
            output.at<Vec3b>(Point(x,y))[2]=b;
        }

        myfile.close();
        imwrite( "output_nointerpretation.jpg", output );

    std::cout <<"done\n";
    return 0;
  }
}
