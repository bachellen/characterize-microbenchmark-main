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
// #include <emmintrin.h>
// #include "common/vmath.h"
// float CNDF(float InputX)
// {
//     int sign;
    
//     float OutputX;
//     float xInput;
//     float xNPrimeofX;
//     float expValues;
//     float xK2;
//     float xK2_2, xK2_3;
//     float xK2_4, xK2_5;
//     float xLocal, xLocal_1;
//     float xLocal_2, xLocal_3;
    
//     // Check for negative value of InputX
//     if (InputX < 0.0f) {
//         InputX = -InputX;
//         sign = 1;
//     } else 
//         sign = 0;
    
//     xInput = InputX;
    
//     // Compute NPrimeX term common to both four & six decimal accuracy calcs
//     expValues = expf(-0.5f * InputX * InputX);
//     xNPrimeofX = expValues;
//     xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;
    
//     xK2 = 0.2316419f * xInput;
//     xK2 = 1.0f + xK2;
//     xK2 = 1.0f / xK2;
//     xK2_2 = xK2 * xK2;
//     xK2_3 = xK2_2 * xK2;
//     xK2_4 = xK2_3 * xK2;
//     xK2_5 = xK2_4 * xK2;
    
//     xLocal_1 = xK2 * 0.319381530f;
//     xLocal_2 = xK2_2 * (-0.356563782f);
//     xLocal_3 = xK2_3 * 1.781477937f;
//     xLocal_2 = xLocal_2 + xLocal_3;
//     xLocal_3 = xK2_4 * (-1.821255978f);
//     xLocal_2 = xLocal_2 + xLocal_3;
//     xLocal_3 = xK2_5 * 1.330274429f;
//     xLocal_2 = xLocal_2 + xLocal_3;
    
//     xLocal_1 = xLocal_2 + xLocal_1;
//     xLocal = xLocal_1 * xNPrimeofX;
//     xLocal = 1.0f - xLocal;
    
//     OutputX = xLocal;
    
//     if (sign) {
//         OutputX = 1.0f - OutputX;
//     }
    
//     return OutputX;
// }

// float blackScholesSSE(float sptprice, float strike, float rate, float volatility,
//                    float otime, int otype)
// {
//     float OptionPrice;
    
//     // local private working variables for the calculation
//     float xStockPrice;
//     float xStrikePrice;
//     float xRiskFreeRate;
//     float xVolatility;
//     float xTime;
//     float xSqrtTime;
    
//     float logValues;
//     float xLogTerm;
//     float xD1;
//     float xD2;
//     float xPowerTerm;
//     float xDen;
//     float d1;
//     float d2;
//     float FutureValueX;
//     float NofXd1;
//     float NofXd2;
//     float NegNofXd1;
//     float NegNofXd2;
    
//     xStockPrice = sptprice;
//     xStrikePrice = strike;
//     xRiskFreeRate = rate;
//     xVolatility = volatility;
    
//     xTime = otime;
//     xSqrtTime = sqrtf(xTime);
    
//     logValues = logf(sptprice / strike);
    
//     xLogTerm = logValues;
    
//     xPowerTerm = xVolatility * xVolatility;
//     xPowerTerm = xPowerTerm * 0.5f;
    
//     xD1 = xRiskFreeRate + xPowerTerm;
//     xD1 = xD1 * xTime;
//     xD1 = xD1 + xLogTerm;
    
//     xDen = xVolatility * xSqrtTime;
//     xD1 = xD1 / xDen;
//     xD2 = xD1 - xDen;
    
//     d1 = xD1;
//     d2 = xD2;
    
//     // SIMD variables
//     __m128 xmm_stockPrice = _mm_set1_ps(xStockPrice);
//     __m128 xmm_strikePrice = _mm_set1_ps(xStrikePrice);
//     __m128 xmm_riskFreeRate= _mm_set1_ps(xRiskFreeRate);
//     __m128 xmm_volatility = _mm_set1_ps(xVolatility);
//     __m128 xmm_time = _mm_set1_ps(xTime);
//     __m128 xmm_sqrtTime = _mm_set1_ps(xSqrtTime);
//     __m128 xmm_logTerm = _mm_set1_ps(xLogTerm);
//     __m128 xmm_powerTerm = _mm_set1_ps(xPowerTerm);
    
//     // Calculate d1 and d2 using SIMD instructions
//     __m128 xmm_d1 = _mm_add_ps(xmm_riskFreeRate, xmm_powerTerm);
//     xmm_d1 = _mm_mul_ps(xmm_d1, xmm_time);
//     xmm_d1 = _mm_add_ps(xmm_d1, xmm_logTerm);
//     xmm_d1 = _mm_div_ps(xmm_d1, xmm_volatility);
//     xmm_d2 = _mm_sub_ps(xmm_d1, xmm_volatility);
    
