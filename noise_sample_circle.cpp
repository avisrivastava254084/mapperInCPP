
#include "header.hpp"
#include "utilities.cpp"

class Data {
    
public:
    Point2f pt;
    void Perturb(double noise_bound )
    {
        double random_shift_x;
        double random_shift_y;
        random_shift_x = random_range(noise_bound, -noise_bound);
        random_shift_y = random_range(noise_bound, -noise_bound);
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


void Bounding_Box (std::vector<Data>const& cloud, std::pair<Point2d, Point2d>& cloud_box ) {
    
    cloud_box.first = Point2d( big_constant, big_constant );
    
    cloud_box.second = Point2d( -big_constant, -big_constant );
    
    for ( auto p : cloud )   {
        
        if ( cloud_box.first.x > p.pt.x ) cloud_box.first.x = p.pt.x;
        
        if ( cloud_box.first.y > p.pt.y ) cloud_box.first.y = p.pt.y;
        
        if ( cloud_box.second.x < p.pt.x ) cloud_box.second.x = p.pt.x;
        
        if ( cloud_box.second.y < p.pt.y ) cloud_box.second.y = p.pt.y;
        
    }
    
}




void Scaling_Parameters (std::vector<Data>const& cloud, Point image_sizes, double& scale, Point2d& shift) {
    
    std::pair<Point2d, Point2d> box;
    
    Bounding_Box( cloud, box );
    
    Point2d cloud_sizes( box.second.x - box.first.x, box.second.y - box.first.y );
    
    Point2d cloud_center( box.second.x + box.first.x, box.second.y + box.first.y );
    
    cloud_center /= 2;
    
    Point2d image_center( image_sizes.x / 2, image_sizes.y / 2 );
    
    scale = std::min( image_sizes.x / cloud_sizes.x, image_sizes.y / cloud_sizes.y );
    
    //point =( scale * (( point - cloud_center ) + image_center));
    
    shift = image_center - (scale * cloud_center);
    
}




void Draw_Cloud (std::vector<Data>const& cloud,Scalar color, Mat& image) {
    
    double scale;
    
    Point2d shift;
    Point2f center;
    
    Scaling_Parameters( cloud, image.size(), scale, shift );
    
    for ( int i=0; i<(cloud.size()); i++){
        
        center.x = scale*cloud[i].pt.x + shift.x;
        center.y = scale*cloud[i].pt.y + shift.y;
        
        circle( image, center, 1, CV_RGB(255,0,0), 1);
    }
    
}


double Random_Angle()
{
    double random = random_range(2*M_PI,0.0);
    return (random);
}

void Noise_Generator(Data& point)
{
    point.Perturb(1.0);
}


void Generate_Cloud(vector <Data>& circle_cloud,float radius)
{
    float angle;
    for (int i = 0; i < 100 ; i++)
    {
        angle = Random_Angle();
        circle_cloud[i].pt.x = radius*(cos(angle));
        circle_cloud[i].pt.y = radius*(sin(angle));
        Noise_Generator(circle_cloud[i]);
    }
}

int main()
{
    Mat Image = Mat(100,100, CV_8UC3, Scalar(255,255,255));
    vector <Data> circle_cloud(100);
    float radius;
    cout <<"please enter the radius of the circle"<<endl;
    scanf("%f",&radius);
    Generate_Cloud(circle_cloud, radius);
    Draw_Cloud(circle_cloud, Scalar(255,0,0), Image);
    imwrite("/Users/utkarsh/Desktop/Image_circle.png",Image);
    waitKey(0);
    return 0;
}
