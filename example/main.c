

/*

This progam is just for testing purpose

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void sort_array(int* arr, int len) {
    for (int i = 0; i < len - 1; ++i) {
        for (int j = 0; j < len - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

void print_array(int* arr, int len) {
    for (int i = 0; i < len; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    char name[64];
    printf("Enter your name: ");
    scanf("%63s", name);

    printf("Hello, %s!\n", name);

    int nums[5] = {5, 2, 9, 1, 3};
    printf("Original array: ");
    print_array(nums, 5);

    sort_array(nums, 5);
    printf("Sorted array: ");
    print_array(nums, 5);

    int fib = fibonacci(10);
    printf("Fibonacci(10) = %d\n", fib);

    return 0;
}