//     // Convert d1 and d2 to scalar variables
//     _mm_store_ss(&d1, xmm_d1);
//     _mm_store_ss(&d2, xmm_d2);
    
//     // Calculate N(d1), N(d2), -N(-d1), -N(-d2) using SIMD instructions
//     __m128 xmm_d1_abs = _mm_abs_ps(xmm_d1);
//     __m128 xmm_NofXd1 = _mm_set1_ps(CNDF(d1));
//     __m128 xmm_NofXd2 = _mm_set1_ps(CNDF(d2));
//     __m128 xmm_NegNofXd1 = _mm_sub_ps(_mm_set1_ps(1.0f), xmm_NofXd1);
//     __m128 xmm_NegNofXd2 = _mm_sub_ps(_mm_set1_ps(1.0f), xmm_NofXd2);
    
//     // Convert N(d1), N(d2), -N(-d1), -N(-d2) to scalar variables
//     _mm_store_ss(&NofXd1, xmm_NofXd1);
//     _mm_store_ss(&NofXd2, xmm_NofXd2);
//     _mm_store_ss(&NegNofXd1, xmm_NegNofXd1);
//     _mm_store_ss(&NegNofXd2, xmm_NegNofXd2);
    
//     // Calculate future value using SIMD instructions
//     __m128 xmm_futureValueX = _mm_mul_ps(xmm_strikePrice, xmm_NegNofXd2);
//     xmm_futureValueX = _mm_mul_ps(xmm_futureValueX, xmm_time);
//     xmm_futureValueX = _mm_exp_ps(xmm_futureValueX);
    
//     // Convert future value to a scalar variable
//     _mm_store_ss(&FutureValueX, xmm_futureValueX);
    
//     // Calculate option price using SIMD instructions
//     __m128 xmm_optionPrice = _mm_mul_ps(xmm_stockPrice, xmm_NofXd1);
//     xmm_optionPrice = _mm_sub_ps(xmm_optionPrice, xmm_strikePrice);
//     xmm_optionPrice = _mm_mul_ps(xmm_optionPrice, xmm_NegNofXd2);
//     xmm_optionPrice = _mm_mul_ps(xmm_optionPrice, xmm_futureValueX);
    
//     // Convert option price to a scalar variable
//     _mm_store_ss(&OptionPrice, xmm_optionPrice);
    
//     if (otype == 0) {
//         // Calculate call option price
//         return OptionPrice;
//     } else {
//         // Calculate put option price
//         float putPrice = OptionPrice - sptprice + strike * expf(-rate * otime);
//         return putPrice;
//     }
// }
/* Alternative Implementation */
void* impl_vector(void* args)
{
  // استخراج المتغيرات اللازمة لاستدعاء الدالة blackScholes
  // args_t* arguments = (args_t*)args;
  // size_t num_stocks = arguments->num_stocks;
  // float* sptPrice = arguments->sptPrice;
  // float* strike = arguments->strike;
  // float* rate = arguments->rate;
  // float* volatility = arguments->volatility;
  // float* otime = arguments->otime;
  // char* otype = arguments->otype;

  // // احتفظ بالنتيجة في مصفوفة
  // float* optionPrices = malloc(num_stocks * sizeof(float));

  // // استدعاء الدالة blackScholes لحساب سعر الخيار لكل سهم
  // for (size_t i = 0; i < num_stocks; i += 4)
  // {
  //   // قم بتحميل البيانات في متغيرات SIMD
  //   __m128 xmm_sptPrice = _mm_loadu_ps(&sptPrice[i]);
  //   __m128 xmm_strike = _mm_loadu_ps(&strike[i]);
  //   __m128 xmm_rate = _mm_loadu_ps(&rate[i]);
  //   __m128 xmm_volatility = _mm_loadu_ps(&volatility[i]);
  //   __m128 xmm_otime = _mm_loadu_ps(&otime[i]);
  //   _m128i xmm_otype = _mm_loadu_si128((_m128i*)&otype[i]);

  //   // قم بحساب سعر الخيار باستخدام SIMD
  //   __m128 xmm_optionPrice = blackScholesSSE(xmm_sptPrice, xmm_strike, xmm_rate, xmm_volatility, xmm_otime, xmm_otype);

  //   // احفظ النتائج في مصفوفة الأسعار
  //   _mm_storeu_ps(&optionPrices[i], xmm_optionPrice);
  // }

  // اعداد القيمة التي ترغب في إرجاعها من الدالة impl_scalar
  // في هذا المثال، سنعود بمؤشر إلى مصفوفة الأسعار
  // return optionPrices;
  return NULL;
}