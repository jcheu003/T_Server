#ifndef INCLUDED_T_Server_H
#define INCLUDED_T_Server_H

//OPENCV SECTION

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

//TUIO SECTION

#include "TuioServer.h"
#include "TuioCursor.h"
#include "osc/OscTypes.h"
#include <list>
#include <math.h>

#include "FlashSender.h"
#include "TcpSender.h"
#include "WebSockSender.h"

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include "SDL.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

using namespace TUIO;

class T_Server {

public:
	T_Server(TuioServer *server);
	~T_Server() {};

	void run(int size, int history, float x, float y);
	TuioServer *tuioServer;
	std::list<TuioCursor*> activeCursorList;
	std::list<TuioCursor*> stickyCursorList;
	std::list<TuioCursor*> jointCursorList;

private:
	void processEvents(int size, int history, float x, float y);

	bool verbose, fullupdate, periodic, fullscreen, running, help;

	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;
	TuioTime frameTime;

	void mousePressed(float x, float y);
	void mouseReleased(float x, float y);
	void mouseDragged(float x, float y);

	//int s_id;
};



#endif /* INCLUDED_T_Server_H */