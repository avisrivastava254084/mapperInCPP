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
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;

void euclideanVertices(Graph& g, int m, int n) {
	int a = 0;
	for(int i = 0; i <= m; i++) {
		for(int j = 0; j <= n; j++) {
			g[a].pt.x = i;
			g[a].pt.y = j;
			g[a].name = a;
			a++;
		}
	}
}

void addEdges(Graph& g, int m, int n) {
	int i, j;
	for (i = 0; i < (( m + 1 ) * ( n )); i++) { 
		add_edge(i, i+(m+1), g);
		cout<<endl<<"An edge has been added in:"<<i<<" "<<i+m+1<<endl;
	}
	int breadthLimit = m;
	for (i = 0; i < ((m+1) * (n+1) - 1); i++) {
		if ( i == breadthLimit) {
			breadthLimit += (m+1);
			continue;
		}
		add_edge(i, (i + 1), g);
		cout<<endl<<"An edge has been added in:"<<i<<" "<<i+1<<endl;
	}
}

int main(){
	cout << "Enter the m and n:" << endl;
	int m, n;
	cin >> m >> n;
	Graph g((m+1)*(n+1));
	euclideanVertices(g, m, n);
	addEdges(g, m, n);
}
