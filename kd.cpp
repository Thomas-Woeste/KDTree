/* Author: Thomas Woeste
 * Program: kd.cpp
 * Date: 3/12/2020
 * Purpose: The purpose of this program is to implement a 4 dimensional kd tree. 
 */

#include <iostream>
#include <vector>

using namespace std;

//Make a struct to represent an element of a bucket
struct dataPoint {
    int dimensions[4];
}; //dataPoint()

//Make a node for the KD tree
struct kdNode {
    int dimension;
    int data;
    vector<dataPoint> bucket;
    kdNode* left;
    kdNode* right;
}; //kdNode()

//Makes a new dataPoint with the passed in dimensions
dataPoint makeDataPoint(int dimension0, int dimension1, int dimension2, int dimension3){
    dataPoint data;
    data.dimensions[0] = dimension0;
    data.dimensions[1] = dimension1;
    data.dimensions[2] = dimension2;
    data.dimensions[3] = dimension3;
    return data;
} //makeDataPoint()

//Print the dimensions of the passed in dataPoint in order
void printDataPoints(dataPoint data){
    cout << data.dimensions[0] << " " << data.dimensions[1] << " ";
    cout << data.dimensions[2] << " " << data.dimensions[3] << endl; 
} //printDataPoints()


//Makes a new node and sets its children to null
kdNode* makeNode(){
  kdNode* answer = new kdNode;
  answer->left = NULL; 
  answer->right = NULL;
  return answer;
} //makeNode()


//Finds the max of the array
int findMax(vector<dataPoint> allPts, int dim){
    int max = -1;
    for(auto i = allPts.begin(); i != allPts.end(); i++){
        if(i->dimensions[dim] > max){
            max = i->dimensions[dim];
        }
    }
    return max;
} //findMax

//Finds the min of the array
int findMin(vector<dataPoint> allPts, int dim){
    int min = 1000;
    for(auto i = allPts.begin(); i != allPts.end(); i++){
        if(i->dimensions[dim] < min){
            min = i->dimensions[dim];
        }
    }
    return min;
} //findMin()

//Finds what dimensions has the largest range
int findRange(vector<dataPoint> allPts){
    int maxRangeDim;

    int max0 = findMax(allPts, 0);
    int min0 = findMin(allPts, 0);
    int max1 = findMax(allPts, 1);
    int min1 = findMin(allPts, 1);
    int max2 = findMax(allPts, 2);
    int min2 = findMin(allPts, 2);
    int max3 = findMax(allPts, 3);
    int min3 = findMin(allPts, 3);
    int diff[4];

    diff[0] = max0-min0;
    diff[1] = max1-min1;
    diff[2] = max2-min2;
    diff[3] = max3-min3;
    int maxDiff = -1;

    for(int j = 0; j < 4; j++){
        if(diff[j] > maxDiff){
            maxDiff = diff[j];
            maxRangeDim = j;
        }
    }
    
    return maxRangeDim;
} //findRange()

//creates a new array with only data from the passed in dimension
void makeDimArray(vector<dataPoint> allPts, int dim, int dimArray[]){
    int j = 0;
    for(auto k = allPts.begin(); k != allPts.end(); k++){
        dimArray[j] = k->dimensions[dim];
        j++;
    } 
} //makeDimArray()

//swaps the value at the first input with the second 
void swap(int smallDimArray[], int lowIndex, int highIndex){
    int temp = smallDimArray[lowIndex];
    smallDimArray[lowIndex] = smallDimArray[highIndex];
    smallDimArray[highIndex] = temp;
} //Swap()

//Partions the the array using lomutos method
int lPartition(int smallDimArray[], int lowIndex, int highIndex){
    int pivotValue = smallDimArray[lowIndex];
    int divideIndex = lowIndex;
    for (int combIndex = lowIndex+1; combIndex <= highIndex; combIndex++){
        if(smallDimArray[combIndex] < pivotValue){
            divideIndex += 1;
            swap(smallDimArray, divideIndex, combIndex);
        }
    }
    swap(smallDimArray, divideIndex, lowIndex);
    return divideIndex;
} //lPartition()

