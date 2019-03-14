//
// Created by giuli on 13/03/2019.
//

#pragma once
#include "qft.hpp"

// Input: We are given access to a function f : Z_M --> Z_N, f = a^x mod N (for some integers M, N)
// M is chosen such that: M = 2^m >= N^2
// Output: The algorithm finds the period r with some probability p > 4/pi^2 - O(1/N) > 0
// Therefore, the algorithm must be applied multiple times in order to find r.

// implement the bit oracle O_f on both registers
void applyModExpOracle(Qureg& qureg, int m, long long a, long long N) {
    for (long long i = 0L; i < (1L<<m); ++i) {
        Complex amp = getAmp(qureg, i); 
        qreal nreal=0; qreal nimag=0;
        setAmps(qureg, i, &nreal, &nimag,1);
        long long f = modExp(a, N, i);
        // std::cout << i << "  " << f << "  " << (f<<m) + i << std::endl;
        setAmps(qureg, (f<<m) + i, &amp.real, &amp.imag,1);
    }
}

// determines the period r from z = y/M using continued fraction expansion (CFE)
// or return -1 if unsuccessful
long long periodUsingCFE(long long y, long long M, long long a, long long N) {
    long long r = 0;
    // z = y/M
    // In theory, we know that y < M, therefore a0 = 0
    // index i starting from 0.
    long long hi_1 = 0, hi_2 = 1, ki_1 = 1, ki_2 = 0; // initial conditions, given that a0 = 0
    long long zi_nom, zi_denom; // inverted parameter, zi = ai + bi, with integer ai and bi < 1
    // std::cout << " z = " << y << "/" << M << std::endl;
    long long ai, hi, ki; // The convergents of z are of the form hi/ki
    long long bi_nom = y, bi_denom = M;
    while (true){
        if (y == 0){ // to prevent from division by 0
            // std::cout << "Algorithm failed, y = 0!" << std::endl;
            return -1;
        }
        zi_nom = bi_denom;
        zi_denom = bi_nom; // zi = 1/bi
        ai = zi_nom/zi_denom; // ai = floor(zi)
        // std::cout << "a = " << ai << "  ";
        bi_nom = zi_nom - ai*zi_denom;
        bi_denom = zi_denom; // bi = zi - ai
        cancelFraction(bi_nom, bi_denom);
        hi = ai*hi_1 + hi_2;
        ki = ai*ki_1 + ki_2; // recursive formulas for the convergents
        cancelFraction(hi,ki);
        r = ki;
        // std::cout << " conv = " << hi << "/" << ki << std::endl;
        if (modExp(a,N,r) == 1 && r <= N){
            // std::cout << "Found it!" << std::endl;
            return r;
        }
        if (bi_nom == 0){
            // std::cout << "Algorithm failed!" << std::endl;
            return -1;
        }

        hi_2 = hi_1; ki_2 = ki_1;
        hi_1 = hi; ki_1 = ki;
    }
    return -1;

}

long long PeriodFinding(QuESTEnv& env, long long a, long long N){

    // find smallest natural number n such that N <= 2^n
    int n = (int) ceil_log2(N);
    // find smallest natural number m such that N^2 <= 2^m
    int m = 2 * n;
    long long M = 1L << m;

    // create quantum register (with two 'subregisters')
    Qureg qureg = createQureg(m+n, env);

    // perform QFT to the first register
    QFT(qureg, m);
    // equivalent to performing QFT to the first register:
    // for (int j = 0; j < m ; ++j) {
    //     hadamard(qureg, j);
    // }
    applyModExpOracle(qureg, m, a, N);

    // measure the second register (and collapse the full wavefunction)
    for (int j = m; j < m + n ; ++j) {
        measure(qureg,j);
    }

    // Apply again a qft to the first register
    QFT(qureg, m);

    // measure the first register, find answer y
    long long y = 0;
    int result;
    for (int j = 0; j < m ; ++j) {
        result = measure(qureg,j);
        if (result==1){
            y = y | (1 << j);
        }
    }

    // determine the period r from z = y/M using continued fraction expansion (CFE)
    long long r = periodUsingCFE(y, M, a, N);

    destroyQureg(qureg, env);
    return r;
}

