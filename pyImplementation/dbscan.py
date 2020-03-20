import numpy as np
import pandas as pd
from sklearn.cluster import DBSCAN

data = pd.read_csv('../dataset/dataset.txt', sep=",", header=None).to_numpy()

X = data[:1000000]

db = DBSCAN(eps=30, min_samples=10).fit(X)
cls_labels = db.labels_

max_clusters = max(cls_labels) + 1
clusters = {}
noises = []
for i in range(0, max_clusters):
    cluster = []
    for j in range(0, len(cls_labels)):
        if i == cls_labels[j]:
            cluster.append(list(X[j]))
    clusters[i] = cluster

for j in range(0, len(cls_labels)):
    if cls_labels[j] == - 1:
        noises.append(list(X[j]))


print("Number of clusters: ", max_clusters)
for i in range(0, max_clusters):
    print("Cluster", i, "has", len(clusters[i]), "data" )
