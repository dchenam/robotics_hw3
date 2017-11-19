// This program is a simple template of an C++ program loading and showing image with OpenCV.
// You can ignore this file and write your own program.
// The program takes a image file name as an argument.

#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

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

	// Create windows for debug.
	namedWindow("SrcImage", cv::WINDOW_AUTOSIZE);
	namedWindow("WorkImage", cv::WINDOW_AUTOSIZE);
	namedWindow("ComImage", cv::WINDOW_AUTOSIZE);
	// Show the source image.
	imshow("SrcImage", SrcImage);
	waitKey();

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
	erode(WorkImage, WorkImage, element);
	dilate(WorkImage, WorkImage, element);

	// Duplicate the working iamge.
	ComImage = WorkImage.clone();

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(ComImage, canny_output, thresh, thresh*2, 3 );
	imshow("WorkImage", canny_output);
	waitKey();

	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	cout << contours.size() << endl;

	// Get the moments
	vector<Moments> mu(contours.size());
	for( int i = 0; i < contours.size(); i++ )
	 { mu[i] = moments( contours[i], false ); }

	//  Get the mass centers:
	vector<Point2f> mc( contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	 {
	 	mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
	 	cout << mc[i] << endl;
	 }

	 
	/// Draw contours
	Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	 {
	   Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	   drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
	   circle( drawing, mc[i], 4, color, -1, 8, 0 );
	 }

	// Show the working image after preprocessing.

	cv::imshow("ComImage", drawing);
  	cv::waitKey();

	return 0;
}

