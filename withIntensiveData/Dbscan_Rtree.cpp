#include <Rtree.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#define DATASET_SIZE 10
#define ELIPSON 10
#define MIN_POINTS 5

using namespace std;

struct Point {
  double x, y;
};

struct Rect {
  Rect() {}
  int min[2];
  int max[2];
  Rect(int a_minX, int a_minY, int a_maxX, int a_maxY) {
    min[0] = a_minX;
    min[1] = a_minY;

    max[0] = a_maxX;
    max[1] = a_maxY;
  }
};

struct Cluster {
  int id;
  vector<int> data;
};

vector<int> searchNeighbors;
bool searchBoxCallback(int id) {
  searchNeighbors.push_back(id);
  return true;
}

class DBSCAN {
 private:
  Point dataset[DATASET_SIZE];
  int elipson;
  int minPoints;
  int cluster;
  int clusters[DATASET_SIZE];
  double getDistance(int center, int neighbor);
  vector<int> findNeighbors(int pos);
  RTree<int, int, 2, float> tree; 

 public:
  DBSCAN(Point dataset[DATASET_SIZE]);
  void run();
  void results();
};

int main(int, char **) {
  // Generate random datasets
  Point dataset[DATASET_SIZE];

  // Import Dataset from a file
  ifstream file("./dataset.txt");
  if (file.is_open()) {
    string token;
    int count = 0;
    while (getline(file, token)) {
      char* x = (char*)token.c_str();
      char* field = strtok(x, ",");
      double tmp;
      sscanf(field, "%lf", &tmp);
      dataset[count].x = tmp;
      
      field = strtok(NULL, ",");
      sscanf(field, "%lf", &tmp);
      dataset[count].y = tmp;

      count++;
    }
    file.close();
  }

  printf("Random Dataset created \n ############################### \n");

  // Print dataset in an array structure
  printf("[");
  for (int i = 0; i < DATASET_SIZE; i++) {
    printf("[%lf, %lf], ", dataset[i].x, dataset[i].y);
  }
  printf("]\n");

  printf("############################### \n");

  // Initialize DBSCAN with dataset
  DBSCAN dbscan(dataset);

  // Run the DBSCAN algorithm
  dbscan.run();

  // Print the cluster results of DBSCAN
  dbscan.results();

  return 0;
}

DBSCAN::DBSCAN(Point loadData[DATASET_SIZE]) {
  elipson = ELIPSON;
  minPoints = MIN_POINTS;
  cluster = 0;

  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i].x = loadData[i].x;
    dataset[i].y = loadData[i].y;
    clusters[i] = 0;

    // Insert Data into tree
    Rect rectange = Rect(dataset[i].x, dataset[i].y, dataset[i].x, dataset[i].y);
    tree.Insert(rectange.min, rectange.max, i);
  }
}

double DBSCAN::getDistance(int center, int neighbor) {
  double dist = (dataset[center].x - dataset[neighbor].x) *
                 (dataset[center].x - dataset[neighbor].x) +
             (dataset[center].y - dataset[neighbor].y) *
                 (dataset[center].y - dataset[neighbor].y);

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
  for(int x = 1; x <= cluster; x++) {
    printf("CLuster %d: \n[\n", x);
    for(int i = 0; i < DATASET_SIZE; i++) {
      if(clusters[i] == x) {
        printf("  [%lf, %lf]\n", dataset[i].x, dataset[i].y);
      }
    }
    printf("]\n");
  }
  
}

vector<int> DBSCAN::findNeighbors(int pos) {
  vector<int> neighbors;

  Rect searchRect = Rect(dataset[pos].x - elipson, dataset[pos].y - elipson,
                         dataset[pos].x + elipson, dataset[pos].y + elipson);

  searchNeighbors.clear();
  tree.Search(searchRect.min, searchRect.max, searchBoxCallback);

  for (int x = 0; x < searchNeighbors.size(); x++) {
    // Compute neighbor points of a point at position "pos"
    double distance = getDistance(pos, searchNeighbors[x]);
    if (distance <= elipson && pos != searchNeighbors[x]) {
      neighbors.push_back(searchNeighbors[x]);
    }
  }

  return neighbors;
}