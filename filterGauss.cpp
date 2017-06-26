#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <string>
#include <algorithm>
#define w 400
#define pi 3.14159265358979323846  /* pi */
using namespace std;
using namespace cv;
struct dataPoint {
    Point2d pt;
    double GaussDensity;
};
double distanceBetweenCoordinates(dataPoint pt1, dataPoint pt2){
    double distanceX = (pt1.pt.x-pt2.pt.x)*(pt1.pt.x-pt2.pt.x);
    double distanceY = (pt1.pt.y-pt2.pt.y)*(pt1.pt.y-pt2.pt.y);
    double distance = sqrt(abs(distanceX - distanceY));
    return distance;
}
void calculateGaussDensity(vector <dataPoint>& points, double sigma){
    unsigned long long int N = points.size();
    for(vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        double summation = 0;
        for(vector<dataPoint>::iterator jt = points.begin(); jt != points.end(); ++jt) {
            summation += exp((-1)*(pow(distanceBetweenCoordinates(*it, *jt),2.0)/2*(pow(sigma,2))));
        }
        dataPoint currentVector = *it;
        currentVector.GaussDensity = summation/(N*(pow((sqrt(2*pi)*sigma),2)));
        cout<<currentVector.GaussDensity<<endl;;
        *it = currentVector;
    }
}
/*
double calculateEccentricity(const vector <dataPoint> &points){
    unsigned long long int N = points.size();
    float exponent = 1.0;
    for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        double summation = 0;
        for(std::vector<dataPoint>::iterator jt = points.begin(); jt != points.end(); ++jt) {
            summation += distanceBetweenCoordinates(*it, *jt);
            dataPoint currentVector = *it;
            currentVector.eccentricity = summation/N;
            *it = currentVector;
        }
    }
}
*/  
void readPoints(vector <dataPoint>& points, Mat mapperImage){
    ifstream input;
    string filename;
    cout << "Enter the file name :" << endl;
    cin >> filename;
    input.open(filename.c_str());
    char ch;
    Point2d center;
    dataPoint myPoint;
    while (input.fail()){
        input.clear();
        cout << "Incorrect filename, please enter again" << endl;
        cin >> filename;
        input.open(filename.c_str());
    }
    while (input >> myPoint.pt.x >>ch>>myPoint.pt.y){
        myPoint.GaussDensity = 1;
        points.push_back(myPoint);
        cout << myPoint.pt.x << " " << myPoint.pt.y << endl;
        center = Point(myPoint.pt.x, myPoint.pt.x);
        circle(mapperImage, center, 1, CV_RGB(255, 0, 0), 3);
     }
     cout<<"Size of the vector:"<<points.size()<<endl;
}
double scalingCloud(vector <dataPoint> points, int size){
    dataPoint currentVector;
    float x_min, y_min = +9999999999999.00;
    float x_max, y_max = -9999999999999.00;
    for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        currentVector = *it;
        if(currentVector.pt.x < x_min)
            x_min = currentVector.pt.x;
        if(currentVector.pt.x > x_max)
            x_max = currentVector.pt.x;
        if(currentVector.pt.y < y_min)
            y_min = currentVector.pt.y;
        if(currentVector.pt.y > y_max)
            y_max = currentVector.pt.y;
    }
    float scalingFactor = max(x_max-x_min, y_max-y_min);
    size /= scalingFactor;
    return size;
}
void write_image(String const& name, Mat const& mapperImage){
    try{
        cv::imwrite( name, mapperImage ); 
    }
    catch (std::runtime_error& ex) {
        fprintf(stderr, "Error in writing the image\n", ex.what()); 
    }
}
// uniform random number in [0,1]
double Rand() { return  (double) rand() / RAND_MAX; }
// uniform random number in [-1,1]
double RandUnity() { return  2 * (double) rand() / RAND_MAX -1 ; }
// perturb points up to a noise bound
void Perturb_Cloud (double noise_bound, std::vector<dataPoint>& cloud, double scalingFactor, int imageSize) {
    Mat mapperImage = Mat::zeros(w, w, CV_8UC3);
    mapperImage = cv::Scalar(255,255,255);
    Point2d center;
    cout<<"The image size ka half is:"<<(imageSize/2)<<endl;
    for ( int i = 0; i < cloud.size(); i++) {
           cloud[i].pt.x = (((cloud[i].pt.x + (noise_bound * Rand()))*scalingFactor) + imageSize/2);
           cloud[i].pt.y = (((cloud[i].pt.y + (noise_bound * Rand()))*scalingFactor) + imageSize/2);
           //if(cloud[i].pt.x<0) { cloud[i].pt.x *= -1; }
           //if(cloud[i].pt.y<0) { cloud[i].pt.y *= -1; }
           cout << "Final Point is:" << cloud[i].pt.x << " " << cloud[i].pt.y << endl;
           center = Point(cloud[i].pt.x,cloud[i].pt.y);
           circle(mapperImage, center, 1, CV_RGB(255,0,0), 3);
    }
    String perturbedCloud = "testPerturbedCloud";
    write_image(perturbedCloud+".png", mapperImage);
}
void randomCloudNearCircle(double noiseBound, int imageSize, int cloudSize){
    //selecting a random angle for the complete cloud//
    vector<dataPoint> cloud(cloudSize);
    for(int i = 0; i<cloudSize; i++){
        double shiftAngle = Rand()* 2 * pi;
        cloud[i].pt.x = cos(shiftAngle);
        cloud[i].pt.y = sin(shiftAngle);
        cout<<"Angle generated points are:"<<cloud[i].pt.x<<" "<<cloud[i].pt.y<<endl;
    }
    double scalingFactor = scalingCloud(cloud,imageSize);
    Perturb_Cloud(noiseBound, cloud, scalingFactor, imageSize);
}

