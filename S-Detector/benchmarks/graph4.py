import numpy as np

#just copied from results spreadsheet
v_u = np.array([20968,15992, 16104,16112,16120,16016,15896,16016])
v_m = np.array([25112,16168,16104,16160,16168,16016,15896,16016])
v_n = np.array([25112,16168, 16152,16160,20264,16120,20160,16120])

nodes = ["alexnet.c", "backtrack.c", "bubblesort.c", "insertionsort.c", "kmeans.c", "offsets.c", "patterns.c", "summation.c"]
X_axis = np.arange(len(nodes)) 

import matplotlib.pyplot as plt
W = 0.25
plt.bar(X_axis, v_u, W, color='green', edgecolor='black', label="Unmodified")   
plt.bar(X_axis + W, v_m, W, color='blue',  edgecolor='black', label="MAS Based Pass")
plt.bar(X_axis + 2*W, v_n, W, color='red', edgecolor='black', label="Naive Pass")
 

plt.xticks(X_axis + W, nodes, rotation=40) 
plt.xlabel("Benchmark")
plt.ylim([0, 26000])
plt.ylabel("Program Size (Bytes)")
plt.legend()
plt.tight_layout()
plt.show()

# Save the plot
#plt.savefig('graph4.png', dpi=300, bbox_inches='tight')