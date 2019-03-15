#include <iostream>
#include <cmath>
#include <QuEST.h>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>
#include "cxxopts.hpp"
#include <algorithm>

bool isPrime(long long n) { // TODO: Change this function, because we are cheating!
    if (n == 1) {
        return false;
    }

    auto max_divisor = (long long) std::ceil(std::sqrt(n));
    for (long long i = 2; i <= max_divisor; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// Input: We are given access to a function f : Z_M --> Z_N, f = a^x mod N (for some integers M, N)
// M is chosen such that: M = 2^m >= N^2
// Output: The algorithm finds the period r with some probability p > 4/pi^2 - O(1/N) > 0
// Therefore, the algorithm must be applied multiple times in order to find r.

// define gcd
long long gcd(long long x, long long y){
    while (y != 0){
        long long r = x % y;
        x = y;
        y = r;
    }
    return x;
}

// Cancel the fractions, for rational numbers z = x/y
void cancelFraction(long long &x, long long &y){
    long long temp = gcd(x, y);
    x /= temp;
    y /= temp;
}

// define the function for modular exponentiation
long long modExp(long long a, long long N, int x){
    // we use the identity: a^(i+1) % N = a*(a^i % N) % N to avoid overflow
    long long r = 1ll;
    for (int i = 0; i < x; ++i) {
        r = r * a % N;
    }
    return r;
}

template<typename T>
T ceil_log2(T n) {
    return std::ceil(std::log2<T>(n));
}

const double pi = 3.14159265358979323846;

long long sample_uniformly(long long low = 0, long long high = 1) {
    // use random device to seed the random number generator named mt.
    // it requests random data from the operating system
    // static means that it will be created only once and then will be reused
    // in all further calls of this function
    static std::random_device rd;
    static std::mt19937 mt(rd()); // random number generator
    std::uniform_int_distribution<long long> distribution(low, high);

    return distribution(mt);
}
