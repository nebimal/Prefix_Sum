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

            int prev_Chunk_End = j * length_per_thread - 1;
            int chunk_End = (j == num_threads - 1) ? length - 1 : (j + 1) * length_per_thread - 1;

            // Gets the value from the previous thread last index.
            int offset = output_values[prev_Chunk_End];

            output_values[chunk_End] = args->op(offset, output_values[chunk_End], args->n_loops);
            
        }
    }

    pthread_barrier_wait(&barrier);
    
    // This is for every thread besides 0.
    if (id > 0 && start < end)
    {
        int offset = output_values[start - 1];
        // Offset being the value of the previous threads last index.

        for (int k = start; k < end - 1; k++)
        {
            output_values[k] = args->op(offset, output_values[k], args->n_loops);
        }

    }

    pthread_barrier_wait(&barrier);
    // All threads must wait for all threads to finish before returning.

    return NULL;
}
