#include <iostream>
#include <cmath>
#include <QuEST.h>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>
#include "cxxopts.hpp"
#include <algorithm>

typedef long long qInt;

bool isPrime(qInt n) { // TODO: Change this function, need something with complexity O(log(N))
    if (n == 1) {
        return false;
    }

    auto max_divisor = (qInt) std::ceil(std::sqrt(n));
    for (qInt i = 2; i <= max_divisor; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// define gcd
qInt gcd(qInt x, qInt y){
    while (y != 0){
        qInt r = x % y;
        x = y;
        y = r;
    }
    return x;
}

// Cancel the fractions, for rational numbers z = x/y
void cancelFraction(qInt &x, qInt &y){
    qInt temp = gcd(x, y);
    x /= temp;
    y /= temp;
}

// define the function for modular exponentiation
qInt modExp(qInt a, qInt N, qInt x){
    // we use the identity: a^(i+1) % N = a*(a^i % N) % N to avoid overflow
    qInt r = 1;
    for (qInt i = 0; i < x; ++i) {
        r = r * a % N;
    }
    return r;
}

template<typename T>
T ceil_log2(T n) {
    return std::ceil(std::log2<T>(n));
}

const double pi = 3.14159265358979323846;

qInt sample_uniformly(qInt low = 0, qInt high = 1) {
    // use random device to seed the random number generator named mt.
    // it requests random data from the operating system
    // static means that it will be created only once and then will be reused
    // in all further calls of this function
    static std::random_device rd;
    static std::mt19937 mt(rd()); // random number generator
    std::uniform_int_distribution<qInt> distribution(low, high);

    return distribution(mt);
}
