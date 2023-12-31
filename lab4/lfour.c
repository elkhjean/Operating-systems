#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Macros
#define MAX_CYLINDERS 5000
#define REQUESTS 1000

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
int findShortest(bool *processed, int currentPosition, int size);
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
        indexOfShortest = findShortest(processed, currentPosition, REQUESTS);
        processed[indexOfShortest] = true;
        currentPosition = requests[indexOfShortest];
        seq[i] = currentPosition;
    }
    printf("SSTF: %d\n", calculateTotalHeadMovement(seq, initialPosition, sizeof(seq) / sizeof(seq[0])));
}
// Comparison function for qsort
int compareIntegers(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// Function to reverse an array
void reverseArray(int *array, size_t start, size_t end)
{
    while (start < end)
    {
        int temp = array[start];
        array[start] = array[end];
        array[end] = temp;
        start++;
        end--;
    }
}

// Function to rotate an array based on a given value
void rotateArray(int *array, size_t arraySize, int value)
{
    // Find the index where the value is located in the array
    size_t index = 0;
    while (index < arraySize && array[index] < value)
    {
        index++;
    }

    // If the value is found, reverse the two subarrays around the value
    if (index < arraySize)
    {
        reverseArray(array, 0, index - 1);
        reverseArray(array, index, arraySize - 1);
        reverseArray(array, 0, arraySize - 1);
    }
}
void printArray(int *array, size_t arraySize)
{
    for (size_t i = 0; i < arraySize; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
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
    size_t size = sizeof(requests) / sizeof(requests[0]);
    int sortedRequest[size + 2];
    sortedRequest[0] = 0;
    sortedRequest[size + 1] = MAX_CYLINDERS - 1;
    qsort(requests, size, sizeof(int), compareIntegers);
    for (size_t i = 0; i < size; i++)
    {
        sortedRequest[i + 1] = requests[i];
    }
    rotateArray(sortedRequest, size + 2, initialPosition);

    printf("CSCAN: %d\n", calculateTotalHeadMovement(sortedRequest, initialPosition, size + 2));
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
    size_t size = sizeof(requests) / sizeof(requests[0]);
    int sortedRequest[size];
    for (size_t i = 0; i < size; i++)
    {
        sortedRequest[i] = requests[i];
    }
    qsort(sortedRequest, size, sizeof(int), compareIntegers);

    rotateArray(sortedRequest, size, initialPosition);
    printf("C-LOOK: %d\n", calculateTotalHeadMovement(sortedRequest, initialPosition, size));
}

// Calculates the total head movement for a given sequence of requests
int calculateTotalHeadMovement(int *sequence, int initialPosition, int size)
{
    int headDistance = abs(sequence[0] - initialPosition);
    for (int i = 0; i < (size - 1); i++)
    {
  //      printf("%d ", sequence[i]);
        headDistance += abs(sequence[i + 1] - sequence[i]);
    }
  //  printf("%d ", sequence[size - 1]);
    return headDistance;
}

// Finds index of shortest distance request
int findShortest(bool *processed, int currentPosition, int size)
{
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