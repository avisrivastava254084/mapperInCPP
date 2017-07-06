#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

using namespace std;
using namespace cv;

#define big_constant 1e+64

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

void write_image(String const& name, Mat const& mapperImage) {
  try {
    cv::imwrite(name, mapperImage);
  } catch (std::runtime_error& ex) {
    fprintf(stderr, "Error in writing the image\n", ex.what());
  }
}

void euclideanVertices(Graph& g, int m, int n) {
	int a = 0;
	Mat aviImage = Mat::zeros(600, 600, CV_8UC3);
	aviImage = cv::Scalar(255,255,255);
	for(int i = 0; i <= m; i++) {
		for(int j = 0; j <= n; j++) {
			g[a].pt.x = i;
			g[a].pt.y = j;
			g[a].name = a;
			a++;
			//circle( aviImage, g[a].pt, 1, CV_RGB(0,255,0), 3 );
		}
	}
	//write_image("aviImage.png", aviImage);
}

void addEdges(Graph& g, int m, int n) {
	int i, j;
	//adding edges vertically
	for (i = 0; i < (( m + 1 ) * ( n )); i++) { 
		add_edge(i, i+(m+1), g);
		cout<<endl<<"A vertical edge has been added in:"<<i<<" "<<i+m+1<<endl;
	}
	int breadthLimit = m;
	//adding edges horizontally
	for (i = 0; i < ((m+1) * (n+1) - 1); i++) {
		if ( i == breadthLimit) {
			breadthLimit += (m+1);
			continue;
		}
		add_edge(i, (i + 1), g);
		cout<<endl<<"A horizontal edge has been added in:"<<i<<" "<<i+1<<endl;
	}
}

void Bounding_Box (Graph const& cloud, std::pair<Point2d, Point2d>& cloud_box ) {
	cloud_box.first = Point2d( big_constant, big_constant );
	cloud_box.second = Point2d( -big_constant, -big_constant );
	VItr vitr, vend;
	boost::tie(vitr, vend) =  boost::vertices(cloud);
	for (; vitr != vend; ++vitr ) {
		if ( cloud_box.first.x > cloud[*vitr].pt.x ) cloud_box.first.x = cloud[*vitr].pt.x;
		if ( cloud_box.first.y > cloud[*vitr].pt.y ) cloud_box.first.y = cloud[*vitr].pt.y;
		if ( cloud_box.second.x < cloud[*vitr].pt.x ) cloud_box.second.x = cloud[*vitr].pt.x;
		if ( cloud_box.second.y < cloud[*vitr].pt.y ) cloud_box.second.y = cloud[*vitr].pt.y;
	}
}

void Scaling_Parameters (Graph const& cloud, Point image_sizes, double& scale, Point2d& shift) {
	std::pair<Point2d, Point2d> box;
	Bounding_Box( cloud, box );
	Point2d cloud_sizes( box.second.x - box.first.x, box.second.y - box.first.y );
	Point2d cloud_center( box.second.x + box.first.x, box.second.y + box.first.y );
	cloud_center /= 2;
	Point2d image_center( image_sizes.x / 2, image_sizes.y / 2 );
	scale = std::min( image_sizes.x / cloud_sizes.x, image_sizes.y / cloud_sizes.y );
	// transformation: point -> scale * ( point - cloud_center ) + image_center
	shift = image_center - scale * cloud_center;
}

// Draw a scaled cloud to the image of a given size
void Draw_Cloud (Graph const& cloud, int radius, Scalar color, Mat& image) {
	double scale;
	Point2d shift;
	Scaling_Parameters( cloud, image.size(), scale, shift );
	VItr vitr, vend;
	boost:tie(vitr, vend) = boost::vertices(cloud);
	VertexDescriptor u,v; EdgePair ep; Point2d vertexOne, vertexTwo;
	for(ep = edges(cloud); ep.first != ep.second; ++ep.first) {
		u = source(*ep.first, cloud); vertexOne = cloud[u].pt;
		v = target(*ep.first, cloud); vertexTwo = cloud[v].pt;
		vertexOne *= scale;
		vertexTwo *= scale;
		line(image, vertexOne, vertexTwo, CV_RGB(0,0,255));
	}
	for (; vitr != vend; ++vitr) {
		circle( image, Point( scale * cloud[*vitr].pt + shift ), radius, color, -1 );
	}
	write_image("squareGridScaled.png", image);
}

int main() {
	cout << "Enter the m and n:" << endl;
	int m, n;
	cin >> m >> n;
	Graph g((m+1)*(n+1));
	euclideanVertices(g, m, n);
	addEdges(g, m, n);
	Point2d shift; int imageSize; double scale; 
	cout<<"Enter the image size:"<<endl; cin >> imageSize;
	Mat mapperImage = Mat::zeros(imageSize, imageSize, CV_8UC3);
	mapperImage = cv::Scalar(255,255,255);
	cout << "Please enter the radius of the vertices' circles." << endl;
	int radius; cin >> radius;
	Draw_Cloud(g, radius, CV_RGB(255,0,0), mapperImage);
}
