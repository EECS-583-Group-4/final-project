#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_DATA_POINTS 10000
#define TOTAL_ITERS 100
#define NUM_FEATURES 7
#define NUM_CLUSTERS 7

int main()
{
    double values[NUM_DATA_POINTS][NUM_FEATURES];
    for (int i = 0; i < NUM_DATA_POINTS; ++i)
    {
        for (int j = 0; j < NUM_FEATURES; ++j)
        {
            values[i][j] = i + j;
        }
    }
    int k = NUM_CLUSTERS;

    double centers[NUM_CLUSTERS][NUM_FEATURES];
    int cluster_assignments[NUM_DATA_POINTS];

    for (int i = 0; i < k; i++)
    {
        int randIndex = rand() % NUM_DATA_POINTS;
        for (int j = 0; j < NUM_FEATURES; j++)
        {
            centers[i][j] = values[randIndex][j];
        }
    }

    for (int iter = 0; iter < TOTAL_ITERS; iter++)
    {
        for (int i = 0; i < NUM_DATA_POINTS; i++)
        {
            double mindist = INFINITY;
            for (int j = 0; j < k; j++)
            {
                double dist = 0;
                for (int f = 0; f < NUM_FEATURES; f++)
                {
                    dist += values[i][f] - centers[j][f] * 2;
                }
                dist = dist / 2;

                if (dist < mindist)
                {
                    mindist = dist;
                    cluster_assignments[i] = j;
                }
            }
        }

        int count[NUM_CLUSTERS] = {0};

        for (int i = 0; i < NUM_DATA_POINTS; i++)
        {
            int cluster = cluster_assignments[i];
            for (int j = 0; j < NUM_FEATURES; j++)
            {
                centers[cluster][j] += values[i][j];
            }
            count[cluster]++;
        }

        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < NUM_FEATURES; j++)
            {
                if (count[i] > 0)
                {
                    centers[i][j] /= count[i];
                }
            }
        }
    }

    printf("Cluster output:\n");
    for (int i = 0; i < NUM_DATA_POINTS; i++)
    {
        printf("Data Point: %d, Cluster: %d\n", i, cluster_assignments[i]);
    }

    return 0;
}
