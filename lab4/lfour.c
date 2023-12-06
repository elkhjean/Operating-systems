#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Macros
#define MAX_CYLINDERS 50
#define REQUESTS 10

// Global Variables
int requests[REQUESTS]; // Array to hold the disk requests
int headPosition;       // Current position of the disk head

// Methods
void generateRequests();
void fcfs(int initialPosition);
void sstf(int initialPosition);
void scan(int initialPosition);
void cscan(int initialPosition);
void look(int initialPosition);
void clook(int initialPosition);
int calculateTotalHeadMovement(int *sequence, int initialPosition, int size);
int findShortest(bool *processed, int currentPosition);
int compareAscending(const void *a, const void *b);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <initial head position>\n", argv[0]);
        return 1;
    }

    headPosition = atoi(argv[1]);
    if (headPosition < 0 || headPosition >= MAX_CYLINDERS)
    {
        printf("Invalid initial head position. It must be between 0 and 4999.\n");
        return 1;
    }

    generateRequests();

    fcfs(headPosition);
    sstf(headPosition);
    scan(headPosition);
    cscan(headPosition);
    look(headPosition);
    clook(headPosition);

    return 0;
}

// Generates a random series of disk requests
void generateRequests()
{
    srand(time(NULL));
    for (int i = 0; i < REQUESTS; i++)
    {
        requests[i] = rand() % MAX_CYLINDERS;
    }
}

// FCFS (First Come First Served) disk scheduling algorithm
void fcfs(int initialPosition)
{
    printf("FCFS: %d\n", calculateTotalHeadMovement(requests, initialPosition, REQUESTS));
}

// SSTF (Shortest Seek Time First) disk scheduling algorithm
void sstf(int initialPosition)
{
    int seq[REQUESTS];
    bool processed[REQUESTS] = {false};
    int i = 0, currentPosition = initialPosition, indexOfShortest;
    for (int i = 0; i < REQUESTS; i++)
    {
        indexOfShortest = findShortest(processed, currentPosition);
        processed[indexOfShortest] = true;
        currentPosition = requests[indexOfShortest];
        seq[i] = currentPosition;
    }
    printf("SSTF: %d\n", calculateTotalHeadMovement(seq, initialPosition, sizeof(seq) / sizeof(seq[0])));
}

// SCAN disk scheduling algorithm
void scan(int initialPosition)
{
    // Sort the requests in ascending order
    int sortedRequests[REQUESTS];
    for (size_t i = 0; i < REQUESTS; i++)
    {
        sortedRequests[i] = requests[i];
    }
    qsort(sortedRequests, REQUESTS, sizeof(int), compareAscending);

    // Find where to split the array
    int pivotIndex = 0;
    while (pivotIndex < REQUESTS && sortedRequests[pivotIndex] < initialPosition)
    {
        pivotIndex++;
    }

    // Split array into higher and lower parts
    int *seq = malloc((REQUESTS + 2) * sizeof(int));
    int i, j;
    for (i = 0; i < (REQUESTS - pivotIndex); i++)
    {
        seq[i] = sortedRequests[pivotIndex + i];
    }
    seq[i] = MAX_CYLINDERS - 1;
    i++;
    for (j = pivotIndex - 1; j >= 0; j--, i++)
    {
        seq[i] = sortedRequests[j];
    }
    i++;
    seq[REQUESTS + 1] = 0;

    // Count distance
    printf("SCAN: %d\n", calculateTotalHeadMovement(seq, initialPosition, REQUESTS + 2));
    free(seq);
}

// C-SCAN disk scheduling algorithm
void cscan(int initialPosition)
{
}

// LOOK disk scheduling algorithm
void look(int initialPosition)
{
    // Sort the requests in ascending order
    int sortedRequests[REQUESTS];
    for (size_t i = 0; i < REQUESTS; i++)
    {
        sortedRequests[i] = requests[i];
    }
    qsort(sortedRequests, REQUESTS, sizeof(int), compareAscending);

    // Find where to split the array
    int pivotIndex = 0;
    while (pivotIndex < REQUESTS && sortedRequests[pivotIndex] < initialPosition)
    {
        pivotIndex++;
    }

    // Split array into higher and lower parts
    int *seq = malloc((REQUESTS) * sizeof(int));
    int i, j;
    for (i = 0; i < (REQUESTS - pivotIndex); i++)
    {
        seq[i] = sortedRequests[pivotIndex + i];
    }
    for (j = pivotIndex - 1; j >= 0; j--, i++)
    {
        seq[i] = sortedRequests[j];
    }

    // Count distance
    printf("LOOK: %d\n", calculateTotalHeadMovement(seq, initialPosition, REQUESTS));
    free(seq);
}

// C-LOOK disk scheduling algorithm
void clook(int initialPosition)
{
}

// Calculates the total head movement for a given sequence of requests
int calculateTotalHeadMovement(int *sequence, int initialPosition, int size)
{
    int headDistance = abs(sequence[0] - initialPosition);
    for (int i = 0; i < (size - 1); i++)
    {
        printf("%d ", sequence[i]);
        headDistance += abs(sequence[i + 1] - sequence[i]);
    }
    printf("%d ", sequence[size-1]);
    return headDistance;
}

// Finds index of shortest distance request
int findShortest(bool *processed, int currentPosition)
{
    size_t size = sizeof(processed) / sizeof(processed[0]);
    int min = MAX_CYLINDERS, minIndex = 0;
    for (int i = 0; i < size; i++)
    {
        if (!processed[i] && abs(requests[i] - currentPosition) < min)
        {
            min = abs(requests[i] - currentPosition);
            minIndex = i;
        }
    }
    return minIndex;
}

// Comparison function for ascending order
int compareAscending(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}