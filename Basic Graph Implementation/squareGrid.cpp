#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <random>

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

int RandEdgeNumber() {
	//improve this reinitialisation of variables as this function will be called thousand times.
	const int range_from = 1;
    const int range_to = 10;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(range_from, range_to);
    int randomCellNumber = distr(generator);
    return randomCellNumber;
}
    
void euclideanVertices(Graph& graph, int size_m, int size_n) {
	int vertex_iterator = 0;
	Mat aviImage = Mat::zeros(600, 600, CV_8UC3);
	aviImage = cv::Scalar(255,255,255);
	for(int x_axis_iterator = 0; x_axis_iterator <= size_m; x_axis_iterator++) {
		for(int y_axis_iterator = 0; y_axis_iterator <= size_n; y_axis_iterator++) {
			g[vertex_iterator].pt.x = x_axis_iterator;
			g[vertex_iterator].pt.y = y_axis_iterator;
			g[vertex_iterator].name = vertex_iterator;
			vertex_iterator++;
			//circle( aviImage, g[a].pt, 1, CV_RGB(0,255,0), 3 );
		}
	}
	//write_image("aviImage.png", aviImage);
}

void addEdges(Graph& graph, int size_m, int size_n) {
	int vertex_iterator, j;
	//adding edges vertically
	for (vertex_iterator = 0; vertex_iterator < (( size_m + 1 ) * ( size_n )); vertex_iterator++) { 
		add_edge(vertex_iterator, vertex_iterator+(size_m+1), graph);
		cout<<endl<<"A vertical edge has been added in: "<<vertex_iterator<<" "<<vertex_iterator+size_m+1<<endl;
	}
	int breadthLimit = size_m;
	//adding edges horizontally
	for (vertex_iterator = 0; vertex_iterator < ((size_m+1) * (size_n+1) - 1); vertex_iterator++) {
		if ( vertex_iterator == breadthLimit) {
			breadthLimit += (size_m+1);
			continue;
		}
		add_edge(vertex_iterator, (vertex_iterator + 1), graph);
		cout<<endl<<"A horizontal edge has been added in:"<<vertex_iterator<<" "<<vertex_iterator+1<<endl;
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
void Draw_Cloud (Graph const& cloud, int radius, Scalar color, Mat& image, double& scale, Point2d& shift) {
	Scaling_Parameters( cloud, image.size(), scale, shift );
	VItr vitr, vend;
	boost:tie(vitr, vend) = boost::vertices(cloud);
	VertexDescriptor u,v; EdgePair ep; Point2d vertexOne, vertexTwo;
	for(ep = edges(cloud); ep.first != ep.second; ++ep.first) {
		u = source(*ep.first, cloud); vertexOne = cloud[u].pt;
		v = target(*ep.first, cloud); vertexTwo = cloud[v].pt;
		vertexOne *= scale; vertexOne + shift;
		vertexTwo *= scale; vertexTwo + shift;
		line(image, vertexOne, vertexTwo, CV_RGB(0,0,255));
	}
	for (; vitr != vend; ++vitr) {
		circle( image, Point( scale * cloud[*vitr].pt + shift ), radius, color, -1 );
	}
	write_image("squareGridDouble.png", image);
}

int main() {
	srand(time(NULL));
	cout << "Enter the m and n:" << endl;
	int size_m, size_n;
	cin >> size_m >> size_n;
	Graph graph((size_m+1)*(size_n+1));
	euclideanVertices(graph, size_m, size_n);
	addEdges(graph, size_m, size_n);
	Point2d shift; int imageSize; double scale; 
	cout<<"Enter the image size:"<<endl; cin >> imageSize;
	Mat mapperImage = Mat::zeros(imageSize, imageSize, CV_8UC3);
	mapperImage = cv::Scalar(255,255,255);
	cout << "Please enter the radius of the vertices' circles." << endl;
	int radius; cin >> radius;
	Draw_Cloud(g, radius, CV_RGB(255,0,0), mapperImage, scale, shift);
	return 0;
}
