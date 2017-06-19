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
};

double distanceBetweenCoordinates(dataPoint pt1, dataPoint pt2){
	double distanceX = (pt1.pt.x-pt2.pt.x)*(pt1.pt.x-pt2.pt.x);
	double distanceY = (pt1.pt.y-pt2.pt.y)*(pt1.pt.y-pt2.pt.y);
	double distance = sqrt(distanceX - distanceY);
	return distance;
}

double calculateGaussDensity(vector <dataPoint> points, double sigma){
	unsigned long long int N = points.size();
	for(vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
		double summation = 0;
		for(vector<dataPoint>::iterator jt = points.begin(); jt != points.end(); ++jt) {
			summation += exp((-1)*(pow(distanceBetweenCoordinates(*it, *jt),2.0)/2*(pow(sigma,2))));
		}
		dataPoint currentVector = *it;
		currentVector.GaussDensity = summation/(N*(pow((sqrt(2*pi)*sigma),2)));
		*it = currentVector;
	}
}

/*
double calculateEccentricity(const vector <dataPoint> &points){
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
*/	
void readPoints(vector <dataPoint>& points){
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
    while (input >> myPoint.pt.x >>ch>>myPoint.pt.y){
        myPoint.GaussDensity = 1;
        points.push_back(myPoint);
        cout << myPoint.pt.x << " " << myPoint.pt.y << endl;
        //center = Point(myPoint.pt.x, myPoint.pt.x);
        //circle(mapperImage, center, 1, CV_RGB(255, 0, 0), 3);
     }
     //cout<<"Size of the vector:"<<points.size()<<endl;
}

double scalingCloud(vector <dataPoint> points, int size){
	dataPoint currentVector;
	float x_min, y_min = +9999999999999.00;
	float x_max, y_max = -9999999999999.00;
	for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
		currentVector = *it;
		if(currentVector.pt.x < x_min)
			x_min = currentVector.pt.x;
		if(currentVector.pt.x > x_max)
			x_max = currentVector.pt.x;
		if(currentVector.pt.y < y_min)
			y_min = currentVector.pt.y;
		if(currentVector.pt.y > y_max)
			y_max = currentVector.pt.y;
	}
	float scalingFactor = max(x_max-x_min, y_max-y_min);
	scalingFactor /= size;
	return scalingFactor;
}

void write_image(String const& name, Mat const& mapperImage){

	try{
		cv::imwrite( name, mapperImage ); 
	}
    catch (std::runtime_error& ex) {
		fprintf(stderr, "Error in writing the image\n", ex.what()); 
	}
}

int main(){
    vector<dataPoint> points;
    Point center;
    readPoints(points); //input taken as a point cloud.
	//calculateEccentricity(points);
    calculateGaussDensity(points, 1.0);
    char window[] = "Scaled Cloud";
    String scaledCloud = "test";
    Mat mapperImage = Mat::zeros(w, w, CV_8UC3);
    cout<<"Enter the size of the image to be saved"<<endl;
    int size;
    cin>>size;
    double scalingFactor = scalingCloud(points, size);
    cout<<"The scaling factor is:"<<scalingFactor<<endl;
    dataPoint currentVector;
    cout<<"The size of the input cloud is:"<<points.size()<<endl;
    for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
		currentVector = *it;
		currentVector.pt.x *= scalingFactor;
		currentVector.pt.y *= scalingFactor;
		cout<<currentVector.pt.x<<" "<<currentVector.pt.y<<endl;;
		center = Point(currentVector.pt.x, currentVector.pt.y);
		circle(mapperImage, center, 1, CV_RGB(255,0,0), 3);
    }
    //imshow(window, mapperImage);
    //waitKey(10000000);
    write_image(scaledCloud+".png", mapperImage);
	return 0;
}
