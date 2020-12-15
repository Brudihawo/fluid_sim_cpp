#include "discrete_math.h"
#include <limits>

void find_min_max(interval& iv, const double* s, long MAXINDEX) {
    iv = {.max = -std::numeric_limits<double>::max(),
          .min =  std::numeric_limits<double>::max()};
    for (long i = 0; i < MAXINDEX; i++) {
            if (iv.min > s[i]) iv.min = s[i];
            if (iv.max < s[i]) iv.max = s[i];
    }
}


