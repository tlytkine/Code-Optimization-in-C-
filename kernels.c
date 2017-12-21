/********************************************************
 * Kernels to be optimized for the  Performance Project
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <sys/time.h>
#include <sys/resource.h>

/* Below are statements to set up the performance measurement utilities */
/* we use rdtsc, clock, and getusage utilities to measure performance */

//#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))
#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#elif defined(__x86_64__)


static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif

/* end of definitions to set up measurement utilities */


/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "G37173537",              /* Team name */

    "Timothy Lytkine",     /* First member full name */
    "tlytkine@gwu.edu",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* the getUserTime function is used for measurement, you should not change the code for this function */

long int getUserTime()
{
	int who= RUSAGE_SELF;
	int ret;
	struct rusage usage;
	struct rusage *p=&usage;
	//long int current_time;

	ret=getrusage(who,p);
	if(ret == -1)
	{
		printf("Could not get GETRUSAGE to work in function %s at line %d in file %s\n",
				__PRETTY_FUNCTION__, __LINE__, __FILE__);
		exit(1);
	}
	return (p->ru_utime.tv_sec * 1000000 + p->ru_utime.tv_usec);
}

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
 /* The parameters, pointers, rusage_time, rdtsc_time, and cpu_time_used are used to measure performance and return values to caller. */
 /* You should not change the code that uses these parameters and variables. */
 
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst, int *rusage_time, unsigned long long *rdtsc_time) 
{
	int i, j;
	/* the variables below are used for performance measurement and not for computing the results of the algorithm */
	long int rusage_start_time, rusage_end_time = 0;
	unsigned long long rdtsc_start_time, rdtsc_end_time = 0;
	/* call system functions to start measuring performance. you should not bother to change these. */
	
	rusage_start_time = getUserTime();
	rdtsc_start_time = rdtsc();

/* below is the main computations for the rotate function */

	for (j = 0; j < dim; j++)
		for (i = 0; i < dim; i++)
			dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];

/* the remaining lines in this function stop the measurement and set the values before returning. */

	rusage_end_time = getUserTime();
	rdtsc_end_time = rdtsc();

	*rusage_time = rusage_end_time - rusage_start_time;
	*rdtsc_time = rdtsc_end_time - rdtsc_start_time;
}

/* Macro designed to access blocks of memory at a time. */


#define rotateFunction(num) dst[RIDX(dim-1-j,i+num,dim)] = src[RIDX(i+num,j,dim)];

 /* The parameters, pointers, rusage_time, rdtsc_time, and cpu_time_used are used to measure performance and return values to caller. */
 /* You should not change the code that uses these parameters and variables. */
