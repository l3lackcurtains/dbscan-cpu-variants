#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#define DATASET_SIZE 1864620
// #define DATASET_SIZE 1000
#define DIMENTION 2
#define ELIPSON 1.5
#define MIN_POINTS 4

using namespace std;
class DBSCAN {
 private:
  double **dataset;
  int elipson;
  int minPoints;
  int cluster;
  long int *clusters;
  double getDistance(long int center, long int neighbor);
  vector<long int> findNeighbors(long int pos);

 public:
  DBSCAN(double **loadData);
  void run();
  void results();
};

void remove(vector<long int> &v) {
  auto end = v.end();
  for (auto it = v.begin(); it != end; ++it) {
    end = std::remove(it + 1, end, *it);
  }

  v.erase(end, v.end());
}

int main(int, char **) {
  // Generate random datasets
  double **dataset = (double **)malloc(sizeof(double *) * DATASET_SIZE);
  for (long int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (double *)malloc(sizeof(double) * DIMENTION);
  }

  // Import Dataset from a file
  ifstream file("../dataset/dataset.txt");
  if (file.is_open()) {
    string token;
    int rowCount = 0;
    while (getline(file, token)) {
      int colCount = 0;
      char *x = (char *)token.c_str();
      char *field = strtok(x, ",");
      double tmp;
      sscanf(field, "%lf", &tmp);
      dataset[rowCount][colCount] = tmp;
      while (field) {
        colCount++;
        if (colCount == DIMENTION) break;
        field = strtok(NULL, ",");
        if (field != NULL) {
          double tmp;
          sscanf(field, "%lf", &tmp);
          dataset[rowCount][colCount] = tmp;
        }
      }
      rowCount++;
      if (rowCount == DATASET_SIZE) break;
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

DBSCAN::DBSCAN(double **loadData) {
  clusters = (long int *)malloc(sizeof(long int) * DATASET_SIZE);

  dataset = (double **)malloc(sizeof(double *) * DATASET_SIZE);
  for (long int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (double *)malloc(sizeof(double) * DIMENTION);
  }

  elipson = ELIPSON;
  minPoints = MIN_POINTS;
  cluster = 0;

  for (long int i = 0; i < DATASET_SIZE; i++) {
    dataset[i][0] = loadData[i][0];
    dataset[i][1] = loadData[i][1];
    clusters[i] = 0;
  }
}

double DBSCAN::getDistance(long int center, long int neighbor) {
  double dist = (dataset[center][0] - dataset[neighbor][0]) *
                    (dataset[center][0] - dataset[neighbor][0]) +
                (dataset[center][1] - dataset[neighbor][1]) *
                    (dataset[center][1] - dataset[neighbor][1]);

  return sqrt(dist);
}

void DBSCAN::run() {
  // Neighbors of the point
  vector<long int> neighbors;

  for (long int i = 0; i < DATASET_SIZE; i++) {
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

        vector<long int> seedNeighbors = neighbors;
        remove(seedNeighbors);

        // Expand the neighbors of point P
        for (long int j = 0; j < seedNeighbors.size(); j++) {
          // Mark neighbour as point Q
          long int dataIndex = seedNeighbors[j];

          if (clusters[dataIndex] == -1) {
            clusters[dataIndex] = cluster;
          } else if (clusters[dataIndex] == 0) {
            clusters[dataIndex] = cluster;

            // Expand more neighbors of point Q
            vector<long int> moreNeighbors;
            moreNeighbors = findNeighbors(dataIndex);

            // Continue when neighbors point is higher than minPoint threshold
            if (moreNeighbors.size() >= minPoints) {
              // Check if neighbour of Q already exists in neighbour of P
              for (long int x = 0; x < moreNeighbors.size(); x++) {
                seedNeighbors.push_back(moreNeighbors[x]);
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
  for (int x = 1; x <= cluster; x++) {
    int count = 0;
    for (long int i = 0; i < DATASET_SIZE; i++) {
      if (clusters[i] == x) {
        count++;
      }
      if (clusters[i] == -1) {
        noises++;
      }
    }
    printf("Cluster %d has %d data\n", x, count);
  }
  printf("Noises: %d\n", noises);
}

vector<long int> DBSCAN::findNeighbors(long int pos) {
  vector<long int> neighbors;
  for (long int x = 0; x < DATASET_SIZE; x++) {
    // Compute neighbor points of a point at position "pos"
    double distance = getDistance(pos, x);
    if (distance < elipson && pos != x) {
      neighbors.push_back(x);
    }
  }
  return neighbors;
}