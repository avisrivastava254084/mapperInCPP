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

#define PI 3.14159265

struct VertexProperty {
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
	}
	catch (std::runtime_error& ex) {
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

void star_graph(Graph& graph, int numRays, Mat& image) {
	Point image_size = image.size();
	Point2d image_center(image_size.x / 2, image_size.y / 2);
	graph[0].pt.x = image_center.x;
	graph[0].pt.y = image_center.x;
	circle(image, graph[0].pt, 1, CV_RGB(0, 0,255 ), -1);
	for (int rayIterator = 1; rayIterator < numRays + 1; rayIterator++) {
			graph[rayIterator].pt.x = image_center.x + 10*cos((2 * PI * ( rayIterator - 1 )) / numRays);
			graph[rayIterator].pt.y = image_center.x + 10*sin((2 * PI * ( rayIterator - 1 )) / numRays);
			circle(image, graph[rayIterator].pt, 1, CV_RGB(0, 0, 255), -1);
			cout << rayIterator << endl;
			cout << graph[rayIterator].pt.x << endl;
			cout << graph[rayIterator].pt.y << endl;
	}
}

void addEdges(Graph& graph, int numRays, Mat& image) {
	int rayIterator = 1;
	while(rayIterator < numRays + 1 ) {
		add_edge(0, rayIterator, graph);
		cout << endl << "An edge has been added" << endl;
		line(image, graph[0].pt, graph[rayIterator].pt, CV_RGB(0, 255, 255));
		rayIterator++;
	}
	write_image("Star.png", image);
}

int main() {
	srand(time(NULL));
	cout << "Enter the number of rays: " << endl;
	int numRays;
	cin >> numRays;
	Point2d shift; int imageSize; double scale;
	cout << "Enter the image size: " << endl; cin >> imageSize;
	Mat mapperImage = Mat::zeros(imageSize, imageSize, CV_8UC3);
	mapperImage = cv::Scalar(255, 255, 255);
	Graph graph(numRays + 1);
	star_graph(graph, numRays, mapperImage);
	addEdges(graph, numRays, mapperImage);
	return(0);
}
