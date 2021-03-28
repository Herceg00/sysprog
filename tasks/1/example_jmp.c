#define CORO_LOCAL_DATA struct {                \
    int deep;                           \
    char local_data[128];               \
    int arg;                           \
    double time_start;                 \
    double time_end;                   \
    double elapsed_time;               \
    char* own_file;                    \
    char* file_to_merge;               \
    int num_merged;                    \
    int *file_mem;                     \
    int *merge_mem;                      \
    int len1;                           \
    int len2;                           \
    }

#include "coro_jmp.h"


void sort() {
    FILE *file = fopen(coro_this()->own_file, "r");
    int len = 10;
    coro_this()->file_mem = (int *) malloc(len * sizeof(int));
    int i = 0;
    while (fscanf(file, "%d", &coro_this()->file_mem[i]) != EOF) {
        i++;
        if (i == len) {
            printf("i == %d\n", i);
            len = len * 2;
            coro_this()->file_mem = (int *) realloc(coro_this()->file_mem, len * sizeof(int));
        }
    };
    fclose(file);
    coro_this()->len1 = i;
    int *arr = coro_this()->file_mem;
    int key, j;
    for (i = 1; i < coro_this()->len1; i++) {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
    sprintf(coro_this()->own_file, "tmp%d.txt",
            curr_coro_i);
    FILE *file_write = fopen(coro_this()->own_file, "w");
    for (int m = 0; m < coro_this()->len1; m++) {
        fprintf(file_write, "%d ", arr[m]);
    }
    fclose(file_write);
    if ((curr_coro_i % 2 == 1)) {
        (&coros[curr_coro_i - 1])->len2 = coro_this()->len1;
        (&coros[curr_coro_i - 1])->merge_mem = coro_this()->file_mem;
    }
    coro_return();
}


void merge() {
    int i1 = 0; // Initial index of first subarray
    int i2 = 0;
    int k = 0;
    int *new_array = (int *) malloc((coro_this()->len1 + coro_this()->len2) * sizeof(int));
    while (i1 < coro_this()->len1 && i2 < coro_this()->len2) {
        if (coro_this()->file_mem[i1] <= coro_this()->merge_mem[i2]) {
            new_array[k] = coro_this()->file_mem[i1];
            i1++;
        } else {
            new_array[k] = coro_this()->merge_mem[i2];
            i2++;
        }
        k++;
    }

    while (i1 < coro_this()->len1) {
        new_array[k] = coro_this()->file_mem[i1];
        i1++;
        k++;
    }

    while (i2 < coro_this()->len2) {
        new_array[k] = coro_this()->merge_mem[i2];
        i2++;
        k++;
    }

    free(coro_this()->merge_mem);
    free(coro_this()->file_mem);
    coro_this()->num_merged += 1;
    if (((curr_coro_i/coro_this()->num_merged) % 2 == 0) && (curr_coro_i + 2 >= coro_count)) {
        coro_this()->file_mem = new_array;
    }
    coro_return();
}

/**
 * You can compile and run this example using the commands:
 *
 * $> gcc example_jmp.c
 * $> ./a.out
 */

/**
 * A function, called from inside of coroutines, and even
 * recursively.
 */
void
other_function(int arg) {
    printf("Coro %d: entered function, deep = %d, arg = %d\n", curr_coro_i,
           coro_this()->deep, arg);
    coro_this()->arg = arg;
    coro_yield();
    printf("Coro %d: after yield arg = %d, but coro_this()->arg = %d\n",
           curr_coro_i, arg, coro_this()->arg);
    /*
     * Here I've decided to call it recursively 2 times in
     * each coro.
     */
    if (++coro_this()->deep < 2)
        coro_call(other_function, curr_coro_i * 10);
    coro_return();
}

/**
 * Coroutine body. This code is executed by all the corutines at
 * the same time, but with different struct coro. Here you
 * implement your solution.
 */

void
my_coroutine() {
    /*
     * Note - all the data access is done via 'coro_this()'.
     * It is not safe to store anything in local variables
     * here.
     */
    sprintf(coro_this()->local_data, "Local data for coro id%d",
            curr_coro_i);
    fprintf(stderr, "Coro %d: before re-schedule\n", curr_coro_i);
    coro_yield();
    fprintf(stderr, "Coro %d: after first re-schedule\n", curr_coro_i);
    coro_yield();
    fprintf(stderr, "Coro %d: after second re-schedule\n", curr_coro_i);
    /* Other functions can be called, but via coro_call(). */
    coro_call(other_function, curr_coro_i * 10);
    fprintf(stderr, "Coro %d: this is local data: %s\n", curr_coro_i,
            coro_this()->local_data);
    coro_finish();
    coro_wait_all();
}

int
main(int argc, char **argv) {
    coro_count = argc - 1;
    coros = (struct coro *) malloc(sizeof(struct coro) * coro_count);
    for (int i = 0; i < coro_count; ++i) {
        if (coro_init(&coros[i]) != 0)
            break;
    }

    coro_call(my_coroutine);
    printf("Finished\n");
    return 0;
}
