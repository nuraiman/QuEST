//
// Created by giuli on 11/03/2019.
//
#include <iostream>
#include <cmath>
#include <QuEST.h>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>

#include "qft.cpp"
// Input: We are given access to a function f : Z_M --> Z_N (for some integers M, N)
// The function f must have the following properties:
// (1) f is PERIODIC: There exists r such that r divides M and f(x+r) = f(x) for all x in Z_M .
// (2) f is ONE-TO-ONE on each period: for all pairs (x, y) such that |x-y| < r, f(x) != f(y) .
// Output: The algorithm finds the period r

// define gcd
int gcd(int x, int y){
    while (y != 0){
        int r = x % y;
        x = y;
        y = r;
    }
    return x;
}

// define the function for modular exponentiation
int ModExp(int a, int N, int x){

    return int((long long) pow(a,x) % N);
}


int PeriodFinding(int a, int M, int N){ // M should be a power of 2

    // find smallest natural number m such that M <= 2^m
    int m = int(ceil(log2(M)));
    // find smallest natural number n such that N <= 2^n
    int n = int(ceil(log2(N)));

    //load quest
    QuESTEnv env = createQuESTEnv();

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
    int f;
    for (int i = 0; i < (1<<m); ++i) {
         Complex amp = getAmp(qureg, i);
         qreal ar=0; qreal ai=0;
         setAmps(qureg, i, &ar, &ai,1);
         f = ModExp(a, N, i);
         std::cout << i << "  " << f << "  " << (f<<m) + i << std::endl;
         setAmps(qureg, (f<<m) + i, &amp.real, &amp.imag,1);
     }

    // measure the second register
    for (int j = m; j < m + n ; ++j) {
        measure(qureg,j);
    }

    // Apply again a qft to the first register
    QFT(qureg, m);

    // measure the first register, find answer k
    int k = 0;
    int result;
    for (int j = 0; j < n ; ++j) {
        result = measure(qureg,j);
        if (result==1){
            k = k | (1 << j);
        }
    }

    // determine the period r
    int temp = gcd(k, N);
    N /= temp;
    int r = N;

    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return r;
}

// What is the best way to
int main(int narg, char *varg[]) {

    int r = PeriodFinding(7,8,15);

    std::cout << r << std::endl;

    return 0;
}