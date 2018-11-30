#include "highgui.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

#define dilation_size 10 
#define Width 640
#define Height 480 

int main( int argc, char** argv )
{
   
/* Loop invariant variables declaration block */
    	vector<vector<Point> > contours;
    	vector<Vec4i> hierarchy;	
	Mat gray, blur, firstFrame, frameDiff, dist, lastFrame, final;
	Point leftmost(0,0);
	Point rightmost(0,0);
	
	IplImage* img;
	Mat m_hsv;								
	Mat mask_g;

	Vec3b r(0,0,255); 			//Tags de cor para os pixels.
	Vec3b g(0,255,0);
	Vec3b b(255,0,0);
	Vec3b black(0,0,0);						
	
	Mat opening_kernel = getStructuringElement(0,Size(7,7),Point(3,3));	//Essas estruturas são utilizadas para as operações 
	Mat close_kernel = getStructuringElement(0,Size(6,6),Point(2,2));	//morfológicas de abertura e fechamento.
    	
	int experiment,i,j,k;
	long dif;
	double result;
	/* Camera inicialization */
	VideoCapture cap(-1); // open the default camera
    	if(!cap.isOpened())  // check if we succeeded
    		return -1;
	
	cap.set (CV_CAP_PROP_FRAME_WIDTH, Width);
	cap.set (CV_CAP_PROP_FRAME_HEIGHT, Height);

	struct timespec init;
	struct timespec end;
	
	FILE *log;
	log = fopen("log1.txt", "w");
	if (log == NULL) {
	  printf("Can't open input file in.list!\n");
	  return -1;
	}


	for(experiment = 0; experiment < 5; experiment++){
		clock_gettime(CLOCK_MONOTONIC, &init);	
		/* Main loop */    
	    	for(i = 0; i < 100; i++)
		//for(;;)
	    	{
			Mat frame; //This frame receives the current camera frame
			cap >> frame; // get a new frame from camera
		
			if(i==0)
				imwrite("feed.jpg",frame);


			cvtColor(frame, m_hsv, CV_BGR2HSV); //Converts the frame to HSV
			
			inRange (m_hsv, Scalar(40,40,0), Scalar(80,255,255), mask_g); //Thresholding operation. Detects green

			morphologyEx( mask_g, mask_g, MORPH_CLOSE, opening_kernel ); //Morphologic operations. Eliminate noise
			morphologyEx( mask_g, mask_g, MORPH_OPEN, opening_kernel );

			//imshow("mask", mask_g); //Use for debugging (shows the generated mask).

			cvtColor(mask_g,mask_g, CV_GRAY2BGR, 3); //Adds channels to the mask

			for(j=0;j<Height;j++){			//Inside this loop the each pixel is matched agaisnt the mask in order to paint the picture.
				for(k=0;k<Width;k++){	
					if(mask_g.at<Vec3b>(j,k)!=Vec3b(0,0,0)){
						frame.at<Vec3b>(j,k) = g;
					}
					else
						frame.at<Vec3b>(j,k) = black;			
				}
			}

			//imshow("img", frame);	//Use for debugging (shows the painted frame).	

			cvtColor(frame, frame, CV_BGR2GRAY); //Converts the frame to grayscale
			GaussianBlur(frame, blur, Size(7,7), 1.5, 1.5); // Blur image for easy diference capture

			if(firstFrame.total()==0)	//If its the first frame, save it.
				blur.copyTo(firstFrame);

			// These two operations find the contours in the image, effectively tracking the points of desired color.
			Canny(frame, final, 0, 30, 3); 
			findContours( final, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			
			vector<vector<Point> > contours_poly( contours.size() );
			vector<Rect> boundRect( contours.size() );

			/* Color focus localization block */
			/* This block takes in consideration all the contours that have sufficient area to be recognized as a 
			valid color source. It then calculates the leftmost and rightmost point, and calculates the mean 
			central point for the frame. */
			int count=0;
			for( int j = 0; j< contours.size(); j++ )
			{
				if(contourArea(contours[j],true)>6){
					leftmost += contours[j][0];
					rightmost += contours[j][0];
					count++;
				}
	     		}
			/*if(count >0){
				int mean = (leftmost.x + rightmost.x)/(2*count);
				if(mean>640/2 + 30)
					cout << "esquerda\n";
				else if(mean<640/2 - 30)
					cout << "direita\n";
				else
					cout << "centro\n";
			}*/
		
	
			leftmost = Point(0,0); //reinitializes the variables for the next loop
			rightmost = Point(0,0); 
				
			//imshow("rect", final); //Use for debugging (shows the tracked rectangles)
				
			if(waitKey(30) >= 0) break; // Awaits for any key to be pressed. closes the program.
		
		}
		
		clock_gettime(CLOCK_MONOTONIC, &end);
		dif = end.tv_nsec - init.tv_nsec;
		result = dif/1000000000;

		fprintf(log, "Experimento%d: %ld\n", experiment, dif);
	}    

	fclose(log);
	imwrite("first.jpg",firstFrame); // Saves the first frame.
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
