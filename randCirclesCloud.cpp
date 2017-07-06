#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <string.h>

#define imagesize 512
#define num_of_points 100
#define big_constant 99999999
#define pi 3.14159265358979323846  /* pi */

using namespace std;
using namespace cv;
const Scalar White = cv::Scalar(255,255,255);

 
// uniform random number in [0,1]S
double Rand() { return  (double) rand() / RAND_MAX; }
// uniform random number in [-1,1]
double RandUnity() { return  2 * (double) rand() / RAND_MAX -1 ; }

struct Data {
    Point2d point;
    double GaussDensity;
};

void Bounding_Box (std::vector<Data>const& cloud, std::pair<Point2d, Point2d>& cloud_box ) {
    cloud_box.first = Point2d( big_constant, big_constant );
    cloud_box.second = Point2d( -big_constant, -big_constant );
    for ( auto p : cloud )   {
        if ( cloud_box.first.x > p.point.x ) cloud_box.first.x = p.point.x;
        if ( cloud_box.first.y > p.point.y ) cloud_box.first.y = p.point.y;
        if ( cloud_box.second.x < p.point.x ) cloud_box.second.x = p.point.x;
        if ( cloud_box.second.y < p.point.y ) cloud_box.second.y = p.point.y;
    }
}

void Scaling_Parameters (std::vector<Data>const& cloud, Point image_sizes, double& scale, Point2d& shift) {
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

void Draw_Cloud (std::vector<Data>const& cloud, int radius, Scalar color, Mat& image) {
    double R=(Rand()*255);
    double G=(Rand()*255);
    double B=(Rand()*255);
    double scale;
    Point2d shift;
    Scaling_Parameters( cloud, image.size(), scale, shift );
    for ( int i=0; i<(cloud.size()); i++){
		Point2d center = Point((scale*cloud[i].point) + shift);
        cv::circle( image, center, 1, CV_RGB(R,G,B), 1);
        }
    
    
} 


void Pertube_Cloud (std::vector<Data>& cloud,double noise_bound) {
	for ( int i = 0; i < (cloud.size()); i++) {
           cloud[i].point.x = (cloud[i].point.x + (noise_bound*Rand()) );
           cloud[i].point.y = (cloud[i].point.y + (noise_bound*Rand()) );
           cout << "Point after Pertubing is:" << cloud[i].point.x << " " << cloud[i].point.y << endl;
           }
           }

void Cloud_On_Circles(vector<Data>& cloud, int num_parts, int num_of_circles){
	int part=(100/num_parts);
	int cloudsize= cloud.size(); int f=0, alpha=1, mult_factor=1, check=(mult_factor*part); 
	for (int radius=1; radius <= num_of_circles; radius++){
	     	
	    for(int i=f; i<100; i++){
		double Angle = Rand()* 2 * pi;
		cloud[i].point.x =radius*(cos(Angle));
		cloud[i].point.y =radius*(sin(Angle));
		cout<<"Angle generated points are:"<<cloud[i].point.x<<" "<<cloud[i].point.y<<endl;
	    cout<<i<<endl;
	     if(i==check) {
	      if(radius<num_of_circles) {
	      f=(check+1); mult_factor++; alpha++; check+=(part*alpha); break; }
	     else { continue; }
	     }
	     }
	     cout<<radius<<endl;
	      	     
	     }
	     }
	 
int main(){
	Mat image = Mat::zeros(imagesize, imagesize, CV_8UC3);
	image = cv::Scalar(0,0,0);
	double noisebound=0.1;
	int sum=0;
	cout<<"Enter the number of circles"<<endl;
	int num_of_circles; cin>>num_of_circles;
	for(int z=num_of_circles; z>=1; z--){sum+=z; //cout<<sum<<endl; 
	}
	vector<Data> cloud(num_of_points);
	
	Cloud_On_Circles(cloud, sum, num_of_circles);
	Pertube_Cloud(cloud, 0.1);
    Draw_Cloud(cloud, num_of_circles, White, image); 

    String perturbedCloud = "ConcentricCloud";
	cv::imwrite(perturbedCloud+".png", image);
    return 0;
}
