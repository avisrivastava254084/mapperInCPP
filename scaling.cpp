#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <cstdlib>


using namespace std;
using namespace cv;

struct VertexProperty {
	char name;
	Point2d pt;
};

typedef boost::property<boost::edge_weight_t, double> EdgeProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_iterator VItr;
typedef boost::graph_traits<Graph>::edge_iterator EItr;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef vector<pair< pair< Point2d, Point2d>, double> > EdgeLinear;

double distanceBetweenTwoPoints(Point2d pt1, Point2d pt2) {
	double distanceX = (pt1.x - pt2.x)*(pt1.x - pt2.x);
	double distanceY = (pt1.y - pt2.y)*(pt1.y - pt2.y);
	double distance = sqrt(distanceX + distanceY);
	return distance;
}

void write_image(String const& name, Mat const& mapperImage) {
	try {
		cv::imwrite(name, mapperImage);
	}
	catch (std::runtime_error& ex) {
		fprintf(stderr, "Error in writing the image\n", ex.what());
	}
}

const double big_constant = 1e+64;
const Scalar white = CV_RGB(255, 255, 255);
const Scalar black = CV_RGB(0, 0, 0);

class Data

{

public:

	Point2d point;

	Data(Point2d p) { point = p; }

};


void Bounding_Box(std::vector<Data>const& cloud, std::pair<Point2d,
	Point2d>& cloud_box) {

	cloud_box.first = Point2d(big_constant, big_constant);

	cloud_box.second = Point2d(-big_constant, -big_constant);

	for (auto p : cloud) {

		if (cloud_box.first.x < p.point.x) cloud_box.first.x = p.point.x;

		if (cloud_box.first.y < p.point.y) cloud_box.first.y = p.point.y;

		if (cloud_box.second.x > p.point.x) cloud_box.second.x = p.point.x;

		if (cloud_box.second.y > p.point.y) cloud_box.second.y = p.point.y;


	}


}

void Scaling_Parameters(std::vector<Data>const& cloud, Point
	image_sizes, double& scale, Point2d& shift) {

	std::pair<Point2d, Point2d> box;

	Bounding_Box(cloud, box);

	Point2d cloud_sizes(box.second.x - box.first.x, box.second.y -
		box.first.y);

	Point2d cloud_center(box.second.x + box.first.x, box.second.y +
		+box.first.y);

	cloud_center.x /= 2;
	cloud_center.y /= 2;

	Point2d image_center(image_sizes.x / 2, image_sizes.y / 2);

	scale = std::min(image_sizes.x / cloud_sizes.x, image_sizes.y /
		cloud_sizes.y);
		//scale = 5;
		//shift = image_center - scale * cloud_center;
	shift = Point2d(image_sizes.x / 2, image_sizes.y / 2);

}

void Draw_Cloud(std::vector<Data>const& cloud, int radius, Scalar
	color, Mat const& mapperImage) {

	double scale;

	Point2d shift;

	Scaling_Parameters(cloud, mapperImage.size(), scale, shift);

	for (auto p : cloud)

		cv::circle(mapperImage, Point(scale * p.point.x + shift.x, scale * p.point.y + shift.y), radius, color, -1);

}

double Rand() { return rand() % 100; }
double RandUnity() { return (double)rand() / RAND_MAX; }

bool sortByEdgelength(const pair<pair<Point2d, Point2d>, double> &a, const pair<pair<Point2d, Point2d>, double> &b) {
	return (a.second < b.second);
}

void randomPoint(const double& totalLength, vector<double>& edgesLinearInterval, EdgeLinear& edgesLinear, Graph& graph, Mat const& mapperImage) {
	double linearPoint = RandUnity() * totalLength; double t;
	if (linearPoint == 0) { linearPoint += RandUnity(); }
	cout << endl << "The random linear point is:" << linearPoint << endl; int noOfEdge = 0;
	int check = 0;
	for (vector<double>::iterator linearEdgeIntervalIterator = edgesLinearInterval.begin(); linearEdgeIntervalIterator != edgesLinearInterval.end(); ++linearEdgeIntervalIterator) {
		if ((*linearEdgeIntervalIterator) >(linearPoint)) {
			cout << endl << "the value at it is:" << (*linearEdgeIntervalIterator) << "and at the prev pos is:" << (*(linearEdgeIntervalIterator - 1)) << endl;
			cout << "The denominator is:" << ((*linearEdgeIntervalIterator) - (*(linearEdgeIntervalIterator - 1))) << endl;
			t = ((linearPoint - (*(linearEdgeIntervalIterator - 1))) / ((*linearEdgeIntervalIterator) - (*(linearEdgeIntervalIterator - 1))));
			cout << endl << "The value of t is:" << t << endl;
			check = 1;
			break;
		}
		cout << endl << "The check variable is:" << check << " ";
		noOfEdge++;
	}
	VertexDescriptor u, v; Point2d vertexOne, vertexTwo; int trackOfEdge = 0;
	EdgePair edge_pair;
	cout << "Now, iterate through the edges' lengths." << endl;
	for (edge_pair = edges(graph); edge_pair.first != edge_pair.second; ++edge_pair.first) {
		if (trackOfEdge == noOfEdge) {
			cout << endl << "Hi" << endl;
			u = source(*edge_pair.first, graph); vertexOne = graph[u].pt;
			v = target(*edge_pair.first, graph); vertexTwo = graph[v].pt;
			break;
		}
		else {
			cout << endl << "it is OK hi" << endl;
			trackOfEdge++;
		}
	}
	Point2d plottedPoint;
	plottedPoint.x = ((1 - t) * graph[u].pt.x) + ((t)* graph[v].pt.x); cout << "The x is:" << plottedPoint.x << endl;
	plottedPoint.y = ((1 - t) * graph[u].pt.y) + ((t)* graph[v].pt.y); cout << "The y is:" << plottedPoint.y << endl;
	circle(mapperImage, plottedPoint, 1, CV_RGB(0, 0, 0), 1);
	waitKey(0);

}