char my_rotate_descr[] = "my_rotate: Naive baseline implementation";
void my_rotate(int dim, pixel *src, pixel *dst, int *rusage_time, unsigned long long *rdtsc_time) 
{
	int i, j;
		/* the variables below are used for performance measurement and not for computing the results of the algorithm */
	long int rusage_start_time, rusage_end_time = 0;
        unsigned long long rdtsc_start_time, rdtsc_end_time = 0;
	/* call system functions to start measuring performance. you should not bother to change these. */
        rusage_start_time = getUserTime();
	rdtsc_start_time = rdtsc();

/* ANY CHANGES ARE MADE HERE */
/* below are the main computations for your implementation of the rotate. Any changes in implementation will go here or the other functions it may call */

/* Changes made to my_rotate documented below: 

In order to maximize the effiency of the my_rotate method, a macro was 
defined. (displayed below)

rotateFunction(num) dst[RIDX(dim-1-j,i+num,dim)] = src[RIDX(i+num,j,dim)];

In order to utilize rotateFunction, the order of the nested for loops 
in the initial code was changed. This is an example of using the Loop Interchange
technique of compilier optimization. The reason why this works is because C stores
2D arrays in row major order and the image which the operation is being performed
on is a 2D matrix (array) of pixels. Swapping the variables allows the pixels 
in the image to be accessed in row major order.

Accesses to the cache are more efficient than accesses to the main memory or disk. 
Therefore, swapping i with j more efficiently accesses the 2D array of pixels. 
The cache is arranged as a set of blocks and by ensuring that the src and dest arrays 
of pixels are accessed using row major order, this results in less cache misses and 
more cache hits. Furthermore, spatial locality is improved since the the elements 
adjacent to the current element being accessed in the arrays have nearby memory 
addresses.

The second change that was made was changing the first for loop to iterate by 
increasing the value of i (initially 0) by 16 while it is less than the value of dim. 
This is an example of using the Blocking compiler optimization technique. 
Blocking improves spatial locality in this case since it goes through 16 
pixels at once that are located at nearby memory addresses. 

The description of the project stated that it can be assumed that the image 
dimension is a multiple of 32. Since 16 is a factor of 32, all the pixels of the image
are accessed and rotated. 32 was not used in the for loop since we have to account for 
potential cache misses that can occur when initially fetching the block of memory.

Inside of the second for loop, rotateFunction is called from 0 to 15 keeping
the change to the first for loop in mind. By adding the value of i to the parameter
num (which in this case are the integers 0 to 15), when calling the rotateFunction, 
it is ensured that the blocking optimization technique is properly implemented. 

By accessing blocks of memory at a time, values can be accessed from the cache
rather than the main memory. This results in less cache misses and more cache hits.
Hence, the performance of the function is significantly more efficient than the code
initially given in the my_rotate method / contained in the naive_rotate method. 

*/ 
	/* Loop interchange */ 
    for (i = 0; i < dim; i = i + 16) { /* Loop incremented by 16 to implement blocking */
        for (j = 0; j < dim; j++) {
        	/* macro rotateFunction called (implements blocking) */
            rotateFunction(0);
            rotateFunction(1);
            rotateFunction(2);
            rotateFunction(3);
            rotateFunction(4);
            rotateFunction(5); 
            rotateFunction(6);
            rotateFunction(7);
            rotateFunction(8);
            rotateFunction(9);
            rotateFunction(10);
            rotateFunction(11);
            rotateFunction(12);
            rotateFunction(13);
            rotateFunction(14);
            rotateFunction(15);
        }
    }

/* end of computation for rotate function. any changes you make should be made above this line. */
/* END OF CHANGES in this function */

/* the remaining lines in this function stop the measurement and set the values before returning. */
	rusage_end_time = getUserTime();
        rdtsc_end_time = rdtsc();

	*rusage_time = rusage_end_time - rusage_start_time;
	*rdtsc_time = rdtsc_end_time - rdtsc_start_time;
}





/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
	int red;
	int green;
	int blue;
	int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int minimum(int a, int b) 
{ return (a < b ? a : b); }
static int maximum(int a, int b) 
{ return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
	sum->red = sum->green = sum->blue = 0;
	sum->num = 0;
	return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
	sum->red += (int) p.red;
	sum->green += (int) p.green;
	sum->blue += (int) p.blue;
	sum->num++;
	return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
	current_pixel->red = (unsigned short) (sum.red/sum.num);
	current_pixel->green = (unsigned short) (sum.green/sum.num);
	current_pixel->blue = (unsigned short) (sum.blue/sum.num);
	return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
	int ii, jj;
	pixel_sum sum;
	pixel current_pixel;

	initialize_pixel_sum(&sum);
	for(ii = maximum(i-1, 0); ii <= minimum(i+1, dim-1); ii++) 
		for(jj = maximum(j-1, 0); jj <= minimum(j+1, dim-1); jj++) 
			accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

	assign_sum_to_pixel(&current_pixel, sum);
	return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
  /* The parameters, pointers, rusage_time, rdtsc_time, and cpu_time_used are used to measure performance and return values to caller. */
 /* You should not change the code that uses these parameters and variables. */
 
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst, int *rusage_time, unsigned long long *rdtsc_time) 
{
	int i, j;
	
	/* the variables below are used for performance measurement and not for computing the results of the algorithm */
	long int rusage_start_time, rusage_end_time = 0;
        unsigned long long rdtsc_start_time, rdtsc_end_time = 0;

	/* call system functions to start measuring performance. you should not bother to change these. */
        rusage_start_time = getUserTime();
	rdtsc_start_time = rdtsc();

/* below are the main computations for the smooth function */
	for (j = 0; j < dim; j++)
		for (i = 0; i < dim; i++)
			dst[RIDX(i, j, dim)] = avg(dim, i, j, src);

/* the remaining lines in this function stop the measurement and set the values before returning. */
	rusage_end_time = getUserTime();
        rdtsc_end_time = rdtsc();

	*rusage_time = rusage_end_time - rusage_start_time;
	*rdtsc_time = rdtsc_end_time - rdtsc_start_time;
}


 /* The parameters, pointers, rusage_time, rdtsc_time, and cpu_time_used are used to measure performance and return values to caller. */
 /* You should not change the code that uses these parameters and variables. */
 



