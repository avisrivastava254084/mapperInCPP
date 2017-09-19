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

bool sortByEdgelength(const pair<pair<Point2d, Point2d>, double> &a, const pair<pair<Point2d, Point2d>, double> &b) {
	return (a.second < b.second);
}


void euclideanCloud(Graph& cloud, VectorPoint2d RandomPoints, int randomCloudSize, Mat& cloudImage) {
	cout << endl << "Here we are assigning the random cloud its actual coords:" << endl;
	VItr vitr, vend;
	tie(vitr, vend) = boost::vertices(cloud);
	for (vector<Point2d>::iterator it = RandomPoints.begin(); ((vitr != vend) && (it != RandomPoints.end())); ++vitr, ++it) {
		Point2d pt = *it;
		cloud[*vitr].pt.x = pt.x;
		cloud[*vitr].pt.y = pt.y;
		//centre = Point(g[*vitr].pt.x, g[*vitr].pt.y);
		//circle(mapperImage, centre, 1, CV_RGB(255,0,0), 6);
		cout << "The coordinates are:" << cloud[*vitr].pt.x << " " << cloud[*vitr].pt.y << endl;
	}
}

void euclideanGraph(Graph& g, Mat& mapperImage) {
	srand(time(NULL));
	VItr vitr, vend;
	vector<double> edgesLinearInterval;
	srand(time(NULL));
	Point2d vertexOne, vertexTwo, centre;
	EdgeLinear edgesLinear;
	double length, totalLength; totalLength = 0;
	cout << "Now, keep on adding edges, when done, enter -1." << endl;
	int check; int i, j; check = 0;

	boost::tie(vitr, vend) = boost::vertices(g);
	for (; vitr != vend; ++vitr) {
		g[*vitr].pt.x = RandUnity();
		g[*vitr].pt.y = RandUnity();
		//centre = Point(g[*vitr].pt.x, g[*vitr].pt.y);
		//circle(mapperImage, centre, 1, CV_RGB(255,0,0), 6);
		cout << "The coordinates are:" << g[*vitr].pt.x << " " << g[*vitr].pt.y << endl;
	}

	/*while (check != -1) {
	cout << endl << "Enter the two vertices which you want to connect through an edge" << endl;
	cin >> i >> j;
	add_edge(i, j, g);
	cout << endl << "Enter -1 if you do not want to add any more edge(s)" << endl;
	cin >> check;
	}*/
}

