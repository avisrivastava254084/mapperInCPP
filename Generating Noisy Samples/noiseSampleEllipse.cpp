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

struct Ellipse {
    Point2f radius;
    Point2f center;
};
Ellipse e;

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

void Generate_Cloud(vector <Data>& ellipse_cloud)
{

    float angle;
    for (int i = 0; i < 100 ; i++)
    {
        angle = Random_Angle();
        ellipse_cloud[i].pt.x = e.radius.x*(cos(angle));
        ellipse_cloud[i].pt.y = e.radius.y*(sin(angle));
        Noise_Generator(ellipse_cloud[i]);
    }
}



int main()
{
    Mat Image = Mat(100, 100, CV_8UC3, Scalar(255,255,255));
    vector <Data> ellipse_cloud(100);
    cout <<"Generation of a noisy cloud around an Ellipse"<<endl;
    e.radius.x = 2;
    e.radius.y = 1;
    Generate_Cloud(ellipse_cloud);
    Draw_Cloud(ellipse_cloud, Scalar(255,0,0), Image);
    imwrite("/Users/utkarsh/Desktop/image_ellipse.png",Image);
    waitKey(0);
    return 0;
}
