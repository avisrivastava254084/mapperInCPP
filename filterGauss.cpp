#include "opencv2/core/core.hpp"
#include <iostream>
#include <vector>
#include <fstream>

#ifdef _WIN32
#define WINPAUSE system("pause")
#endif

using namespace std;
using namespace cv;

struct dataPoint{
		Point2d pt;
		double GaussDensity;
};


int main()
{
	ifstream input;
    string filename;
    cout << "Enter the file name :" << endl;
    cin >> filename;
    input.open(filename.c_str());
    char ch;
	dataPoint myPoint;
    while(input.fail())
    {
        input.clear();
        cout <<"Incorrect filename, please enter again" << endl;
        cin >> filename ;
        input.open(filename.c_str()) ;
    }
	vector<dataPoint> points;
	while (input >> myPoint.pt.x >> myPoint.pt.y)
    {
        myPoint.GaussDensity = 1;
        points.push_back(myPoint);
        cout<<myPoint.pt.x<<" "<<myPoint.pt.y<<endl;
    };     
	WINPAUSE;
	return 0;
}
