// This program is a simple template of an C++ program loading and showing image with OpenCV.
// You can ignore this file and write your own program.
// The program takes a image file name as an argument.

#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

// Point2f getCentralMoment(vector<Point_<int> > contour) {
// 	Moments mu = moments(contour, false);
// 	Point2f mc = Point2f(mu.m10/mu.m00 , mu.m01/mu.m00);
//  	cout << mc << endl;
//  	return mc;
// }

int main(int argc, char **argv) {
	int thresh = 100;
	int max_thresh = 255;
	RNG rng(12345);

	// Check number of arguments.
  // If no argument input, exit the program.
	if ( argc != 2 )
		exit(1);

	Mat SrcImage, WorkImage, ComImage;

	// Load a gray scale picture.
	SrcImage = imread(argv[1], 0);

	if (!SrcImage.data)
		exit(1);

	namedWindow("ComImage", cv::WINDOW_AUTOSIZE);

	// Duplicate the source iamge.
	WorkImage = SrcImage.clone();	
	
  //Extract the contour of 
	/* If you're familiar with OpenCV, findContours() will be a better way.*/
	GaussianBlur(WorkImage, WorkImage, Size(5, 5), 0, 0);
	threshold(WorkImage, WorkImage, 128, 255, cv::THRESH_BINARY);

	// Opening
	int element_size = 4;
	int element_type = MORPH_RECT;
	Mat element = getStructuringElement( element_type,
			Size( 2*element_size + 1, 2*element_size + 1 ),
			Point( element_size, element_size ) );
	// erode(WorkImage, WorkImage, Mat());
	// dilate(WorkImage, WorkImage, Mat());
	erode(WorkImage, WorkImage, element);
	dilate(WorkImage, WorkImage, element);
	// Duplicate the working iamge.
	ComImage = WorkImage.clone();

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(ComImage, canny_output, thresh, thresh*2, 3 );

	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	// Get the Central Moments and Draw
	vector<Moments> mu(contours.size());
	vector<Point2f> mc(contours.size());
	vector<double> pa(contours.size());
	Mat drawing = SrcImage.clone();

  	for( int i = 0; i < contours.size(); i++ ){
  		mu[i] = moments( contours[i], false );
  	}

	//Get the mass centers:
	for( int i = 0; i < contours.size(); i++ ){
	 	mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
	 }
	 //Get the principle angle:
	 const double PI = 3.141592653589793;
	 for (int i = 0; i < contours.size(); ++i)
	 {
	 	pa[i] = 0.5 * atan2(2 * mu[i].mu11, mu[i].mu20 - mu[i].mu02);
	 	pa[i] *= 180 / PI;
	 	cout << format("(%.2f,%.2f, %.2f)", mc[i].x, mc[i].y, pa[i])<<endl;
	 }
	 
	//Draw contours
	for( int i = 0; i< contours.size(); i++ ){
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		const cv::Scalar BLACK(0, 0, 0);
	   	drawContours( drawing, contours, i, Scalar(0, 0 ,0), 2, 8, hierarchy, 0, Point() );
	   	circle( drawing, mc[i], 4, BLACK, -1, 8, 0 );
	   	Vec4f le;
	   	fitLine(contours[i], le, CV_DIST_L2, 0 , 0.01 , 0.01);
	   	float lefty = float((- le[2] * le[1] / le[0]) + le[3]);
	   	float righty = float(((drawing.cols - le[2]) * le[1]/le[0]) + le[3]);
	   	Point2f point_a(drawing.cols - 1, righty);
	   	Point2f point_b(0, lefty);
	   	line(drawing, point_a, point_b, BLACK, 2);
	   	//putText(drawing, format("(%.2f,%.2f, %.2f)", mc[i].x, mc[i].y, pa[i]), mc[i], FONT_HERSHEY_COMPLEX_SMALL, 0.75, RED, 1);
	}

	// Show the working image after preprocessing.
	cv::imshow("ComImage", drawing);
  	cv::waitKey();

	return 0;
}

