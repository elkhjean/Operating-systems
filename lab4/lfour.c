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
int calculateTotalHeadMovement(int *sequence, int initialPosition);
int findShortest(bool *processed, int currentPosition);

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
    printf("FCFS: %d\n", calculateTotalHeadMovement(requests, initialPosition));
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
    printf("SSTF: %d\n", calculateTotalHeadMovement(seq, initialPosition));
}

// SCAN disk scheduling algorithm
void scan(int initialPosition)
{
}

// C-SCAN disk scheduling algorithm
void cscan(int initialPosition)
{
}

// LOOK disk scheduling algorithm
void look(int initialPosition)
{
}

// C-LOOK disk scheduling algorithm
void clook(int initialPosition)
{
}

// Calculates the total head movement for a given sequence of requests
int calculateTotalHeadMovement(int *sequence, int initialPosition)
{
    int headDistance = abs(sequence[0] - initialPosition);
    for (int i = 0; i < (REQUESTS - 1); i++)
    {
        headDistance += abs(sequence[i + 1] - sequence[i]);
    }
    return headDistance;
}

// Finds index of shortest distance request
int findShortest(bool *processed, int currentPosition)
{
    int min = MAX_CYLINDERS, minIndex = 0;
    for (int i = 0; i < REQUESTS; i++)
    {
        if (!processed[i] && abs(requests[i] - currentPosition) < min)
        {
            min = abs(requests[i] - currentPosition);
            minIndex = i;
        }
    }
    return minIndex;
}