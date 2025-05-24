#pragma once

#ifdef USE_MPFR

#include <mpfr.h>
#include <vector>
#include <string.h>

#define NUMBER_SERIAL_SIZE 512
#define DEFAULT_PRECISION 256

class number {
private:
    explicit number(mpfr_prec_t precision)
    {
        mpfr_init2(n_ptr, precision);
    }

public:
    mpfr_t n_ptr;

    number()
    {
        mpfr_init2(n_ptr, DEFAULT_PRECISION);
    }

    static number from_precision(mpfr_prec_t precision)
    {
        return number(precision);
    }

    number(const number& other)
    {
        mpfr_init2(n_ptr, mpfr_get_prec(other.n_ptr));
        mpfr_set(n_ptr, other.n_ptr, MPFR_RNDN);
    }

    number(double d, unsigned int precision = DEFAULT_PRECISION)
    {
        mpfr_init2(n_ptr, precision);
        mpfr_set_d(n_ptr, d, MPFR_RNDN);
    }

    number(int i, unsigned int precision = DEFAULT_PRECISION)
    {
        mpfr_init2(n_ptr, precision);
        mpfr_set_si(n_ptr, i, MPFR_RNDN);
    }

    ~number()
    {
        mpfr_clear(n_ptr);
    }

    number& operator=(const number& other)
    {
        if (this != &other) {
            if (mpfr_get_prec(n_ptr) != mpfr_get_prec(other.n_ptr)) {
                mpfr_clear(n_ptr);
                mpfr_init2(n_ptr, mpfr_get_prec(other.n_ptr));
            }
            mpfr_set(n_ptr, other.n_ptr, MPFR_RNDN);
        }
        return *this;
    }

    number& operator=(double d)
    {
        mpfr_set_d(n_ptr, d, MPFR_RNDN);
        return *this;
    }

    number& operator=(int i)
    {
        mpfr_set_si(n_ptr, i, MPFR_RNDN);
        return *this;
    }

    // Math operations
    number log() const
    {
        number result = from_precision(mpfr_get_prec(n_ptr));
        mpfr_log(result.n_ptr, n_ptr, MPFR_RNDN);
        return result;
    }

    number log2() const
    {
        number result = from_precision(mpfr_get_prec(n_ptr));
        mpfr_log2(result.n_ptr, n_ptr, MPFR_RNDN);
        return result;
    }

    number operator+(const number& rhs) const
    {
        number result = from_precision(mpfr_get_prec(n_ptr));
        mpfr_add(result.n_ptr, n_ptr, rhs.n_ptr, MPFR_RNDN);
        return result;
    }

    number operator-(const number& rhs) const
    {
        number result = from_precision(mpfr_get_prec(n_ptr));
        mpfr_sub(result.n_ptr, n_ptr, rhs.n_ptr, MPFR_RNDN);
        return result;
    }

    number operator*(const number& rhs) const
    {
        number result = from_precision(mpfr_get_prec(n_ptr));
        mpfr_mul(result.n_ptr, n_ptr, rhs.n_ptr, MPFR_RNDN);
        return result;
    }

    number operator/(const number& rhs) const
    {
        number result = from_precision(mpfr_get_prec(n_ptr));
        mpfr_div(result.n_ptr, n_ptr, rhs.n_ptr, MPFR_RNDN);
        return result;
    }

    // Comparison operators
    bool operator==(const number& rhs) const { return mpfr_cmp(n_ptr, rhs.n_ptr) == 0; }
    bool operator!=(const number& rhs) const { return mpfr_cmp(n_ptr, rhs.n_ptr) != 0; }
    bool operator<(const number& rhs) const { return mpfr_cmp(n_ptr, rhs.n_ptr) < 0; }
    bool operator<=(const number& rhs) const { return mpfr_cmp(n_ptr, rhs.n_ptr) <= 0; }
    bool operator>(const number& rhs) const { return mpfr_cmp(n_ptr, rhs.n_ptr) > 0; }
    bool operator>=(const number& rhs) const { return mpfr_cmp(n_ptr, rhs.n_ptr) >= 0; }

    // Casts
    explicit operator double() const { return mpfr_get_d(n_ptr, MPFR_RNDN); }
    explicit operator float() const { return mpfr_get_flt(n_ptr, MPFR_RNDN); }

    void serialize(char* buffer) const
    {
        constexpr int DIGITS = 100;
        char* str = nullptr;

        mpfr_asprintf(&str, "%.*Rf", DIGITS, n_ptr);

        if (!str) {
            buffer[0] = '\0';
            return;
        }

        memset(buffer, 0, NUMBER_SERIAL_SIZE);
        strncpy(buffer, str, NUMBER_SERIAL_SIZE - 1);
        mpfr_free_str(str);
    }
    void deserialize(const char* data)
    {
        mpfr_set_str(n_ptr, data, 10, MPFR_RNDN);
    }
};

#define LOG(X) X.log()
#define LOG2(X) X.log2()

#define SERIALIZE_NUM(X, Y) X.serialize(Y)
#define DESERIALIZE_NUM(X, Y) X.deserialize(Y)

#elif PRECISION_128
#include <quadmath.h>
typedef long double number;
#define NUMBER_SERIAL_SIZE 128
#define SERIALIZE_NUM(X, Y) sprintf(Y, "%.36Lf", X)
#define DESERIALIZE_NUM(X, Y) sscanf(Y, "%Lf", &X)
#define LOG(X) logq(X)
#define LOG2(X) log2q(X)

#elif PRECISION_32
#include <math.h>
typedef float number;
#define NUMBER_SERIAL_SIZE 128
#define SERIALIZE_NUM(X, Y) sprintf(Y, "%.9g", X)
#define DESERIALIZE_NUM(X, Y) sscanf(Y, "%f", &X)
#define LOG(X) logf(X)
#define LOG2(X) log2f(X)

#else
#include <math.h>
typedef double number;
#define NUMBER_SERIAL_SIZE 128

#define SERIALIZE_NUM(X, Y) sprintf(Y, "%.17g", X)
#define DESERIALIZE_NUM(X, Y) sscanf(Y, "%lf", &X)

#define LOG(X) log(X)
#define LOG2(X) log2(X)

#endif