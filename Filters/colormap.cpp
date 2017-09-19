#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#define w 400

using namespace std;
using namespace cv;

struct dataPoint{
       Point2d pt;
       double GaussDensity;
};

int main()
{
    dataPoint mypoint;
    vector<dataPoint> points;
    
    vector<double> gd1;
    double t[10]={0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};

//    gd1=mypoint.GaussDensity;
   //copy(currentVector.GaussDensity.begin(), currentVector.GaussDensity.end(), gd1.begin()) ;
    gd1.push_back(0.74);
    gd1.push_back(0.21);
    gd1.push_back(0.96);
    gd1.push_back(0.20);
    gd1.push_back(0.55);
    
sort(gd1.begin(),gd1.end());

    std::vector<double>::iterator itfirst=gd1.begin();

    //cout<< itfirst <<endl;
    std::vector<double>::iterator itlast=gd1.end();

  vector<Vec3b> colormap;
    colormap.push_back(Vec3b(255,0,0));
    colormap.push_back(Vec3b(0,255,0));
    colormap.push_back(Vec3b(0,255,255));
    colormap.push_back(Vec3b(0,0,255));
    int intervals= colormap.size()-1;
    cout<<intervals<<" Intervals in the color map"<<endl;
    int intrvals= gd1.size()-1;
	cout<<<<intrvals<<" Intervals in the vector";
    int interval=gd1.size()/colormap.size()-1;
    for(std::vector<double>::iterator it=gd1.begin(); it!=gd1.begin()+interval;++it) {
        int temp=gd1.at(*it);
       Vec3b color=colormap.at(*it);
    }

	void Find_Colors (std::vector<Vec3b> color_map, multimap<double, Vec3b>& values_colors)
{

}


 // imshow(window, mapperImage);
  // waitKey(100000000);
   return 0;
}
