//
// Created by giuli on 13/03/2019.
//
#include <iostream>
#include <cmath>
#include <QuEST.h>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>

#include "qft.hpp"
// Input: We are given access to a function f : Z_M --> Z_N, f = a^x mod N (for some integers M, N)
// M is chosen such that: M = 2^m >= N^2
// Output: The algorithm finds the period r with some probability p > 4/pi^2 - O(1/N) > 0
// Therefore, the algorithm must be applied multiple times in order to find r.

// define gcd
int gcd(int x, int y){
    while (y != 0){
        int r = x % y;
        x = y;
        y = r;
    }
    return x;
}

// Cancel the fractions, for rational numbers z = x/y
void CancelFraction(int &x, int &y){
    int temp = gcd(x, y);
    x /= temp;
    y /= temp;
}

// define the function for modular exponentiation
int ModExp(int a, int N, int x){
    // MK: this temporary line should not be necessary anymore
    // x = x % 18; // temporary line, since I already know the period (r=6) and in order to test Continued Fraction
    // MK: we use the identity: a^(i+1) % N = a*(a^i % N) % N to avoid overflow
    int r = 1;
    for (int i = 0; i < x; ++i) {
        r = r * a % N;
    }
    return r;
}

int ceil_log2(int n) {
    return (int) std::ceil(std::log2(n));
}

// implement the bit oracle O_f on both registers
void apply_modexp_oracle(Qureg& qureg, int m, int a, int N) {
    for (int i = 0; i < (1<<m); ++i) {
        Complex amp = getAmp(qureg, i); 
        qreal nreal=0; qreal nimag=0;
        setAmps(qureg, i, &nreal, &nimag,1);
        int f = ModExp(a, N, i);
        // std::cout << i << "  " << f << "  " << (f<<m) + i << std::endl;
        setAmps(qureg, (f<<m) + i, &amp.real, &amp.imag,1);
    }
}

// determines the period r from z = y/M using continued fraction expansion (CFE)
// or return -1 if unsuccessful
int period_using_CFE(int y, int M, int a, int N) {
    int r = 0;
    // z = y/M
    // In theory, we know that y < M, therefore a0 = 0
    // index i starting from 0.
    int hi_1 = 0, hi_2 = 1, ki_1 = 1, ki_2 = 0; // initial conditions, given that a0 = 0
    int zi_nom, zi_denom; // inverted parameter, zi = ai + bi, with integer ai and bi < 1
    // std::cout << " z = " << y << "/" << M << std::endl;
    int ai, hi, ki; // The convergents of z are of the form hi/ki
    int bi_nom = y, bi_denom = M;
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
        CancelFraction(bi_nom, bi_denom);
        hi = ai*hi_1 + hi_2;
        ki = ai*ki_1 + ki_2; // recursive formulas for the convergents
        CancelFraction(hi,ki);
        r = ki;
        // std::cout << " conv = " << hi << "/" << ki << std::endl;
        if (ModExp(a,N,r) == 1 && r <= N){
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

int PeriodFinding(QuESTEnv& env, int a, int N){

    // find smallest natural number n such that N <= 2^n
    int n = ceil_log2(N);
    // find smallest natural number m such that N^2 <= 2^m
    int m = 2 * n; // MK: instead of ceil_log2(N*N) to avoid overflow
    int M = 1 << m;

    // create quantum register (with two 'subregisters')
    Qureg qureg = createQureg(m+n, env);
    // MK: this is not necessary as createQureg already invokes this
    // initZeroState(qureg);

    // perform QFT to the first register
    QFT(qureg, m);
    // equivalent to performing QFT to the first register:
    // for (int j = 0; j < m ; ++j) {
    //     hadamard(qureg, j);
    // }
    apply_modexp_oracle(qureg, m, a, N);

    // measure the second register (and collapse the full wavefunction)
    for (int j = m; j < m + n ; ++j) {
        measure(qureg,j);
    }

    // Apply again a qft to the first register
    QFT(qureg, m);

    // measure the first register, find answer y
    int y = 0;
    int result;
    for (int j = 0; j < m ; ++j) {
        result = measure(qureg,j);
        if (result==1){
            y = y | (1 << j);
        }
    }

    // determine the period r from z = y/M using continued fraction expansion (CFE)
    int r = period_using_CFE(y, M, a, N);

    destroyQureg(qureg, env);
    return r;
}

// test the functions
//int main(int narg, char *varg[]) {
//    // load quest
//    QuESTEnv env = createQuESTEnv();
//
//    // number of repetitions of the algorithms
//    int n_rep = 100;
//
//    // counts how many times the algorithm was successful
//    int counter = 0;
//
//    int period = 0;
//    for (int i = 0; i < n_rep; ++i) {
//        int r = PeriodFinding(env, 10,21);
//        counter += (r > -1) ? 1 : 0;
//        if (r> -1){ period = r;}
//    }
//
//    double success_probability = (100.0 * counter) / n_rep;
//
//    std::cout << "The probability of finding the right solution, based on " << n_rep << " repetitions, is approximatelly = " << success_probability << std::endl;
//
//    double target_probability = 100.0 * 4.0/(pi*pi);
//
//    std::cout << "\nTarget probability = " << target_probability << std::endl;
//
//    std::cout << "r = " << period << std::endl;
//
//    destroyQuESTEnv(env);
//
//    return 0;
//}
