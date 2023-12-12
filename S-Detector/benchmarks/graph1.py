import numpy as np

#just copied from results spreadsheet
v_u = np.array([27.3,29.35,27.45,29.51,24.02,26.87])
v_m = np.array([27.49,52.26,28.09,29.23,24.27,26.93])
v_n = np.array([64.51,62.24,41.62,54.64,77.14,76.86])

nodes = ["bubblesort.c", "insertionsort.c", "kmeans.c", "offsets.c", "patterns.c", "summation.c"]
X_axis = np.arange(len(nodes)) 

import matplotlib.pyplot as plt
W = 0.25
plt.bar(X_axis, v_u, W, color='white', hatch="//", edgecolor='black', label="Unmodified")   
plt.bar(X_axis + W, v_m, W, color='gray',  edgecolor='black', label="MAS Based Pass")
plt.bar(X_axis + 2*W, v_n, W, color='black', edgecolor='black', label="Naive Pass")
 

plt.xticks(X_axis + W, nodes, rotation=40) 
plt.xlabel("Benchmark")
plt.ylim([0, 80])
plt.ylabel("Runtime (seconds)")
plt.legend()
plt.tight_layout()
plt.show()