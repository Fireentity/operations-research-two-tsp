#include <stdio.h>
#include <stdlib.h>

// Structure to represent a point
typedef struct {
    int x;
    int y;
} Point;

int main(void) {
    int n, seed;
    printf("Enter the number of points to generate: ");
    scanf("%d", &n);

    printf("Enter the seed for the random number generator: ");
    scanf("%d", &seed);

    // Initialize the random number generator with the provided seed
    srand(seed);

    for (int i = 0; i < n; i++) {
        Point p;
        p.x = rand() % 101;  // Random number between 0 and 100 for x
        p.y = rand() % 101;  // Random number between 0 and 100 for y

        printf("Point %d: (%d, %d)\n", i + 1, p.x, p.y);
    }

    return 0;
}