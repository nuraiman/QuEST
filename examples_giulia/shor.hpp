//
// Created by giuli on 14/03/2019.
//

#pragma once
#include "approx-pf.hpp"

// compute gcd(pow(a,r)-1,N) avoiding overflow // y == N
qInt gcdForShor(qInt a, qInt r, qInt y){
    qInt x = (modExp(a,y,r)-1 + y) ;
    while (y != 0){
        qInt t = x % y; // compute tem = (a^r-1) mod N = (a^r mod N) - 1 = (a^r mod N - 1 + N) mod N
        x = y;
        y = t;
    }
    return x;
}

// The Algorithm assumes that N is odd and not a power of an integer
std::vector<qInt> ShorFactoring(QuESTEnv& env,const qInt N, RandomGen &rng) {
    // recursion stopping criteria
    if (N == 1) {
        return {};
    }
    if (isPrime(N)) {
        return {N};
    }

    // check if N is odd
    if (N % 2 == 0) {
        std::vector<qInt> factors = {2};
        auto other_factors = ShorFactoring(env, N/2, rng);
        factors.insert(factors.end(), other_factors.begin(), other_factors.end());
        return factors;
    }

    // check if N is a power of an int
    qInt p;
    for (int k = 2; k <= std::ceil(std::log2(N)); ++k) {
        p = (qInt) std::pow(N,1.0/k);
        auto pk = (qInt) std::pow(p,k);
        if (pk == N) {
            std::vector<qInt> factors = ShorFactoring(env, p, rng);
            std::vector<qInt> new_factors;
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
    qInt a = rng.sampleUniformly(2, N-1);
    qInt b = gcd(a, N);
    if (b > 1){
        std::vector<qInt> factors = ShorFactoring(env, b, rng);
        auto other_factors = ShorFactoring(env, N/b, rng);
        factors.insert(factors.end(), other_factors.begin(), other_factors.end());
        return factors;
    }

    qInt r = PeriodFinding(env, a, N);
    if (r % 2 == 1) {
        return ShorFactoring(env, N, rng);
    }
    r /= 2;
    // b = (qInt) std::pow(a,r); qInt s = gcd(b - 1, N); // --> get overflow...
    qInt s = gcdForShor(a, r, N);
    if (s == 1){
        return ShorFactoring(env, N, rng);
    }
    std::vector<qInt> factors = ShorFactoring(env, s, rng);
    auto other_factors = ShorFactoring(env, N/s, rng);
    factors.insert(factors.end(), other_factors.begin(), other_factors.end());
    return factors;
}
