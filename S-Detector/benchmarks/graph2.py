import numpy as np

#just copied from results spreadsheet
v_u = np.array([29.27,27.69,27.3,29.35,27.45,29.51,24.02,26.87])
v_m = np.array([46.49,31.23,27.49,52.26,28.09,29.23,24.27,26.93])
v_n = np.array([44.42,32.11,64.51,62.24,41.62,54.64,77.14,76.86])

nodes = ["alexnet.c", "backtrack.c", "bubblesort.c", "insertionsort.c", "kmeans.c", "offsets.c", "patterns.c", "summation.c"]
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
#plt.savefig('graph2.png', dpi=300, bbox_inches='tight')