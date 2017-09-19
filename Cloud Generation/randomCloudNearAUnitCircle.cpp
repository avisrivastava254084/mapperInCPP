#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

using namespace std;
using namespace cv;

#define pi 3.14159265358979323846  /* pi */

// uniform random number in [0,1]
double Rand() { return  (double) rand() / RAND_MAX; }
// uniform random number in [-1,1]
double RandUnity() { return  2 * (double) rand() / RAND_MAX -1 ; }

struct dataPoint {
    Point2d pt;
    double GaussDensity;
};

double scalingCloud(vector <dataPoint>& points, int size){
	vector<dataPoint> copyPoints(points);
	dataPoint currentVector;
	float x_min, y_min = +9999999999999.00;
	float x_max, y_max = -9999999999999.00;
	for(std::vector<dataPoint>::iterator it = copyPoints.begin(); it != copyPoints.end(); ++it) {
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
	double scalingFactor = max(x_max-x_min, y_max-y_min);
	scalingFactor = size/scalingFactor;
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

void Perturb_Cloud (double noise_bound, std::vector<dataPoint>& cloud, double scalingFactor, int imageSize) {
	Mat mapperImage = Mat::zeros(imageSize, imageSize, CV_8UC3);
	mapperImage = cv::Scalar(255,255,255);
	Point2d center;
	cout<<"The image size ka half is:"<<(imageSize/2)<<endl;
    for ( int i = 0; i < cloud.size(); i++) {
           cloud[i].pt.x = (((cloud[i].pt.x + (noise_bound * Rand()))*scalingFactor) + imageSize/2);
           cloud[i].pt.y = (((cloud[i].pt.y + (noise_bound * Rand()))*scalingFactor) + imageSize/2);
           cout << "Final Point is:" << cloud[i].pt.x << " " << cloud[i].pt.y << endl;
           center = Point(cloud[i].pt.x,cloud[i].pt.y);
           circle(mapperImage, center, 1, CV_RGB(255,0,0), 3);
    }
    String perturbedCloud = "testPerturbedCloud";
    write_image(perturbedCloud+".png", mapperImage);
}

void randomCloudNearCircle(double noiseBound, int imageSize, int cloudSize){
	//selecting a random angle for the complete cloud//
	vector<dataPoint> cloud(cloudSize);
	for(int i = 0; i<cloudSize; i++){
		double shiftAngle = Rand()* 2 * pi;
		cloud[i].pt.x = cos(shiftAngle);
		cloud[i].pt.y = sin(shiftAngle);
		cout<<"Angle generated points are:"<<cloud[i].pt.x<<" "<<cloud[i].pt.y<<endl;
	}
	double scalingFactor = scalingCloud(cloud,imageSize);
	Perturb_Cloud(noiseBound, cloud, scalingFactor, imageSize);
}

int main(){
	cout<<"Enter the image's Size."<<endl;
	int imageSize; cin>>imageSize;
	cout<<"Enter the cloud size"<<endl;
	int cloudSize; cin >> cloudSize; 
	double noiseBound = 0.1;
    randomCloudNearCircle(noiseBound, imageSize, cloudSize);
    return 0;
}
