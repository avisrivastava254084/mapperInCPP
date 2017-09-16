#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <string.h>

#define imagesize 512
#define num_of_points 1000
#define big_constant 99999999
#define pi 3.14159265358979323846  /* pi */

using namespace std;
using namespace cv;
const Scalar White = cv::Scalar(255,255,255);

 
// uniform random number in [0,1]S
double Rand() { return  (double) rand() / RAND_MAX; }
// uniform random number in [-1,1]
double RandUnity() { return  2 * (double) rand() / RAND_MAX -1 ; }

struct Circle {
    Point2d point;
    Point2d center;
    double radius;
    double GaussDensity;
};

double distanceBetweenCoordinates(Circle point1, Circle point2){
	double distanceX = (point1.point.x-point2.point.x)*(point1.point.x-point2.point.x);
	double distanceY = (point1.point.y-point2.point.y)*(point1.point.y-point2.point.y);
	double distance = sqrt(distanceX + distanceY);
	return distance;
}

void calculateGaussDensity(vector <Circle>& cloud, double sigma){
	unsigned long long int N = cloud.size();
	for(vector<Circle>::iterator it = cloud.begin(); it != cloud.end(); ++it) {
		double summation = 0;
		for(vector<Circle>::iterator jt = cloud.begin(); jt != cloud.end(); ++jt) {
			summation += exp((-1)*(pow(distanceBetweenCoordinates(*it, *jt),2.0)/(2*(pow(sigma,2)))));
		}
		Circle currentVector = *it;
		currentVector.GaussDensity = summation/(N*(pow((sqrt(2*pi)*sigma),2)));
		*it = currentVector;
	}
}
	
	
	
	
	



void Bounding_Box (std::vector<Circle>const& cloud, std::pair<Point2d, Point2d>& cloud_box ) {
    cloud_box.first = Point2d( big_constant, big_constant );
    cloud_box.second = Point2d( -big_constant, -big_constant );
    for ( auto p : cloud )   {
        if ( cloud_box.first.x > p.point.x ) cloud_box.first.x = p.point.x;
        if ( cloud_box.first.y > p.point.y ) cloud_box.first.y = p.point.y;
        if ( cloud_box.second.x < p.point.x ) cloud_box.second.x = p.point.x;
        if ( cloud_box.second.y < p.point.y ) cloud_box.second.y = p.point.y;
    }
}

