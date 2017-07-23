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

struct VertexProperty {
	char name;
	Point2d pt;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_iterator Vertex_Itr;
typedef boost::graph_traits<Graph>::edge_iterator Edge_Itr;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;

void write_image(String const& name, Mat const& mapperImage) {
	try {
		cv::imwrite(name, mapperImage);
	}
	catch (std::runtime_error& ex) {
		fprintf(stderr, "Error in writing the image\n", ex.what());
	}
}

double Rand() { return 10 * (double)rand() / RAND_MAX; }

void drawGraph(Graph graph, int size, Mat const& mapperImage) {
	Graph graphCopy = graph;
	Vertex_Itr vitr, vend;
	Point vertexOne, vertexTwo;
	boost::tie(vitr, vend) = boost::vertices(graphCopy);
	IndexMap index = get(boost::vertex_index, graphCopy);
	int currentVertex = 0;
	for (; vitr != vend; ++vitr) {
		vertexOne = Point(graph[*vitr].pt.x, graph[*vitr].pt.y);
		cout << "Neighbours of " << currentVertex << " are:";
		std::pair<adjacency_iterator, adjacency_iterator> neighbors = boost::adjacent_vertices(vertex(currentVertex, graphCopy), graphCopy);
		for (; neighbors.first != neighbors.second; ++neighbors.first) {
			vertexTwo = Point(graph[*neighbors.first].pt.x, graph[*neighbors.first].pt.y);
			line(mapperImage, vertexOne, vertexTwo, CV_RGB(0, 0, 0));
			std::cout << index[*neighbors.first] << " ";
		}
		cout << endl;
		currentVertex++;
	}
	write_image("scaledGraph.png", mapperImage);
}

void scalingCloud(Graph graph, int size, Mat const& mapperImage) {
	Point2d vertexOne;
	float x_min, y_min = +9999999999999.00;
	float x_max, y_max = -9999999999999.00;
	Graph graphCopy = graph;
	IndexMap index = get(boost::vertex_index, graphCopy);
	Vertex_Itr vitr, vend;
	boost::tie(vitr, vend) = boost::vertices(graphCopy);
	for (; vitr != vend; ++vitr) {
		if (graphCopy[*vitr].pt.x < x_min) x_min = graphCopy[*vitr].pt.x;
		if (graphCopy[*vitr].pt.x > x_max) x_max = graphCopy[*vitr].pt.x;
		if (graphCopy[*vitr].pt.y < y_min) y_min = graphCopy[*vitr].pt.y;
		if (graphCopy[*vitr].pt.y > y_max) y_max = graphCopy[*vitr].pt.y;
	}
	float scalingFactor = max(x_max - x_min, y_max - y_min);
	scalingFactor = size / scalingFactor;
	boost::tie(vitr, vend) = boost::vertices(graphCopy);
	for (; vitr != vend; ++vitr) {
		graph[*vitr].pt.x *= scalingFactor;
		graph[*vitr].pt.y *= scalingFactor;
		vertexOne = Point(graph[*vitr].pt.x, graph[*vitr].pt.y);
		circle(mapperImage, vertexOne, 1, CV_RGB(255, 0, 0), 1);
	}
	drawGraph(graphCopy, 500, mapperImage);
}

void euclideanGraph(Graph& graph) {
	boost::add_edge(0, 1, graph);
	boost::add_edge(1, 2, graph);
	boost::add_edge(2, 3, graph);
	boost::add_edge(3, 1, graph);
	Vertex_Itr vitr, vend;
	boost::tie(vitr, vend) = boost::vertices(graph);
	int i = 0;
	for (; vitr != vend; ++vitr) {
		graph[*vitr].pt.x = Rand();
		graph[*vitr].pt.y = Rand();
	}
}

int main() {
	Graph graph;
	euclideanGraph(graph);
	Mat mapperImage = Mat::zeros(400, 400, CV_8UC3);
	mapperImage = cv::Scalar(255, 255, 255);
	scalingCloud(graph, 400, mapperImage);
	return 0;
}
