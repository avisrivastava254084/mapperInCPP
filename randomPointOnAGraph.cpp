#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>


#define big_constant 1e+64

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
typedef vector<Point2d> VectorPoint2d;

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

double Rand() { return rand() % 100; }
double RandUnity() { return (double)rand() / RAND_MAX; }

bool sortByEdgelength(const pair<pair<Point2d, Point2d>, double> &edge1, const pair<pair<Point2d, Point2d>, double> &edge2) {
	return (edge1.second < edge2.second);
}

void randomPoint(const double& totalLength, vector<double>& edgesLinearInterval, EdgeLinear& edgesLinear, Graph& graph, Mat const& mapperImage, VectorPoint2d& RandomPoints) {
	double linearPoint, t; int noOfEdge = 0; int check = 0;
	linearPoint = RandUnity() * totalLength;
	if (linearPoint == 0) { linearPoint += RandUnity(); }
	cout << endl << "The random linear point is:" << linearPoint << endl;
	noOfEdge = 0;
	check = 0;
	for (vector<double>::iterator linearEdgeIterator = edgesLinearInterval.begin(); linearEdgeIterator != edgesLinearInterval.end(); ++linearEdgeIterator) {
		if ((*linearEdgeIterator) >(linearPoint)) {
			cout << endl << "the value at it is:" << (*linearEdgeIterator) << "and at the prev pos is:" << (*(linearEdgeIterator - 1)) << endl;
			cout << "The denominator is:" << ((*linearEdgeIterator) - (*(linearEdgeIterator - 1))) << endl;
			t = ((linearPoint - (*(linearEdgeIterator - 1))) / ((*linearEdgeIterator) - (*(linearEdgeIterator - 1))));
			cout << endl << "The value of t is:" << t << endl;
			check = 1;
			break;
		}
		cout << endl << "The check variable is:" << check << " ";
		noOfEdge++;
	}

	VertexDescriptor u, v; Point2d vertexOne, vertexTwo; int trackOfEdge = 1; //because noOfEdge is zero and I have made sure that 0 can't be the random point on the linear edges.
	EdgePair edge_pair;
	cout << "Now, iterate through the edges' lengths." << endl;
	for (edge_pair = edges(graph); edge_pair.first != edge_pair.second; ++edge_pair.first) {
		if (trackOfEdge == noOfEdge) {
			cout << endl << "reached the interval" << endl;
			u = source(*edge_pair.first, graph); vertexOne = graph[u].pt;
			v = target(*edge_pair.first, graph); vertexTwo = graph[v].pt;
			break;
		}
		else {
			cout << endl << "not reached the interval yet" << endl;
			trackOfEdge++;
		}
	}
	Point2d plottedPoint;
	plottedPoint.x = ((1 - t) * graph[u].pt.x) + ((t)* graph[v].pt.x); cout << "The x is:" << plottedPoint.x << endl;
	plottedPoint.y = ((1 - t) * graph[u].pt.y) + ((t)* graph[v].pt.y); cout << "The y is:" << plottedPoint.y << endl;
	RandomPoints.push_back(plottedPoint);
	//circle(mapperImage, plottedPoint, 1, CV_RGB(0,0,0), 3);
	//write_image("randomPoint.png", mapperImage);
}

void euclideanCloud(Graph& cloud, VectorPoint2d RandomPoints, int randomCloudSize, Mat& cloudImage) {
	cout << endl << "Here we are assigning the random cloud its actual coords:" << endl;
	VItr vitr, vend;
	tie(vitr, vend) = boost::vertices(cloud);
	for (vector<Point2d>::iterator randomPointIterator = RandomPoints.begin(); ((vitr != vend) && (randomPointIterator != RandomPoints.end())); ++vitr, ++randomPointIterator) {
		Point2d pt = *randomPointIterator;
		cloud[*vitr].pt.x = pt.x;
		cloud[*vitr].pt.y = pt.y;
		//centre = Point(g[*vitr].pt.x, g[*vitr].pt.y);
		//circle(mapperImage, centre, 1, CV_RGB(255,0,0), 6);
		cout << "The coordinates are:" << cloud[*vitr].pt.x << " " << cloud[*vitr].pt.y << endl;
	}
}

