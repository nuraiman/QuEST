//
// Created by giuli on 14/03/2019.
//

#pragma once
#include "approx-pf.hpp"

// compute gcd(pow(a,r)-1,N) avoiding overflow // y == N
long long gcdForShor(long long a, long long r, long long y){
    long long x = (modExp(a,y,r)-1 + y) ;
    while (y != 0){
        long long t = x % y; // compute tem = (a^r-1) mod N = (a^r mod N) - 1 = (a^r mod N - 1 + N) mod N
        x = y;
        y = t;
    }
    return x;
}

// The Algorithm assumes that N is odd and not a power of an integer
std::vector<long long> ShorFactoring(QuESTEnv& env,const long long N) {
    // recursion stopping criteria
    if (N == 1) {
        return {};
    }
    if (isPrime(N)) {
        return {N};
    }

    // check if N is odd
    if (N % 2 == 0) {
        std::vector<long long> factors = {2};
        auto other_factors = ShorFactoring(env, N/2);
        factors.insert(factors.end(), other_factors.begin(), other_factors.end());
        return factors;
    }

    // check if N is a power of an int
    long long p;
    for (int k = 2; k <= std::ceil(std::log2(N)); ++k) {
        p = (long long) std::pow(N,1.0/k);
        auto pk = (long long) std::pow(p,k);
        if (pk == N) {
            std::vector<long long> factors = ShorFactoring(env, p);
            std::vector<long long> new_factors;
            new_factors.reserve(k * factors.size());
            for (const auto& f : factors) {
                for (int i = 0; i < k; ++i) {
                    new_factors.push_back(f);
                }
            }
            return new_factors;
        }
    }

    // shor's algorithm
    long long a = sample_uniformly(2, N-1);
    long long b = gcd(a, N);
    if (b > 1){
        std::vector<long long> factors = ShorFactoring(env, b);
        auto other_factors = ShorFactoring(env, N/b);
        factors.insert(factors.end(), other_factors.begin(), other_factors.end());
        return factors;
    }

    long long r = PeriodFinding(env, a, N);
    if (r % 2 == 1) {
        return ShorFactoring(env, N);
    }
    r /= 2;
    // b = (long long) std::pow(a,r); long long s = gcd(b - 1, N); // --> get overflow...
    long long s = gcdForShor(a, r, N);
    if (s == 1){
        return ShorFactoring(env, N);
    }
    std::vector<long long> factors = ShorFactoring(env, s);
    auto other_factors = ShorFactoring(env, N/s);
    factors.insert(factors.end(), other_factors.begin(), other_factors.end());
    return factors;
}
