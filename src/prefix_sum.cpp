#include "prefix_sum.h"
#include "helpers.h"

void* compute_prefix_sum(void *a)
{

    /********************************************************************************************************** 
     * For prefix sum with threads, we will have the program have a for loop which starts at 1 (int stride),
     * then in the for loop, our array will add its current value and its next array with [index + stride].
     * For now, I am going to make a simple function for the prefix sum with no threads involved.                            
    ***********************************************************************************************************/

    prefix_sum_args_t *args = (prefix_sum_args_t *)a; 


    // A barrier is needed so multiple threads dont try to do assignments that has not been updated.
    // Use pthread barrier.

    // in order for threads to know what indices they need to work on, we need to have the threads have a specific
    // length, with all the work divided equally.

    /*********************************************************************************
     * This will be the array that we need to do the prefix sum on.
     * Note: Prefix_sum_args_t is a struct, we will pull the id from the struct.
    **********************************************************************************/
    int id = args->t_id;
    int *input_values = (int*)args->input_vals;
    int *output_values = (int*)args->output_vals;
    int length = args->n_vals;

    int num_threads = args->n_threads;              
    int length_per_thread = length/num_threads;         // The work get divided equally among the threads.
    int start = id * length_per_thread;                 // Where the thread should start index of the array.


    output_values[start] = input_values[start]; 
    // inputArray { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
    // Thread 3
    // length of array = 12
    // Length per thread = 4
    // For thread = 0; start = 0;
    // OutputArray[start] = input_array[start] (I think)
    // outputArray[1] = inputArray[0] + inputArray[1]; // i = 0, 1 + 2
    // i++
    // inputArray { 1, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12} // i = 1,
    // outputArray[2] = inputArray[1] + inputArray[2];
    // inputArray { 1, 3, 6, 4, 5, 6, 7, 8, 9, 10, 11, 12} i = 1
    // ...  
    // outputArray[3] = inputArray[2] + inputArray[3]; 
    // inputArray { 1, 3, 6, 10, 5, 6, 7, 8, 9, 10, 11, 12} i = 2
    // Now that the thread reaches it's length, jump to the next thread.
    // thread = 1; start = 4.
    // outputArray[4] = inputArray[4] (Outside of for loop I believe)
    // for(...)
    // outputArray[5] = inputArray[4] + inputArray[5]];
    // inputArray { 1, 3, 6, 10, 5, 11, 18, 26, 9, 10, 11, 12}
    // thread = 2
    // inputArray { 1, 3, 6, 10, 5, 11, 18, 26, 9, 19, 30, 42}

    for (int i = start; i < start + length_per_thread; i++)
    {   
        output_values[i+1] = args->op(input_values[i+1], output_values[i], args->n_loops);    
    }
    // We need to make sure to lock the thread so only 1 thread at a time, I want the thread to be in order, 0 then 1 then 2.
    // Also dont forget on the last element of each thread we need to add it to all the remaining element after it.
    /*********************************************************
     * Now we begin at for loop, and we will loop until n/2, 
     * and increment stride by shifting bitwise operator.
    **********************************************************/

    return 0;
}
