#include <stdio.h>
#include <opencv2/opencv.hpp> //image operations
#include "../pixel.hpp"
#include <time.h>             //time measuring & seed
#include <math.h>       /* sqrt */
//#include <highgui.h>
//#include <opencv2/highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/types_c.h>
#include "opencv2/core/core_c.h"

using namespace cv;

/*
expects double images, not integers!*/
class Evaluator{
public:
 Evaluator(Mat const& ref_image):
      _Ref_image(ref_image)
      {}

  void evaluate_ssim(Mat & image, std::fstream & stream)
  {
    /*
     * The equivalent of Zhou Wang's SSIM matlab code using OpenCV.
     * from http://www.cns.nyu.edu/~zwang/files/research/ssim/index.html
     * The measure is described in :
     * "Image quality assessment: From error measurement to structural similarity"
     * C++ code by Rabah Mehdi. http://mehdi.rabah.free.fr/SSIM
     *
     * This implementation is under the public domain.
     * @see http://creativecommons.org/licenses/publicdomain/
     * The original work may be under copyrights.
     */

    /*
     * Parameters : complete path to the two image to be compared
     * The file format must be supported by your OpenCV build
     */
     double C1 = 6.5025, C2 = 58.5225;

   	IplImage
   		*img1=NULL, *img2=NULL, *img1_img2=NULL,
   		*img1_temp=NULL, *img2_temp =NULL,
   		*img1_sq=NULL, *img2_sq=NULL,
   		*mu1=NULL, *mu2=NULL,
   		*mu1_sq=NULL, *mu2_sq=NULL, *mu1_mu2=NULL,
   		*sigma1_sq=NULL, *sigma2_sq=NULL, *sigma12=NULL,
   		*ssim_map=NULL, *temp1=NULL, *temp2=NULL, *temp3=NULL;


   	/***************************** INITS **********************************/

    //img1_temp = new IplImage(_Ref_image);    //!!!!!!!!!!!!!!!

    //IplImage in_img = image;
    //IplImage* img2_temp = &in_img;
     //wimage;//new IplImage();    //!!!!!!!!!!!!!!!
    /*
   	if(img1_temp==NULL || img2_temp==NULL)
   		return ;

   	int x=img1_temp->width, y=img1_temp->height;
   	int nChan=img1_temp->nChannels, d=IPL_DEPTH_32F;
   	CvSize size = cvSize(x, y);

   	img1 = cvCreateImage( size, d, nChan);
   	img2 = cvCreateImage( size, d, nChan);

   	cvConvert(img1_temp, img1);
   	cvConvert(img2_temp, img2);



   	img1_sq = cvCreateImage( size, d, nChan);
   	img2_sq = cvCreateImage( size, d, nChan);
   	img1_img2 = cvCreateImage( size, d, nChan);

   	cvPow( img1, img1_sq, 2 );
   	cvPow( img2, img2_sq, 2 );
   	cvMul( img1, img2, img1_img2, 1 );

   	mu1 = cvCreateImage( size, d, nChan);
   	mu2 = cvCreateImage( size, d, nChan);

   	mu1_sq = cvCreateImage( size, d, nChan);
   	mu2_sq = cvCreateImage( size, d, nChan);
   	mu1_mu2 = cvCreateImage( size, d, nChan);


   	sigma1_sq = cvCreateImage( size, d, nChan);
   	sigma2_sq = cvCreateImage( size, d, nChan);
   	sigma12 = cvCreateImage( size, d, nChan);

   	temp1 = cvCreateImage( size, d, nChan);
   	temp2 = cvCreateImage( size, d, nChan);
   	temp3 = cvCreateImage( size, d, nChan);

   	ssim_map = cvCreateImage( size, d, nChan);
    */
   	// *************************** END INITS **********************************


   	//////////////////////////////////////////////////////////////////////////
   	// PRELIMINARY COMPUTING
    /*
   	cvSmooth( img1, mu1, CV_GAUSSIAN, 11, 11, 1.5 );
   	cvSmooth( img2, mu2, CV_GAUSSIAN, 11, 11, 1.5 );

   	cvPow( mu1, mu1_sq, 2 );
   	cvPow( mu2, mu2_sq, 2 );
   	cvMul( mu1, mu2, mu1_mu2, 1 );


   	cvSmooth( img1_sq, sigma1_sq, CV_GAUSSIAN, 11, 11, 1.5 );
   	cvAddWeighted( sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq );

   	cvSmooth( img2_sq, sigma2_sq, CV_GAUSSIAN, 11, 11, 1.5 );
   	cvAddWeighted( sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq );

   	cvSmooth( img1_img2, sigma12, CV_GAUSSIAN, 11, 11, 1.5 );
   	cvAddWeighted( sigma12, 1, mu1_mu2, -1, 0, sigma12 );
    */

   	//////////////////////////////////////////////////////////////////////////
   	// FORMULA

   	// (2*mu1_mu2 + C1)
    /*
   	cvScale( mu1_mu2, temp1, 2 );
   	cvAddS( temp1, cvScalarAll(C1), temp1 );

   	// (2*sigma12 + C2)
   	cvScale( sigma12, temp2, 2 );
   	cvAddS( temp2, cvScalarAll(C2), temp2 );

   	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
   	cvMul( temp1, temp2, temp3, 1 );

   	// (mu1_sq + mu2_sq + C1)
   	cvAdd( mu1_sq, mu2_sq, temp1 );
   	cvAddS( temp1, cvScalarAll(C1), temp1 );

   	// (sigma1_sq + sigma2_sq + C2)
   	cvAdd( sigma1_sq, sigma2_sq, temp2 );
   	cvAddS( temp2, cvScalarAll(C2), temp2 );

   	// ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
   	cvMul( temp1, temp2, temp1, 1 );

   	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
   	cvDiv( temp3, temp1, ssim_map, 1 );


   	CvScalar index_scalar = cvAvg( ssim_map );

   	// through observation, there is approximately
   	// 1% error max with the original matlab program

   	std::cout << "(R, G & B SSIM index)\n";
   	std::cout << index_scalar.val[2] * 100 << "%\n" ;
   	std::cout << index_scalar.val[1] * 100 << "%\n" ;
   	std::cout << index_scalar.val[0] * 100 << "%\n" ;
    stream <<"\tSSIM:"<< (index_scalar.val[0]+index_scalar.val[1]+index_scalar.val[2]) * 100/3.0f;
    stream <<"\tR:"<< index_scalar.val[0] * 100;
    stream <<"\tG:"<< index_scalar.val[1] * 100;
    stream <<"\tB:"<< index_scalar.val[2] * 100;

    */

    //cvReleaseImage(in_img);// = image;


    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    cvReleaseImage(&img1_img2);
    //cvReleaseImage(&img1_temp);
    //cvReleaseImage(&img2_temp);
    cvReleaseImage(&img1_sq);
    cvReleaseImage(&img2_sq);
    cvReleaseImage(&mu1);
    cvReleaseImage(&mu2);
    cvReleaseImage(&mu1_sq);
    cvReleaseImage(&mu2_sq);
    cvReleaseImage(&mu1_mu2);
    cvReleaseImage(&sigma1_sq);
    cvReleaseImage(&sigma2_sq);
    cvReleaseImage(&sigma12);
    cvReleaseImage(&ssim_map);
    cvReleaseImage(&temp1);
    cvReleaseImage(&temp2);
    cvReleaseImage(&temp3);
    }


