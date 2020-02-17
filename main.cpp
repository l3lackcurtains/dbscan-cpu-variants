#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#define DATASET_SIZE 100
#define ELIPSON 10
#define MIN_POINTS 5

using namespace std;

struct Point { 
    int id, x, y;
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

    public:
        DBSCAN(Point dataset[DATASET_SIZE]);
        void run();
        vector<int> findNeighbors(int pos);
        void expandCluster(int pointId, vector<int> &neighbors);
        void results();
};

int main(int, char **) {

    // Generate random datasets
    Point dataset[DATASET_SIZE]; 

    for(int i = 0; i < DATASET_SIZE; i++) {
        int x = rand() % 50;
        int y = rand() % 50;
        dataset[i].id = i;
        dataset[i].x = x;
        dataset[i].y = y;
    }
    
    printf("Random Dataset created \n###############################\n");
    for(int i = 0; i < DATASET_SIZE; i++) {
        printf("\n%d: [%d, %d] \n", i, dataset[i].x, dataset[i].y);
    }
    printf("############################### \n");

    DBSCAN dbscan(dataset);

    dbscan.run();

    dbscan.results();

    return 0;
}

DBSCAN::DBSCAN(Point loadData[DATASET_SIZE]) {
    elipson = ELIPSON;
    minPoints = MIN_POINTS;

    for(int i = 0; i < DATASET_SIZE; i++) {
        dataset[i].x = loadData[i].x;
        dataset[i].y = loadData[i].y;
        dataset[i].id = loadData[i].id;
        visited[i] = 0;
    }
}

int DBSCAN::getDistance(int center, int neighbor) {
    int dist = pow(dataset[center].x - dataset[neighbor].x, 2) + pow(dataset[center].y - dataset[neighbor].y, 2);
    return sqrt(dist);
}

void DBSCAN::run() {
    vector<int> neighbors;
    clusterCount = 0;

    for (int i = 0; i < DATASET_SIZE; i++) {
        if(visited[i] == 0) {
            visited[i] = 1;

            neighbors = findNeighbors(i);
            
            if (neighbors.size() < minPoints) {
                
                clusters[clusterCount].id = clusterCount;
                clusters[clusterCount].data.push_back(i);

            } else {

                clusterCount++;

                Cluster cluster;
                cluster.id = clusterCount;
                cluster.data = {};

                clusters.push_back(cluster);
                
                expandCluster(i, neighbors);
                
            }
        }
    }

    
}

void DBSCAN::results() {
    for(int j = 0; j < clusters.size(); j++) {
        printf("Data for cluster %d \n", j);
        for(int k = 0; k < clusters[j].data.size(); k++) {
            printf("Point [%d, %d] \n", dataset[clusters[j].data[k]].x, dataset[clusters[j].data[k]].y);
        }
    }
}

vector<int> DBSCAN::findNeighbors(int pos) {

    vector<int> neighbors;

    int neighborCount = 0;
    
    for (int x = 0; x < DATASET_SIZE; x++) {
        int distance = getDistance(pos, x);

        if (distance < elipson) {
            neighbors.push_back(x);
        }
    }

    
    printf("Neighbor of dataset [%d, %d]", dataset[pos].x, dataset[pos].y);

    for(int i = 0; i < neighbors.size(); i++) {
        printf("\n%d: [%d, %d] \n", neighbors[i], dataset[neighbors[i]].x, dataset[neighbors[i]].y ); 
    }
    

    return neighbors;
}

void DBSCAN::expandCluster(int pointId, vector<int> &neighbors) {

    

    clusters[clusterCount].data.push_back(pointId);

    for(int b = 0; b < clusters.size(); b++) {
        cout<<clusters[b].data.size()<<endl;
    }

    cout<<"here!"<<endl;

    vector<int> moreNeighbors;
    
    for(int i = 0; i < neighbors.size(); i++) {
        if (visited[i] == 0) {
            visited[i] = 1;

            moreNeighbors = findNeighbors(i);
            int moreNeighborSize = moreNeighbors.size();

            if(moreNeighborSize >= minPoints) {

                for(int x = 0; x < moreNeighborSize; x++) {                    
                    neighbors.push_back(moreNeighbors[x]);
                }
            }

            for(int x = 0; x < clusterCount; x++) {
                for(int y = 0; y < clusters[x].data.size(); y++) {
                    
                    if(clusters[x].data[y] != i) { 
                        clusters[clusterCount].data.push_back(i);
                    }
                } 
            }
        }
    }
}