char my_smooth_descr[] = "my_smooth: Naive baseline implementation";
void my_smooth(int dim, pixel *src, pixel *dst, int *rusage_time, unsigned long long *rdtsc_time) 
{
	int i, j;
	
	/* the variables below are used for performance measurement and not for computing the results of the algorithm */
	long int rusage_start_time, rusage_end_time = 0;
        unsigned long long rdtsc_start_time, rdtsc_end_time = 0;
	/* call system functions to start measuring performance. you should not bother to change these. */
        rusage_start_time = getUserTime();
	rdtsc_start_time = rdtsc();
	
/* ANY CHANGES TO BE MADE SHOULD BE BELOW HERE */
/* below are the main computations for your implementation of the smooth function. Any changes in implementation will go here or the other functiosn it calls */
/* Changes made to my_smooth documented below 

The first change that was made was swapping the i and j variables of the nested 
for loop. This change was made in order to access blocks of memory in the image 
consisting of a 2D array of pixels in row major order since C stores 2D arrays 
in this way rather than the initial format of column major order. 
This leads to increased spatial locality, more cache hits and less cache misses.
This is an example of Loop Interchange. 

The rest of the changes below are based heavily on utilizing the function inlining compiler
optimization method by swapping the code and methods from the avg method and plugging 
in the parameters from the previous version. Most of the methods from the avg method were also
inlined into the code below. In addition to this, the Code Motion and Strength Reduction
techniques of compiler optimization were utilized in order to reduce the frequency
of computations performed. The variables x1, x2, y1, and y2 created as part of the 
function inling optimization were moved outside of the loops they were invariant in.
Furthermore, while I did not make this change and simply plugged in the proper parameters 
into the maximum and minimum methods. The maximum and minimum methods (the basis of which
x1, x2, y1 and y2 are computed with) are an example of Strength Reduction since it finds
the maximum and minimum of 2 numbers using less costly operations than I have observed in
previous techniques of doing so.

These changes improved the performance of the my_smooth method due to
improved temporal and spatial locality.


*/

/*  Changes below based on using the Function Inlining compiler optimization method. */ 
	for (i = 0; i < dim; i++){
				/* Code Motion example:
				Moving loop variants saves multiplication 
				Improves spatial locality */
				/* Function Inlining example: maximum and minimum functions
				implemented inline with proper variables initially called 
				plugged in. */
				int x1 = ((i-1) > (0) ? (i-1) : (0));
				int x2 = ((i+1) < (dim-1) ? (i+1) : (dim-1));
				for (j = 0; j < dim; j++){
				int ii, jj;
				pixel_sum sum;
				pixel current_pixel;
				/* Eliminating dead code and maximizing efficiency by 
				setting all to 0 in one line. (Same operation performed on all)*/
				sum.red = sum.green = sum.blue = sum.num = 0;
				/* Code Motion example */
				/* Function Inling example */
				int y1 = ((j-1) > (0) ? (j-1) : (0));
				int y2 = ((j+1) < (dim-1) ? (j+1) : (dim-1));
						for(ii = x1; ii <= x2; ii++) 
							for(jj = y1; jj <= y2; jj++)
								accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);
					
					/* Function Inlining examples below */	
					current_pixel.red = (unsigned short) (sum.red/sum.num);
					current_pixel.green = (unsigned short) (sum.green/sum.num);
					current_pixel.blue = (unsigned short) (sum.blue/sum.num);
					dst[RIDX(i,j,dim)] = current_pixel;
					}
				}



/* end of computation for smooth function. so don't change anything after this in this function. */
/* END OF CHANGES */

/* the remaining lines in this function stop the measurement and set the values before returning. */
	rusage_end_time = getUserTime();
        rdtsc_end_time = rdtsc();

	*rusage_time = rusage_end_time - rusage_start_time;
	*rdtsc_time = rdtsc_end_time - rdtsc_start_time;
}