  Mat evaluate_abs(Mat const& image, std::fstream & stream)
  {
    //red is wrong
    double max=255*3;
    double min=0;
    //blue is correct
    double sum=0;
    std::vector<std::pair<double,int> > borders;
    double val=0.0;
    double step=0.002;
    borders.push_back(std::pair<double,int>(0,0));    //correct Pixel
    /*
    borders.push_back(std::pair<double,int>(0.001,0));
    borders.push_back(std::pair<double,int>(0.002,0));
    borders.push_back(std::pair<double,int>(0.005,0));
    borders.push_back(std::pair<double,int>(0.01,0));
    borders.push_back(std::pair<double,int>(0.1,0));
    */
    borders.push_back(std::pair<double,int>(0.5,0));  //inbetween
    borders.push_back(std::pair<double,int>(1.0,0));  //bad Pixel
    /*
    while(val<1.0)
    {
      borders.push_back(std::pair<double,int>(val,0));
      val+=0.002;
    }
    */

    Mat output(_Ref_image.rows, _Ref_image.cols, CV_64FC3, Scalar(0,0,0));
    for(int x=0; x<_Ref_image.cols; x++)
    {
      for(int y=0; y<_Ref_image.rows; y++)
      {
        double error=0;
        for (int c=0; c<3; c++)
        {
          error+=abs(((double)_Ref_image.at<Vec3d>(Point(x,y))[c])-image.at<Vec3d>(Point(x,y))[c]);
        }
        sum+=error;
        if(error<255)
        {
          output.at<Vec3d>(Point(x,y))[0]=error;
        }else if(error<510)
        {
          output.at<Vec3d>(Point(x,y))[0]=255;
          output.at<Vec3d>(Point(x,y))[1]=(error-255);
        }else
        {
          output.at<Vec3d>(Point(x,y))[0]=255;
          output.at<Vec3d>(Point(x,y))[1]=255;
          output.at<Vec3d>(Point(x,y))[2]=(error-2*255);
        }


        for(std::vector<std::pair<double,int> > ::iterator o = borders.begin(); o !=  borders.end(); ++o) {
          if(error<= max*(*o).first)
          {
            (*o).second++;
            break;
          }
        }





        /*
        output.at<Vec3d>(Point(x,y))[2]=error/((double)3);
        //output.at<Vec3d>(Point(x,y))[1]=255.0f-error/((double)3);
        output.at<Vec3d>(Point(x,y))[0]=255.0f-error/((double)3);
        //output.at<Vec3d>(Point(x,y))[2]=255.0f-error/((double)3);
        //double r=
        */

      }
    }
    std::cout<<"error_abs:"<<sum<<"\n";
    //---------next line outcommented for runnning code

  //  stream<<"\n sum:"<<sum<<"\t";
    int total=0;
    //std::vector<Mat> eval(1280,border.size(), CV_64FC3, Scalar(0,0,0));

    //int coll=0;
    for(std::vector<std::pair<double,int> > ::iterator o = borders.begin(); o !=  borders.end(); ++o) {

      total+=(*o).second;
      /*
      for(int i=0; i<(*o).second; i++)
      {
        eval.at<Vec3d>(Point(coll,i))[0]=255;
      }
      coll++;*/
      std::cout<<"better than: "<<(*o).first<<": "<<total<<"got:"<<(*o).second<<"\n";
      //---------next line outcommented for runnning code
    //  stream<<"\n"<<(*o).first<<": "<<(*o).second;
    }
    return output;
  }

  Mat evaluate_3d(Mat const& image)
  {
    //red is wrong
    double max=255*sqrt(3);
    double min=0;
    //blue is correct
    double sum=0;
    Mat output(_Ref_image.rows, _Ref_image.cols, CV_64FC3, Scalar(0,0,0));
    for(int x=0; x<_Ref_image.cols; x++)
    {
      for(int y=0; y<_Ref_image.rows; y++)
      {
        double error=0;
        for (int c=0; c<3; c++)
        {
          error+=pow(_Ref_image.at<Vec3d>(Point(x,y))[c]-image.at<Vec3d>(Point(x,y))[c],2);
        }
        error=sqrt(error);
        sum+=error;
        output.at<Vec3d>(Point(x,y))[0]=error/((double)sqrt(3));
        output.at<Vec3d>(Point(x,y))[2]=255.0f-error/((double)sqrt(3));;
      }
    }
    std::cout<<"error_3D:"<<sum<<"\n";
    return output;
  }

private:
  Mat _Ref_image;
};

//using namespace cv;
//sample_video
//sample_picture
//file:
