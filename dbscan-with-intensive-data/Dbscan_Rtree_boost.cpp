#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

// #define DATASET_SIZE 1000000
#define DIMENTION 2
#define ELIPSON 30
#define MIN_POINTS 10

using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> dataPoint;
typedef bg::model::box<dataPoint> box;
typedef std::pair<box, int> value;

class DBSCAN {
 private:
  double** dataset;
  int elipson;
  int minPoints;
  int cluster;
  int* clusters;
  double getDistance(int center, int neighbor);
  vector<int> findNeighbors(int pos);
  void expandCluster(int pointId, vector<int> &neighbors);
  bgi::rtree<value, bgi::quadratic<4>> rtree;

 public:
  DBSCAN(double ** dataset);
  void run();
  void results();
};

int main(int, char **) {
  // Generate random datasets
  double **dataset =
      (double **)malloc(sizeof(double *) * DATASET_SIZE);
  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (double *)malloc(sizeof(double) * DIMENTION);
  }

  // Import Dataset from a file
  ifstream file("../dataset/dataset.txt");
  if (file.is_open()) {
    string token;
    int rowCount = 0;
    while (getline(file, token)) {
      int colCount = 0;
      char* x = (char*)token.c_str();
      char* field = strtok(x, ",");
      double tmp;
      sscanf(field, "%lf", &tmp);
      dataset[rowCount][colCount] = tmp;
      while (field) {
        colCount++;
        if(colCount == DIMENTION) break;
        field = strtok(NULL, ",");
        if (field!=NULL) {
          double tmp;
          sscanf(field,"%lf",&tmp);
          dataset[rowCount][colCount] = tmp;
        }
      }
      rowCount++;
      if(rowCount == DATASET_SIZE) break;
    }
    file.close();
  }

  // Initialize DBSCAN with dataset
  DBSCAN dbscan(dataset);

  // Run the DBSCAN algorithm
  dbscan.run();

  // Print the cluster results of DBSCAN
  dbscan.results();

  return 0;
}

DBSCAN::DBSCAN(double **loadData) {

  dataset =
      (double **)malloc(sizeof(double *) * DATASET_SIZE);
  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (double *)malloc(sizeof(double) * DIMENTION);
  }
  clusters = (int *)malloc(sizeof(int) * DATASET_SIZE);
  elipson = ELIPSON;
  minPoints = MIN_POINTS;
  cluster = 0;

  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i][0] = loadData[i][0];
    dataset[i][1] = loadData[i][1];
    clusters[i] = 0;
  }

  // Create an Rtree of the dataset
  for (int i = 0; i < DATASET_SIZE; i++) {
    // create a box for each points
    box b(dataPoint(dataset[i][0], dataset[i][1]),
          dataPoint(dataset[i][0], dataset[i][1]));
    // insert points to the rtree
    rtree.insert(std::make_pair(b, i));
    
  }
}

double DBSCAN::getDistance(int center, int neighbor) {
  int dist = (dataset[center][0] - dataset[neighbor][0]) *
                 (dataset[center][0] - dataset[neighbor][0]) +
             (dataset[center][1] - dataset[neighbor][1]) *
                 (dataset[center][1] - dataset[neighbor][1]);

  return sqrt(dist);
}

void DBSCAN::run() {
  // Neighbors of the point
  vector<int> neighbors;

  for (int i = 0; i < DATASET_SIZE; i++) {
    
    if (clusters[i] == 0) {

      // Find neighbors of point P
      neighbors = findNeighbors(i);

      // Mark noise points
      if (neighbors.size() < minPoints) {
        clusters[i] = -1;
      } else {
        // Increment cluster and initialize it will the current point
        cluster++;

        clusters[i] = cluster; 

        // Expand the neighbors of point P
        for (int j = 0; j < neighbors.size(); j++) {

          // Mark neighbour as point Q
          int dataIndex = neighbors[j];

          if(clusters[dataIndex] == -1) {
            clusters[dataIndex] = cluster;
          } else if (clusters[dataIndex] == 0) {

            clusters[dataIndex] = cluster;
            
            // Expand more neighbors of point Q
            vector<int> moreNeighbors;
            moreNeighbors = findNeighbors(dataIndex);

            // Continue when neighbors point is higher than minPoint threshold

            if (moreNeighbors.size() >= minPoints) {
              // Check if neighbour of Q already exists in neighbour of P
              for (int x = 0; x < moreNeighbors.size(); x++) {
                bool doesntExist = true;
                for (int y = 0; y < neighbors.size(); y++) {
                  if (moreNeighbors[x] == neighbors[y]) {
                    doesntExist = false;
                    break;
                  }
                }

                // If neighbour doesn't exist, add to neighbor list
                if (doesntExist) {
                  neighbors.push_back(moreNeighbors[x]);
                }
              }
            }
          }         
      }
    }
  }
}
}

void DBSCAN::results() {
  printf("Number of clusters: %d\n", cluster);
  int noises = 0;
  for(int x = 1; x <= cluster; x++) {
    int count = 0;
    for(int i = 0; i < DATASET_SIZE; i++) {
      if(clusters[i] == x) {
        count++;
      }
      if(clusters[i] == -1) {
        noises++;
      }
    }
    printf("Cluster %d has %d data\n", x, count);
  }
  printf("Noises: %d\n", noises);
  
}

vector<int> DBSCAN::findNeighbors(int pos) {

  vector<int> neighbors;
  vector<value> result_n;

  // Create a search box for the given poiny
  box searchBox(dataPoint(dataset[pos][0] - elipson, dataset[pos][1] - elipson),
                dataPoint(dataset[pos][0] + elipson, dataset[pos][1] + elipson));

  // Query the intersection of search box on Rtree
  rtree.query(bgi::intersects(searchBox), std::back_inserter(result_n));

  // collect the points of box
  vector<int> pointsInBox = {};
  for (value pair : result_n) pointsInBox.push_back(pair.second);

  // Compute the distance only with points in a box
  for (int x = 0; x < pointsInBox.size(); x++) {
    // Compute neighbor points
    double distance = getDistance(pos, pointsInBox[x]);
    if (distance <= elipson && pos != pointsInBox[x]) {
      neighbors.push_back(pointsInBox[x]);
    }
  }

  return neighbors;

}