
#include "header.hpp"

struct VertexProperty {
    char name;
    Point2f pt;
};

class Data {
    
public:
    Point2f pt;
    void Perturb(double noise_bound )
    {
        double random_shift_x;
        double random_shift_y;
        random_shift_x = -noise_bound + (rand() * (2*noise_bound) / RAND_MAX);
        random_shift_y = -noise_bound + (rand() * (2*noise_bound) / RAND_MAX);
        pt.x = pt.x+random_shift_x;
        pt.y = pt.y+random_shift_y;
        
    }
    
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Data> Graph; // for creating a graph
typedef boost::graph_traits<Graph>::vertex_iterator VItr; // for providing cordinates to each of the vertex
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef pair<EdgeIterator, EdgeIterator> EdgePair;
typedef boost::graph_traits<Graph>::vertex_iterator VItr; // for providing cordinates to each of the vertex
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef pair<EdgeIterator, EdgeIterator> EdgePair;

// for providing two attributes to the vertex ie name and cordinates




void Bounding_Box_Graph(Graph const& graph, std::pair<Point2f, Point2f>& graph_box) {
    graph_box.first = Point2f(big_constant, big_constant);
    graph_box.second = Point2f(-big_constant, -big_constant);
    VItr vitr, vend;
    boost::tie(vitr, vend) = boost::vertices(graph);
    for (; vitr != vend; ++vitr) {
        if (graph_box.first.x > graph[*vitr].pt.x) graph_box.first.x = graph[*vitr].pt.x;
        if (graph_box.first.y > graph[*vitr].pt.y) graph_box.first.y = graph[*vitr].pt.y;
        if (graph_box.second.x < graph[*vitr].pt.x) graph_box.second.x = graph[*vitr].pt.x;
        if (graph_box.second.y < graph[*vitr].pt.y) graph_box.second.y = graph[*vitr].pt.y;
    }
}

void Scaling_Parameters_Graph(Graph const& graph, Point image_sizes, double& scale, Point2f& shift)
{
    std::pair<Point2f, Point2f> box;
    Bounding_Box_Graph(graph, box);
    Point2f graph_sizes(box.second.x - box.first.x, box.second.y - box.first.y);
    Point2f graph_center(box.second.x + box.first.x, box.second.y + box.first.y);
    graph_center /= 2;
    Point2f image_center(image_sizes.x / 2, image_sizes.y / 2);
    scale = std::min(image_sizes.x / graph_sizes.x, image_sizes.y / graph_sizes.y);
    // transformation: point -> scale * ( point - cloud_center ) + image_center
    shift = image_center - scale * graph_center;
}

void Draw_Graph(Graph const& graph, int radius, Scalar color, Mat& image, double& scale, Point2f& shift, String name)
{
    cout << endl << "Now, we will be plotting the given boost graph:" << endl;
    Scaling_Parameters_Graph(graph, image.size(), scale, shift);
    VItr vitr, vend;
boost:tie(vitr, vend) = boost::vertices(graph);
    VertexDescriptor u, v; EdgePair ep; Point2f vertexOne, vertexTwo;
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
    
}



void cloud_generator ( )
{
    Mat Image = Mat(100, 100, CV_8UC3, Scalar(255,255,255));
    Graph graph(100);
    VItr vitr, vend;
    boost::tie(vitr, vend) = boost::vertices(graph);
    double scaleGraph,d; Point2f shiftGraph;
    for (; vitr != vend; ++vitr) {
        
        d = (double)rand() / RAND_MAX;
        graph[*vitr].pt.x = d;
        graph[*vitr].pt.y = d;
        
    }
    Draw_Graph(graph, 4, CV_RGB(255, 0, 0), Image, scaleGraph, shiftGraph, "scaledBoostGraph.png");
    imwrite("/Users/utkarsh/Desktop/image_digonal.png", Image);
    waitKey(0);
    
}


int main()
{
    cloud_generator( );
    return 0;
}


