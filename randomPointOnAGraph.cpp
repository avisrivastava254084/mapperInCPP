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

double distanceBetweenTwoPoints(Point2d pt1, Point2d pt2){
	double distanceX = (pt1.x-pt2.x)*(pt1.x-pt2.x);
	double distanceY = (pt1.y-pt2.y)*(pt1.y-pt2.y);
	double distance = sqrt(distanceX + distanceY);
	return distance;	
}

void write_image(String const& name, Mat const& mapperImage) {
  try {
    cv::imwrite(name, mapperImage);
  } catch (std::runtime_error& ex) {
    fprintf(stderr, "Error in writing the image\n", ex.what());
  }
}

double Rand() { return rand() % 100; }
double RandUnity() { return (double) rand() / RAND_MAX ; }

bool sortByEdgelength(const pair<pair<Point2d, Point2d>, double> &a, const pair<pair<Point2d, Point2d>, double> &b){
	return (a.second < b.second);
}

void randomPoint(const double& totalLength, vector<double>& edgesLinearInterval, EdgeLinear& edgesLinear, Graph& g, Mat const& mapperImage, vector<Point2d>& RandomPoints){
	double linearPoint = RandUnity() * totalLength; double t;
	if(linearPoint == 0) { linearPoint += RandUnity(); }
	cout<<endl<<"The random linear point is:"<<linearPoint<<endl; int noOfEdge = 0;
	int check = 0 ;
	for(vector<double>::iterator it = edgesLinearInterval.begin(); it != edgesLinearInterval.end(); ++it){
		if((*it) > (linearPoint)){
			cout<<endl<<"the value at it is:"<<(*it)<<"and at the prev pos is:"<<(*(it-1))<<endl;
			cout<<"The denominator is:"<< ((*it) - (*(it-1)))<<endl;
			t = ((linearPoint - (*(it-1))) / ((*it) - (*(it-1))) );
			cout<<endl<<"The value of t is:"<<t<<endl;
			check = 1;
			break;
		}
		cout<<endl<<"The check variable is:"<<check<<" ";
		noOfEdge++;
	}
	
	VertexDescriptor u,v; Point2d vertexOne, vertexTwo; int trackOfEdge = 1; //because noOfEdge is zero and I have made sure that 0 can't be the random point on the linear edges.
	EdgePair ep;
	cout<<"Now, iterate through the edges' lengths."<<endl; 
	for(ep = edges(g); ep.first != ep.second; ++ep.first){
		if(trackOfEdge == noOfEdge){
			cout<<endl<<"Hi"<<endl;
			u = source(*ep.first,g); vertexOne = g[u].pt;
			v = target(*ep.first,g); vertexTwo = g[v].pt;
			break;
		}
		else{
			cout<<endl<<"it is OK hi"<<endl;
			trackOfEdge++;
		}
	}
	Point2d plottedPoint;
	plottedPoint.x = ((1-t) * g[u].pt.x) + ((t) * g[v].pt.x); cout<<"The x is:"<<plottedPoint.x<<endl;
	plottedPoint.y = ((1-t) * g[u].pt.y) + ((t) * g[v].pt.y); cout<<"The y is:"<<plottedPoint.y<<endl;
	RandomPoints.push_back(plottedPoint);
	circle(mapperImage, plottedPoint, 1, CV_RGB(0,0,0), 3);
    write_image("randomPoint.png", mapperImage);
}	

void euclideanGraph(Graph& g, vector<Point2d>& RandomPoints, int randomCloudSize) {
  Mat mapperImage = Mat::zeros(600, 600, CV_8UC3);
  mapperImage = cv::Scalar(255,255,255);
  VItr vitr, vend;
  vector<double> edgesLinearInterval;
  srand(time(NULL));
  Point2d vertexOne, vertexTwo, centre;
  EdgeLinear edgesLinear;
  double length, totalLength; totalLength = 0;
  boost::add_edge(0, 1, g);
  boost::add_edge(1, 2, g);
  boost::add_edge(2, 3, g);
  boost::add_edge(3, 1, g);
  boost::tie(vitr, vend) = boost::vertices(g);
  for (; vitr != vend; ++vitr) {
    g[*vitr].pt.x = Rand();
    g[*vitr].pt.y = Rand();
    centre = Point(g[*vitr].pt.x, g[*vitr].pt.y);
    circle(mapperImage, centre, 1, CV_RGB(255,0,0), 6);
    cout << "The coordinates are:" << g[*vitr].pt.x << " " << g[*vitr].pt.y << endl;
  }
  EdgePair ep;
  VertexDescriptor u,v;
  cout<<"The lengths are"<<endl;
  for (ep = edges(g); ep.first != ep.second; ++ep.first){
	  // Get the two vertices that are joined by this edge //
	  u=source(*ep.first,g); vertexOne = g[u].pt;
	  v=target(*ep.first,g); vertexTwo = g[v].pt;
	  length = distanceBetweenTwoPoints(vertexOne, vertexTwo);
	  line(mapperImage, vertexOne, vertexTwo, CV_RGB(0,0,255));
	  cout<<length<<" ";
	  edgesLinear.push_back(make_pair(make_pair(vertexOne, vertexTwo), length));
  }
  cout<<endl;
  sort(edgesLinear.begin(), edgesLinear.end(), sortByEdgelength);
  cout<<endl<<"And here are the sorted edges, by lengths:"<<endl;
  //the program will fail because of the following while loop if all the edges are of length zero
  edgesLinearInterval.push_back(0);
  for(EdgeLinear::iterator it = edgesLinear.begin(); it != edgesLinear.end(); ++it){
	  //making the linear length line, starting from 0 but making sure that if some edge's length is already 0, then replication of that zero doesnt occur.
	  while(it->second == 0){
		  continue;
		  ++it;
	  }
	  totalLength += it->second;
	  edgesLinearInterval.push_back(totalLength);
	  cout<<it->second<<" ";
  }
  cout<<endl<<"And the intervals are:"<<endl;
  for(vector<double>::iterator it = edgesLinearInterval.begin(); it != edgesLinearInterval.end(); ++it){
	  cout<<(*it)<<" ";
  }
  cout<<endl;
  cout<<endl<<"And this is the total length:"<<totalLength<<endl;
  
  for(int iterate = 0; iterate < randomCloudSize; iterate++) {
	  randomPoint(totalLength, edgesLinearInterval, edgesLinear, g, mapperImage, RandomPoints);
  }
}

int main() {
    Graph g(4);
    vector<Point2d> RandomPoints;
    int randomCloudSize; cout << endl << "Please enter the size of the random cloud that you want to generate" << endl; cin >> randomCloudSize;
    euclideanGraph(g, RandomPoints, randomCloudSize);
  }