void euclideanGraph(Graph& graph) {
	Mat mapperImage = Mat::zeros(600, 600, CV_8UC3);
	mapperImage = cv::Scalar(255, 255, 255);
	VItr vitr, vend;
	vector<double> edgesLinearInterval;
	srand(time(NULL));
	Point2d vertexOne, vertexTwo, centre;
	EdgeLinear edgesLinear;
	double length, totalLength; totalLength = 0;
	boost::add_edge(0, 1, graph);
	boost::add_edge(1, 2, graph);
	boost::add_edge(2, 3, graph);
	boost::add_edge(3, 1, graph);
	boost::tie(vitr, vend) = boost::vertices(graph);
	for (; vitr != vend; ++vitr) {
		graph[*vitr].pt.x = Rand();
		graph[*vitr].pt.y = Rand();
		centre = Point(graph[*vitr].pt.x, graph[*vitr].pt.y);
		circle(mapperImage, centre, 1, CV_RGB(255, 0, 0), 6);
		cout << "The coordinates are:" << graph[*vitr].pt.x << " " << graph[*vitr].pt.y << endl;
		vector<Data> cloud{ Data(Point2d(graph[*vitr].pt.x,graph[*vitr].pt.y)) };
		Draw_Cloud(cloud, 2, black, mapperImage);
	}
	EdgePair edge_pair;
	VertexDescriptor u, v;
	cout << "The lengths are" << endl;
	for (edge_pair = edges(graph); edge_pair.first != edge_pair.second; ++edge_pair.first) {
		// Get the two vertices that are joined by this edge //
		u = source(*edge_pair.first, graph); vertexOne = graph[u].pt;
		v = target(*edge_pair.first, graph); vertexTwo = graph[v].pt;
		length = distanceBetweenTwoPoints(vertexOne, vertexTwo);
		line(mapperImage, vertexOne, vertexTwo, CV_RGB(0, 0, 255));
		cout << length << " ";
		edgesLinear.push_back(make_pair(make_pair(vertexOne, vertexTwo), length));
	}
	cout << endl;
	sort(edgesLinear.begin(), edgesLinear.end(), sortByEdgelength);
	cout << endl << "And here are the sorted edges, by lengths:" << endl;
	//the program will fail because of the following while loop if all the edges are of length zero
	edgesLinearInterval.push_back(0);
	for (EdgeLinear::iterator linearEdgeIterator = edgesLinear.begin(); linearEdgeIterator != edgesLinear.end(); ++linearEdgeIterator) {
		//making the linear length line, starting from 0 but making sure that if some edge's length is already 0, then replication of that zero doesnt occur.
		while (linearEdgeIterator->second == 0) {
			continue;
			++linearEdgeIterator;
		}
		totalLength += linearEdgeIterator->second;
		edgesLinearInterval.push_back(totalLength);
		cout << linearEdgeIterator->second << " ";
	}
	cout << endl << "And the intervals are:" << endl;
	for (vector<double>::iterator linearEdgeIntervalIterator = edgesLinearInterval.begin(); linearEdgeIntervalIterator != edgesLinearInterval.end(); ++linearEdgeIntervalIterator) {
		cout << (*linearEdgeIntervalIterator) << " ";
	}
	cout << endl;
	cout << endl << "And this is the total length:" << totalLength << endl;
	write_image("randomPoint.png", mapperImage);
	randomPoint(totalLength, edgesLinearInterval, edgesLinear, graph, mapperImage);
}

int main() {
	Graph graph(4);
	euclideanGraph(graph);
	waitKey(0);
}