void Scaling_Parameters (std::vector<Circle>const& cloud, Point image_sizes, double& scale, Point2d& shift) {
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


bool sortByGauss(const pair<double,Point2d> &a,const pair<double,Point2d> &b)
{
return (a.first < b.first);
}
void colormap(vector<Circle>& cloud)
{
	double scale; Point2d shift;
	Scaling_Parameters(cloud, Point(imagesize, imagesize), scale, shift);
    Circle myPoint;
    double lBound,rBound,t,R,G,B;
    long i=0;
    Vec3b clBound,crBound,computed_color;
    Point2d center;
    Mat fdpImage = Mat::zeros(imagesize, imagesize, CV_8UC3);
	fdpImage = cv::Scalar(0,0,0);
    vector<Vec3b> colormap;
    colormap.push_back(Vec3b(255,0,0));
    colormap.push_back(Vec3b(0,255,0));
    colormap.push_back(Vec3b(0,255,255));
    colormap.push_back(Vec3b(0,0,255));
    int pointssize=cloud.size();
    int colorsize=colormap.size()-1;
    float fit=(float)pointssize/(float)colorsize;
    int ffit=ceil((float)fit);
    vector< pair<double, Point2d> > fdp;
    for(vector<Circle>::iterator it= cloud.begin(); it!= cloud.end(); it++) {
		Circle copyPoint = *it;
		fdp.push_back( make_pair(copyPoint.GaussDensity, copyPoint.point));
	}
	sort(fdp.begin(), fdp.end(), sortByGauss);
	cout<<"Testing whether the sorting works"<<endl;
	int iAvi = 0;
	for(vector<Circle>::iterator it= cloud.begin(); it!= cloud.end(); it++) {
	cout<<fdp[iAvi].first<<" "<<endl;
	cout<<"x "<<fdp[iAvi].second.x<<"y "<<fdp[iAvi].second.y<<endl;
		iAvi++;
	}
    //std::sort(points.begin(), points.end());
    for(vector< pair<double, Point2d> >::iterator it=fdp.begin(); it!= fdp.end(); it++) {
       pair<double, Point2d> myPoint= *it;
        if(i%ffit==0) {
            lBound=fdp[i].first;
        //  cout<< lBound <<endl;
                if((i+ffit)<cloud.size()) {
                rBound=fdp[(i+(ffit-1))].first; }
            else { rBound=fdp[(cloud.size()-1)].first; }
        }
            cout<<lBound<<" to "<<rBound<<endl;
            if(rBound-lBound!=0) {
			t=(fdp[i].first-lBound)/(rBound-lBound);
             }
			else { t=0; }
                     if(i%ffit==0) {
            clBound=colormap[i/ffit];
             if(((i/ffit)+1)<colormap.size()) {
                 crBound=colormap[((i/ffit)+1)]; }
             else { crBound=colormap[(colormap.size()-1)]; } }
            cout<<"t "<<t<<endl;
		    computed_color=(((1-t)*clBound)+(t*crBound));
            cout<<computed_color<<endl;
            double R = computed_color[2];
            double G = computed_color[1];
            double B = computed_color[0];
            cout<<"R G B "<<R<<" "<<G<<" "<<B<<endl;
            center=Point((scale*myPoint.second) + shift);
            circle(fdpImage, center, 1, CV_RGB(R,G,B), 1); 
            i++;
		        
                }
                String coloredCloud = "conc.coloredCloud";
                cv::imwrite(coloredCloud+".png", fdpImage);
}



    

void Draw_Cloud (std::vector<Circle>const& cloud, int radius, Mat& image, int num_parts) {
    //cout<<radius<<endl;
    int part=(num_of_points/num_parts);
	int cloudsize= cloud.size(); int f=0, alpha=1, mult_factor=1, check=(mult_factor*part);
    double R=(Rand()*255);
    double G=(Rand()*255);
    double B=(Rand()*255);
    double scale;
    Point2d shift;
    
    Scaling_Parameters(cloud, Point(imagesize, imagesize), scale, shift);
        
    for (int i=0; i<num_of_points; i++){ 
		Point2d center = Point((scale*cloud[i].point) + shift);
            cv::circle( image, center, 1, CV_RGB(R,G,B), 1);
         }
       }
       
      


void Pertube_Cloud (std::vector<Circle>& cloud,double noise_bound) {
	for ( int i = 0; i < (cloud.size()); i++) {
           cloud[i].point.x = (cloud[i].point.x + (noise_bound*Rand()) );
           cloud[i].point.y = (cloud[i].point.y + (noise_bound*Rand()) );
           cout << "Point after Pertubing is:" << cloud[i].point.x << " " << cloud[i].point.y << endl;
           }
    }

void Cloud_On_Circles(vector<Circle>& cloud, int num_parts, int num_of_circles){
	int part=(num_of_points/num_parts);
	int cloudsize= cloud.size(); int f=0, alpha=1, mult_factor=1, check=(mult_factor*part); 
	for (int radius=1; radius <= num_of_circles; radius++){
		      
	     	for(int i=f; i<num_of_points; i++){
		    double Angle = Rand()* 2 * pi;
		    cloud[i].point.x =radius*(cos(Angle));
		    cloud[i].point.y =radius*(sin(Angle));
		    cout<<"Angle generated points are:"<<cloud[i].point.x<<" "<<cloud[i].point.y<<endl;
	        cout<<i<<endl;
	        if(i==(check-1)) {
	        if(radius<num_of_circles) {
	         f=(check); mult_factor++; alpha++; check+=(part*alpha); break; }
	         else { continue; }
	      }
	      }
	      cout<<radius<<endl;
	      	     }
	      }
	
	
	
	
	
	 
int main(){
	srand(time(NULL));
	Mat image = Mat::zeros(imagesize, imagesize, CV_8UC3);
	image = cv::Scalar(0,0,0);
	double noisebound=0.1;
	int sum=0;
	cout<<"Enter the number of circles"<<endl;
	int num_of_circles; cin>>num_of_circles;
	for(int z=num_of_circles; z>=1; z--){sum+=z; 
	}
	vector<Circle> cloud(num_of_points);
	
	Cloud_On_Circles(cloud, sum, num_of_circles);
	Pertube_Cloud(cloud, 0.1);
	calculateGaussDensity(cloud, 7.0);
	colormap(cloud);
		 
	//cout<<"Finish"<<endl;
    Draw_Cloud(cloud, num_of_circles, image, sum); 

    String perturbedCloud = "cCloud";
	cv::imwrite(perturbedCloud+".png", image);
	
    return 0;
}