//Find the median of the array
int findMedian(int smallDimArray[], int size){
    int targetIndex = 0;
    if(size % 2 == 1){
        targetIndex = size/2;
    }
    else{
        targetIndex = size/2 - 1;
    }
    int lowIndex = 0;
    int highIndex = size-1;

    while(lowIndex < highIndex) {
        int midIndex = lPartition(smallDimArray, lowIndex, highIndex);
        if(midIndex == targetIndex){
            return smallDimArray[targetIndex];
        }
        else if (midIndex < targetIndex){
            lowIndex = midIndex + 1;
        }
        else {
            highIndex = midIndex - 1;
        }
    }
    return smallDimArray[targetIndex];
} //findMedian()

//makes a tree with the topNode at the top. The leaf will have the resulting buckets.
void makeTree(kdNode* topNode){
    if(topNode->bucket.size() <= 10){
        return;
    }
    else{
        int dimension = findRange(topNode->bucket);
        int *dimArray = new int[topNode->bucket.size()];
        makeDimArray(topNode->bucket, dimension, dimArray);
        int median = findMedian(dimArray, topNode->bucket.size());

        topNode->dimension = dimension;
        topNode->data = median;

        kdNode *nLeftNode = makeNode();
        kdNode *nRightNode = makeNode();

        for(auto i = topNode->bucket.begin(); i != topNode->bucket.end(); i++){
            if(i->dimensions[topNode->dimension] <= median){
                nLeftNode->bucket.push_back(makeDataPoint(i->dimensions[0], i->dimensions[1], i->dimensions[2], i->dimensions[3]));   
            }
            else{
                nRightNode->bucket.push_back(makeDataPoint(i->dimensions[0], i->dimensions[1], i->dimensions[2], i->dimensions[3]));
            }
        }
        topNode->left = nLeftNode;
        topNode->right = nRightNode;

        makeTree(topNode->left);
        makeTree(topNode->right);
    } 
} //makeTree()

//Is given a probe and a head pointer to the tree created
kdNode *searchTree(kdNode *node, dataPoint probe){
    int dim = node->dimension;
    int data = node->data;

    while((node->left != nullptr) && (node->right != nullptr)){
        if(probe.dimensions[dim] <= data){
            return searchTree(node->left, probe);
        }
        else if (probe.dimensions[dim] > data){
            return searchTree(node->right, probe);
        }
    }
    return node;
} //searchTree()


//The number of input arguments
const int ARGUMENTS = 3;

int main(int argc, char* argv[]){
    if(argc != ARGUMENTS){
        cout << "Warning: need excatally" << ARGUMENTS - 1 << " command line arguments." << endl;

        return 1;
    }
    
    //Converts the argv value to a int
    const int NUMPOINTS = atoi(argv[1]);
    const int NUMPROBES = atoi(argv[2]);
    
    //Makes sure your have more than 1 point
    if(NUMPOINTS < 0 ){
        cout << "The number of data points has to be greater than 0" << endl;

        return 1;
    }
    int Data0 = 0;
    int Data1 = 0;
    int Data2 = 0;
    int Data3 = 0;
    
    //Inputs the data into a vector
    vector<dataPoint> totalPoints;
    for(int i = 0; i <  NUMPOINTS; i++){
        cin >> Data0 >> Data1 >> Data2 >> Data3;
        totalPoints.push_back(makeDataPoint(Data0, Data1, Data2, Data3));
    }
    
    //Creates a head node
    kdNode *head = makeNode();
    head->bucket = totalPoints;
    //Makes the tree
    makeTree(head);

    dataPoint probe;

    //Creates the probes and iterates through them
    for(int j = 0; j < NUMPROBES; j++){
        cin >> Data0 >> Data1 >> Data2 >> Data3;
        probe = makeDataPoint(Data0, Data1, Data2, Data3);

        kdNode *probeBucket = searchTree(head, probe);
        cout << "Probe: " << Data0 << " " << Data1 << " " << Data2 << " " << Data3 << " ";
        cout << "reaches bucket: " << probeBucket->bucket.size() << " elements." << endl;

        for(auto i = probeBucket->bucket.begin(); i != probeBucket->bucket.end(); i++){
            printDataPoints(*i);
        }
    }
    
    
    return 0;    
}








