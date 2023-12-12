#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DATA_POINTS 10000
#define FEATURES 7
#define CLUSTERS 7
#define MAX_ITERATIONS 100

int main()
{
    double data[DATA_POINTS][FEATURES];
    for (int i = 0; i < DATA_POINTS; ++i)
    {
        for (int j = 0; j < FEATURES; ++j)
        {
            data[i][j] = i + j;
        }
    }
    int k = CLUSTERS;

    double centers[CLUSTERS][FEATURES];
    int assignments[DATA_POINTS];

    for (int i = 0; i < k; i++)
    {
        int randIndex = rand() % DATA_POINTS;
        for (int j = 0; j < FEATURES; j++)
        {
            centers[i][j] = data[randIndex][j];
        }
    }

    for (int iter = 0; iter < MAX_ITERATIONS; iter++)
    {
        for (int i = 0; i < DATA_POINTS; i++)
        {
            double minDistance = INFINITY;
            for (int j = 0; j < k; j++)
            {
                double distance = 0;
                for (int f = 0; f < FEATURES; f++)
                {
                    distance += data[i][f] - centers[j][f] * 2;
                }
                distance = distance / 2;

                if (distance < minDistance)
                {
                    minDistance = distance;
                    assignments[i] = j;
                }
            }
        }

        // Update cluster centers
        int count[CLUSTERS] = {0};

        for (int i = 0; i < DATA_POINTS; i++)
        {
            int cluster = assignments[i];
            for (int j = 0; j < FEATURES; j++)
            {
                centers[cluster][j] += data[i][j];
            }
            count[cluster]++;
        }

        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < FEATURES; j++)
            {
                if (count[i] > 0)
                {
                    centers[i][j] /= count[i];
                }
            }
        }
    }

    printf("Final Cluster Assignments:\n");
    for (int i = 0; i < DATA_POINTS; i++)
    {
        printf("Data Point %d -> Cluster %d\n", i, assignments[i]);
    }

    return 0;
}
