#include <fstream>
#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#define w 400

#ifdef _WIN32
#define WINPAUSE system("pause")
#endif

using namespace std;
using namespace cv;

struct dataPoint {
	Point2d pt;
	double GaussDensity;
};

int main(){
	
	char window[] = "Plotting Points";
	cv::Mat mapperImage;
	mapperImage = cv::Mat::zeros(w, w, CV_8UC3);
	ifstream input;
	string filename;
	cout << "Enter the file name :" << endl;
	cin >> filename;
	input.open(filename.c_str());
	char ch;
	Point2d center;
	dataPoint myPoint;
	while (input.fail()){
		input.clear();
		cout << "Incorrect filename, please enter again" << endl;
		cin >> filename;
		input.open(filename.c_str());
	}

	vector<dataPoint> points;

	while (input >> myPoint.pt.x >> myPoint.pt.y){
		myPoint.GaussDensity = 1;
		points.push_back(myPoint);
		cout << myPoint.pt.x << " " << myPoint.pt.y << endl;
		center = Point(myPoint.pt.x, myPoint.pt.x);
		circle(mapperImage, center, 1, CV_RGB(255, 0, 0), 3);
	}
	imshow(window, mapperImage);
	WINPAUSE;
	return 0;
}
