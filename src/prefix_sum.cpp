#include "prefix_sum.h"
#include "helpers.h"

extern pthread_barrier_t barrier;
// Barrier initialized in main.

void* compute_prefix_sum(void *a)
{

    /********************************************************************************************************** 
     * For prefix sum with threads, we will have the program have a for loop which starts at 1 (int stride),
     * then in the for loop, our array will add its current value and its next array with [index + stride].
     * For now, I am going to make a simple function for the prefix sum with no threads involved.                            
    ***********************************************************************************************************/

    prefix_sum_args_t *args = (prefix_sum_args_t *)a; 

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

    int end = (id == num_threads - 1) ? length : start + length_per_thread;
    // This is for the last thread so if there are anymore elements we missed then the last thread will take the the rest.

    // Does the prefix sum for each thread ONLY in the chunk they have.
    if (start < end)
    {
        output_values[start] = input_values[start];
        for (int i = start + 1; i < end; i++)
        {   
            output_values[i] = args->op(output_values[i - 1], input_values[i], args->n_loops);
        }
    }
    // Make sure all threads wait here until all the threads have completed the part above.
    // If we don't wait, then one thread can get ahead while another is still computing the first part and add the incorrect values to the final result.
    pthread_barrier_wait(&barrier);

    if (id == 0) // Only thread 0 will compute this, after all thread chunk values have been finalized.
    {
        for (int j = 1; j < num_threads; j++) // J represting the threads.
        {
            int chunk_Start = j * length_per_thread;
            // The index the chunk of that thread starts at.

            int chunk_End = (j == num_threads - 1) ? length : chunk_Start + length_per_thread;

            // Gets the value from the previous thread last index.
            int offset = output_values[chunk_Start - 1];

            // For this for loop, it will add the value of the last index from the previous thread to each value of the current thread.
            // [0, 1, 3, 3, 7, 12, 6, 11, 19] 
            // For 3 Threads.
            // Thread 0 start = index[0] ends = index[2]
            // Thread 1 start = index[3] ends = index[5]
            // Thread 2 start = index[6] ends = index[8]
            // With this for loop, the first thread would be the 2nd one (thread 1), instead of thread 0.
            // thread 0 last index(2) with value (3) will get added to all the values to the chunk in thread 1.
            // [0, 1, 3, 6, 10, 15, 6, 11, 19]
            // then the 3rd thread (thread 2), will then get the last index of the last thread 1
            // thread 1 last index(5) with NEW value (15), then that gets added to all the chunks in threads 2.
            // // [0, 1, 3, 6, 10, 15, 21, 28, 36]
            for (int k = chunk_Start; k < chunk_End; k++)
            {
                output_values[k] = args->op(offset, output_values[k], args->n_loops);
            }
            // This is currently O(n^2), correct output tho but fix this and make the 2nd part run o(n).
        }
    }

    pthread_barrier_wait(&barrier);
    // This makes all the other threads wait for thread 0 to complete, ensuring correct results.

    return 0;
}
