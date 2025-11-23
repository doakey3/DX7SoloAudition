#include "arm_math.h"

#include <cstring>                // memset
#include <algorithm>              // std::max, std::min
#include <chrono>                 // millis()
#include <atomic>

bool use_HP_prefilter = false;

/* ---------------- DSP function implementations ---------------- */

void arm_float_to_q15(const float32_t *pSrc,
                      q15_t          *pDst,
                      uint32_t        blockSize)
{
    for (uint32_t i = 0; i < blockSize; ++i)
    {
        float32_t x = pSrc[i];

        if (x > 0.999969f) x = 0.999969f;
        if (x < -1.0f)     x = -1.0f;

        int32_t v = static_cast<int32_t>(x * 32768.0f);

        if (v >  32767) v =  32767;
        if (v < -32768) v = -32768;

        pDst[i] = static_cast<q15_t>(v);
    }
}

void arm_fill_f32(float32_t value,
                  float32_t *pDst,
                  uint32_t   blockSize)
{
    for (uint32_t i = 0; i < blockSize; ++i)
        pDst[i] = value;
}

void arm_sub_f32(const float32_t *pSrcA,
                 const float32_t *pSrcB,
                 float32_t       *pDst,
                 uint32_t         blockSize)
{
    for (uint32_t i = 0; i < blockSize; ++i)
        pDst[i] = pSrcA[i] - pSrcB[i];
}

void arm_scale_f32(const float32_t *pSrc,
                   float32_t        scale,
                   float32_t       *pDst,
                   uint32_t         blockSize)
{
    for (uint32_t i = 0; i < blockSize; ++i)
        pDst[i] = pSrc[i] * scale;
}

void arm_offset_f32(const float32_t *pSrc,
                    float32_t        offset,
                    float32_t       *pDst,
                    uint32_t         blockSize)
{
    for (uint32_t i = 0; i < blockSize; ++i)
        pDst[i] = pSrc[i] + offset;
}

void arm_mult_f32(const float32_t *pSrcA,
                  const float32_t *pSrcB,
                  float32_t       *pDst,
                  uint32_t         blockSize)
{
    for (uint32_t i = 0; i < blockSize; ++i)
        pDst[i] = pSrcA[i] * pSrcB[i];
}

void arm_biquad_cascade_df1_f32(const arm_biquad_casd_df1_inst_f32 *S,
                                const float32_t *pSrc,
                                float32_t       *pDst,
                                uint32_t         blockSize)
{
    const uint32_t numStages = S->numStages;
    float32_t     *pState    = S->pState;
    const float32_t *pCoeffs = S->pCoeffs;

    for (uint32_t n = 0; n < blockSize; ++n)
    {
        float32_t x = pSrc[n];
        float32_t y = x;

        for (uint32_t stage = 0; stage < numStages; ++stage)
        {
            float32_t *s = &pState[4 * stage];       // x1, x2, y1, y2
            const float32_t *c = &pCoeffs[5 * stage]; // b0,b1,b2,a1,a2

            const float32_t b0 = c[0];
            const float32_t b1 = c[1];
            const float32_t b2 = c[2];
            const float32_t a1 = c[3];
            const float32_t a2 = c[4];

            const float32_t x0 = y;

            float32_t y0 = b0 * x0
                         + b1 * s[0]
                         + b2 * s[1]
                         - a1 * s[2]
                         - a2 * s[3];

            // Shift state
            s[1] = s[0];
            s[0] = x0;
            s[3] = s[2];
            s[2] = y0;

            y = y0;
        }

        pDst[n] = y;
    }
}

void arm_biquad_cascade_df1_init_f32(arm_biquad_casd_df1_inst_f32 *S,
                                     uint8_t                       numStages,
                                     const float32_t              *pCoeffs,
                                     float32_t                    *pState)
{
    S->numStages = numStages;
    S->pCoeffs   = pCoeffs;
    S->pState    = pState;

    if (pState)
        std::memset(pState, 0, sizeof(float32_t) * 4u * numStages);
}

/* ---------------- Host helpers implementations ---------------- */

uint32_t millis(void)
{
    using namespace std::chrono;
    static const steady_clock::time_point start = steady_clock::now();
    const auto now = steady_clock::now();
    return (uint32_t)duration_cast<milliseconds>(now - start).count();
}

int32_t signed_saturate_rshift(int32_t x, int bits, int rshift)
{
    // Right shift first
    int32_t shifted = x >> rshift;

    if (bits >= 32)
        return shifted;

    const int32_t maxVal = ((int32_t)1 << (bits - 1)) - 1;
    const int32_t minVal = -((int32_t)1 << (bits - 1));

    if (shifted > maxVal) return maxVal;
    if (shifted < minVal) return minVal;
    return shifted;
}
