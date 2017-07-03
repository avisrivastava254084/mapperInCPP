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

struct VertexProperty{
	Point2d pt;
	char name;
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

void write_image(String const& name, Mat const& mapperImage) {
  try {
    cv::imwrite(name, mapperImage);
  } catch (std::runtime_error& ex) {
    fprintf(stderr, "Error in writing the image\n", ex.what());
  }
}

void euclideanVertices(){
	int m, n, i, j, imageLength, imageBreadth, mItr, nItr; mItr = nItr = 0;
	cout<<"Enter the image's length and breadth:"<<endl;
	cin >> imageLength >> imageBreadth;
	Mat mapperImage = Mat::zeros(600, 600, CV_8UC3);
	mapperImage = cv::Scalar(255,255,255);
	int vertexName = 65;
	VItr vitr, vend;
	cout<<"Enter the m and n:"<<endl;
	cin >> m >> n;
	Point2d origin;
	origin.x = imageLength / 2;
	origin.y = imageBreadth / 2;
	m *= 10; m += ceil(origin.x); n *= 10; n += ceil(origin.y);
	Graph g(m*n);
	boost::tie(vitr, vend) = boost::vertices(g);
	cout<<"And the points are:"<<endl;
	while( vitr != vend){
		for( i = ceil(double(origin.x)); i < m; i += 10){
			for( j = ceil(double(origin.y)); j < n; j += 10){
				g[*vitr].pt.x = i;
				g[*vitr].pt.y = j;
				g[*vitr].name = char(vertexName);
				++vertexName;
				cout << g[*vitr].pt.x <<" "<< g[*vitr].pt.y << endl;
				circle(mapperImage, Point(g[*vitr].pt.x, g[*vitr].pt.y), 1, CV_RGB(255,0,0), 3);
				++vitr;
			}
		}
	}
	write_image("squareGrid.png", mapperImage);
}

int main(){
	euclideanVertices();
	return 0;
}
