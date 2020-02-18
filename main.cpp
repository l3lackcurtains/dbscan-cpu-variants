#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#define DATASET_SIZE 500
#define ELIPSON 20
#define MIN_POINTS 10

using namespace std;

struct Point { 
    int x, y;
};

struct Cluster {
    int id;
    vector<int> data;
};

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

    public:
        DBSCAN(Point dataset[DATASET_SIZE]);
        void run();
        void results();

};

int main(int, char **) {

    // Generate random datasets
    Point dataset[DATASET_SIZE]; 

    for(int i = 0; i < DATASET_SIZE; i++) {
        int x = rand() % 50;
        int y = rand() % 50;
        dataset[i].x = x;
        dataset[i].y = y;
    }
    
    printf("Random Dataset created \n ############################### \n");

    // Print dataset in an array structure
    printf("[");
    for(int i = 0; i < DATASET_SIZE; i++) {
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

    for(int i = 0; i < DATASET_SIZE; i++) {
        dataset[i].x = loadData[i].x;
        dataset[i].y = loadData[i].y;
        visited[i] = 0;
    }

}

int DBSCAN::getDistance(int center, int neighbor) {

    int dist = pow(dataset[center].x - dataset[neighbor].x, 2) + pow(dataset[center].y - dataset[neighbor].y, 2);

    return sqrt(dist);

}

void DBSCAN::run() {

    // Neighbors of the point
    vector<int> neighbors;

    // Cluster count
    clusterCount = 0;

    for (int i = 0; i < DATASET_SIZE; i++) {

        // Continue when P is not visited
        if(visited[i] == 0) {

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
            for(int j = 0; j < neighbors.size(); j++) {

                // Mark neighbour as point Q
                int dataIndex = neighbors[j];

                // If point is a noise push to the cluster
                for(int k = 0; k < noises.size(); k++) {
                    if(noises[k] == dataIndex)
                        cluster.data.push_back(dataIndex);
                }

                // Continue when Q is not visited
                if(visited[dataIndex] == 0) {

                    // Mark Q as visited
                    visited[dataIndex] = 1;

                    // Check if point Q already exists in cluster
                    bool notFound = false;
                    for(int l = 0; l < cluster.data.size(); l++) {
                        if(cluster.data[l] != dataIndex)
                            notFound = true;
                    }

                    // If point Q doesn't exist in any cluster, add to current cluster
                    if(notFound) {
                        cluster.data.push_back(dataIndex);
                    }
                    
                    // Expand more neighbors of point Q
                    vector<int> moreNeighbors;
                    moreNeighbors = findNeighbors(dataIndex);

                    // Continue when neighbors point is higher than minPoint threshold

                    if (moreNeighbors.size() >= minPoints) {

                        // Check if neighbour of Q already exists in neighbour of P
                        bool doesntExist = true;
                        for(int x = 0; x < moreNeighbors.size(); x++) { 
                            for(int y = 0; y < neighbors.size(); y++) {
                                if(moreNeighbors[x] == neighbors[y]) {
                                    doesntExist = false;
                                }
                            }

                            // If neighbour doesn't exist, add to neighbor list
                            if(doesntExist) {
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

    for(int j = 0; j < clusters.size(); j++) {
        printf("Data for cluster %d \n", j);
        printf("[\n");
        for(int k = 0; k < clusters[j].data.size(); k++) {
            printf("  [%d, %d]\n", dataset[clusters[j].data[k]].x, dataset[clusters[j].data[k]].y);
        }
        printf("]\n");
    }

}

vector<int> DBSCAN::findNeighbors(int pos) {

    vector<int> neighbors;
    
    for (int x = 0; x < DATASET_SIZE; x++) {

        // Compute neighbor points of a point at position "pos"
        int distance = getDistance(pos, x);
        if (distance <= elipson && pos != x) {
            neighbors.push_back(x);
        }

    }

    return neighbors;

}