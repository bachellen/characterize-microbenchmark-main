/* para.c */

/* Standard C includes */
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
/* Include common headers */
#include "common/macros.h"
#include "common/types.h"
#include <ctype.h>
/* Include application-specific headers */
#include "include/types.h"
// #include "common/vmath.h"
#define inv_sqrt_2xPI 0.39894228040143270286
/* Define the maximum number of threads */
#define MAX_THREADS 4

/* Define the maximum number of threads */
#define MAX_THREADS 4
float CNDF_para (float InputX) 
{
    int sign;

    float OutputX;
    float xInput;
    float xNPrimeofX;
    float expValues;
    float xK2;
    float xK2_2, xK2_3;
    float xK2_4, xK2_5;
    float xLocal, xLocal_1;
    float xLocal_2, xLocal_3;

    // Check for negative value of InputX
    if (InputX < 0.0) {
        InputX = -InputX;
        sign = 1;
    } else 
        sign = 0;

    xInput = InputX;
 
    // Compute NPrimeX term common to both four & six decimal accuracy calcs
    expValues = exp(-0.5f * InputX * InputX);
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    xK2 = 0.2316419 * xInput;
    xK2 = 1.0 + xK2;
    xK2 = 1.0 / xK2;
    xK2_2 = xK2 * xK2;
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;
    
    xLocal_1 = xK2 * 0.319381530;
    xLocal_2 = xK2_2 * (-0.356563782);
    xLocal_3 = xK2_3 * 1.781477937;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_4 * (-1.821255978);
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_5 * 1.330274429;
    xLocal_2 = xLocal_2 + xLocal_3;

    xLocal_1 = xLocal_2 + xLocal_1;
    xLocal   = xLocal_1 * xNPrimeofX;
    xLocal   = 1.0 - xLocal;

    OutputX  = xLocal;
    
    if (sign) {
        OutputX = 1.0 - OutputX;
    }
    
    return OutputX;
} 


float blackScholes_para(float sptprice, float strike, float rate, float volatility,
                   float otime, char otype,float timet )
{
    float OptionPrice;

    // local private working variables for the calculation
    float xStockPrice;
    float xStrikePrice;
    float xRiskFreeRate;
    float xVolatility;
    float xTime;
    float xSqrtTime;

    float logValues;
    float xLogTerm;
    float xD1; 
    float xD2;
    float xPowerTerm;
    float xDen;
    float d1;
    float d2;
    float FutureValueX;
    float NofXd1;
    float NofXd2;
    float NegNofXd1;
    float NegNofXd2;    
    int type = ( tolower ( otype ) == 'p')? 1 : 0;

    xStockPrice = sptprice;
    xStrikePrice = strike;
    xRiskFreeRate = rate;
    xVolatility = volatility;

    xTime = otime;
    xSqrtTime = sqrt(xTime);

    logValues = log(sptprice / strike);
        
    xLogTerm = logValues;
        
    
    xPowerTerm = xVolatility * xVolatility;
    xPowerTerm = xPowerTerm * 0.5;
        
    xD1 = xRiskFreeRate + xPowerTerm;
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    xD1 = xD1 / xDen;
    xD2 = xD1 - xDen;

    d1 = xD1;
    d2 = xD2;
    
    NofXd1 = CNDF_para(d1);
    NofXd2 = CNDF_para(d2);

    FutureValueX = strike * (exp(-(rate)*(otime)));        
    if (type == 0) {            
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else { 
        NegNofXd1 = (1.0 - NofXd1);
        NegNofXd2 = (1.0 - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }
    
    return OptionPrice;
}
/* Parallel implementation */
void* parallelBlackScholes(void* args) {
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
  float* optionPrices = malloc(num_stocks * sizeof(float));
  /* Calculate the number of elements per thread */
  for (size_t i = 0; i < num_stocks; ++i) {
          optionPrices[i] = blackScholes_para(sptPrice[i], strike[i], rate[i], volatility[i], otime[i], otype[i],0);
      }
  memcpy(output, optionPrices, num_stocks * sizeof(float));
}

/* Alternative Implementation */
void* impl_parallel(void* args) {
  /* Cast the input arguments to the appropriate type */
  args_t* para_args = (args_t*)args;

  /* Access the required data from the arguments */
  size_t num_stocks = para_args->num_stocks;
  float* sptPrice = para_args->sptPrice;
  float* strike = para_args->strike;
  float* rate = para_args->rate;
  float* volatility = para_args->volatility;
  float* otime = para_args->otime;
  char* otype = para_args->otype;
  float* output = para_args->output;
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
    pthread_create(&threads[i], NULL, parallelBlackScholes, (void*)&thread_args[i]);
  }

   for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  /* Return the result (optional) */
  // return output;
}