void euclideanGraph(Graph& graph, VectorPoint2d& RandomPoints, int randomCloudSize, Mat& mapperImage) {
	srand(time(NULL));
	VItr vitr, vend;
	vector<double> edgesLinearInterval;
	srand(time(NULL));
	Point2d vertexOne, vertexTwo, centre;
	EdgeLinear edgesLinear;
	double length, totalLength; totalLength = 0;
	cout << "Now, keep on adding edges, when done, enter -1." << endl;
	int check; int vertex1, vertex2; check = 0;
	while (check != -1) {
		cout << endl << "Enter the two vertices which you want to connect through an edge" << endl;
		cin >> vertex1 >> vertex2;
		add_edge(vertex1, vertex2, graph);
		cout << endl << "Enter -1 if you do not want to add any more edge(s)" << endl;
		cin >> check;
	}
	boost::tie(vitr, vend) = boost::vertices(graph);
	for (; vitr != vend; ++vitr) {
		graph[*vitr].pt.x = RandUnity();
		graph[*vitr].pt.y = RandUnity();
		//centre = Point(g[*vitr].pt.x, g[*vitr].pt.y);
		//circle(mapperImage, centre, 1, CV_RGB(255,0,0), 6);
		cout << "The coordinates are:" << graph[*vitr].pt.x << " " << graph[*vitr].pt.y << endl;
	}
	EdgePair edge_pair;
	VertexDescriptor u, v;
	cout << "The lengths are" << endl;
	for (edge_pair = edges(graph); edge_pair.first != edge_pair.second; ++edge_pair.first) {
		// Get the two vertices that are joined by this edge //
		u = source(*edge_pair.first, graph); vertexOne = graph[u].pt;
		v = target(*edge_pair.first, graph); vertexTwo = graph[v].pt;
		length = distanceBetweenTwoPoints(vertexOne, vertexTwo);
		//line(mapperImage, vertexOne, vertexTwo, CV_RGB(0,0,255));
		cout << length << " ";
		edgesLinear.push_back(make_pair(make_pair(vertexOne, vertexTwo), length));
	}
	cout << endl;
	sort(edgesLinear.begin(), edgesLinear.end(), sortByEdgelength);
	cout << endl << "And here are the sorted edges, by lengths:" << endl;
	//the program will fail because of the following while loop if all the edges are of length zero
	edgesLinearInterval.push_back(0);
	for (EdgeLinear::iterator linearEdgeItr = edgesLinear.begin(); linearEdgeItr != edgesLinear.end(); ++linearEdgeItr) {
		//making the linear length line, starting from 0 but making sure that if some edge's length is already 0, then replication of that zero doesnt occur.
		while (linearEdgeItr->second == 0) {
			continue;
			++linearEdgeItr;
		}
		totalLength += linearEdgeItr->second;
		edgesLinearInterval.push_back(totalLength);
		cout << linearEdgeItr->second << " ";
	}
	cout << endl << "And the intervals are:" << endl;
	for (vector<double>::iterator linearEdgeIntervalItr = edgesLinearInterval.begin(); linearEdgeIntervalItr != edgesLinearInterval.end(); ++linearEdgeIntervalItr) {
		cout << (*linearEdgeIntervalItr) << " ";
	}
	cout << endl;
	cout << endl << "And this is the total length:" << totalLength << endl;

	for (int iterate = 0; iterate < randomCloudSize; iterate++) {
		randomPoint(totalLength, edgesLinearInterval, edgesLinear, graph, mapperImage, RandomPoints);
	}
}

void Bounding_Box_Graph(Graph const& graph, std::pair<Point2d, Point2d>& graph_box) {
	graph_box.first = Point2d(big_constant, big_constant);
	graph_box.second = Point2d(-big_constant, -big_constant);
	VItr vitr, vend;
	boost::tie(vitr, vend) = boost::vertices(graph);
	for (; vitr != vend; ++vitr) {
		if (graph_box.first.x > graph[*vitr].pt.x) graph_box.first.x = graph[*vitr].pt.x;
		if (graph_box.first.y > graph[*vitr].pt.y) graph_box.first.y = graph[*vitr].pt.y;
		if (graph_box.second.x < graph[*vitr].pt.x) graph_box.second.x = graph[*vitr].pt.x;
		if (graph_box.second.y < graph[*vitr].pt.y) graph_box.second.y = graph[*vitr].pt.y;
	}
}

void Bounding_Box_Cloud(Graph const& cloud, std::pair<Point2d, Point2d>& cloud_box) {
	cloud_box.first = Point2d(big_constant, big_constant);
	cloud_box.second = Point2d(-big_constant, -big_constant);
	VItr vitr, vend;
boost:tie(vitr, vend) = boost::vertices(cloud);
	for (; vitr != vend; ++vitr) {
		if (cloud_box.first.x > cloud[*vitr].pt.x) cloud_box.first.x = cloud[*vitr].pt.x;
		if (cloud_box.first.y > cloud[*vitr].pt.y) cloud_box.first.y = cloud[*vitr].pt.y;
		if (cloud_box.second.x < cloud[*vitr].pt.x) cloud_box.second.x = cloud[*vitr].pt.x;
		if (cloud_box.second.y < cloud[*vitr].pt.y) cloud_box.second.y = cloud[*vitr].pt.y;
	}
}

