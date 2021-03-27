#include "stdio.h"
#include "stdlib.h"
#include "string.h"


void insertionSort(int *arr, int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int* merge(int *arr1, int *arr2, int len1, int len2) {
    /* Merge the temp arrays back into arr[l..r]*/
    int i1 = 0; // Initial index of first subarray
    int i2 = 0;
    int k = 0;
    int *new_array = (int*) malloc((len1 + len2) * sizeof(int));
    while (i1 < len1 && i2 < len2)
    {
        if (arr1[i1] <= arr2[i2])
        {
            new_array[k] = arr1[i1];
            i1++;
        }
        else
        {
            new_array[k] = arr2[i2];
            i2++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i1 < len1)
    {
        new_array[k] = arr1[i1];
        i1++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (i2 < len2)
    {
        new_array[k] = arr2[i2];
        i2++;
        k++;
    }

    free(arr1);
    free(arr2);
    return new_array;
}


int main(int argc, char **argv) {
    FILE *myfile;
    printf("received parameters %d ", argc);
    int **file_mem = (int **) malloc((argc - 1) * sizeof(int *));
    int *file_lenghts = (int *) malloc((argc - 1) * sizeof(int));
    for (int nfile = 0; nfile < argc - 1; nfile++) {
        myfile = fopen(argv[nfile + 1], "r");
        int len = 10;
        file_mem[nfile] = (int *) malloc(len * sizeof(int));
        int i = 0;
        while (fscanf(myfile, "%d", &file_mem[nfile][i]) != EOF) {
            i++;
            if (i == len) {
                printf("i == %d\n", i);
                len = len * 2;
                file_mem[nfile] = (int *) realloc(file_mem[nfile], len * sizeof(int));
            }
        };
        file_lenghts[nfile] = i;

        insertionSort(file_mem[nfile], i);
    }

    for (int nfile = 0; nfile < argc - 1; nfile++) {
        if (nfile != argc - 2) {
            printf("EKEKEKEK\n");
            int *new_arr = merge(file_mem[nfile], file_mem[nfile + 1], file_lenghts[nfile], file_lenghts[nfile + 1]);
            file_mem[nfile + 1] = new_arr;
            file_lenghts[nfile + 1] = file_lenghts[nfile] + file_lenghts[nfile + 1];
            for (int z = 0; z < file_lenghts[nfile + 1]; z++ ){
                printf("%d ", file_mem[nfile + 1][z]);
            }
            printf("\n\n");
        }
    }
}
