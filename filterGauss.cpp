#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

#define pi 3.14159265358979323846 /* pi */

using namespace std;
using namespace cv;

struct dataPoint {
  Point2d pt;
  double GaussDensity;
};

void write_image(String const& name, Mat const& mapperImage) {
  try {
    cv::imwrite(name, mapperImage);
  } catch (std::runtime_error& ex) {
    fprintf(stderr, "Error in writing the image\n", ex.what());
  }
}

double distanceBetweenCoordinates(dataPoint pt1, dataPoint pt2) {
  double distanceX = (pt1.pt.x - pt2.pt.x) * (pt1.pt.x - pt2.pt.x);
  double distanceY = (pt1.pt.y - pt2.pt.y) * (pt1.pt.y - pt2.pt.y);
  double distance = sqrt(distanceX + distanceY);
  return distance;
}

void calculateGaussDensity(vector<dataPoint>& points, double sigma) {
  unsigned long long int N = points.size();
  for (vector<dataPoint>::iterator it = points.begin(); it != points.end();
       ++it) {
    double summation = 0;
    for (vector<dataPoint>::iterator jt = points.begin(); jt != points.end();
         ++jt) {
      summation += exp((-1) * (pow(distanceBetweenCoordinates(*it, *jt), 2.0) /
                               (2 * (pow(sigma, 2)))));
    }
    dataPoint currentVector = *it;
    currentVector.GaussDensity =
        summation / (N * (pow((sqrt(2 * pi) * sigma), 2)));
    *it = currentVector;
  }
}

/*
double calculateEccentricity(const vector <dataPoint> &points){
        unsigned long long int N = points.size();
        float exponent = 1.0;
        for(std::vector<dataPoint>::iterator it = points.begin(); it !=
points.end(); ++it) {
                double summation = 0;
                for(std::vector<dataPoint>::iterator jt = points.begin(); jt !=
points.end(); ++jt) {
                        summation += distanceBetweenCoordinates(*it, *jt);
                        dataPoint currentVector = *it;
                        currentVector.eccentricity = summation/N;
                        *it = currentVector;
                }
        }
}
*/

void readPoints(vector<dataPoint>& points) {
  ifstream input;
  string filename;
  cout << "Enter the file name :" << endl;
  cin >> filename;
  input.open(filename.c_str());
  char ch;
  Point2d center;
  dataPoint myPoint;
  while (input.fail()) {
    input.clear();
    cout << "Incorrect filename, please enter again" << endl;
    cin >> filename;
    input.open(filename.c_str());
  }
  while (input >> myPoint.pt.x >> ch >> myPoint.pt.y) {
    myPoint.GaussDensity = 1;
    points.push_back(myPoint);
    cout << myPoint.pt.x << " " << myPoint.pt.y << endl;
    // center = Point(myPoint.pt.x, myPoint.pt.x);
    // circle(mapperImage, center, 1, CV_RGB(255, 0, 0), 3);
  }
  // cout<<"Size of the vector:"<<points.size()<<endl;
}

