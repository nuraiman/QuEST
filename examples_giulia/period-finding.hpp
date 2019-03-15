//
// Created by giuli on 11/03/2019.
//
#pragma once
#include "qft.hpp"
// Input: We are given access to a function f : Z_M --> Z_N, f = a^x mod N (for some integers M, N)
// The function f must have the following properties:
// (1) f is PERIODIC: There exists r such that r divides M and f(x+r) = f(x) for all x in Z_M .
// (2) f is ONE-TO-ONE on each period: for all pairs (x, y) such that |x-y| < r, f(x) != f(y) .
// Output: The algorithm finds the period r

long long PeriodFinding(QuESTEnv &env, int a, int M, int N){ // M must be divisible by the period r! Otherwise use approx-pf

    // find smallest natural number m such that M <= 2^m
    int m = int(ceil(log2(M)));
    // find smallest natural number n such that N <= 2^n
    int n = int(ceil(log2(N)));

    // perform QFT to the first register
    Qureg qureg = createQureg(m+n, env);
    initZeroState(qureg);
    QFT(qureg, m);

    // equivalent to performing QFT to the first register
    // Qureg qureg = createQureg(n+m, env);
    // initZeroState(qureg);
    // for (int j = 0; j < m ; ++j) {
    //     hadamard(qureg, j);
    // }

    // implement the bit oracle O_f on both registers
    long long f;
    for (int i = 0; i < (1<<m); ++i) {
         Complex amp = getAmp(qureg, i);
         qreal ar=0; qreal ai=0;
         setAmps(qureg, i, &ar, &ai,1);
         f = modExp(a, N, i);
         // std::cout << i << "  " << f << "  " << (f<<m) + i << std::endl;
         setAmps(qureg, (f<<m) + i, &amp.real, &amp.imag,1);
     }

    // measure the second register (and collapse the full wavefunction)
    for (int j = m; j < m + n ; ++j) {
        measure(qureg,j);
    }

    // Apply again a qft to the first register
    QFT(qureg, m);

    // measure the first register, find answer k
    int k = 0;
    int result;
    for (int j = 0; j < m ; ++j) {
        result = measure(qureg,j);
        if (result==1){
            k = k | (1 << j);
        }
    }

    // determine the period r
    std::cout << "k = " << k << "  M = "<< M << std::endl;
    long long temp = gcd(k, M);
    M /= temp;
    long long r = M;

    destroyQureg(qureg, env);
    return r;
}
