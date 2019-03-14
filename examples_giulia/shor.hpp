//
// Created by giuli on 14/03/2019.
//

#pragma once
#include "approx-pf.hpp"

// The Algorithm assumes that N is odd and not a power of a prime
int ShorFactoring(int N) {
    // check if N is odd
    if (N % 2 == 0) {
        return 2;
    }

    // check if N is a power of a prime
    int p;
    for (int k = 2; k <= std::log2(N); ++k) {
        p = (int) std::pow(N,1.0/k);
        if (N %  p == 0){
            return p;
        }
    }

    // shor's algorithm
    int a = sample_uniformly(2, N-1);
    int b = gcd(a, N);
    if (b > 1){
        return b;
    }

    //load quest
    QuESTEnv env = createQuESTEnv();
    int r = PeriodFinding(env, a, N);
    if (r % 2 == 1){
        destroyQuESTEnv(env);
        return -1;
    }
    r /= 2;
    b = int((long long) pow(a,r));;
    int s = gcd(b, N);
    if (s == 1){
        destroyQuESTEnv(env);
        return -1;
    }
    destroyQuESTEnv(env);
    return s;
}