bool sortByGauss(const pair<double, Point2d>& a,
                 const pair<double, Point2d>& b) {
  return (a.first < b.first);


void colormap(std::vector<dataPoint>& points, double scalingFactor, int imageSize)
{
    dataPoint myPoint;
    double lBound,rBound,t,R,G,B;
    long iter_var=0;
    Vec3b color_lBound,color_rBound,computed_color;
    Point2d center;
    Mat coloredimage = Mat::zeros(imageSize, imageSize, CV_8UC3);
	coloredimage = cv::Scalar(255,255,255);
    vector<Vec3b> colormap;
    colormap.push_back(Vec3b(255,0,0)); //blue
    colormap.push_back(Vec3b(0,255,0)); //green
    colormap.push_back(Vec3b(0,255,255));//yellow
    colormap.push_back(Vec3b(0,0,255));//red
    int pointssize=points.size();
    int colorsize=colormap.size()-1;
    float fit=(float)pointssize/(float)colorsize;
    int corrected_fit=ceil((float)fit);
    vector< pair<double, Point2d> > copyvec;
    for(vector<dataPoint>::iterator it=points.begin(); it!= points.end(); it++) {
		dataPoint copyPoint = *it;
		copyvec.push_back( make_pair(copyPoint.GaussDensity, copyPoint.pt));
	}
	sort(copyvec.begin(), copyvec.end(), sortByGauss);
	cout<<"Testing whether the sorting works"<<endl;
	int copy_var = 0;
	for(vector<dataPoint>::iterator it=points.begin(); it!= points.end(); it++) {
	cout<<copyvec[copy_var].first<<" "<<endl;
	cout<<"x "<<copyvec[copy_var].second.x<<"y "<<copyvec[copy_var].second.y<<endl;
		copy_var++;
	}
    //std::sort(points.begin(), points.end());
    for(vector< pair<double, Point2d> >::iterator it=copyvec.begin(); it!= copyvec.end(); it++) {
       pair<double, Point2d> myPoint= *it;
        if(iter_var%corrected_fit==0) {
            lBound=copyvec[iter_var].first;
        //  cout<< lBound <<endl;
                if((iter_var+corrected_fit)<points.size()) {
                rBound=copyvec[(iter_var+(corrected_fit-1))].first; }
            else { rBound=copyvec[(points.size()-1)].first; }
        }
            cout<<lBound<<" to "<<rBound<<endl;
            if(rBound-lBound!=0) {
			t=(copyvec[iter_var].first-lBound)/(rBound-lBound);
             }
			else { cout<<"Couldn't be calculated for this one"; t=0;  }
                     if(iter_var%corrected_fit==0) {
            color_lBound=colormap[iter_var/corrected_fit];
             if(((iter_var/corrected_fit)+1)<colormap.size()) {
                 color_rBound=colormap[((iter_var/corrected_fit)+1)]; }
             else { color_rBound=colormap[(colormap.size()-1)]; } }
            cout<<"t "<<t<<endl;
		    computed_color=(((1-t)*color_lBound)+(t*color_rBound));
            cout<<computed_color<<endl;
            cout<<"R G B "<<R<<" "<<G<<" "<<B<<endl;
            center=Point((scalingFactor*myPoint.second.x),(scalingFactor*myPoint.second.y));
            circle(coloredimage, center, 2, Scalar(computed_color), 2); 
            iter_var++;
		        String coloredCloud = "Colored Cloud";
                write_image(coloredCloud+".png", coloredimage);
              }


void drawingCloud(float scalingFactor,
                  vector<dataPoint> copyPoints,
                  int imageSize) {
  Mat mapperImage = Mat::zeros(imageSize, imageSize, CV_8UC3);
  mapperImage = cv::Scalar(255, 255, 255);
  dataPoint currentVector;
  Point2d centre;
  for (std::vector<dataPoint>::iterator it = copyPoints.begin();
       it != copyPoints.end(); ++it) {
    currentVector = *it;
    currentVector.pt.x *= scalingFactor;
    currentVector.pt.y *= scalingFactor;
    cout << currentVector.pt.x << " " << currentVector.pt.y << endl;
    
    centre = Point(currentVector.pt.x, currentVector.pt.y);
    circle(mapperImage, centre, 1, CV_RGB(255, 0, 0), 3);
  }
  write_image("scaledCloud.png", mapperImage);
}

void scalingCloud(vector<dataPoint>& points, int size) {
  vector<dataPoint> copyPoints(points);
  dataPoint currentVector;
  float x_min, y_min = +9999999999999.00;
  float x_max, y_max = -9999999999999.00;
  for (std::vector<dataPoint>::iterator it = copyPoints.begin();
       it != copyPoints.end(); ++it) {
    currentVector = *it;
    if (currentVector.pt.x < x_min)
      x_min = currentVector.pt.x;
    if (currentVector.pt.x > x_max)
      x_max = currentVector.pt.x;
    if (currentVector.pt.y < y_min)
      y_min = currentVector.pt.y;
    if (currentVector.pt.y > y_max)
      y_max = currentVector.pt.y;
  }
  float scalingFactor = max(x_max - x_min, y_max - y_min);
  scalingFactor = size / scalingFactor;
  drawingCloud(scalingFactor, copyPoints, size);
  colormap(copyPoints, scalingFactor, size);
}

// uniform random number in [0,1]
double Rand() {
  return (double)rand() / RAND_MAX;
}
// uniform random number in [-1,1]
double RandUnity() {
  return 2 * (double)rand() / RAND_MAX - 1;
}

int main() {
  vector<dataPoint> points;
  readPoints(points);  // input taken as a point cloud.
  // calculateEccentricity(points);
  calculateGaussDensity(points, 7.0);
  String scaledCloud = "test";
  cout << "Enter the size of the image to be saved" << endl;
  int imageSize;
  cin >> imageSize;
  scalingCloud(points, imageSize);

  return 0;
}
