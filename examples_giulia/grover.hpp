//
// Created by giuli on 07/03/2019.
//

#pragma once
#include "math_util.hpp"

// Implementation of Grover's algorithm
// For ONE marked element
// NOTE: it would be possible to extend the algorithm to include arrays with MULTIPLE marked elements

// Input: an Array of length N with exactly one marked element
// Output: Grover finds this marked Element (unstructured search) using O(sqrt(N)) queries

// here we are cheating a little bit... (i guess because it makes no sense to simulate quantum parallelism)
// Implement the phase oracle U_f with f(x0) = 1 and f(x) = 0 when x != x0.
void PhaseOracle(Qureg &qr, qInt x0){

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
    for (qInt k = 0; k < (1<<n) ; ++k) {
        amp = getAmp(qr,k);
        amp.real = -amp.real;
        amp.imag = -amp.imag;
        setAmps(qr, k, &amp.real, &amp.imag,1);
    }
} // one could also use 'createDensityQureg()' in order to build the operator Id - 2 |00..0><0..00|

