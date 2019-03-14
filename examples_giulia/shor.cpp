//
// Created by giuli on 14/03/2019.
//
#include <iostream>
#include <cmath>
#include <QuEST.h>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>

#include "approx-pf.cpp"

int sample_uniformly(int low = 0, int high = 1) {
    // use random device to seed the random number generator named mt.
    // it requests random data from the operating system
    // static means that it will be created only once and then will be reused
    // in all further calls of this function
    static std::random_device rd;
    static std::mt19937 mt(rd()); // random number generator
    std::uniform_int_distribution<int> distribution(low, high);

    return distribution(mt);
}

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

int main(int narg, char *varg[]) {
    int N = 21;
    int factor = ShorFactoring(N);
    if (factor > 0) {
        std::cout << "Factors found: " << factor << ",  " << N / factor << std::endl;
    }
    else {
        std::cout << "Algorithm failed" << std::endl;
    }

    return 0;
}
