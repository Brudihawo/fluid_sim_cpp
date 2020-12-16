#include "discrete_math.h"
#include <limits>
#include <utility>

void find_min_max(std::pair<double, double>& iv, const double* s, long MAXINDEX) {
    iv.first = -std::numeric_limits<double>::max();
    iv.second =  std::numeric_limits<double>::max();
    for (long i = 0; i < MAXINDEX; i++) {
            if (iv.first > s[i]) iv.first = s[i];
            if (iv.second < s[i]) iv.second = s[i];
    }
}


