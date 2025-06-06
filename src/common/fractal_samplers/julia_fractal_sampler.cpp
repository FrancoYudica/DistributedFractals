#include "../fractal.h"
#include "../common.h"
#include <cstdint>

#ifndef USE_MPFR
const number two(2.0);
const number four(4.0);
const number ln_four(1.38629436112);

float julia_sampler(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{

    // Z(n+1) = Z(n)^2 + C
    // Constant C = Cx + Cyi
    const number Cx = settings.julia_settings.Cx;
    const number Cy = settings.julia_settings.Cy;

    number zx = world_x, zy = world_y;
    uint32_t iter = 0;

    number length_squared = zx * zx + zy * zy;
    number xtemp;
    while (length_squared < 4.0 && iter < settings.max_iterations) {
        xtemp = zx * zx - zy * zy + Cx;
        zy = two * zx * zy + Cy;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    number smooth_t = (number)iter - LOG2_NUM(LOG_NUM(length_squared) / ln_four);

    // Normalizes
    return (float)(smooth_t / (number)settings.max_iterations);
}
#else
#include <mpfr.h>
float julia_sampler(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{
    const number Cx = settings.julia_settings.Cx;
    const number Cy = settings.julia_settings.Cy;

    // Define MPFR variables
    mpfr_t two, four;
    mpfr_t zx, zy;
    mpfr_t zx2, zy2;
    mpfr_t xtemp;
    mpfr_t length_squared;
    mpfr_t log_ls, log_four, ratio, log2_ratio;
    mpfr_t smooth_t, max_iterations_f, result;

    // Init with default precision
    mpfr_inits2(DEFAULT_PRECISION,
        two, four, zx, zy, zx2, zy2, xtemp,
        length_squared, log_ls, log_four, ratio, log2_ratio,
        smooth_t, max_iterations_f, result, (mpfr_ptr)0);

    // Constants
    mpfr_set_si(two, 2, MPFR_RNDN);
    mpfr_set_si(four, 4, MPFR_RNDN);
    mpfr_log(log_four, four, MPFR_RNDN);

    // z = world_x + i*world_y
    mpfr_set(zx, world_x.n_ptr, MPFR_RNDN);
    mpfr_set(zy, world_y.n_ptr, MPFR_RNDN);

    uint32_t iter = 0;

    while (true) {
        // zx2 = zx * zx
        mpfr_mul(zx2, zx, zx, MPFR_RNDN);
        // zy2 = zy * zy
        mpfr_mul(zy2, zy, zy, MPFR_RNDN);
        // length_squared = zx2 + zy2
        mpfr_add(length_squared, zx2, zy2, MPFR_RNDN);

        if (mpfr_cmp(length_squared, four) >= 0 || iter >= settings.max_iterations)
            break;

        // xtemp = zx2 - zy2 + cx
        mpfr_sub(xtemp, zx2, zy2, MPFR_RNDN);
        mpfr_add(xtemp, xtemp, Cx.n_ptr, MPFR_RNDN);

        // zy = 2 * zx * zy + cy
        mpfr_mul(zy, zx, zy, MPFR_RNDN);
        mpfr_mul(zy, zy, two, MPFR_RNDN);
        mpfr_add(zy, zy, Cy.n_ptr, MPFR_RNDN);

        // zx = xtemp
        mpfr_set(zx, xtemp, MPFR_RNDN);

        iter++;
    }

    // Smooth iteration count
    mpfr_log(log_ls, length_squared, MPFR_RNDN);
    mpfr_div(ratio, log_ls, log_four, MPFR_RNDN);
    mpfr_log2(log2_ratio, ratio, MPFR_RNDN);
    mpfr_set_si(smooth_t, iter, MPFR_RNDN);
    mpfr_sub(smooth_t, smooth_t, log2_ratio, MPFR_RNDN);

    // Normalize to [0,1]
    mpfr_set_si(max_iterations_f, settings.max_iterations, MPFR_RNDN);
    mpfr_div(result, smooth_t, max_iterations_f, MPFR_RNDN);

    float output = mpfr_get_flt(result, MPFR_RNDN);

    mpfr_clears(two, four, zx, zy, zx2, zy2, xtemp,
        length_squared, log_ls, log_four,
        ratio, log2_ratio, smooth_t,
        max_iterations_f, result, (mpfr_ptr)0);

    return output;
}

#endif