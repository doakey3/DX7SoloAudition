#ifndef ARM_MATH_H_
#define ARM_MATH_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CMSIS-style typedefs */
typedef float    float32_t;
typedef int16_t  q15_t;

/* Minimal biquad struct used by Dexed FX code */
typedef struct
{
    uint32_t         numStages;
    float32_t       *pState;   /* length: 4*numStages */
    const float32_t *pCoeffs;  /* length: 5*numStages */
} arm_biquad_casd_df1_inst_f32;

/* ---- DSP functions Dexed uses ---- */
void arm_float_to_q15(const float32_t *pSrc,
                      q15_t          *pDst,
                      uint32_t        blockSize);

void arm_fill_f32(float32_t value,
                  float32_t *pDst,
                  uint32_t   blockSize);

void arm_sub_f32(const float32_t *pSrcA,
                 const float32_t *pSrcB,
                 float32_t       *pDst,
                 uint32_t         blockSize);

void arm_scale_f32(const float32_t *pSrc,
                   float32_t        scale,
                   float32_t       *pDst,
                   uint32_t         blockSize);

void arm_offset_f32(const float32_t *pSrc,
                    float32_t        offset,
                    float32_t       *pDst,
                    uint32_t         blockSize);

void arm_mult_f32(const float32_t *pSrcA,
                  const float32_t *pSrcB,
                  float32_t       *pDst,
                  uint32_t         blockSize);

void arm_biquad_cascade_df1_f32(const arm_biquad_casd_df1_inst_f32 *S,
                                const float32_t *pSrc,
                                float32_t       *pDst,
                                uint32_t         blockSize);

void arm_biquad_cascade_df1_init_f32(arm_biquad_casd_df1_inst_f32 *S,
                                     uint8_t                       numStages,
                                     const float32_t              *pCoeffs,
                                     float32_t                    *pState);

/* ---- Extra hooks Dexed expects from its host ---- */

/* Arduino-style boolean type sometimes used in the code */
#ifndef boolean
typedef bool boolean;
#endif

/* Global flag used by Compressor in compressor.h */
extern bool use_HP_prefilter;

/* Arduino-style time in ms since program start */
uint32_t millis(void);

/* Saturating right-shift used in Dexed::getSamples */
int32_t signed_saturate_rshift(int32_t x, int bits, int rshift);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ----------------------------------------------------
 * C++-only helpers
 * -------------------------------------------------- */
#ifdef __cplusplus

/* Arduino-style constrain() */
template <typename T, typename A, typename B>
inline T constrain(T x, A minVal, B maxVal)
{
    T lo = static_cast<T>(minVal);
    T hi = static_cast<T>(maxVal);
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

#endif /* __cplusplus */

#endif /* ARM_MATH_H_ */
