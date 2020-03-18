#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#define DATASET_SIZE 1000
#define ELIPSON 30
#define MIN_POINTS 10

using namespace std;
class DBSCAN {
 private:
  double dataset[DATASET_SIZE][2];
  int elipson;
  int minPoints;
  int cluster;
  int clusters[DATASET_SIZE];
  double getDistance(int center, int neighbor);
  vector<int> findNeighbors(int pos);
  void expandCluster(int pointId, vector<int> &neighbors);

 public:
  DBSCAN(double dataset[DATASET_SIZE][2]);
  void run();
  void results();
};

int main(int, char **) {
  // Generate random datasets
  double dataset[DATASET_SIZE][2];
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
        field = strtok(NULL, ",");
        if (field!=NULL) {
          double tmp;
          sscanf(field,"%lf",&tmp);
          dataset[rowCount][colCount] = tmp;
        }
      }
      if(rowCount == DATASET_SIZE) break;
      rowCount++;
    }
    file.close();
  }

  // Print dataset in an array structure
  int count = 0;
  for (int i = 0; i < DATASET_SIZE; i++) {
    count++;
  }
  printf("%d Dataset created\n", count);

  printf("############################### \n");

  // Initialize DBSCAN with dataset
  DBSCAN dbscan(dataset);

  // Run the DBSCAN algorithm
  dbscan.run();

  // Print the cluster results of DBSCAN
  dbscan.results();

  return 0;
}

DBSCAN::DBSCAN(double loadData[DATASET_SIZE][2]) {
  elipson = ELIPSON;
  minPoints = MIN_POINTS;
  cluster = 0;

  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i][0] = loadData[i][0];
    dataset[i][1] = loadData[i][1];
    clusters[i] = 0;
  }
}

double DBSCAN::getDistance(int center, int neighbor) {
  double dist = (dataset[center][0] - dataset[neighbor][0]) *
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
  for(int x = 1; x <= cluster; x++) {
    int count = 0;
    for(int i = 0; i < DATASET_SIZE; i++) {
      if(clusters[i] == x) {
        count++;
      }
    }
    printf("Cluster %d has %d data \n", x, count);
  }
  
}

vector<int> DBSCAN::findNeighbors(int pos) {
  vector<int> neighbors;

  for (int x = 0; x < DATASET_SIZE; x++) {
    // Compute neighbor points of a point at position "pos"
    double distance = getDistance(pos, x);
    if (distance < elipson && pos != x) {
      neighbors.push_back(x);
    }
  }

  return neighbors;
}