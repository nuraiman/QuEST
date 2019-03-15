//
// Created by giuli on 14/03/2019.
//

#pragma once
#include "approx-pf.hpp"

// The Algorithm assumes that N is odd and not a power of a prime
long long ShorFactoring(QuESTEnv& env, long long N) {
    // check if N is odd
    if (N % 2 == 0) {
        return 2;
    }

    // check if N is a power of a prime
    long long p;
    for (int k = 2; k <= std::log2(N); ++k) {
        p = (long long) std::pow(N,1.0/k);
        if (N % p == 0){
            return p;
        }
    }

    // shor's algorithm
    long long a = sample_uniformly(2, N-1);
    long long b = gcd(a, N);
    if (b > 1){
        return b;
    }

    long long r = PeriodFinding(env, a, N);
    if (r % 2 == 1){
        return -1;
    }
    r /= 2;
    b = (long long) std::pow(a,r);;
    long long s = gcd(b, N);
    if (s == 1){
        return -1;
    }
    return s;
}
