#include "randomPointOnAGraph.cpp"





int main()
{
    cout << endl << "Please enter the total number of vertices in the boost graph:" << endl;
    int numberOfVertices; cin >> numberOfVertices;
    Graph graph(numberOfVertices);
    VectorPoint2d RandomPoints;
    Mat cloudImage = Mat::zeros(600, 600, CV_8UC3); cloudImage = cv::Scalar(255, 255, 255);
    Mat coloredCloudImage = Mat::zeros(600, 600, CV_8UC3); coloredCloudImage = cv::Scalar(255, 255, 255);
    Mat graphImage = Mat::zeros(600, 600, CV_8UC3); graphImage = cv::Scalar(255, 255, 255);
    int randomCloudSize; cout << endl << "Please enter the size of the random cloud that you want to generate" << endl; cin >> randomCloudSize;
    Graph randomGraph(randomCloudSize);
    euclideanGraph(graph, RandomPoints, randomCloudSize, graphImage);
    euclideanCloud(randomGraph, RandomPoints, randomCloudSize, cloudImage);
    double scaleGraph; Point2d shiftGraph;
    Draw_Graph(graph, 4, CV_RGB(255, 0, 0), graphImage, scaleGraph, shiftGraph, "scaledBoostGraph.png");
    cout << endl << "Please enter the threshold:" << endl;
    double threshold; cin >> threshold;
    neighbourHoodGraph(randomGraph, threshold);
    vector<Graph> graphVector;
    Draw_Graph(randomGraph, 4, CV_RGB(255, 0, 0), cloudImage, scaleGraph, shiftGraph, "scaledRandomCloudGraph.png");
    cout << endl << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cout << endl << "This is the scale and shift sent after computing the random graph!" << endl;
    cout << endl << scaleGraph << " " << shiftGraph << endl;
    findNoOfComponents(randomGraph, randomCloudSize, scaleGraph, shiftGraph, graphVector, coloredCloudImage, threshold);
    
    
    VItr vitr, vend;
    tie(vitr, vend) = boost::vertices(randomGraph);
    
    vector<dataPoint> points;
    dataPoint point;
    
    for (vector<Point2d>::iterator randomPointIterator = RandomPoints.begin(); ((vitr != vend) && (randomPointIterator != RandomPoints.end())); ++vitr, ++randomPointIterator) {
        Point2d pt = *randomPointIterator;
        point.pt.x = randomGraph[*vitr].pt.x;
        point.pt.y = randomGraph[*vitr].pt.y;
        points.push_back (point);
    }
    
    calculateGaussDensity(points, 7.0);
    String scaledCloud = "test";
    cout << "Enter the size of the image to be saved" << endl;
    int imageSize;
    cin >> imageSize;
    scalingCloud(points, imageSize);
    

}




