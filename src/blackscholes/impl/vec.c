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
/* Include common headers */
#include "common/macros.h"
#include "common/types.h"

/* Include application-specific headers */
#include "include/types.h"
#include <immintrin.h>
#define inv_sqrt_2xPI 0.39894228040143270286

// #include <emmintrin.h>
#include "common/vmath.h"

float vectorized_CNDF(__m256 x) {
    __m256 signMask = _mm256_set1_ps(-0.0f);
    __m256 zero = _mm256_setzero_ps();

    // Check for negative values
    __m256 sign = _mm256_and_ps(x, signMask);
    x = _mm256_max_ps(x, zero);

    // Compute NPrimeX term common to both four & six decimal accuracy calcs
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

    float output[8];
    _mm256_storeu_ps(output, result);

    return output[0];

}
  

float vectorized_BlackScholes(__m256 sptprice, __m256 strike, __m256 rate, __m256 volatility,
                              __m256 otime, char otype) {
    __m256 xStockPrice = sptprice;
    __m256 xStrikePrice = strike;
    __m256 xRiskFreeRate = rate;
    __m256 xVolatility = volatility;

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

    float NofXd1 = vectorized_CNDF(xD1);
    float NofXd2 = vectorized_CNDF(xD2);

    float FutureValueX_scalar = strike[0] * expf(-rate[0] * otime[0]);

    float OptionPrice;
    if (otype == 'C') {
        OptionPrice = (sptprice[0] * NofXd1) - (FutureValueX_scalar * NofXd2);
    } else {
        float NegNofXd1 = 1.0f - NofXd1;
        float NegNofXd2 = 1.0f - NofXd2;
        OptionPrice = (FutureValueX_scalar * NegNofXd2) - (sptprice[0] * NegNofXd1);
    }
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

    size_t alignment = 32;   // Align to 32 bytes for AVX2

  float* optionPrices = (float*)_mm_malloc(num_stocks * sizeof(float) + alignment - 1, alignment);

  for (size_t i = 0; i < num_stocks; i += 4)
  {
        __m256 sptPriceVec = _mm256_loadu_ps(&sptPrice[i]);
        __m256 strikeVec = _mm256_loadu_ps(&strike[i]);
        __m256 rateVec = _mm256_loadu_ps(&rate[i]);
        __m256 volatilityVec = _mm256_loadu_ps(&volatility[i]);
        __m256 otimeVec = _mm256_loadu_ps(&otime[i]);

  //   __m256i xmm_optionPrice = blackScholesSSE(xmm_sptPrice, xmm_strike, xmm_rate, xmm_volatility, xmm_otime, xmm_otype);
        
        // _mm256_storeu_ps(&optionPrices[i], 100);

  }
  //  _mm_free(optionPrices);
  // return optionPrices;
  return NULL;
#elif defined(__aarch__) || defined(__aarch64__) || defined(__arm64__)
#endif
}