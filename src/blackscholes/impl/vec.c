/* vec.c
 *
 * Author:
 * Date  :
 *
 *  Description
 */

/* Standard C includes */
#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <stdio.h>
#include <time.h>
/* Include common headers */
#include "common/macros.h"
#include "common/types.h"
#include <ctype.h>
/* Include application-specific headers */
#include "include/types.h"
#include <immintrin.h>
#define inv_sqrt_2xPI 0.39894228040143270286

// #include <emmintrin.h>
#include "common/vmath.h"


float CNDFScalar (float InputX) 
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


float BlackScholesScalar(float sptprice, float strike, float rate, float volatility,
                   float otime, char otype, float timet)
{
    float OptionPrice;
    int type = ( tolower ( otype ) == 'p')? 1 : 0;
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
    
    NofXd1 = CNDFScalar(d1);
    NofXd2 = CNDFScalar(d2);
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
// Function to calculate cumulative normal distribution using AVX2
__m256 vectorized_CNDF(__m256 x) {
    __m256 signMask = _mm256_set1_ps(-0.0f);
    __m256 zero = _mm256_setzero_ps();

    // Check for negative values
    __m256 sign = _mm256_and_ps(x, signMask);
    x = _mm256_max_ps(x, zero);

    // Compute NPrimeX term
    __m256 expValues = _mm256_exp_ps(_mm256_mul_ps(_mm256_set1_ps(-0.5f), _mm256_mul_ps(x, x)));
    __m256 xNPrimeofX = _mm256_mul_ps(expValues, _mm256_set1_ps(inv_sqrt_2xPI));

    // Compute K2 terms
    __m256 xK2 = _mm256_set1_ps(0.2316419f) * x;
    xK2 = _mm256_add_ps(_mm256_set1_ps(1.0f), xK2);
    xK2 = _mm256_div_ps(_mm256_set1_ps(1.0f), xK2);
    __m256 xK2_2 = _mm256_mul_ps(xK2, xK2);
    __m256 xK2_3 = _mm256_mul_ps(xK2_2, xK2);
    __m256 xK2_4 = _mm256_mul_ps(xK2_3, xK2);
    __m256 xK2_5 = _mm256_mul_ps(xK2_4, xK2);

    // Compute Local terms
    __m256 xLocal_1 = _mm256_mul_ps(xK2, _mm256_set1_ps(0.319381530f));
    __m256 xLocal_2 = _mm256_mul_ps(xK2_2, _mm256_set1_ps(-0.356563782f));
    __m256 xLocal_3 = _mm256_mul_ps(xK2_3, _mm256_set1_ps(1.781477937f));
    xLocal_2 = _mm256_add_ps(xLocal_2, xLocal_3);
    xLocal_3 = _mm256_mul_ps(xK2_4, _mm256_set1_ps(-1.821255978f));
    xLocal_2 = _mm256_add_ps(xLocal_2, xLocal_3);
    xLocal_3 = _mm256_mul_ps(xK2_5, _mm256_set1_ps(1.330274429f));
    xLocal_2 = _mm256_add_ps(xLocal_2, xLocal_3);

    xLocal_1 = _mm256_add_ps(xLocal_2, xLocal_1);
    __m256 xLocal = _mm256_mul_ps(xLocal_1, xNPrimeofX);
    xLocal = _mm256_sub_ps(_mm256_set1_ps(1.0f), xLocal);

    // Handle sign
    __m256 result = _mm256_blendv_ps(xLocal, _mm256_sub_ps(_mm256_set1_ps(1.0f), xLocal), sign);

    return result;
}

// Function to calculate Black-Scholes option price using AVX2
__m256 vectorized_BlackScholes(__m256 sptprice, __m256 strike, __m256 rate, __m256 volatility,
                              __m256 otime, __m256i otype, float timet) {
    __m256 xStockPrice = sptprice;
    __m256 xStrikePrice = strike;
    __m256 xRiskFreeRate = rate;
    __m256 xVolatility = volatility;
    // int type = ( tolower ( otype ) == 'p')? 1 : 0;
    __m256 xTime = otime;
    __m256 xSqrtTime = _mm256_sqrt_ps(xTime);

    __m256 logValues = _mm256_log_ps(_mm256_div_ps(xStockPrice, xStrikePrice));
    
    __m256 xLogTerm = logValues;
    
    __m256 xPowerTerm = _mm256_mul_ps(xVolatility, xVolatility);
    xPowerTerm = _mm256_mul_ps(xPowerTerm, _mm256_set1_ps(0.5f));
    
    __m256 xD1 = _mm256_add_ps(xRiskFreeRate, xPowerTerm);
    xD1 = _mm256_mul_ps(xD1, xTime);
    xD1 = _mm256_add_ps(xD1, xLogTerm);

    __m256 xDen = _mm256_mul_ps(xVolatility, xSqrtTime);
    xD1 = _mm256_div_ps(xD1, xDen);
    __m256 xD2 = _mm256_sub_ps(xD1, xDen);

    __m256 NofXd1 = vectorized_CNDF(xD1);
    __m256 NofXd2 = vectorized_CNDF(xD2);

    __m256 FutureValueX_scalar = _mm256_mul_ps(strike, _mm256_exp_ps(_mm256_mul_ps(_mm256_set1_ps(-1.0f), _mm256_mul_ps(rate, otime))));
    // __m256i zeroMask = _mm256_cmpeq_epi32(otype, _mm256_set1_epi32(0));

    __m256 NegNofXd1 = _mm256_sub_ps(_mm256_set1_ps(1.0f), NofXd1);
    __m256 NegNofXd2 = _mm256_sub_ps(_mm256_set1_ps(1.0f), NofXd2);

    __m256 OptionPrice;
    // Assuming type is a vector (__m256i) where each element is either 0 or 1
    __m256i zeroMask = _mm256_cmpeq_epi32(otype, _mm256_setzero_si256());

    // Calculate OptionPrice based on option type
    OptionPrice = _mm256_sub_ps(
    _mm256_mul_ps(sptprice, NofXd1),
    _mm256_blendv_ps(_mm256_mul_ps(FutureValueX_scalar, NofXd2), _mm256_mul_ps(sptprice, NegNofXd1), _mm256_castsi256_ps(zeroMask))
);
    return OptionPrice;
}


/* Alternative Implementation */
void* impl_vector(void* args)
{
#if defined(__amd64__) || defined(__x86_64__)
    args_t* arguments = (args_t*)args;
    size_t num_stocks = arguments->num_stocks;
    float* sptPrice = arguments->sptPrice;
    float* strike = arguments->strike;
    float* rate = arguments->rate;
    float* volatility = arguments->volatility;
    float* otime = arguments->otime;
    char* otype = arguments->otype;
    float* output = arguments->output;

    size_t alignment = 32;   // Align to 32 bytes for AVX2
    size_t num_full_iterations = num_stocks / 8;
  
  float* optionPrices = (float*)_mm_malloc(num_stocks * sizeof(float) + alignment - 1, alignment);
  for (size_t i = 0; i < num_full_iterations; i += 8)
  {
        __m256 sptPriceVec = _mm256_loadu_ps(&sptPrice[i]);
        __m256 strikeVec = _mm256_loadu_ps(&strike[i]);
        __m256 rateVec = _mm256_loadu_ps(&rate[i]);
        __m256 volatilityVec = _mm256_loadu_ps(&volatility[i]);
        __m256 otimeVec = _mm256_loadu_ps(&otime[i]);
        // __m256i otypeVec = _mm256_set1_epi32((tolower(&otype[i]) == 'p') ? 1 : 0);
        int mask_values[8];
        for (int i = 0; i < 8; ++i) {
            mask_values[i] = (tolower(otype[i]) == 'c') ? 1 : 0;
        }

    // Set each element of the vector individually
    __m256i otypeVec = _mm256_set_epi32(mask_values[7], mask_values[6], mask_values[5], mask_values[4],
                                        mask_values[3], mask_values[2], mask_values[1], mask_values[0]);

        __m256 optionPriceVec= vectorized_BlackScholes(sptPriceVec, strikeVec, rateVec, volatilityVec, otimeVec,otypeVec,0);
        // Store the result in the output array
        _mm256_storeu_ps(&optionPrices[i], optionPriceVec);
  }

    // Handle remaining stocks (less than 8)
    for (size_t i = num_full_iterations * 8; i < num_stocks; ++i) {
        // Process remaining stocks individually without vectorization
        optionPrices[i] = BlackScholesScalar(sptPrice[i], strike[i], rate[i], volatility[i], otime[i], otype[i],0);
    }

  memcpy(output, optionPrices, num_stocks * sizeof(float));

   _mm_free(optionPrices);
#elif defined(__aarch__) || defined(__aarch64__) || defined(__arm64__)
#endif
}