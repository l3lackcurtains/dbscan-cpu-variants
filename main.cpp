#include <iostream>
#include <cmath>
#include <vector>

#define DATASET_SIZE 100
#define ELIPSON 10
#define MIN_POINTS 5

using namespace std;


class DBSCAN {
    private:
        int dataset[DATASET_SIZE][2];
        int elipson;
        int minPoints;
        int cluster = 0;
        vector<vector<int*>> clusters;
        int visited[DATASET_SIZE];
        int noises[DATASET_SIZE];
        int neighborSize = 0;
        int getDistance(int* center, int* point);

    public:
        DBSCAN(int dataset[DATASET_SIZE][2]);
        void run();
        int **findNeighbors(int pos);
        void expandCluster(int pointId, int **neighbors);
        void results();
};

int main(int, char **) {

    // Generate random datasets
    int dataset[DATASET_SIZE][2];

    for(int i = 0; i < DATASET_SIZE; i++) {
        int x = rand() % 100;
        int y = rand() % 100;

        dataset[i][0] = x;
        dataset[i][1] = y;
    }
    printf("Random Dataset created \n ############################### \n");
    for(int i = 0; i < DATASET_SIZE; i++) {
        printf("\n%d: [%d, %d] \n", i, dataset[i][0], dataset[i][1]);
    }
    printf("############################### \n");

    DBSCAN dbscan(dataset);

    dbscan.run();

    dbscan.results();

    return 0;
}

DBSCAN::DBSCAN(int loadData[DATASET_SIZE][2]) {
    elipson = ELIPSON;
    minPoints = MIN_POINTS;

    for(int i = 0; i < DATASET_SIZE; i++) {
        dataset[i][0] = loadData[i][0];
        dataset[i][1] = loadData[i][1];
    }
}

int DBSCAN::getDistance(int* center, int* point) {
    int dist = pow(center[0] - point[0], 2) + pow(center[1] - point[1], 2);
    return sqrt(dist);
}

void DBSCAN::run() {
    
    int noiseCount = 0;
    int** neighbors;

    for (int i = 0; i < DATASET_SIZE; i++) {
        visited[i] = 1;

        neighbors = findNeighbors(i);

        if (neighborSize < minPoints) {
            noises[noiseCount] = i;
            noiseCount++;
        } else {
            expandCluster(i, neighbors);
            cluster++;
        } 
    }
}

void DBSCAN::results() {
    for(int i = 0; i < cluster;  i++) {
        cout<<"["<<clusters[i][0]<<","<<clusters[i][1]<<"]"<<endl;
    }
}

int** DBSCAN::findNeighbors(int pos) {

    int** neighbors = new int*[DATASET_SIZE];

    int point[2];

    point[0] = dataset[pos][0];
    point[1] = dataset[pos][1];

    int neighborCount = 0;
    
    for (int x = 0; x < DATASET_SIZE; x++) {
        int distance = getDistance(point, dataset[x]);

        if (distance < elipson)
        {
            neighbors[neighborCount] = new int[2];
            
            neighbors[neighborCount][0] = dataset[x][0];
            neighbors[neighborCount][1] = dataset[x][1];

            neighborCount++;
        }
    }

    
    printf("Neighbor of dataset [%d, %d]", point[0], point[1]);

    for(int i = 0; i < neighborCount; i++) {
        printf("\n%d: [%d, %d] \n", i, neighbors[i][0], neighbors[i][1]); 
    }

    neighborSize = neighborCount;

    return neighbors;
}

void DBSCAN::expandCluster(int pointId, int **neighbors) {
    clusters[cluster].push_back(dataset[pointId]);

    cout<<clusters[cluster][0]<<endl;

    int** moreNeighbors;

    int currentNeighborSize = neighborSize;

    for(int i = 0; i < currentNeighborSize; i++) {
        if (visited[pointId] != 1) {
            visited[pointId] = 1;

            moreNeighbors = findNeighbors(pointId);
            
            int moreNeighborSize = neighborSize;

            if(moreNeighborSize >= minPoints) {

                for(int x = currentNeighborSize; x < currentNeighborSize + moreNeighborSize; x++) {
                    neighbors[x] = moreNeighbors[x - currentNeighborSize];
                }
            }

        }
    }

    neighborSize = currentNeighborSize;

}