import numpy as np

#just copied from results spreadsheet
v_u = np.array([16104,16112,16120,16016,15896,16016,20968,15992])
v_m = np.array([16104,16160,16168,16016,15896,16016,25112,16168])
v_n = np.array([16152,16160,20264,16120,20160,16120,25112,16168])

nodes = ["bubblesort.c", "insertionsort.c", "kmeans.c", "offsets.c", "patterns.c", "summation.c", "alexnet.c", "backtrack.c"]
X_axis = np.arange(len(nodes)) 

import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
W = 0.4
plt.bar(X_axis, (v_m /v_u), W, color='blue', edgecolor='black', label="MAS Based Pass")
plt.bar(X_axis + W, (v_n /v_u), W, color='red', edgecolor='black', label="Naive Pass")
 

plt.xticks(X_axis + W/2, nodes, rotation=40) 
plt.xlabel("Benchmark")
plt.ylim([0,3.5])
plt.ylabel("Performance Overhead")
plt.legend()
plt.tight_layout()
plt.gca().yaxis.set_major_formatter(mtick.PercentFormatter(xmax=1.0))
plt.subplots_adjust(left=0.135)
plt.show()

# Save plot
plt.savefig('graph5.png', dpi=300, bbox_inches='tight')