import numpy as np

#just copied from results spreadsheet
v_u = np.array([16104,16112,16120,16016,15896,16016,20968,15992])
v_m = np.array([16104,16160,16168,16016,15896,16016,25112,16168])
v_n = np.array([16152,16160,20264,16120,20160,16120,25112,16168])

nodes = ["bubblesort.c", "insertionsort.c", "kmeans.c", "offsets.c", "patterns.c", "summation.c", "alexnet.c", "backtrack.c"]
X_axis = np.arange(len(nodes)) 

import matplotlib.pyplot as plt
W = 0.4
plt.bar(X_axis, v_m, W, color='blue',  edgecolor='black', label="MAS Based Pass")
plt.bar(X_axis + W, v_n, W, color='red', edgecolor='black', label="Naive Pass")
 

plt.xticks(X_axis + W, nodes, rotation=40) 
plt.xlabel("Benchmark")
plt.ylim([0, 26000])
plt.ylabel("Runtime (seconds)")
plt.legend()
plt.tight_layout()
plt.show()

# Save the plot
plt.savefig('graph6.png', dpi=300, bbox_inches='tight')