#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

#define DATASET_SIZE 500
#define ELIPSON 30
#define MIN_POINTS 10

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
  int cluster;
  int clusters[DATASET_SIZE];
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

  for (int i = 0; i < DATASET_SIZE; i++) {
    int x = rand() % 50;
    int y = rand() % 50;
    dataset[i].x = x;
    dataset[i].y = y;
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
  cluster = 0;

  for (int i = 0; i < DATASET_SIZE; i++) {
    dataset[i].x = loadData[i].x;
    dataset[i].y = loadData[i].y;
    clusters[i] = 0;
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
        printf("  [%d, %d]\n", dataset[i].x, dataset[i].y);
      }
    }
    printf("]\n");
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
      neighbors.push_back(pointsInBox[x]);
    }
  }

  return neighbors;

}