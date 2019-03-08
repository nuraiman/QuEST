//
// Created by giuli on 08/03/2019.
//
// Implementation of the Quantum Fourier Transform
// (e.g., this is used in Shor's factoring algorithm)
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <QuEST.h>
#include <chrono>
#include <random>

const double pi = 3.14159265358979323846;

// Generic textbook approach for implementing the QFT
// Require N = 2^n, where n = number of qubits

// circuit performing the QFT
void QFT(Qureg &qr, int n){
    for (int j = 0; j < n ; ++j) {
        for (int k = 0; k < j; ++k) {
            controlledPhaseShift(qr, (n-1)-j, (n-1)-k, pi/(1<<(j-k)));
        }
        hadamard(qr, (n-1)-j);
    }
}


// Kitaev's approach for implementing the QFT
void KQFT(Qureg &qr){

}















// check functions
int main(int narg, char *varg[]) {
    //load quest
    int n = 13;
    QuESTEnv env = createQuESTEnv();

    Qureg qureg = createQureg(n, env);
    initZeroState(qureg);

    QFT(qureg, n);

    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;
}