bool sortbysec(const pair<double,Point2d> &a,
              const pair<double,Point2d> &b)
{
    return (a.first < b.first);
}

void colormap(std::vector<dataPoint>& points)
{
    dataPoint myPoint;
    double lBound,rBound,t,R,G,B;
    long i=0;
    Vec3b clBound,crBound,computed_color;
    Point2d center;
    Mat fdpImage = Mat::zeros(w, w, CV_8UC3);
    vector<Vec3b> colormap;
    colormap.push_back(Vec3b(255,0,0));
    colormap.push_back(Vec3b(0,255,0));
    colormap.push_back(Vec3b(0,255,255));
    colormap.push_back(Vec3b(0,0,255));
    int pointssize=points.size();
    int colorsize=colormap.size()-1;
    float fit=(float)pointssize/(float)colorsize;
    int ffit=ceil((float)fit);
    vector< pair<double, Point2d> > fdp;
    for(vector<dataPoint>::iterator it=points.begin(); it!= points.end(); it++) {
		dataPoint copyPoint = *it;
		fdp.push_back( make_pair(copyPoint.GaussDensity, copyPoint.pt));
	}
	sort(fdp.begin(), fdp.end(), sortbysec);
	cout<<"Testing whether the sorting works"<<endl;
	int iAvi = 0;
	for(vector<dataPoint>::iterator it=points.begin(); it!= points.end(); it++) {
		
		cout<<fdp[iAvi].first<<" "<<endl;
		cout<<"x "<<fdp[iAvi].second.x<<"y "<<fdp[iAvi].second.y<<endl;
		iAvi++;
	}
    //std::sort(points.begin(), points.end());
    for(vector< pair<double, Point2d> >::iterator it=fdp.begin(); it!= fdp.end(); it++) {
       pair<double, Point2d> myPoint= *it;
        if(i%ffit==0) {
            lBound=fdp[i].first;
        //  cout<< lBound <<endl;
                if((i+ffit)<points.size()) {
                rBound=fdp[(i+(ffit-1))].first; }
            else { rBound=fdp[(points.size()-1)].first; }
        }
            cout<<lBound<<" to "<<rBound<<endl;
            t=(fdp[i].first-lBound)/(rBound-lBound);
            //cout<<t<<endl;
            
         if(i%ffit==0) {
             
             clBound=colormap[i/ffit];
             if(((i/ffit)+1)<colormap.size()) {
                 crBound=colormap[((i/ffit)+1)]; }
             else { crBound=colormap[(colormap.size()-1)]; } }
            computed_color=(((1-t)*clBound)+(t*crBound));
            cout<<computed_color<<endl;
            R=computed_color[2];
            G=computed_color[1];
            B=computed_color[0];
			cout<<"R G B "<<R<<" "<<G<<" "<<B<<endl;
            center=Point(myPoint.second.x,myPoint.second.y);
            circle(fdpImage, center, 2, CV_RGB(R,G,B), 2); 
            
            //cout<<i<<endl; 
            i++;
                String coloredCloud = "fdptestcoloredCloud";
                write_image(coloredCloud+".png", fdpImage);
    
    }
    
}
int main(){
    vector<dataPoint> points;
    /*bool compare(int a, int b) {
        double left= points[a].GaussDensity;
        double right=points[b].GaussDensity;
        return left<right; } */
    Point center;
    Mat mapperImage = Mat::zeros(w, w, CV_8UC3);
    mapperImage = cv::Scalar(255,255,255);
    readPoints(points, mapperImage); //input taken as a point cloud.
    //calculateEccentricity(points);
    calculateGaussDensity(points, 1.0);
    char window[] = "Scaled Cloud";
    String scaledCloud = "test";
    
    cout<<"Enter the size of the image to be saved"<<endl;
    int imageSize;
    cin>>imageSize;
    double scalingFactor = scalingCloud(points, imageSize);
    cout<<"The scaling factor is:"<<scalingFactor<<endl;
    dataPoint currentVector;
    cout<<"The size of the input cloud is:"<<points.size()<<endl;
    for(std::vector<dataPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        currentVector = *it;
        currentVector.pt.x *= scalingFactor;
        currentVector.pt.y *= scalingFactor;
        cout<<currentVector.pt.x<<" "<<currentVector.pt.y<<endl;;
        center = Point(currentVector.pt.x, currentVector.pt.y);
        circle(mapperImage, center, 1, CV_RGB(255,0,0), 3);
    }
    colormap(points);
    write_image(scaledCloud+".png", mapperImage);
    cout << "5th task begins:"<<endl;
    cout << "Please input the size of the point cloud"<<endl;
    int cloudSize;
    cin >> cloudSize;
    double noiseBound = 0.1;
    randomCloudNearCircle(noiseBound, imageSize, cloudSize);
    return 0;
}
