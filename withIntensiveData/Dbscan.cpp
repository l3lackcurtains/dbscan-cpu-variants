#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#define DATASET_SIZE 10000
#define DIMENTION 2
#define ELIPSON 30
#define MIN_POINTS 10

using namespace std;
class DBSCAN {
 private:
  double** dataset;
  int elipson;
  int minPoints;
  int cluster;
  int* clusters;
  double getDistance(int center, int neighbor);
  void findNeighbors(int pos, int* neighbors, int &neighborsSize);

 public:
  DBSCAN(double** loadData);
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
        field = strtok(NULL, ",");
        if (field!=NULL) {
          double tmp;
          sscanf(field,"%lf",&tmp);
          dataset[rowCount][colCount] = tmp;
        }
        colCount++;
        if(colCount == DIMENTION) break;
      }
      rowCount++;
      if(rowCount == DATASET_SIZE) break;
    }
    file.close();
  }

  // Print dataset in an array structure
  printf("############################### \n");

  // Initialize DBSCAN with dataset
  DBSCAN dbscan(dataset);

  // Run the DBSCAN algorithm
  dbscan.run();

  // Print the cluster results of DBSCAN
  dbscan.results();

  return 0;
}

DBSCAN::DBSCAN(double** loadData) {
  
  clusters = (int *)malloc(sizeof(int) * DATASET_SIZE);

  dataset =
      (double **)malloc(sizeof(double *) * DATASET_SIZE);
  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (double *)malloc(sizeof(double) * DIMENTION);
  }

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

  for (int i = 0; i < DATASET_SIZE; i++) {
    int* neighbors = (int*)malloc(sizeof(int) * DATASET_SIZE);
    int neighborsSize = 0;
    if (clusters[i] == 0) {

      // Find neighbors of point P
      findNeighbors(i, neighbors, neighborsSize);

      // Mark noise points
      if (neighborsSize < minPoints) {
        clusters[i] = -1;
      } else {
        // Increment cluster and initialize it will the current point
        cluster++;

        clusters[i] = cluster; 

        // Expand the neighbors of point P
        for (int j = 0; j < neighborsSize; j++) {

          // Mark neighbour as point Q
          int dataIndex = neighbors[j];

          if(clusters[dataIndex] == -1) {
            clusters[dataIndex] = cluster;
          } else if (clusters[dataIndex] == 0) {

            clusters[dataIndex] = cluster;
            
            // Expand more neighbors of point Q
            int* moreNeighbors = (int*)malloc(sizeof(int) * DATASET_SIZE);
            int moreNeighbourSize = 0;
            findNeighbors(dataIndex, moreNeighbors, moreNeighbourSize);

            // Continue when neighbors point is higher than minPoint threshold
            if (moreNeighbourSize >= minPoints) {
              // Check if neighbour of Q already exists in neighbour of P
              for (int x = 0; x < moreNeighbourSize; x++) {
                bool doesntExist = true;
                for (int y = 0; y < neighborsSize; y++) {
                  if (moreNeighbors[x] == neighbors[y]) {
                    doesntExist = false;
                    break;
                  }
                }

                // If neighbour doesn't exist, add to neighbor list
                if (doesntExist) {
                  neighbors[neighborsSize] = moreNeighbors[x];
                  neighborsSize++;
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

void DBSCAN::findNeighbors(int pos, int* neighbors, int &neighborsSize) {
  for (int x = 0; x < DATASET_SIZE; x++) {
    // Compute neighbor points of a point at position "pos"
    double distance = getDistance(pos, x);
    if (distance < elipson && pos != x) {
      neighbors[neighborsSize] = x;
      neighborsSize++;
    }
  }
}