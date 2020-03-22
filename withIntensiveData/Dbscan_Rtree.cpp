#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#include "Rtree.h"

#define DATASET_SIZE 1864620
// #define DATASET_SIZE 10000
#define DIMENTION 2
#define ELIPSON 1.5
#define MIN_POINTS 4

using namespace std;

struct Rect {
  Rect() {}
  long double min[2];
  long double max[2];
  Rect(long double a_minX, long double a_minY, long double a_maxX,
       long double a_maxY) {
    min[0] = a_minX;
    min[1] = a_minY;

    max[0] = a_maxX;
    max[1] = a_maxY;
  }
};

vector<long int> searchNeighbors;
bool searchBoxCallback(int id) {
  searchNeighbors.push_back(id);
  return true;
}

class DBSCAN {
 private:
  long double **dataset;
  int elipson;
  int minPoints;
  int cluster;
  long int *clusters;
  long double getDistance(long int center, long int neighbor);
  set<long int> findNeighbors(long int pos);
  RTree<long double, long double, 2, double> tree;

 public:
  DBSCAN(long double **dataset);
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

int importDataset(char const *fname, int N, long double **dataset) {
  FILE *fp = fopen(fname, "r");

  if (!fp) {
    printf("Unable to open file\n");
    return (1);
  }

  char buf[4096];
  int rowCnt = 0;
  int colCnt = 0;
  while (fgets(buf, 4096, fp) && rowCnt < N) {
    colCnt = 0;

    char *field = strtok(buf, ",");
    long double tmp;
    sscanf(field, "%Lf", &tmp);
    dataset[rowCnt][colCnt] = tmp;

    while (field) {
      colCnt++;
      field = strtok(NULL, ",");

      if (field != NULL) {
        long double tmp;
        sscanf(field, "%Lf", &tmp);
        dataset[rowCnt][colCnt] = tmp;
      }
    }
    rowCnt++;
  }

  fclose(fp);

  return 0;
}

int main(int, char **) {
  // Generate random datasets
  long double **dataset =
      (long double **)malloc(sizeof(long double *) * DATASET_SIZE);
  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (long double *)malloc(sizeof(long double) * DIMENTION);
  }

  // Import Dataset from a file
  importDataset("../dataset/dataset.txt", DATASET_SIZE, dataset);

  // Initialize DBSCAN with dataset
  DBSCAN dbscan(dataset);

  // Run the DBSCAN algorithm
  dbscan.run();

  // Print the cluster results of DBSCAN
  dbscan.results();

  return 0;
}

DBSCAN::DBSCAN(long double **loadData) {
  dataset = (long double **)malloc(sizeof(long double *) * DATASET_SIZE);
  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i] = (long double *)malloc(sizeof(long double) * DIMENTION);
  }

  clusters = (long int *)malloc(sizeof(long int) * DATASET_SIZE);

  elipson = ELIPSON;
  minPoints = MIN_POINTS;
  cluster = 0;

  for (long int i = 0; i < DATASET_SIZE; i++) {
    dataset[i][0] = loadData[i][0];
    dataset[i][1] = loadData[i][1];
    clusters[i] = 0;

    // Insert Data into tree
    Rect rectange =
        Rect(dataset[i][0], dataset[i][1], dataset[i][0], dataset[i][1]);
    tree.Insert(rectange.min, rectange.max, i);
  }
}

long double DBSCAN::getDistance(long int center, long int neighbor) {
  long double dist = (dataset[center][0] - dataset[neighbor][0]) *
                         (dataset[center][0] - dataset[neighbor][0]) +
                     (dataset[center][1] - dataset[neighbor][1]) *
                         (dataset[center][1] - dataset[neighbor][1]);
  return dist;
}

void DBSCAN::run() {
  // Neighbors of the point
  set<long int> neighbors;

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

        set<long int> seedNeighbors = neighbors;

        // Expand the neighbors of point P
        for (long int dataIndex : seedNeighbors) {
          // Mark neighbour as point Q
          if (clusters[dataIndex] == -1) {
            clusters[dataIndex] = cluster;
          } else if (clusters[dataIndex] == 0) {
            clusters[dataIndex] = cluster;

            // Expand more neighbors of point Q
            neighbors = findNeighbors(dataIndex);

            if (neighbors.size() >= minPoints) {
              for (long int newNeighbor : neighbors) {
                seedNeighbors.insert(newNeighbor);
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
  for (int i = 0; i < DATASET_SIZE; i++) {
    if (clusters[i] == -1) {
      noises++;
    }
  }

  printf("Noises: %d\n", noises);
}

set<long int> DBSCAN::findNeighbors(long int pos) {
  set<long int> neighbors;

  Rect searchRect = Rect(dataset[pos][0] - elipson, dataset[pos][1] - elipson,
                         dataset[pos][0] + elipson, dataset[pos][1] + elipson);

  searchNeighbors.clear();
  tree.Search(searchRect.min, searchRect.max, searchBoxCallback);

  for (int x = 0; x < searchNeighbors.size(); x++) {
    // Compute neighbor points of a point at position "pos"
    int distance = getDistance(pos, searchNeighbors[x]);
    if (distance <= elipson * elipson && searchNeighbors[x] != pos) {
      neighbors.insert(searchNeighbors[x]);
    }
  }

  return neighbors;
}