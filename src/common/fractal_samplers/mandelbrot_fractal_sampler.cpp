
#include "../fractal.h"
#include "../common.h"
#include <cstdint>

#ifndef USE_MPFR
const number two(2.0);
const number four(4.0);
const number ln_two(0.69314718056);

float mandelbrot_sampler(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{
    number zx = 0.0;
    number zy = 0.0;
    number zx2 = 0.0;
    number zy2 = 0.0;
    uint32_t iter = 0;

    const number escape_radius_squared = 4.0;

    while (zx2 + zy2 < escape_radius_squared && iter < settings.max_iterations) {
        zy = two * zx * zy + world_y;
        zx = zx2 - zy2 + world_x;

        zx2 = zx * zx;
        zy2 = zy * zy;

        ++iter;
    }

    // Avoid log() of zero or negative
    number smooth_t = (number)iter;
    if (zx2 + zy2 > 0.0) {
        number log_zn = LOG_NUM(zx2 + zy2) / 2.0;
        number nu = LOG2_NUM(log_zn / ln_two);
        smooth_t = (number)iter - nu;
    }

    return (float)(smooth_t / (number)settings.max_iterations);
}

#else
#include <mpfr.h>
float mandelbrot_sampler(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{
    // Define MPFR variables
    mpfr_t two, four;
    mpfr_t zx, zy;
    mpfr_t zx2, zy2; // zx^2 and zy^2
    mpfr_t xtemp;
    mpfr_t length_squared;
    mpfr_t log_ls; // log(length_squared)
    mpfr_t log_four; // log(4)
    mpfr_t ratio; // log_ls / log_four
    mpfr_t log2_ratio; // log2(ratio)
    mpfr_t smooth_t;
    mpfr_t max_iterations_f;
    mpfr_t result;

    // Init with default precision
    mpfr_inits2(DEFAULT_PRECISION, two, four, zx, zy, zx2, zy2, xtemp,
        length_squared, log_ls, log_four, ratio, log2_ratio,
        smooth_t, max_iterations_f, result, (mpfr_ptr)0);

    // Constants
    mpfr_set_si(two, 2, MPFR_RNDN);
    mpfr_set_si(four, 4, MPFR_RNDN);
    mpfr_log(log_four, four, MPFR_RNDN); // log(4)

    // zx = zy = 0.0
    mpfr_set_d(zx, 0.0, MPFR_RNDN);
    mpfr_set_d(zy, 0.0, MPFR_RNDN);

    // Start with 0 length
    mpfr_set_d(length_squared, 0.0, MPFR_RNDN);

    uint32_t iter = 0;

    // Loop
    while (true) {
        mpfr_mul(zx2, zx, zx, MPFR_RNDN); // zx^2
        mpfr_mul(zy2, zy, zy, MPFR_RNDN); // zy^2
        mpfr_add(length_squared, zx2, zy2, MPFR_RNDN); // zx^2 + zy^2

        if (mpfr_cmp(length_squared, four) >= 0 || iter >= settings.max_iterations)
            break;

        // xtemp = zx^2 - zy^2 + world_x
        mpfr_sub(xtemp, zx2, zy2, MPFR_RNDN);
        mpfr_add(xtemp, xtemp, world_x.n_ptr, MPFR_RNDN);

        // zy = 2 * zx * zy + world_y
        mpfr_mul(zy, zx, zy, MPFR_RNDN);
        mpfr_mul(zy, zy, two, MPFR_RNDN);
        mpfr_add(zy, zy, world_y.n_ptr, MPFR_RNDN);

        // zx = xtemp
        mpfr_set(zx, xtemp, MPFR_RNDN);

        iter++;
    }

    // smooth_t = iter - log2(log(length_squared) / log(4))
    mpfr_log(log_ls, length_squared, MPFR_RNDN); // log(length_squared)
    mpfr_div(ratio, log_ls, log_four, MPFR_RNDN); // ratio = log_ls / log(4)
    mpfr_log2(log2_ratio, ratio, MPFR_RNDN); // log2(ratio)
    mpfr_set_si(smooth_t, iter, MPFR_RNDN); // iter to mpfr
    mpfr_sub(smooth_t, smooth_t, log2_ratio, MPFR_RNDN); // smooth_t = iter - log2_ratio

    // Normalize: smooth_t / settings.max_iterations
    mpfr_set_si(max_iterations_f, settings.max_iterations, MPFR_RNDN);
    mpfr_div(result, smooth_t, max_iterations_f, MPFR_RNDN);
    mpfr_div(result, smooth_t, max_iterations_f, MPFR_RNDN);

    float output = mpfr_get_flt(result, MPFR_RNDN);

    // Clear all variables
    mpfr_clears(two, four, zx, zy, zx2, zy2, xtemp, length_squared,
        log_ls, log_four, ratio, log2_ratio,
        smooth_t, max_iterations_f, result, (mpfr_ptr)0);

    return output;
}

#endif