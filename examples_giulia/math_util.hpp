#include <iostream>
#include <cmath>
#include <QuEST.h>
#include <vector>
#include <numeric>
#include <chrono>
#include <random>

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
void cancelFraction(int &x, int &y){
    int temp = gcd(x, y);
    x /= temp;
    y /= temp;
}

// define the function for modular exponentiation
int modExp(int a, int N, int x){
    // we use the identity: a^(i+1) % N = a*(a^i % N) % N to avoid overflow
    int r = 1;
    for (int i = 0; i < x; ++i) {
        r = r * a % N;
    }
    return r;
}

int ceil_log2(int n) {
    return (int) std::ceil(std::log2(n));
}

const double pi = 3.14159265358979323846;

long long sample_uniformly(int low = 0, long long high = 1) {
    // use random device to seed the random number generator named mt.
    // it requests random data from the operating system
    // static means that it will be created only once and then will be reused
    // in all further calls of this function
    static std::random_device rd;
    static std::mt19937 mt(rd()); // random number generator
    std::uniform_int_distribution<long long> distribution(low, high);

    return distribution(mt);
}
