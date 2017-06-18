#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

#define w 400
#define pi 3.14159265358979323846  /* pi */

using namespace std;
using namespace cv;

struct dataPoint {
    Point2d pt;
    double GaussDensity;
    double eccentricity;
};

double distanceBetweenCoordinates(dataPoint pt1, dataPoint pt2){
	double distanceX = (pt1.pt.x-pt2.pt.x)*(pt1.pt.x-pt2.pt.x);
	double distanceY = (pt1.pt.y-pt2.pt.y)*(pt1.pt.y-pt2.pt.y);
	double distance = sqrt(distanceX - distanceY);
	return distance;
}

double calculateGaussDensity(vector<dataPoint> points, double sigma){
	unsigned long long int N = points.size();
	for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
		double summation = 0;
		for(std::vector<dataPoint>::iterator jt = points.begin(); jt != points.end(); ++jt) {
			summation += exp((-1)*(pow(distanceBetweenCoordinates(*it, *jt),2.0)/2*(pow(sigma,2))));
		}
		dataPoint currentVector = *it;
		currentVector.GaussDensity = summation/(N*(pow((sqrt(2*pi)*sigma),2)));
		*it = currentVector;
	}
}

double calculateEccentricity(vector<dataPoint> points){
	unsigned long long int N = points.size();
	float exponent = 1.0;
	for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
		double summation = 0;
		for(std::vector<dataPoint>::iterator jt = points.begin(); jt != points.end(); ++jt) {
			summation += distanceBetweenCoordinates(*it, *jt);
			dataPoint currentVector = *it;
			currentVector.eccentricity = summation/N;
			*it = currentVector;
		}
	}
}
	
int main(){
    
    char window[] = "Plotting Points";
    Mat mapperImage = Mat::zeros(w, w, CV_8UC3);
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

    while (input >> myPoint.pt.x >>ch>>myPoint.pt.y){
        //myPoint.GaussDensity = 1;
        points.push_back(myPoint);
        cout << myPoint.pt.x << " " << myPoint.pt.y << endl;
        //center = Point(myPoint.pt.x, myPoint.pt.x);
        //circle(mapperImage, center, 1, CV_RGB(255, 0, 0), 3);
        
    }
    calculateEccentricity(points);
    calculateGaussDensity(points, 1.0);
    for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
		dataPoint currentVector = *it;
		cout<<currentVector.GaussDensity<<" "<<currentVector.eccentricity<<endl;
	}
    

    //imshow(window, mapperImage);
    waitKey(1000000);
    return 0;
}
