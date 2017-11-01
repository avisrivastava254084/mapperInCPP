#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

using namespace std;
using namespace cv;
#define imagesize 512
const Scalar White = cv::Scalar(255,255,255);
#define big_constant 99999999
#define pi 3.14159265358979323846  /* pi */
#define num_of_points 100
// uniform random number in [0,1]
double Rand() { return (double) rand() / RAND_MAX; }


struct dataPoint {
    Point2d pt;
    double GaussDensity;
};

void Bounding_Box (std::vector<dataPoint>const& cloud, std::pair<Point2d, Point2d>& cloud_box ) {
    cloud_box.first = Point2d( big_constant, big_constant );
    cloud_box.second = Point2d( -big_constant, -big_constant );
    for ( auto p : cloud )   {
        if ( cloud_box.first.x > p.pt.x ) cloud_box.first.x = p.pt.x;
        if ( cloud_box.first.y > p.pt.y ) cloud_box.first.y = p.pt.y;
        if ( cloud_box.second.x < p.pt.x ) cloud_box.second.x = p.pt.x;
        if ( cloud_box.second.y < p.pt.y ) cloud_box.second.y = p.pt.y;
    }
}
void Scaling_Parameters (std::vector<dataPoint>const& cloud, Point image_sizes, double& scale, Point2d& shift) {
    std::pair<Point2d, Point2d> box;
    Bounding_Box( cloud, box );
    Point2d cloud_sizes( box.second.x - box.first.x, box.second.y - box.first.y );
    Point2d cloud_center( box.second.x + box.first.x, box.second.y + box.first.y );
    cloud_center /= 2;
    Point2d image_center( image_sizes.x / 2, image_sizes.y / 2 );
    scale = std::min( image_sizes.x / cloud_sizes.x, image_sizes.y / cloud_sizes.y );
    //point =( scale * (( point - cloud_center ) + image_center));
    shift = image_center - (scale * cloud_center);  
}
void write_image(String const& name, Mat const& mapperImage){

	try{
		cv::imwrite( name, mapperImage ); 
	}
    catch (std::runtime_error& ex) {
		fprintf(stderr, "Error in writing the image\n", ex.what()); 
	}
}


void Draw_Cloud (std::vector<dataPoint>const& cloud,Mat& image,int major_axis,int minor_axis) 
{
    double R=(Rand()*255);
    double G=(Rand()*255);
    double B=(Rand()*255);
    double scale;
    Point2d shift;
    Scaling_Parameters( cloud, image.size(), scale, shift );
    for ( int i=0; i<(cloud.size()); i++){
		Point2d center = Point((scale*cloud[i].pt) + shift);
        ellipse(image,center,Size(major_axis,minor_axis),45,0,360,CV_RGB(R,G,B),1,8);
        }
} 

void Pertube_Cloud (std::vector<dataPoint>& cloud,double noise_bound) {
	for ( int i = 0; i < (cloud.size()); i++) {
           cloud[i].pt.x = (cloud[i].pt.x + (noise_bound*Rand()) );
           cloud[i].pt.y = (cloud[i].pt.y + (noise_bound*Rand()) );
           cout << "Point after Pertubing is:" << cloud[i].pt.x << " " << cloud[i].pt.y << endl;
           }
           }

void disjont_ellipse(int imageSize,int cloudSize,vector<dataPoint>& cloud1,vector<dataPoint>& cloud2,int major_axis1,int minor_axis1,int major_axis2,int minor_axis2)
{	
	for(int i = 0; i<cloudSize; i++){
		double shiftAngle1 = Rand()* 2 * pi;
		double shiftAngle2 = Rand()* 2 * pi;
		cloud1[i].pt.x = major_axis1*cos(shiftAngle1);
		cloud1[i].pt.y = minor_axis1*sin(shiftAngle1);
		cloud2[i].pt.x = major_axis1 + major_axis2*cos(shiftAngle2);
		cloud2[i].pt.y = major_axis1 + minor_axis2*sin(shiftAngle2);
		cout<<"Angle generated points are:"<<cloud1[i].pt.x<<" "<<cloud1[i].pt.y<<endl;
		cout<<"Angle generated points are:"<<cloud2[i].pt.x<<" "<<cloud2[i].pt.y<<endl;
	}
}

int main()
{
	srand(time(NULL));
	Mat image = Mat::zeros(imagesize, imagesize, CV_8UC3);
	image = cv::Scalar(0,0,0);
	double noisebound=0.1;
	int cloudSize; cin >> cloudSize;
	int major_axis1; cin >> major_axis1;
	int major_axis2; cin >> major_axis2;
	int minor_axis1; cin >> minor_axis1;
	int minor_axis2; cin >> minor_axis2; 
	double noiseBound = 0.1;
	vector<dataPoint> cloud1(num_of_points);
	vector<dataPoint> cloud2(num_of_points);	
    disjont_ellipse(imagesize,cloudSize,cloud1,cloud2,major_axis1,minor_axis1,major_axis2,minor_axis2);
    Pertube_Cloud(cloud1,noisebound);
    Pertube_Cloud(cloud2,noisebound);
    Draw_Cloud(cloud1,image,major_axis1,minor_axis1);
	Draw_Cloud(cloud2,image,major_axis2,minor_axis2);
	String perturbedCloud = "ConcentricCloud";
	cv::imwrite(perturbedCloud+".png", image);
	return 0;
}