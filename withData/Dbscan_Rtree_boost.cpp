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

#define DATASET_SIZE 25
#define ELIPSON 10
#define MIN_POINTS 5

using namespace std;

struct Point {
  int x, y;
};

struct Cluster {
  int id;
  vector<int> data;
};

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> dataPoint;
typedef bg::model::box<dataPoint> box;
typedef std::pair<box, int> value;

class DBSCAN {
 private:
  Point dataset[DATASET_SIZE];
  int elipson;
  int minPoints;
  vector<Cluster> clusters;
  int clusterCount;
  int visited[DATASET_SIZE];
  vector<int> noises;
  int getDistance(int center, int neighbor);
  vector<int> findNeighbors(int pos);
  void expandCluster(int pointId, vector<int> &neighbors);
  bgi::rtree<value, bgi::quadratic<4>> rtree;

 public:
  DBSCAN(Point dataset[DATASET_SIZE]);
  void run();
  void results();
};

int main(int, char **) {

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
          sscanf(field,"%d",&tmp);
          
          dataset[count].x = tmp;

          field = strtok(NULL, ",");
          sscanf(field,"%d",&tmp);
          dataset[count].y = tmp;
          
          count++;
      }
      file.close();
  }

  printf("Random Dataset created\n");
  printf("###############################\n");

  // Print dataset in an array structure
  printf("[");
  for (int i = 0; i < DATASET_SIZE; i++) {
    printf("[%d, %d], ", dataset[i].x, dataset[i].y);
  }
  printf("]\n");

  printf("###############################\n");

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
  }

  // Create an Rtree of the dataset
  for (int i = 0; i < DATASET_SIZE; i++) {
    // create a box for each points
    box b(dataPoint(dataset[i].x, dataset[i].y),
          dataPoint(dataset[i].x, dataset[i].y));
    // insert points to the rtree
    rtree.insert(std::make_pair(b, i));
    
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
          bool notFound = false;
          for (int l = 0; l < cluster.data.size(); l++) {
            if (cluster.data[l] != dataIndex) notFound = true;
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

    printf("\n]\n");

  }
}

vector<int> DBSCAN::findNeighbors(int pos) {

  vector<int> neighbors;
  Point point = dataset[pos];
  vector<value> result_n;

  // Create a search box for the given poiny
  box searchBox(dataPoint(point.x - elipson, point.y - elipson),
                dataPoint(point.x + elipson, point.y + elipson));

  // Query the intersection of search box on Rtree
  rtree.query(bgi::intersects(searchBox), std::back_inserter(result_n));

  // collect the points of box
  vector<int> pointsInBox = {};
  for (value pair : result_n) pointsInBox.push_back(pair.second);

  // Compute the distance only with points in a box
  for (int x = 0; x < pointsInBox.size(); x++) {
    // Compute neighbor points
    int distance = getDistance(pos, pointsInBox[x]);
    if (distance <= elipson && pos != pointsInBox[x]) {
      neighbors.push_back(x);
    }
  }

  return neighbors;

}