// icp.cpp 
// Author: JJ

// C++ 
#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <numeric>

// OpenCV 
#include <opencv2\opencv.hpp>
#include <opencv2\objdetect\objdetect.hpp>

// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 

// GLFW toolkit
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// OpenGL textures
#include <gli/gli.hpp>

//project includes...
#include "globals.h"
#include "init.h"
#include "callbacks.h"
#include "glerror.h" // Check for GL errors

#include "lua_engine.h"
#include "lua_interface.h"
#include "deq.hpp"

using namespace cv;
using namespace std;

atomic<bool> run;

synced_deque<Point2f> pointQueue = synced_deque<Point2f>(100);
synced_deque<Mat> sceneQueue = synced_deque<Mat>(100);

void draw_cross_relative(Mat& img, const Point2f center_relative, const int size)
{
	Point p;
	p.x = center_relative.x;
	p.y = center_relative.y;

	Point p1 = cv::Point(p.x - size / 2, p.y);
	Point p2 = cv::Point(p.x + size / 2, p.y);
	Point p3 = cv::Point(p.x, p.y - size / 2);
	Point p4 = cv::Point(p.x, p.y + size / 2);

	line(img, p1, p2, CV_RGB(255, 0, 0), 3);
	line(img, p3, p4, CV_RGB(255, 0, 0), 3);
}

cv::Point2f centroid_nonzero(Mat& scene, Scalar& lower_threshold, Scalar& upper_threshold)
{
	Mat scene_hsv;
	cvtColor(scene, scene_hsv, COLOR_BGR2HSV);

	Mat scene_threshold;
	inRange(scene_hsv, lower_threshold, upper_threshold, scene_threshold);

	vector<Point> whitePixels;
	findNonZero(scene_threshold, whitePixels);
	int whiteCnt = whitePixels.size();

	Point whiteAccum = std::accumulate(whitePixels.begin(), whitePixels.end(), cv::Point(0.0, 0.0));

	Point2f centroid_relative(0.0f, 0.0f);
	if (whiteCnt > 0)
	{
		Point centroid = { whiteAccum.x / whiteCnt, whiteAccum.y / whiteCnt };
		centroid_relative = { static_cast<float>(centroid.x), static_cast<float>(centroid.y) };
	}

	return centroid_relative;
}



static void init(void)
{
	globals.capture = cv::VideoCapture(cv::CAP_DSHOW);

	if (!globals.capture.isOpened())
	{
		cerr << "no camera" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "Camera " <<
			": width=" << globals.capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
			", height=" << globals.capture.get(cv::CAP_PROP_FRAME_HEIGHT) <<
			", FPS=" << globals.capture.get(cv::CAP_PROP_FPS) << std::endl;
	}

	if (!globals.capture.set(cv::CAP_PROP_FRAME_WIDTH, 640))
		std::cout << "Failed width." << std::endl;
	if (!globals.capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480))
		std::cout << "Failed height." << std::endl;
	if (!globals.capture.set(cv::CAP_PROP_FPS, 30))
		std::cout << "Failed FPS." << std::endl;

	std::cout << "Camera changed:" <<
		": width=" << globals.capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
		", height=" << globals.capture.get(cv::CAP_PROP_FRAME_HEIGHT) <<
		", FPS=" << globals.capture.get(cv::CAP_PROP_FPS) << std::endl;
}

void trackObject() {

	Mat frame, scene, frameCopy;
	Scalar lower_threshold = Scalar(50, 50, 50);
	Scalar upper_threshold = Scalar(70, 255, 255);

	init();

	while (run)
	{
		globals.capture.read(frame);
		if (frame.empty())
		{
			cerr << "Cam disconnected?" << endl;
			break;
		}

		frame.copyTo(frameCopy);
		auto centroid_1 = centroid_nonzero(frameCopy, lower_threshold, upper_threshold);
		sceneQueue.push_front(frameCopy);
		pointQueue.push_front(centroid_1);
	}
}
//---------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------
int main(int argc, char* argv[])
{

	run = true;
	thread thr(trackObject);

	while (true)
	{
		pointQueue.wait();
		Mat scene = sceneQueue.pop_front();
		draw_cross_relative(scene, pointQueue.pop_front(), 30);
		imshow("scene1", scene);
		if (cv::waitKey(1) == 27)
			break;
	}
	run = false;
	thr.join();
	if (globals.capture.isOpened())
		globals.capture.release();

	exit(EXIT_SUCCESS);
}


//
//PLEASE IGNORE NOW: callbacks
//
void error_callback(int error, const char* description) {};
void fbsize_callback(GLFWwindow* window, int width, int height) {};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {};
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {};
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {};
