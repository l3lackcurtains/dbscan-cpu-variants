import numpy as np
from sklearn.cluster import DBSCAN

X = np.array([[10, 5], [11, 6], [12, 7], [9, 12], [9, 10], [10, 12], [15, 10],
              [40, 45], [45, 40], [45, 38], [38, 40], [45, 39], [40, 50],
              [40, 43], [43, 44], [44, 45], [90, 91], [91, 95], [89, 88],
              [88, 95], [95, 99], [83, 94], [96, 91], [96, 93], [90, 98]])

print('Runing scikit-learn implementation...')

db = DBSCAN(eps=10, min_samples=5).fit(X)
cls_labels = db.labels_

max_clusters = max(cls_labels) + 1
clusters = {}
for i in range(0, max_clusters):
    cluster = []
    for j in range(0, len(cls_labels)):
        if(i == cls_labels[j]):
            cluster.append(list(X[j]))
    clusters[i] = cluster

print("Number of clusters = ", max_clusters)
print(clusters)
