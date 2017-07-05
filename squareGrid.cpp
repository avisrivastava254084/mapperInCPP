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
	int name;
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
	int m, n, i, j;
	int vertexName = 65;
	VItr vitr, vend;
	cout<<"Enter the m and n:"<<endl;
	cin >> m >> n;
	m++; n++;
	Graph g(m*n);
	boost::tie(vitr, vend) = boost::vertices(g);
	cout<<"And the points are:"<<endl;
	for( i = 0; i <= m; i++){
		for( j = 0; j <= n; j++){
			g[*vitr].pt.x = i;
			g[*vitr].pt.y = j;
			g[*vitr].name = vertexName;
			++vertexName;
			cout << g[*vitr].pt.x <<" "<< g[*vitr].pt.y << endl;
			++vitr;
		}
	}
	cout<<"Testing the indices of the graph"<<endl;
	tie(vitr, vend) = vertices(g);
	int lengthLimit = m-1; int breadthLimit = m * (n-1);
	cout << "The values of limits are:" << lengthLimit << " " << breadthLimit << endl;
	for(int i = 0; i < (m*n); i++){
		if( i == lengthLimit && i != breadthLimit ){
			if( (i+m) <= (m*n) ){
				add_edge(i, (i+m), 1, g);
				cout << "LengthLimit An edge is added between the vertices:" << i << " " << (i+m) << endl;
			}
			lengthLimit += m;
			continue;
		}
		if( i == breadthLimit && i != lengthLimit ){
			if( (i+1) <= (m*n) ){
				add_edge(i, (i+1), 1, g);
				cout << "BreadthLimit An edge is added between the vertices:" << i << " " << (i+1) << endl;
			}
			breadthLimit++;
			continue;
		}
		if( i == ((m*n) - 1) ){
			break;
		}
		else{
			if( (i+1) <= (m*n) ){
				add_edge(i, (i+1), 1, g);
				cout << "Else first An edge is added between the vertices:" << i << " " << (i+1) << endl;
			}
			if( (i+m) <= (m*n) ){
				add_edge(i, (i+m), 1, g);
				cout << "Else second An edge is added between the vertices:" << i << " " << (i+m) << endl;
			}
		}
	}
	EdgePair ep; Point2d vertexOne, vertexTwo;
	VertexDescriptor u, v;
	/*for(ep = edges(g); ep.first != ep.second; ++ep.first){
		u = source(*ep.first,g); vertexOne = g[u].pt;
		v = target(*ep.first,g); vertexTwo = g[v].pt;
		cout << "An edge exists between:" << u <<" "<< v <<endl;
	}*/
}
	//write_image("squareGrid.png", mapperImage);


int main(){
	euclideanVertices();
	return 0;
}
