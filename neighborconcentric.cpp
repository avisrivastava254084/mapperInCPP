
#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <random>

using namespace std;
using namespace cv;
using namespace boost;

#define big_constant 1e+64
#define pi 3.14159265358979323846
#define threshold 0.1
#define num_of_points 1000
#define imagesize 512

double Rand() { return  (double) rand() / RAND_MAX; }

struct Data{
	Point2d point;
};

struct VertexProperty{
	Point2d pt;
	int name;
};


typedef boost::property<boost::edge_weight_t, double> EdgeProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty> Graph;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;
typedef boost::graph_traits<Graph>::vertex_iterator VItr;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef vector<pair< pair< Point2d, Point2d>, double> > EdgeLinear;


double distanceBetweenCoordinates(Data point1, Data point2){
	double distanceX = (point1.point.x-point2.point.x)*(point1.point.x-point2.point.x);
	double distanceY = (point1.point.y-point2.point.y)*(point1.point.y-point2.point.y);
	double distance = sqrt(distanceX + distanceY);
	return distance;
}

void distanceCheck(vector<Data>& cloud, Graph& g){
	for(vector<Data>::iterator it = cloud.begin(); it != cloud.end(); ++it) {
         for(vector<Data>::iterator jt = cloud.begin(); jt != cloud.end(); ++jt) {
        
	         double distance = distanceBetweenCoordinates(*it, *jt);
	         if(distance <= 0.1) {
				 add_edge((it-cloud.begin()), (jt-cloud.begin()), g);
				 }
	           } 
	         }   
	        }

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


void Draw_Cloud (std::vector<Data>const& cloud, int radius, Mat& image, int num_parts, Graph& g) {
    
    
	int cloudsize= cloud.size();
    double R=(Rand()*255);
    double G=(Rand()*255);
    double B=(Rand()*255);
    double scale;
    Point2d shift;
    VItr vitr, vend;
	boost:tie(vitr, vend) = boost::vertices(g);
	VertexDescriptor u,v;	    
    EdgePair ep; Point2d vertexOne, vertexTwo;
	
    Scaling_Parameters(cloud, Point(imagesize, imagesize), scale, shift);
    for (int i=0; i<num_of_points; i++){ 
		Point2d center = Point((scale*cloud[i].point) + shift);
            cv::circle( image, center, 1, CV_RGB(R,G,B), 1);
		    }
	for(ep = edges(g); ep.first != ep.second; ++ep.first) {
		u = source(*ep.first, g); vertexOne = cloud[u].point;
		v = target(*ep.first, g); vertexTwo = cloud[v].point;
		vertexOne *= scale; vertexOne += shift;
		vertexTwo *= scale; vertexTwo += shift;
		line(image, vertexOne, vertexTwo, CV_RGB(0,0,255));
	}
    
     }

void Pertube_Cloud (std::vector<Data>& cloud, double noise_bound) {
	for ( int i = 0; i < (cloud.size()); i++) {
           cloud[i].point.x = (cloud[i].point.x + (noise_bound*Rand()) );
           cloud[i].point.y = (cloud[i].point.y + (noise_bound*Rand()) );
           cout << "Point after Pertubing is:" << cloud[i].point.x << " " << cloud[i].point.y << endl;
           }
    }

      
void Cloud_On_Circles(vector<Data>& cloud, int num_parts, int num_of_circles){
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

int main() {
	srand(time(NULL));
    Mat image = Mat::zeros(imagesize, imagesize, CV_8UC3);
	image = cv::Scalar(255,255,255);
	int sum=0;
	cout<<"Enter the number of circles"<<endl;
	int num_of_circles; cin>>num_of_circles;
	for(int z=num_of_circles; z>=1; z--){sum+=z; //cout<<sum<<endl; 
	}
	vector<Data> cloud(num_of_points);
	Graph g(num_of_points);
	Cloud_On_Circles(cloud, sum, num_of_circles);
	Pertube_Cloud(cloud, 0.1);
    distanceCheck(cloud, g);
    Draw_Cloud(cloud, num_of_circles, image, sum, g); 
    String perturbedCloud = "NeighborConcentricCloud";
	cv::imwrite(perturbedCloud+".png", image);
	
    return 0;    
    }
