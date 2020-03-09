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

#define DATASET_SIZE 25
#define ELIPSON 10
#define MIN_POINTS 5

using namespace std;

struct Point {
  int x, y;
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
  vector<Cluster> clusters;
  int clusterCount;
  int visited[DATASET_SIZE];
  vector<int> noises;
  RTree<int, int, 2, float> tree;
  int getDistance(int center, int neighbor);
  vector<int> findNeighbors(int pos);
  void expandCluster(int pointId, vector<int>& neighbors);

 public:
  DBSCAN(Point dataset[DATASET_SIZE]);
  void run();
  void results();
};

int main(int, char**) {
  // Generate random datasets
  Point dataset[DATASET_SIZE];

  // Import Dataset
  ifstream file("./dataset.txt");
  if (file.is_open()) {
    string token;
    int count = 0;
    while (getline(file, token)) {
      char* x = (char*)token.c_str();
      char* field = strtok(x, ",");
      int tmp;
      sscanf(field, "%d", &tmp);
      dataset[count].x = tmp;
      
      field = strtok(NULL, ",");
      sscanf(field, "%d", &tmp);
      dataset[count].y = tmp;

      count++;
    }
    file.close();
  }

  printf("Random Dataset created \n ############################### \n");

  // Print dataset in an array structure
  printf("[");
  for (int i = 0; i < DATASET_SIZE; i++) {
    printf("[%d, %d], ", dataset[i].x, dataset[i].y);
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

  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i].x = loadData[i].x;
    dataset[i].y = loadData[i].y;
    visited[i] = 0;

    // Insert Data into tree
    Rect rectange =
        Rect(dataset[i].x, dataset[i].y, dataset[i].x, dataset[i].y);
    tree.Insert(rectange.min, rectange.max, i);
  }
}

int DBSCAN::getDistance(int center, int neighbor) {
  int dist = (dataset[center].x - dataset[neighbor].x) *
                 (dataset[center].x - dataset[neighbor].x) +
             (dataset[center].y - dataset[neighbor].y) *
                 (dataset[center].y - dataset[neighbor].y);

  return sqrt(dist);
}

void DBSCAN::run() {
  // Neighbors of the point
  vector<int> neighbors;

  // Cluster count
  clusterCount = 0;

  for (int i = 0; i < DATASET_SIZE; i++) {
    // Continue when P is not visited
    if (visited[i] == 0) {
      // Mark P as visited
      visited[i] = 1;

      // Find neighbors of point P
      neighbors = findNeighbors(i);

      // Mark noise points
      if (neighbors.size() < minPoints) {
        noises.push_back(i);
        break;
      }

      // Increment cluster and initialize it will the current point
      clusterCount++;

      Cluster cluster;
      cluster.id = clusterCount;
      cluster.data.push_back(i);

      // Expand the neighbors of point P
      for (int j = 0; j < neighbors.size(); j++) {

        // Mark neighbour as point Q
        int dataIndex = neighbors[j];

        // If point is a noise push to the cluster
        for (int k = 0; k < noises.size(); k++) {
          if (noises[k] == dataIndex) cluster.data.push_back(dataIndex);
        }

        // Continue when Q is not visited
        if (visited[dataIndex] == 0) {
          // Mark Q as visited
          visited[dataIndex] = 1;
          
          // Check if point Q already exists in cluster
          bool notFound = true;
          for(int i = 0; i < clusters.size(); i++) {
            for (int l = 0; l < clusters[i].data.size(); l++) {
              if (clusters[i].data[l] == dataIndex) {
                notFound = false;
                break;
              }
            }
          }

          // If point Q doesn't exist in any cluster, add to current cluster
          if (notFound) {
            cluster.data.push_back(dataIndex);
          }

          // Expand more neighbors of point Q
          vector<int> moreNeighbors;
          moreNeighbors = findNeighbors(dataIndex);

          // Continue when neighbors point is higher than minPoint threshold

          if (moreNeighbors.size() >= minPoints) {
            // Check if neighbour of Q already exists in neighbour of P
            bool doesntExist = true;
            for (int x = 0; x < moreNeighbors.size(); x++) {
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
      // Push cluster into clusters vector
      clusters.push_back(cluster);
    }
  }
}

void DBSCAN::results() {
  for (int j = 0; j < clusters.size(); j++) {
    printf("Data for cluster %d \n", j);
    printf("[\n");
    for (int k = 0; k < clusters[j].data.size(); k++) {
      printf("  [%d, %d]\n", dataset[clusters[j].data[k]].x,
             dataset[clusters[j].data[k]].y);
    }
    printf("]\n");
  }
  
  printf("%d Noises: \n[\n", noises.size());
  for(int i = 0; i < noises.size(); i++) {
    printf("  [%d %d]\n", dataset[noises[i]].x, dataset[noises[i]].y);
  }
  printf("]\n");
}

vector<int> DBSCAN::findNeighbors(int pos) {
  vector<int> neighbors;

  Rect searchRect = Rect(dataset[pos].x - elipson, dataset[pos].y - elipson,
                         dataset[pos].x + elipson, dataset[pos].y + elipson);

  searchNeighbors.clear();
  tree.Search(searchRect.min, searchRect.max, searchBoxCallback);

  for (int x = 0; x < searchNeighbors.size(); x++) {
    // Compute neighbor points of a point at position "pos"
    int distance = getDistance(pos, searchNeighbors[x]);
    if (distance <= elipson && pos != searchNeighbors[x]) {
      neighbors.push_back(searchNeighbors[x]);
    }
  }

  return neighbors;
}