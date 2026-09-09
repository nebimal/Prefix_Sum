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
    /*********************************************************************************
     * This will be the array that we need to do the prefix sum on.
     * Note: Prefix_sum_args_t is a struct, we will pull the id from the struct.
    **********************************************************************************/
    int id = args->t_id;
    int *input_values = (int*)args->input_vals;
    int stride = 0;
    int length = args->n_loops;

    /*********************************************************
     * Now we begin at for loop, and we will loop until n/2, 
     * and increment stride by shifting bitwise operator.
    **********************************************************/

    for (stride = 1; stride <= length/2; stride << 1)
    {
        input_values[id + stride] += input_values[id];
        // Adds the current value of id in array at index id + stride.
    }

    return 0;
}