void euclideanGraphConnected(Graph& g, Mat& mapperImage, VectorPoint2d connectedComponent) {
	srand(time(NULL));
	VItr vitr, vend;
	vector<double> edgesLinearInterval;
	srand(time(NULL));
	Point2d vertexOne, vertexTwo, centre;
	EdgeLinear edgesLinear;
	double length, totalLength; totalLength = 0;
	cout << "Now, keep on adding edges, when done, enter -1." << endl;
	int itr = 0;

	boost::tie(vitr, vend) = boost::vertices(g);
	for (; vitr != vend; ++vitr) {
		g[*vitr].pt.x = connectedComponent[itr].x;
		g[*vitr].pt.y = connectedComponent[itr].x;
		//centre = Point(g[*vitr].pt.x, g[*vitr].pt.y);
		//circle(mapperImage, centre, 1, CV_RGB(255,0,0), 6);
		cout << "The coordinates are:" << g[*vitr].pt.x << " " << g[*vitr].pt.y << endl;
		itr++;
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

// Draw a scaled cloud to the image of a given size

void Draw_Graph(Graph const& graph, int radius, Scalar color, Mat& image, double& scale, Point2d& shift, String name) {
	//cout << endl << "Now, we will be plotting the given boost graph:" << endl;
	Scaling_Parameters_Graph(graph, image.size(), scale, shift);
	VItr vitr, vend;
boost:tie(vitr, vend) = boost::vertices(graph);
	VertexDescriptor u, v; EdgePair ep; Point2d vertexOne, vertexTwo;
	for (ep = edges(graph); ep.first != ep.second; ++ep.first) {
		u = source(*ep.first, graph); vertexOne = scale * graph[u].pt + shift;
		v = target(*ep.first, graph); vertexTwo = scale * graph[v].pt + shift;
		//cout << endl << "A line will be plotted on these:" << vertexOne << " " << vertexTwo << endl;
		line(image, vertexOne, vertexTwo, CV_RGB(0, 0, 255));
	}
	for (; vitr != vend; ++vitr) {
		//cout << endl << "This will be the circle plotted vertices' indices wise:" << graph[*vitr].pt << endl;
		//cout << endl << "Vertex plotted at:" << (scale * graph[*vitr].pt + shift) << endl;
		circle(image, Point(scale * graph[*vitr].pt + shift), radius, color, -1);
	}
	write_image(name, image);
}

void neighbourHoodGraph(Graph& randomGraph, double threshold, VectorPoint2d& connectedComponent) {
	VItr vitr, vend; double distance;
	tie(vitr, vend) = vertices(randomGraph);
	VItr vitr2, vend2;
	tie(vitr2, vend2) = vertices(randomGraph);
	for (; vitr != vend; ++vitr) {
		for (vitr2 = vitr + 1; vitr2 != vend2; ++vitr2) {
			distance = distanceBetweenTwoPoints(randomGraph[*vitr].pt, randomGraph[*vitr2].pt);
			//cout << endl << "The distance between " << randomGraph[*vitr].pt << " " << randomGraph[*vitr2].pt << " is: " << distance << endl;
			if (distance <= threshold) {
				add_edge(*vitr, *vitr2, randomGraph);
				//cout << endl << "An edge has been added to the vertices : " << randomGraph[*vitr].pt << " and " << randomGraph[*vitr2].pt << endl;
			}
		}
	}
	EdgePair ep;
	Point2d vertexOne, vertexTwo, centre;
	EdgeLinear edgesLinear;
	VertexDescriptor u, v;
	double length;
	Point2d point;
	int count = 0;
	vertexTwo = randomGraph[source(*edges(randomGraph).first, randomGraph)].pt;
	point.x = vertexTwo.x;
	point.y = vertexTwo.y;
	//cout << "The lengths are" << endl;
	for (ep = edges(randomGraph); ep.first != ep.second; ++ep.first) {
		// Get the two vertices that are joined by this edge //
		u = source(*ep.first, randomGraph); vertexOne = randomGraph[u].pt;
		if (vertexOne == vertexTwo) {
			//u = source(*ep.first, randomGraph); vertexOne = randomGraph[u].pt;
			v = target(*ep.first, randomGraph); vertexTwo = randomGraph[v].pt;
			length = distanceBetweenTwoPoints(vertexOne, vertexTwo);
			//line(mapperImage, vertexOne, vertexTwo, CV_RGB(0,0,255));
			//cout << length << " " << endl;
			edgesLinear.push_back(make_pair(make_pair(vertexOne, vertexTwo), length));
			point.x += vertexTwo.x;
			point.y += vertexTwo.y;
			count++;
		}
		else {
			cout << "points:- " << point.x << " " << point.y << endl;
			connectedComponent.push_back(point / count);
			vertexTwo = vertexOne;
			point.x = vertexOne.x;
			point.y = vertexOne.y;
		}
	}
	cout << "connected components: " << connectedComponent << endl;

}

int main() {
	cout << endl << "Please enter the total number of vertices in the boost graph:" << endl;
	int numberOfVertices; cin >> numberOfVertices;
	Graph g(numberOfVertices);
	VectorPoint2d connectedComponent;
	Mat cloudImage = Mat::zeros(600, 600, CV_8UC3); cloudImage = cv::Scalar(255, 255, 255);
	Mat graphImage = Mat::zeros(600, 600, CV_8UC3); graphImage = cv::Scalar(255, 255, 255);
	int randomCloudSize; cout << endl << "Please enter the size of the random cloud that you want to generate" << endl; cin >> randomCloudSize;
	euclideanGraph(g, graphImage);
	//euclideanCloud(randomGraph, RandomPoints, randomCloudSize, cloudImage);
	double scaleGraph; Point2d shiftGraph;
	Draw_Graph(g, 4, CV_RGB(255, 0, 0), graphImage, scaleGraph, shiftGraph, "scaledBoostGraph.png");
	cout << endl << "Please enter the threshold: " << endl;
	double threshold; cin >> threshold;
	int connectedGraphSize;
	neighbourHoodGraph(g, threshold, connectedComponent);
	connectedGraphSize = connectedComponent.size();
	cout << "size: " << connectedGraphSize << endl;
	Graph connectedGraph(connectedGraphSize);
	Draw_Graph(g, 4, CV_RGB(255, 0, 0), graphImage, scaleGraph, shiftGraph, "scaledBoostGraph.png");
	euclideanGraphConnected(connectedGraph, graphImage, connectedComponent);
	Draw_Graph(connectedGraph, 6, CV_RGB(255, 0, 255), graphImage, scaleGraph, shiftGraph, "newImage.png");
	
}
