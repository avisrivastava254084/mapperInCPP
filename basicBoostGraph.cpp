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
typedef boost::graph_traits<Graph>::vertex_iterator VItr;
typedef boost::graph_traits<Graph>::edge_iterator EItr;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;

void write_image(String const& name, Mat const& mapperImage) {
  try {
    cv::imwrite(name, mapperImage);
  } catch (std::runtime_error& ex) {
    fprintf(stderr, "Error in writing the image\n", ex.what());
  }
}

double Rand() { return 10 * (double)rand() / RAND_MAX; }
/*
adjacency_list<OutEdgeContainer, VertexContainer, Directedness,
               VertexProperties, EdgeProperties,
               GraphProperties, EdgeList>
*/

void drawGraph(Graph g, int size, Mat const& mapperImage){
    Graph copy = g;
    VItr vitr, vend;
    Point vertexOne, vertexTwo;
	boost::tie(vitr, vend) = boost::vertices(copy);
	IndexMap index = get(boost::vertex_index, copy);
    int i = 0;
    for (; vitr != vend; ++vitr) {
		vertexOne = Point(g[*vitr].pt.x, g[*vitr].pt.y);
		cout << "Neighbours of " << i << " are:";
		std::pair<adjacency_iterator, adjacency_iterator> neighbors = boost::adjacent_vertices(vertex(i, copy), copy);
		for (; neighbors.first != neighbors.second; ++neighbors.first) {
			vertexTwo = Point(g[*neighbors.first].pt.x, g[*neighbors.first].pt.y);
			line(mapperImage, vertexOne, vertexTwo, CV_RGB(0, 0, 0));
			std::cout << index[*neighbors.first] << " ";
    }
    cout << endl;
    i++;
  }
  write_image("scaledGraph.png", mapperImage);
}

void scalingCloud(Graph g, int size, Mat const& mapperImage) {
  Point2d vertexOne;
  float x_min, y_min = +9999999999999.00;
  float x_max, y_max = -9999999999999.00;
  Graph copy = g;
  IndexMap index = get(boost::vertex_index, copy);
  VItr vitr, vend;
  boost::tie(vitr, vend) = boost::vertices(copy);
  for (; vitr != vend; ++vitr) {
    if (copy[*vitr].pt.x < x_min) x_min = copy[*vitr].pt.x;
    if (copy[*vitr].pt.x > x_max) x_max = copy[*vitr].pt.x;
    if (copy[*vitr].pt.y < y_min) y_min = copy[*vitr].pt.y;
    if (copy[*vitr].pt.y > y_max) y_max = copy[*vitr].pt.y;
  }
  float scalingFactor = max(x_max - x_min, y_max - y_min);
  scalingFactor = size / scalingFactor;
  boost::tie(vitr, vend) = boost::vertices(copy);
  for (; vitr != vend; ++vitr) {
    g[*vitr].pt.x *= scalingFactor;
    g[*vitr].pt.y *= scalingFactor;
    vertexOne = Point(g[*vitr].pt.x, g[*vitr].pt.y);
    circle(mapperImage, vertexOne, 1, CV_RGB(255,0,0), 1);
  }
  drawGraph(copy, 500, mapperImage);
}

void euclideanGraph(Graph& g){
  boost::add_edge(0, 1, g);
  boost::add_edge(1, 2, g);
  boost::add_edge(2, 3, g);
  boost::add_edge(3, 1, g);
  VItr vitr, vend;
  boost::tie(vitr, vend) = boost::vertices(g);
  int i = 0;
  for (; vitr != vend; ++vitr) {
    g[*vitr].pt.x = Rand();
    g[*vitr].pt.y = Rand();
  }
}

int main() {
  Graph g;
  euclideanGraph(g);
  Mat mapperImage = Mat::zeros(400, 400, CV_8UC3);
  mapperImage = cv::Scalar(255, 255, 255);
  scalingCloud(g, 400, mapperImage);
  return 0;
}

