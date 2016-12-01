// TrapezoidTransform.cpp : Defines the entry point for the console application.
//

#pragma region includes

#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <Windows.h>

#include <deque>

using namespace cv;
using namespace std;
#pragma endregion libraries and stuff

//Don't think we need this
//int kernel_size = 3;
//int scale = 1;
//int delta = 0;
//int ddepth = CV_16S;

#pragma region parameters
/* Variables for image modification */
double alpha = 2.0; // Contrast control
double beta = 30.0; // Brightness control

					/* Parameters for blob detection */

					// Change thresholds
int minThreshold = 15;
int maxThreshold = 255;

// Filter by Area.
bool filterByArea = true;
int minArea = 20;

// Filter by Circularity
bool filterByCircularity = true;
double minCircularity = 0.02;

// Filter by Convexity
bool filterByConvexity = true;
double minConvexity = 0.01;

// Filter by Inertia
bool filterByInertia = true;
double minInertiaRatio = 0.01;

//Filter by color
bool filterByColor = true;
int blobColor = 255;
#pragma endregion Image adjustment and modification parameters

#pragma prototypes

Point2f getCalibrationPoint();

#pragma endregion

int main(int argc, char* argv[])
{

	/* Windows for displaying different frames */

	namedWindow("Original Video", CV_WINDOW_AUTOSIZE);
	namedWindow("Modified Video", CV_WINDOW_AUTOSIZE);
	namedWindow("Adjusted Modified Video", CV_WINDOW_AUTOSIZE);

	//namedWindow("Modified Video Keypoints", CV_WINDOW_AUTOSIZE);
	namedWindow("Adjusted Modified Video Keypoints", CV_WINDOW_AUTOSIZE);

	/* Calibration */

	/*Point2f topLeft;
	Point2f topRight;
	Point2f bottomRight;
	Point2f bottomLeft;

	cout << "Press enter to begin calibrating TL" << std::endl;
	cin.get();
	topLeft = getCalibrationPoint();
	cout << "Top Left Xcoor: " << topLeft.x << " Ycoor: " << topLeft.y << endl;

	cout << "Press enter to begin calibrating TR" << std::endl;
	cin.get();
	topRight = getCalibrationPoint();

	cout << "Press enter to begin calibrating BR" << std::endl;
	cin.get();
	bottomRight = getCalibrationPoint();

	cout << "Press enter to begin calibrating BL" << std::endl;
	cin.get();
	bottomLeft = getCalibrationPoint();*/

	VideoCapture cap(1); // open the video file for reading

	if (!cap.isOpened()) // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	Mat calibFrame;
	bool bSuccess = cap.read(calibFrame); // read a new frame from video
	if (!bSuccess) //if not success, break loop
	{
		cout << "Cannot read the frame from video file" << endl;
	}

	int height = calibFrame.rows;
	int width = calibFrame.cols;

	vector<Point2f> corners_fixed, corners_display;

	//Create corners vector of the fixed image
	corners_fixed.push_back(Point2f(0, 0));
	corners_fixed.push_back(Point2f(width, 0));
	corners_fixed.push_back(Point2f(width, height));
	corners_fixed.push_back(Point2f(0, height));

	//Create corners vector of the display within the fixed image
	/*corners_display.push_back(topLeft);
	corners_display.push_back(topRight);
	corners_display.push_back(bottomRight);
	corners_display.push_back(bottomLeft);*/

	corners_display.push_back(Point2f(45, 60));
	corners_display.push_back(Point2f(629, 93));
	corners_display.push_back(Point2f(568, 418));
	corners_display.push_back(Point2f(60, 360));

	//Create adjustment matrix
	Mat_<float> H_display_to_fixed = cv::getPerspectiveTransform(corners_fixed, corners_display);

	cv::Mat_<float> map1_32f(height, width), map2_32f(height, width);
	for (int y = 0; y<height; ++y)
	{
		float *buff_mapx = ((float*)map1_32f.data) + y*width;
		float *buff_mapy = ((float*)map2_32f.data) + y*width;
		for (int x = 0; x<width; ++x)
		{
			cv::Mat_<float> pt(3, 1);
			pt(0) = x;
			pt(1) = y;
			pt(2) = 1;
			pt = H_display_to_fixed*pt;
			pt /= pt(2);
			buff_mapx[x] = pt(0);
			buff_mapy[x] = pt(1);
		}
	}

	Mat map1_16u, map2_16u;
	convertMaps(map1_32f, map2_32f, map1_16u, map2_16u, CV_16SC2);
	// Keep map1_16u & map2_16u, discard the rest

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		Mat image = frame;
		Mat new_image;

		double alpha = 1.0; // Contrast control
		double beta = 30.0; // Brightness control

		image.convertTo(new_image, -1, alpha, beta);

		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;

		// Change thresholds
		params.minThreshold = minThreshold;
		params.maxThreshold = maxThreshold;

		// Filter by Area.
		params.filterByArea = filterByArea;
		params.minArea = minArea;

		// Filter by Circularity
		params.filterByCircularity = filterByCircularity;
		params.minCircularity = minCircularity;

		// Filter by Convexity
		params.filterByConvexity = filterByConvexity;
		params.minConvexity = minConvexity;

		// Filter by Inertia
		params.filterByInertia = filterByInertia;
		params.minInertiaRatio = minInertiaRatio;

		//Filter by color
		params.filterByColor = filterByColor;
		params.blobColor = blobColor;

		//Create the simple blob detector
		//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
		cv::SimpleBlobDetector detector(params);

		// Storage for blobs
		std::vector<KeyPoint> keypoints;

		//Adjust image so display occupies entire frame
		Mat adj_new_image;
		remap(new_image, adj_new_image, map1_16u, map2_16u, INTER_LINEAR);

		// Detect blobs
		detector.detect(adj_new_image, keypoints);

		///** Printing **/

		std::cout << "Points: " << keypoints.size() << std::endl;
		for (int i = 0; i < keypoints.size(); i++) {
			std::cout << "Point " << i << " x:" << keypoints[i].pt.x << " y:" << keypoints[i].pt.y << std::endl;
		}

		///** End Printing **/

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_keypoints;
		drawKeypoints(new_image, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


		imshow("Original Video", image); //show the frame in "Original Video" window
		imshow("Modified Video", new_image); //show the frame of which brightness increased
		imshow("Adjusted Modified Video", adj_new_image); //show the frame of which brightness increased
		imshow("Adjusted Modified Video Keypoints", im_with_keypoints);//show the frame with keypoints

		if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;

}

Point2f getCalibrationPoint() {

	deque<int> xQueue;
	deque<int> yQueue;

	Point2f point;

	int xAvg = 0;
	int yAvg = 0;

	VideoCapture cap(1); // open the video file for reading

	if (!cap.isOpened()) // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		Mat image = frame;
		Mat new_image;

		image.convertTo(new_image, -1, alpha, beta);

		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;

		// Change thresholds
		params.minThreshold = minThreshold;
		params.maxThreshold = maxThreshold;

		// Filter by Area.
		params.filterByArea = filterByArea;
		params.minArea = minArea;

		// Filter by Circularity
		params.filterByCircularity = filterByCircularity;
		params.minCircularity = minCircularity;

		// Filter by Convexity
		params.filterByConvexity = filterByConvexity;
		params.minConvexity = minConvexity;

		// Filter by Inertia
		params.filterByInertia = filterByInertia;
		params.minInertiaRatio = minInertiaRatio;

		//Filter by color
		params.filterByColor = filterByColor;
		params.blobColor = blobColor;

		//Create the simple blob detector
		//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
		cv::SimpleBlobDetector detector(params);

		// Storage for blobs
		std::vector<KeyPoint> keypoints;

		// Detect blobs
		detector.detect(new_image, keypoints);

		///** Printing **/

		std::cout << "Points: " << keypoints.size() << std::endl;
		for (int i = 0; i < keypoints.size(); i++) {
			std::cout << "Point " << i << " x:" << keypoints[i].pt.x << " y:" << keypoints[i].pt.y << std::endl;
		}

		///** End Printing **/
		if (keypoints.size() != 1) {
			cout << "Too many or zero points detected. Fix!" << endl;
		}
		else {
			if (xQueue.size() == 10) {
				xQueue.pop_back();
				xQueue.push_front(keypoints[0].pt.x);
				yQueue.pop_back();
				yQueue.push_front(keypoints[0].pt.y);
			}
			else {
				xQueue.push_front(keypoints[0].pt.x);
				yQueue.push_front(keypoints[0].pt.y);
			}
		}

		for (int i = 0; i < xQueue.size(); i++) {
			xAvg += xQueue[i];
			yAvg += yQueue[i];
		}

		if (!xQueue.empty()) {
			xAvg = xAvg / (xQueue.size() + 1);
			yAvg = yAvg / (yQueue.size() + 1);

			cout << "xAvg: " << xAvg << endl;
			cout << "yAvg: " << yAvg << endl;
			cout << "Esc on a window to accept:" << endl;
		}
		else {
			cout << "No input yet." << endl;
		}

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_keypoints;
		drawKeypoints(new_image, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		imshow("Original Video", frame); //show the frame in "Original Video" window
		imshow("Modified Video", new_image); //show the frame of which brightness increased
		imshow("Modified Video Keypoints", im_with_keypoints); //show the frame with keypoints

		if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	point.x = xAvg;
	point.y = yAvg;

	return point;
}

//vector<KeyPoint> getPoints() {
//
//  return 0;
//
//}

//
//// TrapezoidTransform.cpp : Defines the entry point for the console application.
////
//
//#include "opencv2\highgui\highgui.hpp"
//#include <iostream>
//#include <opencv2\opencv.hpp>
//#include <vector>
//#include <map>
//#include <iostream>
//#include <Windows.h>
//
//using namespace cv;
//using namespace std;
//
////int kernel_size = 3;
////int scale = 1;
////int delta = 0;
////int ddepth = CV_16S;
//
//double alpha = 1.0; // Contrast control
//double beta = 30.0; // Brightness control
//
//int main(int argc, char* argv[])
//{
//
//	VideoCapture cap(1); // open the video file for reading
//
//	if (!cap.isOpened()) // if not success, exit program
//	{
//		cout << "Cannot open the video file" << endl;
//		return -1;
//	}
//
//	namedWindow("Original Video", CV_WINDOW_AUTOSIZE);
//	namedWindow("Brightness/Contrast Modified Video", CV_WINDOW_AUTOSIZE);
//	namedWindow("Adjusted Modified Video", CV_WINDOW_AUTOSIZE);
//	namedWindow("Adjusted Modified Video w/ Keypoints", CV_WINDOW_AUTOSIZE);
//
//	double topLeft;
//	double topRight;
//	double bottomRight;
//	double bottomLeft;
//
//	/*std::cout << "Press enter to begin calibrating TL" << std::endl;
//	std::cin.get();
//	std::cout << "Press enter to begin calibrating TR" << std::endl;
//	std::cin.get();
//	std::cout << "Press enter to begin calibrating BR" << std::endl;
//	std::cin.get();
//	std::cout << "Press enter to begin calibrating BL" << std::endl;
//	std::cin.get();*/
//
//	Mat calibFrame;
//	bool bSuccess = cap.read(calibFrame); // read a new frame from video
//	if (!bSuccess) //if not success, break loop
//	{
//		cout << "Cannot read the frame from video file" << endl;
//	}
//
//	int height = calibFrame.rows;
//	int width = calibFrame.cols;
//
//	vector<Point2f> corners_fixed, corners_display;
//
//	corners_fixed.push_back(Point2f(0, 0));
//	corners_fixed.push_back(Point2f(width, 0));
//	corners_fixed.push_back(Point2f(width, height));
//	corners_fixed.push_back(Point2f(0, height));
//
//	corners_display.push_back(Point2f(38, 118));
//	corners_display.push_back(Point2f(590, 90));
//	corners_display.push_back(Point2f(575, 410));
//	corners_display.push_back(Point2f(82, 418));
//
//	Mat_<float> H_display_to_fixed = cv::getPerspectiveTransform(corners_fixed, corners_display);
//
//	cv::Mat_<float> mapx_32f(height, width), mapy_32f(height, width);
//	for (int y = 0; y<height; ++y)
//	{
//		float *buff_mapx = ((float*)mapx_32f.data) + y*width;
//		float *buff_mapy = ((float*)mapy_32f.data) + y*width;
//		for (int x = 0; x<width; ++x)
//		{
//			cv::Mat_<float> pt(3, 1);
//			pt(0) = x;
//			pt(1) = y;
//			pt(2) = 1;
//			pt = H_display_to_fixed*pt;
//			pt /= pt(2);
//			buff_mapx[x] = pt(0);
//			buff_mapy[x] = pt(1);
//		}
//	}
//	cv::Mat mapX_16u, mapY_16u;
//	cv::convertMaps(mapx_32f, mapy_32f, mapX_16u, mapY_16u, CV_16SC2);
//	// Keep map1_16u & map2_16u, discard the rest
//
//	/*Point2f pt1;
//	pt1.x = 180.5;
//	pt1.y = 285.4;
//
//	std::cout << "Original pointX: " << pt1.x << std::endl;
//	std::cout << "Original pointY: " << pt1.y << std::endl;
//
//	Point2f pt2;*/
//
//
//
//	while (1)
//	{
//		Mat frame;
//
//		bool bSuccess = cap.read(frame); // read a new frame from video
//
//		if (!bSuccess) //if not success, break loop
//		{
//			cout << "Cannot read the frame from video file" << endl;
//			break;
//		}
//
//		Mat image = frame;
//		Mat new_image;
//
//		alpha = 1.0; // Contrast control
//		beta = 30.0; // Brightness control
//		image.convertTo(new_image, -1, alpha, beta);
//
//		// Setup SimpleBlobDetector parameters.
//		SimpleBlobDetector::Params params;
//
//		// Change thresholds
//		params.minThreshold = 15;
//		params.maxThreshold = 255;
//
//		// Filter by Area.
//		params.filterByArea = true;
//		params.minArea = 20;
//
//		// Filter by Circularity
//		params.filterByCircularity = true;
//		params.minCircularity = 0.02;
//
//		// Filter by Convexity
//		params.filterByConvexity = true;
//		params.minConvexity = 0.01;
//
//		// Filter by Inertia
//		params.filterByInertia = true;
//		params.minInertiaRatio = 0.01;
//
//		//Filter by color
//		params.filterByColor = true;
//		params.blobColor = 255;
//
//		//Create the simple blob detector
//
//		//Worked in X64. error with create for opencv3.1
//		//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
//
//		cv::SimpleBlobDetector detector(params);
//
//		// Storage for blobs
//		std::vector<KeyPoint> keypoints;
//
//		//Adjust image so display occupies entire frame
//		Mat adjnew_image;
//		remap(new_image, adjnew_image, mapX_16u, mapY_16u, INTER_LINEAR);
//
//		///** Printing **/
//
//		//std::cout << "Points: " << keypoints.size() << std::endl;
//		//for (int i = 0; i < keypoints.size(); i++) {
//		//  std::cout << "Point " << i << " x:" << keypoints[i].pt.x << " y:" << keypoints[i].pt.y << std::endl;
//		//}
//
//		///** End Printing **/
//
//		// Draw detected blobs as red circles.
//		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
//		// the size of the circle corresponds to the size of blob
//
//		Mat im_with_keypoints;
//		drawKeypoints(new_image, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//
//
//		imshow("Original Video", frame); //show the frame in "Original Video" window
//		imshow("Brightness/Contrast Modified Video", new_image); //show the frame of which brightness increased
//		imshow("Adjusted Modified Video", adjnew_image); //show the frame of which brightness increased
//		imshow("Adjusted Modified Video w/ Keypoints", im_with_keypoints);//show the frame with keypoints
//
//		if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
//		{
//			cout << "esc key is pressed by user" << endl;
//			break;
//		}
//	}
//
//	return 0;
//
//}