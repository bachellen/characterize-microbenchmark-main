/* para.c */

/* Standard C includes */
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

/* Include common headers */
#include "common/macros.h"
#include "common/types.h"

/* Include application-specific headers */
#include "include/types.h"
// #include "common/vmath.h"
void* impl_parallel(void* args);
/* Define the maximum number of threads */
#define MAX_THREADS 4

/* Define the maximum number of threads */
#define MAX_THREADS 4
/* Parallel implementation */
void parallelBlackScholes(float* sptPrice, float* strike, float* rate, float* volatility, float* otime, char* otype, float* output, size_t num_stocks) {
  /* Calculate the number of elements per thread */
  size_t elements_per_thread = num_stocks / MAX_THREADS;

  /* Create an array of threads */
  pthread_t threads[MAX_THREADS];

  /* Create an array of thread arguments */
  args_t thread_args[MAX_THREADS];

  /* Create threads and assign work */
  for (int i = 0; i < MAX_THREADS; i++) {
    /* Set the number of stocks for the current thread */
    thread_args[i].num_stocks = (i == MAX_THREADS - 1) ? (num_stocks - i * elements_per_thread) : elements_per_thread;

    /* Assign the required data to the current thread */
    thread_args[i].sptPrice = &sptPrice[i * elements_per_thread];
    thread_args[i].strike = &strike[i * elements_per_thread];
    thread_args[i].rate = &rate[i * elements_per_thread];
    thread_args[i].volatility = &volatility[i * elements_per_thread];
    thread_args[i].otime = &otime[i * elements_per_thread];
    thread_args[i].otype = &otype[i * elements_per_thread];
    thread_args[i].output = &output[i * elements_per_thread];

    /* Create the thread and pass the thread arguments */
    pthread_create(&threads[i], NULL, impl_parallel, (void*)&thread_args[i]);
  }

  /* Wait for all threads to complete */
  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

/* Alternative Implementation */
void* impl_parallel(void* args) {
  /* Cast the input arguments to the appropriate type */
  args_t* thread_args = (args_t*)args;

  /* Access the required data from the arguments */
  size_t num_stocks = thread_args->num_stocks;
  float* sptPrice = thread_args->sptPrice;
  float* strike = thread_args->strike;
  float* rate = thread_args->rate;
  float* volatility = thread_args->volatility;
  float* otime = thread_args->otime;
  char* otype = thread_args->otype;
  float* output = thread_args->output;

  /* Process the data in the specified range */
  for (size_t i = 0; i < num_stocks; i++) {
    /* Perform Black-Scholes calculation */
   parallelBlackScholes(sptPrice, strike, rate, volatility, otime, otype,output, num_stocks);
  }

  /* Return the result (optional) */
  return output;
}