void Scaling_Parameters_Graph(Graph const& graph, Point image_sizes, double& scale, Point2d& shift) {
	std::pair<Point2d, Point2d> box;
	Bounding_Box_Graph(graph, box);
	Point2d graph_sizes(box.second.x - box.first.x, box.second.y - box.first.y);
	Point2d graph_center(box.second.x + box.first.x, box.second.y + box.first.y);
	graph_center /= 2;
	Point2d image_center(image_sizes.x / 2, image_sizes.y / 2);
	scale = std::min(image_sizes.x / graph_sizes.x, image_sizes.y / graph_sizes.y);
	// transformation: point -> scale * ( point - cloud_center ) + image_center
	shift = image_center - scale * graph_center;
}

void Scaling_Parameters_Cloud(Graph const& cloud, Point image_sizes, double& scale, Point2d& shift) {
	std::pair<Point2d, Point2d> box;
	Bounding_Box_Cloud(cloud, box);
	Point2d cloud_sizes(box.second.x - box.first.x, box.second.y - box.first.y);
	Point2d cloud_center(box.second.x + box.first.x, box.second.y + box.first.y);
	cloud_center /= 2;
	Point2d image_center(image_sizes.x / 2, image_sizes.y / 2);
	scale = std::min(image_sizes.x / cloud_sizes.x, cloud_sizes.y / cloud_sizes.y);
	// transformation: point -> scale * ( point - cloud_center ) + image_center
	shift = image_center - scale * cloud_center;
}

// Draw a scaled cloud to the image of a given size

void Draw_Graph(Graph const& graph, int radius, Scalar color, Mat& image, double& scale, Point2d& shift, String name) {
	cout << endl << "Now, we will be plotting the given boost graph:" << endl;
	Scaling_Parameters_Graph(graph, image.size(), scale, shift);
	VItr vitr, vend;
boost:tie(vitr, vend) = boost::vertices(graph);
	VertexDescriptor u, v; EdgePair ep; Point2d vertexOne, vertexTwo;
	for (ep = edges(graph); ep.first != ep.second; ++ep.first) {
		u = source(*ep.first, graph); vertexOne = scale * graph[u].pt + shift;
		v = target(*ep.first, graph); vertexTwo = scale * graph[v].pt + shift;
		cout << endl << "A line will be plotted on these:" << vertexOne << " " << vertexTwo << endl;
		line(image, vertexOne, vertexTwo, CV_RGB(0, 0, 255));
	}
	for (; vitr != vend; ++vitr) {
		cout << endl << "This will be the circle plotted vertices' indices wise:" << graph[*vitr].pt << endl;
		cout << endl << "Vertex plotted at:" << (scale * graph[*vitr].pt + shift) << endl;
		circle(image, Point(scale * graph[*vitr].pt + shift), radius, color, -1);
	}
	write_image(name, image);
}

void neighbourHoodGraph(Graph& randomGraph, double threshold) {
	VItr vitr, vend; double distance;
	tie(vitr, vend) = vertices(randomGraph);
	VItr vitr2, vend2;
	tie(vitr2, vend2) = vertices(randomGraph);
	for (; vitr != vend; ++vitr) {
		for (vitr2 = vitr + 1; vitr2 != vend2; ++vitr2) {
			distance = distanceBetweenTwoPoints(randomGraph[*vitr].pt, randomGraph[*vitr2].pt);
			cout << endl << "The distance between " << randomGraph[*vitr].pt << " " << randomGraph[*vitr2].pt << " is: " << distance << endl;
			if (distance <= threshold) {
				add_edge(*vitr, *vitr2, randomGraph);
				cout << endl << "An edge has been added to the vertices : " << randomGraph[*vitr].pt << " and " << randomGraph[*vitr2].pt << endl;
			}
		}
	}
}

int main() {
	cout << endl << "Please enter the total number of vertices in the boost graph:" << endl;
	int numberOfVertices; cin >> numberOfVertices;
	Graph graph(numberOfVertices);
	VectorPoint2d RandomPoints;
	Mat cloudImage = Mat::zeros(600, 600, CV_8UC3); cloudImage = cv::Scalar(255, 255, 255);
	Mat graphImage = Mat::zeros(600, 600, CV_8UC3); graphImage = cv::Scalar(255, 255, 255);
	int randomCloudSize; cout << endl << "Please enter the size of the random cloud that you want to generate" << endl; cin >> randomCloudSize;
	Graph randomGraph(randomCloudSize);
	euclideanGraph(graph, RandomPoints, randomCloudSize, graphImage);
	euclideanCloud(randomGraph, RandomPoints, randomCloudSize, cloudImage);
	double scaleGraph; Point2d shiftGraph;
	Draw_Graph(graph, 4, CV_RGB(255, 0, 0), graphImage, scaleGraph, shiftGraph, "scaledBoostGraph.png");
	cout << endl << "Please enter the threshold:" << endl;
	double threshold; cin >> threshold;
	neighbourHoodGraph(randomGraph, threshold);
	Draw_Graph(randomGraph, 4, CV_RGB(255, 0, 0), cloudImage, scaleGraph, shiftGraph, "scaledRandomCloudGraph.png");
}
