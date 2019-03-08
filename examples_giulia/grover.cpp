//
// Created by giuli on 07/03/2019.
//

// Implementation of Grover's algorithm
// For ONE marked element
// NOTE: it would be possible to extend the algorithm to include arrays with MULTIPLE marked elements

// Input: an Array of length N with exactly one marked element
// Output: Grover finds this marked Element (unstructured search) using O(sqrt(N)) queries
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <QuEST.h>
#include <chrono>
#include <random>


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

// here we are cheating a little bit... (i guess because it makes no sense to simulate quantum parallelism)
// Implement the phase oracle U_f with f(x0) = 1 and f(x) = 0 when x != x0.
void PhaseOracle(Qureg &qr, long long x0){

    Complex amp = getAmp(qr,x0);
    amp.real = -amp.real;
    amp.imag = -amp.imag;
    setAmps(qr, x0, &amp.real, &amp.imag,1);

}


void GroverStep(Qureg &qr, int n){

    for (int i = 0; i < n; ++i) {
        hadamard(qr,i);
        pauliX(qr,i);
    }
   // std::vector<int> controlqubits(n-1);
   // std::iota(controlqubits.begin(), controlqubits.end(), 0);
   // multiControlledPhaseFlip(qr, controlqubits.data(), n-1); // --> gives wrong result!

   // representing 'multiControlledPhaseFlip()'
    Complex amp = getAmp(qr,(1<<n)-1);
    amp.real = -amp.real;
    amp.imag = -amp.imag;
    setAmps(qr, (1<<n)-1, &amp.real, &amp.imag,1);

    for (int j = 0; j < n; ++j) {
        pauliX(qr,j);
        hadamard(qr,j);

    }

    // apply -id operator
    for (int k = 0; k < (1<<n) ; ++k) {
        amp = getAmp(qr,k);
        amp.real = -amp.real;
        amp.imag = -amp.imag;
        setAmps(qr, k, &amp.real, &amp.imag,1);
    }
} // one could also use 'createDensityQureg()' in order to build the operator Id - 2 |00..0><0..00|

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    long long N = 1 << 13;

    // find smallest natural number n such that N <= 2^n
    int n = 0;
    while(true){
        if (N <= 1<<n){
            break;
        }
        ++n;
    }
    std::cout << "n = " << n << std::endl;
    // choose marked element at random
    long long x0 = sample_uniformly(0,N-1);
    std::cout << "Test x0 = " << x0 << std::endl;


    // Create quantumregister and initialize in the |++...+> state
    Qureg qureg = createQureg(n, env);
    initPlusState(qureg);

    // calculate optimal number T of iterations
    int T = int(round(pi*0.25*sqrt(1<<n)-0.5));
    std::cout << "T = " << T << std::endl;

    // Grover algorithm
    for (int i = 0; i < T ; ++i) {
        PhaseOracle(qureg, x0);
        GroverStep(qureg, n);
    }


    // measure all the qubits and save their values in the variable 'result'
    qreal success = getProbAmp(qureg, x0);

    long long result = 0;
    int a;
    for (int j = 0; j < n ; ++j) {
        a = measure(qureg,j);
        if (a==1){
            result = result | (1 << j);
        }
    }

    std::cout << "The algorithm found x0 = " << result << " with success probability " << success << std::endl;


    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;
}
